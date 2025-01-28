CC = gcc
AR = ar
CFLAGS = -fPIC -Wall -Wextra -O2
LDFLAGS = -shared

# Detect OS
OS := $(shell uname -s)

# Directories
SRC_DIR = src
INCLUDE_DIR = include
LIB_DIR = lib
BIN_DIR = bin

# Files
LIBRARY_NAME = clenzdat
LIBRARY_SRC = $(SRC_DIR)/$(LIBRARY_NAME).c
LIBRARY_OBJ = $(LIBRARY_NAME).o
STATIC_LIB = lib$(LIBRARY_NAME).a
SHARED_LIB_WIN = $(LIBRARY_NAME).dll
SHARED_LIB_UNIX = lib$(LIBRARY_NAME).so
HEADER = $(LIBRARY_NAME).h
SAMPLE_SRC = $(SRC_DIR)/sample.c
SAMPLE_BIN = sample

.PHONY: all clean static shared sample move

all: static shared move sample

static: $(STATIC_LIB)

shared: $(SHARED_LIB_WIN) $(SHARED_LIB_UNIX)

sample: move
    @if [ $$? -eq 0 ]; then \
        echo "Compiling sample..."; \
        $(CC) $(CFLAGS) -o $(SAMPLE_BIN) $(SAMPLE_SRC) -L$(LIB_DIR) -I$(INCLUDE_DIR) -lclenzdat -lm; \
    else \
        echo "Skipping sample compilation due to move failure"; \
    fi

$(LIBRARY_OBJ): $(LIBRARY_SRC)
    $(CC) $(CFLAGS) -c $< -o $@

$(STATIC_LIB): $(LIBRARY_OBJ)
    $(AR) rcs $@ $<

$(SHARED_LIB_WIN): $(LIBRARY_SRC)
    $(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<

$(SHARED_LIB_UNIX): $(LIBRARY_SRC)
    $(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<

move:
ifeq ($(OS), Linux)
    @echo "Moving files for Linux..."
    @sudo mv $(HEADER) /usr/include/ && \
    sudo mv $(STATIC_LIB) /usr/lib/ && \
    sudo mv $(SHARED_LIB_UNIX) /usr/lib/ && \
    sudo ldconfig || (echo "Moving failed on Linux"; exit 1)
else ifeq ($(OS), Darwin)
    @echo "Moving files for macOS..."
    @sudo mv $(HEADER) /usr/local/include/ && \
    sudo mv $(STATIC_LIB) /usr/local/lib/ && \
    sudo mv $(SHARED_LIB_UNIX) /usr/local/lib/ && \
    sudo ldconfig || (echo "Moving failed on macOS"; exit 1)
else ifeq ($(OS), Windows_NT)
    @echo "Moving files for Windows..."
    @move $(HEADER) C:\MinGW\include\ && \
    move $(STATIC_LIB) C:\MinGW\lib\ && \
    move $(SHARED_LIB_WIN) C:\MinGW\bin\ || (echo "Moving failed on Windows"; exit 1)
else
    @echo "Unsupported operating system"
    @exit 1
endif
    @echo "Files moved successfully"

clean:
    rm -f $(LIBRARY_OBJ) $(STATIC_LIB) $(SHARED_LIB_WIN) $(SHARED_LIB_UNIX) $(SAMPLE_BIN)