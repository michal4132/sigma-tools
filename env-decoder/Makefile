CC = clang
CFLAGS = -Wall -Wextra
TARGET = env-codec
SRCS = main.c aes.c uni_base64.c
OBJS = $(SRCS:.c=.o)
HEADERS = aes.h uni_base64.h

.PHONY: all clean rebuild

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

run: $(TARGET)
	./$(TARGET)

rebuild: clean all
