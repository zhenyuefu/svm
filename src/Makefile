

CC = gcc
CFLAGS = -g

SOURCES = value.h value.c varray.h varray.c env.h env.c frame.h frame.c vm.h vm.c prim.c gc.h gc.c gc_mark.c bytecode.h bytecode.c main.c
OBJECTS = constants.o value.o varray.o env.o frame.o prim.o vm.o gc_mark.o gc.o bytecode.o main.o

CTOPDIR = ../../scompiler
CTOP = $(CTOPDIR)/scompiler

all: constants main

constants: constants.h constants.c
	echo "Constants generated"

constants.h: prepare

constants.c: prepare

prepare:
	$(CTOP) --gen-vm-consts

main : $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o svm

%.o : %.c
	$(CC) $(CFLAGS) -c $<

docs:
	doxygen

archive: Makefile $(SOURCES)
	tar cvzf svm.tar.gz Makefile $(SOURCES) 

clean:
	rm -f $(OBJECTS)
	rm -f *~

cleanall: clean
	rm -f constants.h
	rm -f constants.c
	rm -f svm
	rm -rf apidoc
