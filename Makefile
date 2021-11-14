all: clean
	gcc server.c -w -O3 -o server -lssl -lcrypto -lpthread

clean:
	rm -f server
