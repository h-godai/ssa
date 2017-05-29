# TSGraphics  Makefile
DEBUG=1
#RELEASE_i386=1
#CROSS_BUILD_MIPS=yes

ifdef DEBUG
	ENCRYPTION=yes
	RELEASE=no
	CROSS_BUILD_PPC=no
else
ifdef RELEASE_i386
	ENCRYPTION=yes
	RELEASE=yes
	CROSS_BUILD_PPC=no
else
	ENCRYPTION=yes
	RELEASE=yes
	CROSS_BUILD_PPC=yes
endif
endif

TARGETROOT=/usr/local/lib/TS
TARGETBIN=$(TARGETROOT)/bin
TARGETETC=$(TARGETROOT)/etc
PROJROOT=..
LIBTSDIR=../../TSCommon
OBJOUTDIR=./obj
CPP=g++
CC=gcc

#src to link.
LIBTSSRCS = StrTools.cpp STLtools.cpp DebugLog.cpp TSObject.cpp
TARGETSRCS = build_linux/main.cpp TSGL.cpp

ABSSRCS = $(addprefix $(PROJROOT)/,$(TARGETSRCS))
LIBTSABSSRCS = $(addprefix $(LIBTSDIR)/,$(LIBTSSRCS))

SRCS=$(ABSSRCS) #$(LIBTSABSSRCS)

#list the objects to link.
OBJS = $(addsuffix .o,$(basename $(notdir $(SRCS))))
TGABSOBJS = $(addprefix $(OBJOUTDIR)/,$(OBJS))
LIBTSOBJS = $(addsuffix .o, $(basename $(notdir $(LIBTSSRCS))))
LIBTSABSOBJS = $(addprefix $(OBJOUTDIR)/,$(LIBTSOBJS))

ABSOBJS=$(TGABSOBJS) $(LIBTSABSOBJS)
PROGRAM=TSGraphics

vpath %.c $(dir $(SRCS))
vpath %.cpp $(dir $(SRCS))
vpath %.o $(OBJOUTDIR)


#pg is for profiling.
DEBUGFLAG+= -D_DEBUG
DEBUGFLAG+= -g
LDFLAGS = -lpthread -ldl
INCLUDE =  -I$(PROJROOT)/ext_linux -I$(PROJROOT)/ext -I $(PROJROOT)/../TSCommon
ifeq ($(RELEASE),yes)
	CFLAGS= -O2 -D__RELEASE__ -D__LINUX__ $(INCLUDE)
else
	CFLAGS= -O2 -Wall $(DEBUGFLAG) -D__LINUX__  $(INCLUDE)
endif

############ rules ############

all: $(OBJOUTDIR) $(PROGRAM) 


list:
	echo $(SRCS)
	echo $(OBJS)
	echo $(LIBTSOBJS)
	echo $(LIBTSABSOBJS)
	echo $(dir $(SRCS))

$(PROGRAM): $(OBJOUTDIR) $(OBJS) $(LIBTSOBJS)
	$(CPP) $(LDFLAGS) -o $@ $(ABSOBJS)


%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $(OBJOUTDIR)/$@

%.o : %.cpp
	$(CPP) $(CFLAGS) -c $< -o $(OBJOUTDIR)/$@

$(OBJOUTDIR):
	mkdir -p $(OBJOUTDIR)

depend: $(SRCS)
	$(CC) -MM $(CFLAGS) $(SRCS) > makefile.dep

### useful commands ###

clean :
	-rm -f $(ABSOBJS) makefile.dep *~ core.* 

.phony: verbose all

-include makefile.dep



usrlib: $(USRLIBDIR)

$(USRLIBDIR):
	mkdir -p $(USRLIBDIR)


$(TARGETETC):
	mkdir -p $(TARGETETC)

$(TARGETBIN):
	mkdir -p $(TARGETBIN)

install: $(TARGETETC) $(TARGETBIN) usrlib
	cp -p $(PROGRAM) $(TARGETBIN)
	cp -p $(SCRIPT) $(TARGETBIN)
	cp -p $(CFGFILE) $(TARGETETC)
	cp -p $(DHCPDCONF) $(TARGETBIN)
	cp -p $(SETUPSH) $(TARGETBIN)
	cp -p $(SETNAFID) $(TARGETBIN)



