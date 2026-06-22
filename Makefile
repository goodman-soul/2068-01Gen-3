CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2
TARGET = clog_demo
TARGET_EMBEDDED = clog_demo_embedded

SRCS = main.c clog.c
OBJS = $(SRCS:.c=.o)

.PHONY: all clean embedded run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c clog.h
	$(CC) $(CFLAGS) -c $< -o $@

embedded: CFLAGS += -DCLOG_COLOR=0 -DCLOG_FILE_OUTPUT=0
embedded: clean $(TARGET_EMBEDDED)

$(TARGET_EMBEDDED): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET) $(TARGET_EMBEDDED) app.log
