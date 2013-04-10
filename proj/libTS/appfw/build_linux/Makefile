# appfw  Makefile for linux
DEBUG=1
#RELEASE_i386=1
#CROSS_BUILD_MIPS=yes

ifdef DEBUG
	RELEASE=no
else
ifdef RELEASE
	RELEASE=yes
else
	RELEASE=yes
endif
endif

TARGETROOT=.
TARGETBIN=$(TARGETROOT)/bin
TARGETNAME=libappfw.a
PROJROOT=..
LIBTSDIR=../..
LIBROOT=../../..
OBJOUTDIR=./obj
CPP=g++
CC=gcc

TARGETOBJ = $(TARGETBIN)/$(TARGETNAME)

#src to link.
LIBTSSRCS = Util/Logger.cpp PlatformDependent/Socket/TSSocket.cpp
TARGETSRCS = ext_linux/glview.cpp ext_linux/render_control.cpp

ABSSRCS = $(addprefix $(PROJROOT)/,$(TARGETSRCS))
LIBTSABSSRCS = $(addprefix $(LIBTSDIR)/,$(LIBTSSRCS))

SRCS=$(ABSSRCS) $(LIBTSABSSRCS)

#list the objects to link.
OBJS = $(addsuffix .o,$(basename $(notdir $(SRCS))))
TGABSOBJS = $(addprefix $(OBJOUTDIR)/,$(OBJS))
LIBTSOBJS = $(addsuffix .o, $(basename $(notdir $(LIBTSSRCS))))
LIBTSABSOBJS = $(addprefix $(OBJOUTDIR)/,$(LIBTSOBJS))

ABSOBJS=$(TGABSOBJS) $(LIBTSABSOBJS)

vpath %.c $(dir $(SRCS))
vpath %.cpp $(dir $(SRCS))
vpath %.o $(OBJOUTDIR)

#pg is for profiling.
DEBUGFLAG+= -D_DEBUG -DDEBUG
DEBUGFLAG+= -g
LDFLAGS = -lboost_thread-mt -lpng14 -lglut
INCLUDE =  -I$(PROJROOT)/ext_linux -I$(LIBROOT) -I$(LIBTSDIR)
ifeq ($(RELEASE),yes)
	CFLAGS= -O2 -DRELEASE -D__RELEASE__ -D__LINUX__ -DLINUX $(INCLUDE)
else
	CFLAGS= -O2 -Wall $(DEBUGFLAG) -D__LINUX__ -DLINUX  $(INCLUDE)
endif

############ rules ############

all: $(OBJOUTDIR) $(TARGETOBJ) 

run: all

list:
	echo $(SRCS)
	echo $(OBJS)
	echo $(LIBTSOBJS)
	echo $(LIBTSABSOBJS)
	echo $(dir $(SRCS))

$(TARGETOBJ): $(OBJOUTDIR) $(OBJS) $(LIBTSOBJS)
	ar -rc $(TARGETOBJ) $(ABSOBJS)


%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $(OBJOUTDIR)/$@

%.o : %.cpp
	$(CPP) $(CFLAGS) -c $< -o $(OBJOUTDIR)/$@

$(OBJOUTDIR):
	mkdir -p $(OBJOUTDIR)

depend: $(SRCS)
	$(CC) -MM $(CFLAGS) $(SRCS) > makefile.dep

### useful commands ###

.phony: clean
clean:
	-rm -f $(ABSOBJS) makefile.dep *~ core.* $(TARGETOBJ)

.phony: verbose all

-include makefile.dep



usrlib: $(USRLIBDIR)

$(USRLIBDIR):
	mkdir -p $(USRLIBDIR)



$(TARGETBIN):
	mkdir -p $(TARGETBIN)

install: $(TARGETETC) $(TARGETBIN) usrlib
	cp -p $(PROGRAM) $(TARGETBIN)
	cp -p $(SCRIPT) $(TARGETBIN)
	cp -p $(CFGFILE) $(TARGETETC)
	cp -p $(DHCPDCONF) $(TARGETBIN)
	cp -p $(SETUPSH) $(TARGETBIN)
	cp -p $(SETNAFID) $(TARGETBIN)



