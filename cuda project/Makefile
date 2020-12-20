CFLAGS +=
LDLIBS += -lm

.PHONY: all
all: raytracer raytracer-cuda

raytracer: raytracer.c
	gcc ${CFLAGS} -o $@ $< ${LDLIBS}

raytracer-cuda: raytracer.cu
	nvcc ${CFLAGS} -o $@ $< ${LDLIBS}

.PHONY: clean
clean:
	rm -rf raytracer raytracer-cuda
