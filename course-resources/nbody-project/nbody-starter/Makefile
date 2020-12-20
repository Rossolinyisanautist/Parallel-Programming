CFLAGS=-DDEBUG
LDLIBS=-lm

.PHONY: all
all: nbody nbody-mpi

nbody: nbody.c
	cc ${CFLAGS} -o $@ $< ${LDLIBS}

nbody-mpi: nbody-mpi.c
	mpicc ${CFLAGS} -o $@ $< ${LDLIBS}

.PHONY: clean
clean:
	rm -rf nbody nbody-mpi

