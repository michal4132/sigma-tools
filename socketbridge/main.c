#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <termios.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <pty.h>

#define PTY_PATH "/tmp/ttyZigbeeNCP"
#define PORT 1234
#define BUFFER_SIZE 1024

static volatile sig_atomic_t running = 1;
static int master_fd = -1;
static int server_fd = -1;

void cleanup(void) {
    if (master_fd >= 0) {
        close(master_fd);
        master_fd = -1;
    }
    if (access(PTY_PATH, F_OK) == 0) {
        unlink(PTY_PATH);
    }
    if (server_fd >= 0) {
        close(server_fd);
        server_fd = -1;
    }
}

void handle_signal(int sig) {
    (void)sig;
    running = 0;
}

int setup_signal_handlers(void) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    
    sa.sa_handler = handle_signal;
    sa.sa_flags = SA_RESTART;
    
    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        perror("sigaction SIGTERM");
        return -1;
    }
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction SIGINT");
        return -1;
    }

    // Handle SIGCHLD to prevent zombies
    sa.sa_handler = SIG_IGN;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction SIGCHLD");
        return -1;
    }
    
    return 0;
}

int create_pty(void) {
    // Clean up any existing PTY symlink first
    if (access(PTY_PATH, F_OK) == 0) {
        if (unlink(PTY_PATH) == -1) {
            perror("unlink existing PTY");
            return -1;
        }
    }

    int mfd = open("/dev/ptmx", O_RDWR | O_NOCTTY);
    if (mfd == -1) {
        perror("open /dev/ptmx");
        return -1;
    }

    if (grantpt(mfd) == -1 || unlockpt(mfd) == -1) {
        perror("grantpt/unlockpt");
        close(mfd);
        return -1;
    }

    char *slave_name = ptsname(mfd);
    if (!slave_name) {
        perror("ptsname");
        close(mfd);
        return -1;
    }

    if (symlink(slave_name, PTY_PATH) == -1) {
        perror("symlink");
        close(mfd);
        return -1;
    }

    // Set non-blocking mode
    int flags = fcntl(mfd, F_GETFL);
    if (flags == -1 || fcntl(mfd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl");
        close(mfd);
        unlink(PTY_PATH);
        return -1;
    }

    return mfd;
}

int create_server(void) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket");
        return -1;
    }

    int opt = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt");
        close(sock);
        return -1;
    }

    // Disable Nagle's algorithm to reduce latency
    if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt)) == -1) {
        perror("setsockopt TCP_NODELAY");
        close(sock);
        return -1;
    }

    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(PORT)
    };

    if (bind(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(sock);
        return -1;
    }

    if (listen(sock, 5) == -1) {
        perror("listen");
        close(sock);
        return -1;
    }

    // Set non-blocking mode
    int flags = fcntl(sock, F_GETFL);
    if (flags == -1 || fcntl(sock, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl");
        close(sock);
        return -1;
    }

    return sock;
}

void handle_client(int client_fd) {
    char buffer[BUFFER_SIZE];
    fd_set read_fds;
    struct timeval tv;

    // Set client socket to non-blocking mode
    int flags = fcntl(client_fd, F_GETFL);
    if (flags != -1) {
        fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);
    }

    while (running) {
        FD_ZERO(&read_fds);
        FD_SET(client_fd, &read_fds);
        FD_SET(master_fd, &read_fds);

        tv.tv_sec = 1;  // 1 second timeout for checking running flag
        tv.tv_usec = 0;

        int max_fd = (client_fd > master_fd ? client_fd : master_fd) + 1;
        int ret = select(max_fd, &read_fds, NULL, NULL, &tv);

        if (ret == -1) {
            if (errno == EINTR) continue;
            break;
        }

        if (ret == 0) continue;  // timeout, check running flag

        if (FD_ISSET(client_fd, &read_fds)) {
            ssize_t len = read(client_fd, buffer, BUFFER_SIZE);
            if (len <= 0) {
                if (len == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
                    continue;
                }
                break;
            }
            ssize_t written = 0;
            while (written < len) {
                ssize_t n = write(master_fd, buffer + written, len - written);
                if (n == -1) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        continue;
                    }
                    goto client_done;
                }
                written += n;
            }
        }

        if (FD_ISSET(master_fd, &read_fds)) {
            ssize_t len = read(master_fd, buffer, BUFFER_SIZE);
            if (len == -1) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    continue;
                }
                break;
            }
            if (len == 0) break;

            ssize_t written = 0;
            while (written < len) {
                ssize_t n = write(client_fd, buffer + written, len - written);
                if (n == -1) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        continue;
                    }
                    goto client_done;
                }
                written += n;
            }
        }
    }

client_done:
    close(client_fd);
}

int main(void) {
    if (setup_signal_handlers() == -1) {
        fprintf(stderr, "Failed to setup signal handlers\n");
        return EXIT_FAILURE;
    }

    // Register cleanup handler
    if (atexit(cleanup) != 0) {
        fprintf(stderr, "Failed to register cleanup handler\n");
        return EXIT_FAILURE;
    }

    master_fd = create_pty();
    if (master_fd == -1) {
        fprintf(stderr, "Failed to create PTY\n");
        return EXIT_FAILURE;
    }

    server_fd = create_server();
    if (server_fd == -1) {
        fprintf(stderr, "Failed to create server\n");
        return EXIT_FAILURE;
    }

    printf("PTY created at %s, listening on port %d\n", PTY_PATH, PORT);

    while (running) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                usleep(100000);  // Sleep 100ms to prevent busy loop
                continue;
            }
            if (errno == EINTR) {
                continue;
            }
            perror("accept");
            break;
        }

        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            close(client_fd);
            continue;
        }

        if (pid == 0) {  // Child process
            close(server_fd);
            handle_client(client_fd);
            _exit(0);
        }

        // Parent process
        close(client_fd);
    }

    return EXIT_SUCCESS;
}
