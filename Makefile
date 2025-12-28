
TARGET = sv.exe

SRCS = src/main.cpp
SRCS += src/window/window.cpp
SRCS += src/renderer/renderer.cpp
SRCS += src/audio/read.cpp
SRCS += src/audio/audio.cpp

CC = x86_64-w64-mingw32-g++
LFLAGS = -lSDL3 -lm -lsndfile
CFLAGS  = -Wall -Wextra -Wpedantic -O1 -std=c++20 
DEBUG_CFLAGS = -fsanitize=address,undefined,leak -fno-omit-frame-pointer -Wshadow -Wconversion -Wnull-dereference -Wdouble-promotion -g

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o  $(TARGET) $(SRCS) $(LFLAGS)

clean:
	rm $(TARGET)
