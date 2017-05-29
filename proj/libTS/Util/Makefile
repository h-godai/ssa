


CFLAGS = -I. -I.. -I../.. -g -Wall
ifdef DEBUG
CFLAGS += -DDEBUG
else
CFLAGS += -O2
endif

#SRCS = $(shell ls *.cpp)
SRCS = ustring.cpp unittest.cpp
OBJS = ustring.o util.o unittest.o
GPP=g++


%.o: %.cpp
	$(GPP) $(CFLAGS) -o $@ -c $<



all: .depend $(OBJS)
	make -C unittest



clean:
	rm -f *.o *~
	make -C unittest clean


.depend:
	$(GPP) $(CFLAGS) -MM $(SRCS) > .depend

include .depend
