TARGET := libamoloader.a
PREFIX ?= /usr

CC := gcc
CPPFLAGS := -I.
CFLAGS := -Wall -Wextra -std=c89 -ansi -pedantic -flto

LIBNAME=$(subst .a,,$(subst lib,,$(TARGET)))

.PHONY: all
all: $(TARGET) amoloader.pc

$(TARGET): amoloader.o
	gcc-ar rcs $(TARGET) amoloader.o

amoloader.o: amoloader.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c amoloader.c

amoloader.pc: amoloader.pc.in
	echo "prefix=$(PREFIX)" > amoloader.pc
	echo "libname=$(LIBNAME)" >> amoloader.pc
	cat amoloader.pc.in >> amoloader.pc

.PHONY: check
check:
	$(CC) $(CPPFLAGS) $(CFLAGS) -o tests/test tests/test.c -L. -l$(LIBNAME)
	./tests/test

.PHONY: install
install:
	mkdir -p $(DESTDIR)$(PREFIX)/include
	mkdir -p $(DESTDIR)$(PREFIX)/lib
	mkdir -p $(DESTDIR)$(PREFIX)/lib/pkgconfig
	cp amoloader.h $(DESTDIR)$(PREFIX)/include
	cp $(TARGET) $(DESTDIR)$(PREFIX)/lib
	cp amoloader.pc $(DESTDIR)$(PREFIX)/lib/pkgconfig

.PHONY: uninstall
	rm -f $(DESTDIR)$(PREFIX)/include/amoloader.h
	rm -f $(DESTDIR)$(PREFIX)/lib/$(TARGET)
	rm -f $(DESTDIR)$(PREFIX)/lib/pkgconfig/amoloader.pc
	rmdir --ignore-fail-on-non-empty $(DESTDIR)$(PREFIX)/include
	rmdir --ignore-fail-on-non-empty $(DESTDIR)$(PREFIX)/lib/pkgconfig
	rmdir --ignore-fail-on-non-empty $(DESTDIR)$(PREFIX)/lib

.PHONY: clean
clean:
	-rm amoloader.o $(TARGET) amoloader.pc tests/test