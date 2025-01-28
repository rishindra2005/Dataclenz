CC = gcc
AR = ar
CFLAGS = -fPIC -Wall -Wextra -O2
LDFLAGS = -shared

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

.PHONY: all clean

all: static shared sample move

static: $(STATIC_LIB)

shared: $(SHARED_LIB_WIN) $(SHARED_LIB_UNIX)

sample: $(SAMPLE_BIN)

$(LIBRARY_OBJ): $(LIBRARY_SRC)
	$(CC) $(CFLAGS) -c $< -o $@

$(STATIC_LIB): $(LIBRARY_OBJ)
	$(AR) rcs $@ $<

$(SHARED_LIB_WIN): $(LIBRARY_SRC)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<

$(SHARED_LIB_UNIX): $(LIBRARY_SRC)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<

$(SAMPLE_BIN): $(SAMPLE_SRC) $(STATIC_LIB)
	$(CC) $(CFLAGS) -o $@ $< -L. -l$(LIBRARY_NAME)

move:
	mkdir -p $(INCLUDE_DIR) $(LIB_DIR) $(BIN_DIR)
	mv $(HEADER) $(INCLUDE_DIR)/
	mv $(STATIC_LIB) $(LIB_DIR)/
	mv $(SHARED_LIB_WIN) $(LIB_DIR)/
	mv $(SHARED_LIB_UNIX) $(LIB_DIR)/
	mv $(SAMPLE_BIN) $(BIN_DIR)/
	mv $(LIBRARY_OBJ) $(BIN_DIR)/

clean:
	rm -f $(LIBRARY_OBJ) $(STATIC_LIB) $(SHARED_LIB_WIN) $(SHARED_LIB_UNIX) $(SAMPLE_BIN)
	rm -rf $(INCLUDE_DIR) $(LIB_DIR) $(BIN_DIR)