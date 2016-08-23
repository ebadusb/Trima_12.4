#---------------------------------------------------------------------
# The following variables must be defined before including this file.
#---------------------------------------------------------------------

# Got libtool? Blank otherwise
LIBTOOL_CC := libtool --mode=compile
LIBTOOL_LN := libtool --mode=link

# Name of build target.
TARGET := win-predict.exe

#Trima source - for include files
TRIMA_SRC_DIR := /trima/v60

#Predictor base
PREDICTOR_BASE_DIR := $(TRIMA_SRC_DIR)/engr_tools/predictor

#Predictor source
PREDICTOR_SRC_DIR := $(PREDICTOR_BASE_DIR)/src

#Log Convert files             
LOG_CONVERT_DIR := $(PREDICTOR_SRC_DIR)/dlog_read

# Source files to build target from.
BUILD_FILES := win-predict.cpp \
	simulate.cpp \
	analyze.cpp

	
#---------------------------------------------------------------------
# Optional configuration flags.
#---------------------------------------------------------------------

# Build target type: [bin] or [lib].
ifndef BUILD_TYPE
  BUILD_TYPE := bin 
endif

# Build configuration [release] or [debug] or [profile].
ifndef BUILD_CONFIG
  BUILD_CONFIG := debug
endif

# Root directory for build, all paths are relative to this dir.
ifndef BUILD_ROOT
  BUILD_ROOT := .
endif

# Directory for build output (intermediate + target).
ifndef BUILD_OUTPUT
  BUILD_OUTPUT := $(BUILD_ROOT)/$(BUILD_CONFIG)
endif

# Additional source directories (delimited with spaces).
ifndef BUILD_VPATH
  BUILD_VPATH := .
endif

# Additional directories to search for includes.
ifndef BUILD_INCLUDE
  BUILD_INCLUDE := -I../include -I../include/common -I../include/vxworks -I$(LOG_CONVERT_DIR) -I$(TRIMA_SRC_DIR)/trima_common -I$(TRIMA_SRC_DIR)/proc/algorithm -I$(TRIMA_SRC_DIR)/proc/interface -I$(TRIMA_SRC_DIR)/cds -I$(TRIMA_SRC_DIR)/proc/predict
endif

# Additional libraries to link with.
ifndef BUILD_LIBS
  ifeq ($(BUILD_CONFIG), debug)
    BUILD_LIBS := -lpredict60_d 
  endif
  ifeq ($(BUILD_CONFIG), release)
    BUILD_LIBS := -lpredict60
  endif
endif

# Additional search paths for libraries.
ifndef BUILD_LIBPATH
  BUILD_LIBPATH := -Ld:/cygwin/usr/local/lib 
endif

# Custom compiler flags.
ifndef BUILD_CFLAGS
  BUILD_CFLAGS := -D__WIN32__ -DWIN32 -DCPU=SIMNT -D_NONTRIMA 
endif

# Custom linker flags.
ifndef BUILD_LFLAGS
  BUILD_LFLAGS :=
endif

# Custom warning flags (defaults to pedantic mode).
ifndef BUILD_WFLAGS
  BUILD_WFLAGS := -W -Wall 
endif

# Compile C sources using specific standard: [c89] or [c99] or [gnu99].
ifndef BUILD_USING_C_STD
  BUILD_USING_C_STD := c99
endif

# Compile C++ sources using specific standard: [c++98] or [gnu++98]
ifndef BUILD_USING_CPP_STD
  BUILD_USING_CPP_STD := gnu++98
endif

#---------------------------------------------------------------------
# Generic GCC build configurations: [release] or [debug] or [profile].
#---------------------------------------------------------------------

ifeq ($(BUILD_CONFIG),debug)
  BUILD_CFLAGS += -pipe -g -DPREDICT_DEBUG
  BUILD_LFLAGS += -pipe -g
endif

ifeq ($(BUILD_CONFIG),release)
  BUILD_CFLAGS += -pipe -O2 -D NDEBUG
  BUILD_LFLAGS += -pipe -s 
endif

ifeq ($(BUILD_CONFIG),profile)
  BUILD_CFLAGS += -pipe -pg -O2 -D NDEBUG
  BUILD_LFLAGS += -pipe -pg
endif

#---------------------------------------------------------------------
# Generic GCC build rules.
#---------------------------------------------------------------------

# Disable implicit suffix rules.
.SUFFIXES:

# Configure class-based search paths.
vpath
vpath %.c    $(BUILD_VPATH)
vpath %.cpp  $(BUILD_VPATH)
vpath %.o    $(BUILD_OUTPUT)
vpath %.d    $(BUILD_OUTPUT)

# Filter out C object files we know how to build.
BUILD_C_OBJS := $(addsuffix .lo, $(basename $(filter %.c, $(BUILD_FILES))))

# Filter out C++ object files we know how to build.
BUILD_CPP_OBJS := $(addsuffix .lo, $(basename $(filter %.cpp, $(BUILD_FILES))))

# Append build directory path to object files.
OBJS := $(BUILD_C_OBJS:%.lo=$(BUILD_OUTPUT)/%.lo) $(BUILD_CPP_OBJS:%.lo=$(BUILD_OUTPUT)/%.lo)

all: $(BUILD_OUTPUT)/$(TARGET)

# Building a regular executable
ifeq ($(strip $(BUILD_TYPE)),bin)
$(BUILD_OUTPUT)/$(TARGET): $(OBJS)
	@echo -- Linking executable $(notdir $@)...
	@mkdir -p $(BUILD_OUTPUT)
	@echo libtool --mode=link g++ $(BUILD_WFLAGS) $(BUILD_LIBPATH) $(BUILD_LFLAGS) -o $@ $(OBJS) $(BUILD_LIBS)
	@libtool --mode=link g++ $(BUILD_WFLAGS) -L. $(BUILD_LIBPATH) $(BUILD_LFLAGS) -o $@ $(OBJS) $(BUILD_LIBS)
endif

# Build static library
ifeq ($(strip $(BUILD_TYPE)),lib)
$(BUILD_OUTPUT)/$(TARGET): $(OBJS)
	@echo -- Linking static library $(notdir $@)...
	@mkdir -p $(dir $@)
	#@ar cr $@ $(OBJS)
	@libtool --mode=link $@ $(OBJS)
endif

# Compile C source
$(BUILD_OUTPUT)/%.lo: %.c
	@echo $(notdir $<)
	@mkdir -p $(dir $@)
	@echo $(LIBTOOL_CC) gcc -MMD -std=$(BUILD_USING_C_STD) $(BUILD_CFLAGS) $(BUILD_WFLAGS) -I. $(BUILD_INCLUDE) -c $< -o $@ 
	@$(LIBTOOL_CC) gcc -MMD -std=$(BUILD_USING_C_STD) $(BUILD_CFLAGS) $(BUILD_WFLAGS) -I. $(BUILD_INCLUDE) -c $< -o $@ 

# Compile C++ source
$(BUILD_OUTPUT)/%.lo: %.cpp
	@echo $(notdir $<)
	@mkdir -p $(dir $@)
	@echo $(LIBTOOL_CC) g++ -MMD -std=$(BUILD_USING_CPP_STD) $(BUILD_CFLAGS) -fpermissive $(BUILD_WFLAGS) -I. $(BUILD_INCLUDE) -c $< -o $@ 
	@$(LIBTOOL_CC) g++ -MMD -std=$(BUILD_USING_CPP_STD) $(BUILD_CFLAGS) -fpermissive $(BUILD_WFLAGS) -I. $(BUILD_INCLUDE) -c $< -o $@ 

#---------------------------------------------------------------------
# Build tags database for Emacs. 
#--------------------------------------------------------------------

.PHONY: tags
tags:
	@echo -- Updating tags database...
	@gtags

#---------------------------------------------------------------------
# Execute program (max 8 arguments).
#---------------------------------------------------------------------

run: all
	@echo -- Executing $(BUILD_OUTPUT)/$(TARGET) \
		$(ARG1) $(ARG2) $(ARG3) $(ARG4) $(ARG5) $(ARG6) $(ARG7) $(ARG8)
	@$(BUILD_OUTPUT)/$(TARGET) \
		$(ARG1) $(ARG2) $(ARG3) $(ARG4) $(ARG5) $(ARG6) $(ARG7) $(ARG8)

#---------------------------------------------------------------------
# Maintenance.
#---------------------------------------------------------------------

.PHONY: clean
clean: clean-target clean-objs clean-deps clean-gprof clean-tags

.PHONY: clean-target
clean-target:
	@-rm -f $(BUILD_OUTPUT)/$(TARGET)

.PHONY: clean-objs
clean-objs:
	@-rm -rf echo $(OBJS)

.PHONY: clean-deps
clean-deps:
	@-rm -f $(BUILD_C_OBJS:%.o=$(BUILD_OUTPUT)/%.d)
	@-rm -f $(BUILD_CPP_OBJS:%.o=$(BUILD_OUTPUT)/%.d)

.PHONY: clean-gprof
clean-gprof:
	@-rm -f $(BUILD_ROOT)/gmon.out
	@-rm -f $(BUILD_ROOT)/callf.order
	@-rm -f $(BUILD_ROOT)/callo.order
	@-rm -f $(BUILD_ROOT)/gmon.order
	@-rm -f $(BUILD_ROOT)/time.order

.PHONY: clean-tags
clean-tags:
	@-rm -f $(BUILD_ROOT)/GSYMS
	@-rm -f $(BUILD_ROOT)/GPATH
	@-rm -f $(BUILD_ROOT)/GTAGS
	@-rm -f $(BUILD_ROOT)/GRTAGS

#---------------------------------------------------------------------
# Autogenerated dependencies.
#---------------------------------------------------------------------

-include $(BUILD_C_OBJS:%.o=$(BUILD_OUTPUT)/%.d)
-include $(BUILD_CPP_OBJS:%.o=$(BUILD_OUTPUT)/%.d)

#---------------------------------------------------------------------
# The End.
#---------------------------------------------------------------------

