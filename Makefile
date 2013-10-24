CC=g++
DEBUG=
CFLAGS=-I./ -fPIC $(DEBUG)
LFLAGS=-lpthread
AR=ar
AR_OPT=-rcs
gen_LIBS=libconfigor.a libconfigor.so
test_BINS=test_configor

test_configor:test_configor.o libconfigor.a
	${CC} -o $@ $^ ${LFLAGS}

libconfigor.a:configor.o
	${AR} ${AR_OPT} -o $@ $^

libconfigor.so:configor.o
	${CC} -shared -o $@ $^ ${LFLAGS}

%.o:%.cc
	${CC} -o $@ -c $< ${CFLAGS}

all:$(gen_LIBS) $(test_BINS)

.PHONY:clean all

clean:
	rm -f *.o
	rm -f $(gen_LIBS)
	rm -f $(test_BINS)
