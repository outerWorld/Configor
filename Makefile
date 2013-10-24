CC=g++
CFLAGS=-I./ -fPIC
LFLAGS=
AR=ar
AR_OPT=-rcs
gen_Libs=libconfigor.a libconfigor.so

libconfigor.a:configor.o
	${AR} ${AR_OPT} -o $@ $^ ${LFLAGS}

libconfigor.so:configor.o
	${CC} -shared -o $@ $^ ${LFLAGS}

%.o:%.cc
	${CC} -o $@ -c $< ${CFLAGS}

all:$(gen_Libs)

.PHONY:clean all

clean:
	rm -f *.o
	rm -f *.a *.so
