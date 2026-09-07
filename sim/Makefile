CC      = gcc
CFLAGS  = -Wall -Wextra -g -pthread -Iinclude
SRCS    = src/c_lock_manager.c src/core.c src/benchmark.c
TARGET  = sim_clock

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)

.PHONY: all clean
