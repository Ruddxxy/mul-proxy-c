# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O2
LIBS = -lws2_32

# Target executable
TARGET = proxy

# Source files
SRC = src/proxy.c

# Default target
all: $(TARGET)

# Build target
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LIBS)

# Clean up build artifacts
clean:
	rm -f $(TARGET)
