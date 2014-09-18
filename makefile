CC = gcc 
CFLAGS = -g -Wall  -DDEBUG -fPIC -Wl,-rpath,/usr/local/libevent-2.0.21-stable/lib  \
-L/usr/local/libevent-2.0.21-stable/lib \
-levent \
-I/usr/local/libevent-2.0.21-stable/include

TARGET = find

SRCS = find.c

OBJS = $(SRCS:.c=.o)


$(TARGET): $(SRCS)
	$(CC) $^ $(CFLAGS) -o $(TARGET)

%.o: %.c
	$(CC) -o $@ -c $<

clean:
	rm -rf *.o $(TARGET)

.PHONY: clean
