CC = gcc 
CFLAGS = -g -Wall  -DDEBUG -fPIC -Wl,-rpath,/usr/local/libevent-2.0.21-stable/lib  \
-L/usr/local/libevent-2.0.21-stable/lib \
-levent \
-I/usr/local/libevent-2.0.21-stable/include

PROC = find

SRCS = find.c

OBJS = $(SRCS:.c=.o)

TEST = find_test

TEST_SRC = find_test.c

all: $(PROC) $(TEST)

$(PROC): $(SRCS)
	$(CC)  $(CFLAGS) -o $@ $^

$(TEST): $(TEST_SRC)
	$(CC) -g -Wall -DDEBUG -fPIC -lpthread -o $@ $^

%.o: %.c
	$(CC) -o $@ -c $<

clean:
	rm -rf *.o $(TARGET) $(TEST)

.PHONY: clean
