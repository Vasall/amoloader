TARGET := libamoloader.a

CC := gcc
CPPFLAGS := -I.
CFLAGS := -Wall -Wextra -std=c89 -ansi -pedantic

all: $(TARGET)

$(TARGET): amoloader.o
	ar rcs $(TARGET) amoloader.o

amoloader.o: amoloader.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c amoloader.c

check:
	$(CC) $(CPPFLAGS) $(CFLAGS) -o tests/test tests/test.c -L. -lamoloader
	./tests/test

clean:
	-rm amoloader.o $(TARGET) tests/test