all : server/server client/client
.PHONY : all
server : server/server.c header.h
	gcc server/server.c -o server/server
client : client/client.c header.h
	gcc client/client.c -o client/client
clean :
	rm server/server client/client