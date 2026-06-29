CC := gcc
CFLAGS := -Wall -Wextra -Wpedantic -std=c11 -g -D_POSIX_C_SOURCE=200809L -Iinclude -Isrc
LDFLAGS := -pthread

SRC := $(wildcard src/*.c)
OBJ := $(SRC:.c=.o)
BIN := aeroporto

.PHONY: all clean run

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(BIN)
	./$(BIN)

clean:
	rm -f $(OBJ) $(BIN)
