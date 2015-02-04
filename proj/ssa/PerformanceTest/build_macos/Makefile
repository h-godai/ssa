#DEBUG=1
CC = gcc
CPP = c++

TARGET = ./PerformanceTest
OBJOUTDIR = .
#OBJOUTDIR = ./obj
INCL = -I. -I.. -I../../..
LIBTS=../../../libTS
LIBTSSRC=$(LIBTS)/PlatformDependent/Timer/MeasureTime.cpp \
	 $(LIBTS)/math/piml/trigonometric.cpp \
         $(LIBTS)/math/piml/trigonometric_table.cpp
SRCS = PerformanceTest.cpp $(shell ls ../*.cpp) $(LIBTSSRC) $(shell ls ../*.c)
OBJS = $(addsuffix .o,$(basename $(notdir $(SRCS))))
LINKOBJS = $(addprefix $(OBJOUTDIR)/,$(OBJS))
LINKLIBS = -L/usr/local/lib -lboost_system -lc++ -lboost_filesystem -lboost_iostreams
ifdef DEBUG
CFLAGS = -DDEBUG -D_DEBUG $(INCL)
else
CFLAGS = -O3 $(INCL)
endif
CPPFLAGS =  -std=c++11 -stdlib=libc++

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

all: $(OBJOUTDIR) $(TARGET) makefile.dep  $(LINKOBJS)

run: $(TARGET) $(OBJOUTDIR) 
	$(TARGET) -name 024


$(TARGET): $(OBJS)
	$(CPP) $(CFLAGS) -o $(TARGET) $(LINKOBJS) $(LINKLIBS)

clean:
	rm -rf $(TARGET)  $(LINKOBJS)

makefile.dep: Makefile $(SRCS)
	$(CPP) -MM $(CFLAGS) $(SRCS) > makefile.dep

depend:
	$(CPP) -MM $(CFLAGS) $(SRCS) > makefile.dep

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

