YFLAGS=-d

all: calc xml

calc: syntax.o lexer.o 
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

xml: xml.o lexer.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f *.o td4 y.tab.h *.c
