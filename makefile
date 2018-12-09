CC = gcc
CFLAGS = -Wall 
TARGET = oss
TARGET1 = user
OBJ = main.o 
OBJ1 = user.o
LIBS ?= -lpthread
all: $(TARGET) $(TARGET1)

oss: $(OBJ)
	$(CC) $(CFLAGS) -o oss $(OBJ)
main.o : main.c globals.h
	$(CC) $(CFLAGS) -c main.c
user: $(OBJ1)
	$(CC) $(CFLAGS) $(LIBS) -o user $(OBJ1)
user.o : user.c globals.h
	$(CC) $(CFLAGS) -c user.c
clean:
	/bin/rm -f *.o $(TARGET) $(TARGET1)
