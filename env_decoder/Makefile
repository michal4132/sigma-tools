CC = clang
CFLAGS = -Wall -Wextra -I../common
TARGET = env_codec
SRCS = main.c ../common/aes.c ../common/uni_base64.c
OBJS = $(SRCS:.c=.o)
HEADERS = ../common/aes.h ../common/uni_base64.h

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
