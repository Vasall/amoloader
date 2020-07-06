TARGET := libamoloader.a
PREFIX ?= /usr

CC := gcc
CPPFLAGS := -I.
CFLAGS := -Wall -Wextra -std=c89 -ansi -pedantic

.PHONY: all
all: $(TARGET)

$(TARGET): amoloader.o
	ar rcs $(TARGET) amoloader.o

amoloader.o: amoloader.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c amoloader.c

.PHONY: check
check:
	$(CC) $(CPPFLAGS) $(CFLAGS) -o tests/test tests/test.c -L. -lamoloader
	./tests/test

.PHONY: install
install:
	mkdir -p $(DESTDIR)$(PREFIX)/include
	mkdir -p $(DESTDIR)$(PREFIX)/lib
	cp amoloader.h $(DESTDIR)$(PREFIX)/include
	cp $(TARGET) $(DESTDIR)$(PREFIX)/lib

.PHONY: uninstall
	rm -f $(DESTDIR)$(PREFIX)/include/amoloader.h
	rm -f $(DESTDIR)$(PREFIX)/lib/$(TARGET)
	rmdir --ignore-fail-on-non-empty $(DESTDIR)$(PREFIX)/include
	rmdir --ignore-fail-on-non-empty $(DESTDIR)$(PREFIX)/lib

.PHONY: clean
clean:
	-rm amoloader.o $(TARGET) tests/test