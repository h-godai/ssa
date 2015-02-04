#DEBUG=1
ifdef CLANG
CC = clang
CPP = clang++
else
CC = gcc
CPP = g++
endif



TARGET = ./PerformanceTest
OBJOUTDIR = .
#OBJOUTDIR = ./obj
LIBTS=../../../libTS
INCL = -I. -I.. -I../../..
LIBTSSRC=$(LIBTS)/PlatformDependent/Timer/MeasureTime.cpp \
	 $(LIBTS)/math/piml/trigonometric.cpp \
         $(LIBTS)/math/piml/trigonometric_table.cpp
SRCS = $(shell ls ../*.cpp) PerformanceTest.cpp  $(shell ls ../*.c)  $(LIBTSSRC)
OBJS = $(addsuffix .o,$(basename $(notdir $(SRCS))))
LINKOBJS = $(addprefix $(OBJOUTDIR)/,$(OBJS))
LINKLIBS = -lboost_system
CFLAGS = -DLINUX -Wall
ifdef DEBUG
CFLAGS += -g -DDEBUG -D_DEBUG $(INCL)
else
CFLAGS += -g -O3 $(INCL)
endif
CPPFLAGS =  -std=c++11
CPP += $(CPPFLAGS)

# ../TestMain.cpp


%.o : %.cpp
	$(CPP) $(CFLAGS) -c $< -o $(OBJOUTDIR)/$@

%.o : ../%.cpp
	$(CPP) $(CFLAGS) -c $< -o $(OBJOUTDIR)/$@

%.o : ../%.c
	$(CC) $(CFLAGS) -c $< -o $(OBJOUTDIR)/$@

%.o : ../../../libTS/PlatformDependent/Timer/%.cpp
	$(CPP) $(CFLAGS) -c $< -o $(OBJOUTDIR)/$@

%.o : ../../../libTS/math/piml/%.cpp
	$(CPP) $(CFLAGS) -c $< -o $(OBJOUTDIR)/$@

all: $(OBJOUTDIR) $(TARGET) makefile.dep  $(LINKOBJS) $(LINKLIBS)

run: $(TARGET) $(OBJOUTDIR) 
	$(TARGET) -name 020


$(TARGET): $(OBJS)
	$(CPP) $(CFLAGS) -o $(TARGET) $(LINKOBJS) $(LINKLIBS)

clean:
	rm -rf $(TARGET)  $(LINKOBJS)

makefile.dep: Makefile $(SRCS)
	$(CPP) -MM $(CFLAGS) $(SRCS) > makefile.dep

depend: makefile.dep


$(OBJOUTDIR):
	mkdir -p $(OBJOUTDIR)

commit:
	cd ..;svn commit -m "PerformanceTest for linux"
	cd ../../../libTS; svn commit -m "libTS"

update:
	cd ..;svn update
	cd ../../../libTS; svn update

alltest:
	rm -f result.txt
	./PerformanceTest -name 020
	./PerformanceTest -name 021
	./PerformanceTest -name 022
	./PerformanceTest -name 023
	./PerformanceTest -name 024
	./PerformanceTest -name 025
	./PerformanceTest -name 026
	./PerformanceTest -name 027
	./PerformanceTest -name 028
	./PerformanceTest -name 029
	./PerformanceTest -name 030
	./PerformanceTest -name 031


-include makefile.dep

