CC = gcc
CFLAGS = $(CF) -g -Wall -Werror -Wpedantic -Wwrite-strings -Wstack-usage=1024 -Wfloat-equal -Waggregate-return -Winline
OUT = hangman 
OBJ = hangman.o

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OUT): $(OBJ)
	$(CC) -o $@ $(OBJ)

clean:
	@rm -rf *.o

