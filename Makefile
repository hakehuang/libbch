.PHONY : clean

CFLAGS= -fPIC -g
LDFLAGS= -shared

SOURCES = bch.c libbch.c
HEADERS = $(shell echo *.h)
OBJECTS = $(SOURCES:.c=.o)

TARGET=../lib/libbch.so

all: $(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET)
	-rm -f test

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)

test: test.c $(OBJECTS)
	$(CC) $(CFLAGS) test.c $(OBJECTS) -o $@
