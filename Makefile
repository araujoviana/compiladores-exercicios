CC := clang
CFLAGS := -Wall -Wextra -Wpedantic -std=c17 -O0 -g

SRCS := $(wildcard *.c)
BINS := $(SRCS:.c=)

.PHONY: all debug clean

all: $(BINS)

debug: CFLAGS += -DDEBUG
debug: clean $(BINS)

%: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(BINS)
