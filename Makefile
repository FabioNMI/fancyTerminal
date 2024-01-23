CC       = gcc
OBJ      = pong.o fancyTerminal.o
LINKOBJ  = obj/pong.o obj/fancyTerminal.o obj/bars.o
BIN      = pong bars
CFLAGS   = -g3
RM       = rm -f

.PHONY: all clean clean-custom

all: makedir $(BIN) 

clean: clean-custom
	${RM} $(LINKOBJ) $(BIN)

bars: bars.o fancyTerminal.o
	$(CC) obj/bars.o obj/fancyTerminal.o -o bars

pong: pong.o fancyTerminal.o
	$(CC) obj/pong.o obj/fancyTerminal.o -o pong

bars.o: bars.c
	$(CC) -c bars.c -o obj/bars.o $(CFLAGS)

pong.o: pong.c
	$(CC) -c pong.c -o obj/pong.o $(CFLAGS)

fancyTerminal.o: fancyTerminal.c
	$(CC) -c fancyTerminal.c -o obj/fancyTerminal.o $(CFLAGS)

makedir:
	mkdir -p obj
