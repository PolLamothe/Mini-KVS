CC = gcc
CFLAGS = -Wall -Wextra -g -O0 -fsanitize=address
# Add persistence.o to the OBJ list
OBJ = main.o hashage.o error.o persistence.o input.o
EXEC = mini-kvs

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

main.o: main.c hashage.h error.h input.h
hashage.o: hashage.c hashage.h error.h persistence.h
error.o: error.c error.h
input.o: input.c input.h error.h
persistence.o: persistence.c persistence.h error.h hashage.h

clean:
	rm -f $(OBJ) $(EXEC) *.db

.PHONY: all clean