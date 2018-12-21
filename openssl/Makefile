all:
	@gcc -Wall -o client.o  client.c -L/usr/lib -lssl -lcrypto
	@gcc -Wall -o server.o  server.c -L/usr/lib -lssl -lcrypto
clean:
	@rm server.o
	@rm client.o
