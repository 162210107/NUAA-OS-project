CC=gcc
CFLAGS=-O
OBJS=main.o min.o max.o
TARGET=app 

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS)

min.o: min.c min.h
	$(CC) $(CFLAGS) -c min.c

max.o: max.c max.h min.h
	$(CC) $(CFLAGS) -c max.c

main.o: main.c max.h min.h
	$(CC) $(CFLAGS) -c main.c

clean: 
	rm -f $(TARGET) $(OBJS)
