all: server client

server: tserver.c
	gcc tserver.c -o tserver

client: tclient.c
	gcc tclient.c -o tclient

clean:
	rm tserver tclient
