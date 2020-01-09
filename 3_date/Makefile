.c.o:
	gcc -Wall -g $< -c -o $*.o

CLIENT-OBJ = client.o
SERVER-OBJ = server.o

all: client server

client: $(CLIENT-OBJ)
	gcc $(CLIENT-OBJ) -o client

server: $(SERVER-OBJ)
	gcc $(SERVER-OBJ) -o server

clean:
	\rm -f client server $(CLIENT-OBJ) $(SERVER-OBJ) core
