CC = clang
CFLAGS = -Wall -Wextra -O2
TARGET = nand-extract
SRCS = main.c
OBJS = $(SRCS:.c=.o)

.PHONY: all clean rebuild

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

rebuild: clean all
