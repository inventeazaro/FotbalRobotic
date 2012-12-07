CXX		= /usr/bin/g++
CC              = gcc

CCLIBS_default	= -lm
CCLIBS_BSD	= -lm -lc_r
CCLIBS_SunOS_4	= -lm
CCLIBS_SunOS_5	= -lm -L/usr/ucblib -lucb -lsocket -lnsl
CCLIBS_SunOS_5	= -lm -L/usr/ucblib -lsocket -lnsl
CCLIBS_OSF1		= -lm
CCLIBS_NEWS		= -lm
CCLIBS_IRIX		= -lm
#CCLIBS_Linux	= -lm -lbsd -lpthread
CCLIBS_Linux	= -lm
#SMURF: do I need to do something for thread in other OS's?
CXXLIBS			= $(CCLIBS_Linux)
#CXXLIBS			= $(CCLIBS_SunOS_4)
#CXXLIBS			= $(CCLIBS_SunOS_5)

OSFLAGS_default	= 
OSFLAGS_BSD	= 
OSFLAGS_SunOS_4	= -DSunOS4
OSFLAGS_SunOS_5	= -DSolaris -DMEMSET
OSFLAGS_OSF1	= -DOSF1
OSFLAGS_NEWS	= -DNewsOS
OSFLAGS_IRIX	= -DIRIX -D_BSD_SIGNALS
OSFLAGS_Linux	= -DLinux -D_REENTRANT
#SMURF: do I need to do something for thread in other OS's?
CXXFLAGS	= -g $(OSFLAGS_Linux)
#CXXFLAGS	= -Wall $(OSFLAGS_SunOS_4)
#CXXFLAGS	= -Wall -pedantic -fomit-frame-pointer -O3 $(OSFLAGS_SunOS_5)

#this is for lineker
#CXXFLAGS += -w -Wimplicit -Wreturn-type -Wswitch -Wcomment -Wformat -Wuninitialized -Wparentheses -Woverloaded-virtual
CXXFLAGS += -Wall

WD = $(shell pwd)

CXXFLAGS += -g -I$(WD)/../shared -I$(WD) 
#CXXFLAGS	+= -DNO_ACTION_LOG

EXEC	=	client

all: $(EXEC) quicksleep

NORM_OFILES =   netif.o client.o parse.o utils.o 
NORM_OFILES +=	MemOption.o MemPlayer.o MemPosition.o MemAction.o \
	        Memory.o 
NORM_OFILES +=       behave.o kick.o dribble.o
NORM_OFILES +=       test.o
NORM_SRCS	=	$(NORM_OFILES:%.o=%.C)

SHARED_OFILES =  geometry.o soccer_utils.o
SHARED_SRCS = $(SHARED_OFILES:%.o=$(WD)/../shared/%.C)

OFILES = $(NORM_OFILES) $(SHARED_OFILES) $(C45_OFILES)
SRCS += $(NORM_SRCS) $(SHARED_SRCS) $(C45_SRCS)

#These rules grab things from the shared and shared/planning dirs
#we have to have the first rule in case we have files in our dir which
#are the same name as shared ones
%.o : $(WD)/%.C
	$(CXX) -c $(CXXFLAGS) $(CXXINCLUDE) $<
%.o : $(WD)/../shared/planning/%.C
	$(CXX) -c $(CXXFLAGS) $(CXXINCLUDE) $<
%.o : $(WD)/../shared/%.C
	$(CXX) -c $(CXXFLAGS) $(CXXINCLUDE) $<
%.o : $(WD)/c4.5/%.C
	$(CXX) -c $(CXXFLAGS) $(CXXINCLUDE) $<



$(EXEC) : $(OFILES)
	$(CXX) -g $^ -o $@ $(CXXLIBS)

pureclient : $(OFILES)
	purify -collector=/usr/local/lib/gcc-lib/sparc-sun-sunos4.1.3_U1/2.7.2/ld $(CXX) -g $^ -o $@ $(CXXLIBS)

cln:
	echo cleaning up....
	/bin/rm -f $(OFILES) $(EXEC) quicksleep pureclient core

depend:
	$(RM) $(DEPEND)
	$(CXX) -M $(CXXFLAGS) $(SRCS) >> $(DEPEND)

DEPEND = Makefile.depend
ifeq ($(wildcard $(DEPEND)),$(DEPEND))
include $(DEPEND)
endif

quicksleep: $(WD)/quicksleep.C
	$(CC) -o quicksleep $(WD)/quicksleep.C
