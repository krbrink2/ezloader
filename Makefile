CC = gcc
CFLAGS = -g -Wall -Werror -lm -lGL -lGLU -lglut
TARGET = loadertest

default: loadertest

loadertest: loadertest.o ezloader.o
	$(CC) -o loadertest loadertest.o ezloader.o $(CFLAGS)

loadertest.o: loadertest.c ezloader.h
	$(CC) -c loadertest.c $(CFLAGS)

ezloader.o: ezloader.c ezloader.h
	$(CC) -c ezloader.c $(CFLAGS)

clean:
	$(RM) ezloader *.o *~
