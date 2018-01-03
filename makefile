CC = gcc
#flags:
C_COMP_FLAG = -Wall -g -std=gnu99

binaries=client server

client: file_client.c constants.h
	$(CC) $(C_COMP_FLAG) $?  -o $@

server: file_server.c constants.h
	$(CC) $(C_COMP_FLAG) $?  -o $@


all: client server

clean:
	rm -f $(binaries) 
