# pthread_linux  Makefile
DEBUG=1
#RELEASE

ifdef RELEASE
endif

TARGETROOT=..
TARGETBIN=bin
TARGETNAME=unittest
PROJROOT=..
TARGETOBJ=$(TARGETBIN)/$(TARGETNAME)
LIBROOT=../../..

OBJOUTDIR=./obj

CPP=g++
CC=gcc
AR=ar cru

#src to link.
SRCDIR = $(PROJROOT)
SRCFILES = unittest.cpp
LIBFILES = $(LIBROOT)/../../../bin/linux/tslThread_linux.a
INCLUDEDIR=$(PROJROOT) -I. -I $(LIBROOT)


ABSSRCS = $(addprefix $(SRCDIR)/,$(SRCFILES))

SRCS=$(ABSSRCS)

#list the objects to link.
OBJS = $(addsuffix .o,$(basename $(notdir $(SRCFILES))))
ABSOBJS = $(addprefix $(OBJOUTDIR)/,$(OBJS))


#ABSOBJS=$(ABSOBJS)

vpath %.c $(dir $(SRCS))
vpath %.cpp $(dir $(SRCS))
vpath %.o $(OBJOUTDIR)


DEBUGFLAG+= -D_DEBUG
DEBUGFLAG+= -g
LDFLAGS = -lpthread # -ldl

ifeq ($(RELEASE),yes)
	CFLAGS= -O2 -D__RELEASE__ -D__LINUX__  -I$(INCLUDEDIR)
else
	CFLAGS= -O2 -Wall $(DEBUGFLAG) -D__LINUX__  -I$(INCLUDEDIR)
endif

############ rules ############

all: libs $(OBJOUTDIR) $(TARGETOBJ)


list:
	echo $(SRCS)
	echo $(OBJS)
	echo $(dir $(SRCS))

$(TARGETOBJ): $(OBJOUTDIR) $(OBJS) $(TARGETBIN) $(LIBFILES)
	$(CPP) $(LDFLAGS) -o $(TARGETOBJ) $(ABSOBJS) $(LIBFILES) 

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $(OBJOUTDIR)/$@

%.o : %.cpp
	$(CPP) $(CFLAGS) -c $< -o $(OBJOUTDIR)/$@

$(OBJOUTDIR):
	mkdir -p $(OBJOUTDIR)

$(TARGETBIN):
	mkdir -p $(TARGETBIN)

depend: $(SRCS)
	$(CC) -MM $(CFLAGS) $(SRCS) > makefile.dep

### useful commands ###

clean :
	-rm -f $(ABSOBJS) $(PACKAGE) makefile.dep *~ core.* 

.phony: verbose all

-include makefile.dep

libs:
	cd ../../../build/linux; make
