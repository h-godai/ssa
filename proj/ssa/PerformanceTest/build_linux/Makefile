
CC = gcc
CPP = g++
TARGET = ./PerformanceTest
OBJOUTDIR = .
#OBJOUTDIR = ./obj
INCL = -I. -I.. -I../../..
SRCS = $(shell ls ../*.cpp) ../../../libTS/PlatformDependent/Timer/MeasureTime.cpp PerformanceTest.cpp  $(shell ls ../*.c)
OBJS = $(addsuffix .o,$(basename $(notdir $(SRCS))))
LINKOBJS = $(addprefix $(OBJOUTDIR)/,$(OBJS))

CFLAGS = -O3 $(INCL)
# ../TestMain.cpp


%.o : %.cpp
	$(CPP) $(CFLAGS) -c $< -o $(OBJOUTDIR)/$@

%.o : ../%.cpp
	$(CPP) $(CFLAGS) -c $< -o $(OBJOUTDIR)/$@

%.o : ../%.c
	$(CC) $(CFLAGS) -c $< -o $(OBJOUTDIR)/$@

%.o : ../../../libTS/PlatformDependent/Timer/%.cpp
	$(CPP) $(CFLAGS) -c $< -o $(OBJOUTDIR)/$@

all: $(OBJOUTDIR) $(TARGET) makefile.dep  $(LINKOBJS)

run: $(TARGET) $(OBJOUTDIR) 


$(TARGET): $(OBJS)
	g++ $(CFLAGS) -o $(TARGET) $(LINKOBJS)

clean:
	rm -rf $(TARGET)  $(LINKOBJS)

makefile.dep: Makefile $(SRCS)
	g++ -MM $(CFLAGS) $(SRCS) > makefile.dep

depend: makefile.dep


$(OBJOUTDIR):
	mkdir -p $(OBJOUTDIR)

commit:
	cd ..;svn commit -m "PerformanceTest for linux"
	cd ../../../libTS; svn commit -m "libTS"

update:
	cd ..;svn update
	cd ../../../libTS; svn update

-include makefile.dep

