CC := cc
CFLAGS = -Wall -Wpedantic -g
LDFLAGS =
LDLIBS = -lm

SRCS = pngpail.c error.c mapfile.c
OBJS = $(SRCS:.c=.o)

all: pngpail

pngpail: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) pngpail

.PHONY: all clean
