CC = clang
CFLAGS = -Wall -Wextra -Werror -Wpedantic
LDFLAGS = -lm

COMPRESS = encode 
COMPRESS_OBJ = encode.o

DECOMPRESS = decode 
DECOMPRESS_OBJ = decode.o

OBJS = trie.o word.o io.o

%.o: %.c
	$(CC) $(CFLAGS) -c $<

all: $(COMPRESS) $(DECOMPRESS)

$(COMPRESS): $(COMPRESS_OBJ) $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

$(DECOMPRESS): $(DECOMPRESS_OBJ) $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

clean:
	rm -f $(COMPRESS) $(COMPRESS_OBJ) $(DECOMPRESS) $(DECOMPRESS_OBJ) $(OBJS)

format:
	clang-format -i -style=file *.[ch]

scan-build: clean
	scan-build --use-cc=$(CC) make
