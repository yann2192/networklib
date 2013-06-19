FLAGS = -Wall -pedantic -pipe
LIB = ../tmp
BIN = ../bin

all: node

node: node.c $(LIB)/sock.o $(LIB)/random.o $(LIB)/network.o $(LIB)/async.o $(LIB)/sock.o $(LIB)/thread.o $(LIB)/utils.o
	gcc $^ $(FLAGS) -o $(BIN)/$@ -lpthread
