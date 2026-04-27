CC = gcc
CFLAGS = -Wall -Iinclude

SRC = src/main.c src/dfa.c src/enfa.c src/regex.c src/utils.c
OBJ = $(SRC:.c=.o)
TARGET = abdur.out

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) 
