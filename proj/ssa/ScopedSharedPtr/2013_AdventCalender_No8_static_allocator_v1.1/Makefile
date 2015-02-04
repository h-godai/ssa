#DEBUG=1
ifneq ($(wildcard /usr/bin/clang), )
#ifeq ($(wildcard /vmlinuz), )
CLANG=1
#endif
endif

ifdef CLANG
CC = clang
CPP = clang++
CPPFLAGS = -stdlib=libc++
else
CC = gcc
CPP = g++
endif


ROOTDIR=../..
SRCS = $(shell ls *.cpp)
TARGETS = $(basename $(SRCS))
OBJOUTDIR = .
INCL = -I.
LIBTSSRC=
HEADERS= $(shell ls *.hpp)
LINKLIBS = 
CFLAGS += -DLINUX -Wall
ifdef DEBUG
CFLAGS += -g -DDEBUG -D_DEBUG $(INCL)
else
CFLAGS += -g -O3 $(INCL)
endif
CPPFLAGS +=  -std=c++11
CPP += $(CPPFLAGS)
LDFLAGS = -g


%.o : %.cpp
	$(CPP) $(CFLAGS) -c -o $@ $< $(LINKOBJS) $(LINKLIBS)

% : %.o
	$(CPP) $(LDFLAGS) -o $@ $< $(LINKOBJS) $(LINKLIBS)


run: $(TARGETS)
	./StaticMapTest	

all: $(TARGETS) makefile.dep  $(LINKOBJS) $(LINKLIBS)


clean:
	rm -rf $(TARGETS) $(LINKOBJS) $(addsuffix .dSYM, $(TARGETS)) *.hpp~ *.cpp~ *.o Makefile~ makefile.dep

makefile.dep: Makefile $(SRCS) $(HEADERS)
	$(CPP) -MM $(CFLAGS) $(SRCS) > makefile.dep

depend: makefile.dep


$(OBJOUTDIR):
	mkdir -p $(OBJOUTDIR)

commit:
	cd ..;svn commit -m "ScopedSharedPtr for linux"
	cd $(ROOTDIR)/libTS; svn commit -m "libTS"

update:
	cd ..;svn update
	cd $(ROOTDIR)/libTS; svn update

alltest:
	./$(TARGET1)


-include makefile.dep

