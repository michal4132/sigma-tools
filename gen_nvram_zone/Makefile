CXX = clang++
CC = clang
CXXFLAGS = -I. -I../common -D__TOOL__ -Wall -Wextra
CFLAGS = $(CXXFLAGS)
TARGET = gen_nvram_zone

SRCS = ../common/aes.c \
       ../common/uni_base64.c \
       ../common/nvram_core.c \
       file_flash.c \
       file_nvram.c \
       gen_nvram_zone.cpp

OBJS = $(SRCS:.c=.o)
OBJS := $(OBJS:.cpp=.o)

.PHONY: all clean rebuild

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

rebuild: clean all
