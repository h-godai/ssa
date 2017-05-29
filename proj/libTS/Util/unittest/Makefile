
ROOTDIR = ../../..
CFLAGS = -I. -I$(ROOTDIR) -g -Wall
CFLAGS += -DDEBUG -DUNITTEST


SRCS = unittest_linux.cpp ../ustring.cpp ../unittest.cpp ../ustring_test.cpp
OBJS = $(notdir $(SRCS:%.cpp=%.o))
GPP=g++


%.o: %.cpp
	$(GPP) $(CFLAGS)  -c $<

%.o: ../%.cpp
	$(GPP) $(CFLAGS)  -c $<


all: .depend $(OBJS)
	$(GPP) -o unittest $(OBJS)
	./unittest

clean:
	rm -f *.o *~ unittest.log


.depend:
	$(GPP) $(CFLAGS) -MM $(SRCS) > .depend

include .depend



