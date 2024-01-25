CC       = gcc
OBJ      = bars.o fancyTerminal.o
LINKOBJ  = obj/bars.o obj/fancyTerminal.o
BIN      = bars
CFLAGS   = -g3
RM       = rm -f

.PHONY: all clean clean-custom

all: makedir $(BIN) 

clean: clean-custom
	${RM} $(LINKOBJ) $(BIN)

bars: bars.o fancyTerminal.o
	$(CC) obj/bars.o obj/fancyTerminal.o -o output/bars

bars.o: bars.c
	$(CC) -c bars.c -o obj/bars.o $(CFLAGS)

fancyTerminal.o: ../../../src/fancyTerminal.cpp
	$(CC) -x c -c ../../../src/fancyTerminal.cpp -o obj/fancyTerminal.o $(CFLAGS)

makedir:
	mkdir -p obj
	mkdir -p output
