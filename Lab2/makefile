all	:	server/server	client/client
.PHONY	:	all
server	:	server/server.c
	gcc server/server.c -o server.out
client	:	client/client.c
	gcc client/client.c -o client.out
clean	:	server/server	client/client
	rm	server/server	client/client 	

