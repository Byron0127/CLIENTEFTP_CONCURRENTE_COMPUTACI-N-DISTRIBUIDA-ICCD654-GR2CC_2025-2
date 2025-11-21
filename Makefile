# makefile para mi proyecto FTP
CC = gcc
CFLAGS = -g
OBJ = connectsock.o connectTCP.o errexit.o OrtizBy-clienteFTP.o
EXEC = OrtizBy-clienteFTP

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJ)

clean:
	rm -f *.o $(EXEC)