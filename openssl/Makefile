all:
	@gcc -Wall -o client  Client.c -L/usr/lib -lssl -lcrypto
	@gcc -Wall -o server  Server.c -L/usr/lib -lssl -lcrypto
clean:
	@rm server
	@rm client
