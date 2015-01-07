###################################################################################################
ifneq (,)
echo This makefile requires GNU Make.
endif

###################################################################################################
export DEBUG

CFLAGS        = -DOS_LINUX=1 -Wall -Wno-unused-result
CPPFLAGS      =
LINKFLAGS	  = -fmax-errors=20

ifeq ($(DEBUG),1)
DEBUG_POSTFIX = _debug
CFLAGS        += -g -DDEBUG=1 -D_DEBUG=1
else
DEBUG_POSTFIX =
CFLAGS        += -Os -DNDEBUG=1 -D_NDEBUG=1 -fdata-sections -ffunction-sections
CPPFLAGS      += -fno-rtti
LINKFLAGS     += -Wl,-O,-s,--gc-sections
endif

OBJSDIR       = objs$(DEBUG_POSTFIX)

CC            = gcc
CPP           = g++
LINKER	      = g++
RM            = rm
EXE           = gpio.exe


C_FILES       = $(wildcard *.c) $(wildcard *.C)
CPP_FILES     = $(wildcard *.cpp) $(wildcard *.CPP)

SRCS          = $(C_FILES) $(CPP_FILES)
OBJS          = $(sort $(patsubst %.c, $(OBJSDIR)/%.o, $(C_FILES)) $(patsubst %.cpp, $(OBJSDIR)/%.o, $(CPP_FILES)))

###################################################################################################
$(OBJSDIR)/%.o: %.c
	@echo "$(PROJ): $<"
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJSDIR)/%.o: %.C
	@echo "$(PROJ): $<"
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJSDIR)/%.o: %.cpp
	@echo "$(PROJ): $<"
	@$(CPP) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(OBJSDIR)/%.o: %.CPP
	@echo "$(PROJ): $<"
	@$(CPP) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

###################################################################################################
.PHONY : all
all : .depend $(EXE)

.PHONY : depend
depend : .depend

.PHONY : clean
clean : 
	@echo "$(PROJ): Cleaning..."
	@-$(RM) -f .depend $(EXE)
	@-$(RM) -rf $(OBJSDIR)

###################################################################################################
#
#  Note when linking:
#  Objects need to be in order of most dependent to least.
#  Therefore, external libs at the end like math, c_lib, before is package libs,
#  before that is our libs then earliest is source obj files.
#
###################################################################################################

$(EXE) : $(FULLPATHLIBS) $(OBJSDIR) $(OBJS)
	@echo "$(PROJ): Linking..." 
	@$(LINKER) $(LINKFLAGS) $(OBJS) $(LIBS) $(FULLPATHLIBS) -o $(EXE)
	@echo "____________________________________________________________________" 

$(OBJSDIR) :
	@-mkdir $(OBJSDIR)

###################################################################################################
.depend : cmd = gcc -MM -MF depend $(var); echo -n $(OBJSDIR)/ >> .depend; cat depend >> .depend; echo "" >> .depend;
.depend :
	@echo "$(PROJ): deps..."
	@$(foreach var, $(C_FILES), $(cmd))
	@rm -f depend
	@$(foreach var, $(CPP_FILES), $(cmd))
	@rm -f depend
 
include .depend
###################################################################################################

