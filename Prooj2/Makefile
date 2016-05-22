CC = gcc
CFLAGS  = -Wall  -D_REENTRANT -lpthread
TARGET = gerador parque
DEPS = Gerador.c Parque.c util.h
all: bin/gerador bin/parque


bin/gerador:	src/Gerador.c
		$(CC)  src/Gerador.c -o bin/gerador $(CFLAGS)

bin/parque:	src/Parque.c
		$(CC)  src/Parque.c -o bin/parque $(CFLAGS)


clean : $(RM) bin/*
