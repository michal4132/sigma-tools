#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

int main() {
    int fd = open("/dev/led_timer", O_RDWR);
    if (fd >= 0) {
        ioctl(fd, 0xef01, 0);
        close(fd);
    }
    return 0;
}
