CC       = gcc
OBJ      = pong.o fancyTerminal.o
LINKOBJ  = obj/pong.o obj/fancyTerminal.o
BIN      = pong
CFLAGS   = -g3
RM       = rm -f

.PHONY: all clean clean-custom

all: makedir $(BIN) 

clean: clean-custom
	${RM} $(LINKOBJ) $(BIN)

pong: pong.o fancyTerminal.o
	$(CC) obj/pong.o obj/fancyTerminal.o -o output/pong

pong.o: pong.c
	$(CC) -c pong.c -o obj/pong.o $(CFLAGS)

fancyTerminal.o: ../../../src/fancyTerminal.cpp
	$(CC) -x c -c ../../../src/fancyTerminal.cpp -o obj/fancyTerminal.o $(CFLAGS)

makedir:
	mkdir -p obj
	mkdir -p output
