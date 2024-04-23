CC = gcc
CFLAGS = -Wall -g
OBJ = mssv.o
EXEC = mssv

$(EXEC) : $(OBJ)
	$(CC) $(OBJ) -g -o $(EXEC)

mssv.o: mssv.c definitions.h
	$(CC) $(CFLAGS) -c mssv.c

clean:
	rm -f $(EXEC) $(OBJ)