CC = gcc
CFLAGS  = -Wall  -D_REENTRANT -lpthread
TARGET = gerador parque
DEPS = Gerador.c Parque.c util.h


all: bin/gerador bin/parque


bin/gerador:	src/Gerador.c src/util.h
		$(CC)  src/Gerador.c src/util.h -o bin/gerador $(CFLAGS)

bin/parque:	src/Parque.c src/util.h
		$(CC)  src/Parque.c src/util.h -o bin/parque $(CFLAGS)


clean : $(RM) bin/*
