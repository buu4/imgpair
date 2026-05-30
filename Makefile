CC := cc
CFLAGS = -Wall -Wpedantic -g -std=c11
LDFLAGS =
LDLIBS = -lm

SRCS = imgpair.c main.c error.c mapfile.c
OBJS = $(SRCS:.c=.o)

all: imgpair

imgpair: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) imgpair

.PHONY: all clean
