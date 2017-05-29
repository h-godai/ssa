# pthread_linux  Makefile
DEBUG=1
#RELEASE

ifdef RELEASE
endif

TARGETROOT=../..
TARGETBIN=../../../../../bin/sol10gcc
TARGETNAME=tslThread_gcc64.a
PROJROOT=../..
TARGETOBJ=$(TARGETBIN)/$(TARGETNAME)

OBJOUTDIR=./obj

CPP=g++ -m64
CC=gcc -m64
AR=gar cru

#src to link.
SRCDIR = $(PROJROOT)
SRCFILES = ext_linux/pthread_linux.cpp base/ThreadBase.cpp
INCLUDEDIR=$(PROJROOT) -I.


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
LDFLAGS = -lpthread -ldl

ifeq ($(RELEASE),yes)
	CFLAGS= -O2 -D__RELEASE__ -D__LINUX__  -I$(INCLUDEDIR)
else
	CFLAGS= -O2 -Wall $(DEBUGFLAG) -D__LINUX__  -I$(INCLUDEDIR)
endif

############ rules ############

all: $(OBJOUTDIR) $(TARGETOBJ)


list:
	echo $(SRCS)
	echo $(OBJS)
	echo $(dir $(SRCS))

$(TARGETOBJ): $(OBJOUTDIR) $(OBJS) $(TARGETBIN)
	$(AR) $(TARGETOBJ) $(ABSOBJS)

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

