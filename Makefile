
#=======================================================================
# Define every object required by compilation
#=======================================================================
  OBJS =                           \
          $(OBJDIR)/assets.o       \
          $(OBJDIR)/cauldron.o     \
          $(OBJDIR)/collision.o    \
          $(OBJDIR)/config.o       \
          $(OBJDIR)/gesture.o      \
          $(OBJDIR)/gamestate.o    \
          $(OBJDIR)/global.o       \
          $(OBJDIR)/input.o        \
          $(OBJDIR)/main.o         \
          $(OBJDIR)/menustate.o    \
          $(OBJDIR)/object.o       \
          $(OBJDIR)/recipeScroll.o \
          $(OBJDIR)/scrollItem.o   \
          $(OBJDIR)/type.o
#=======================================================================

#=======================================================================
# Define the generated icon
# TODO Uncomment this to add an icon to the game
#      Required files:
#        - assets/icon.ico
#        - assets/icon.rc
#=======================================================================
#  WINICON := assets/icon.o
#=======================================================================

#=======================================================================
# Select which compiler to use (gcc)
#=======================================================================
  CC = gcc
#=======================================================================

#=======================================================================
# Clear the suffixes' default rule, since there's an explicit one
#=======================================================================
.SUFFIXES:
#=======================================================================

#=======================================================================
# Define all targets that doesn't match its generated file
#=======================================================================
.PHONY: all clean
#=======================================================================

#=======================================================================
# Define compilation target
#=======================================================================
  TARGET := game
#=======================================================================

#=======================================================================
# Set OS flag
#=======================================================================
  OS := $(shell uname)
  ifeq ($(OS), MINGW32_NT-6.1)
    OS := Win
#   Also, set the icon
    ICON = $(WINICON)
  endif
  ifeq ($(OS), MINGW32_NT-6.2)
    OS := Win
#   Also, set the icon
    ICON = $(WINICON)
  endif
#=======================================================================

#=======================================================================
# Define CFLAGS (compiler flags)
#=======================================================================
# Add all warnings and default include path
  CFLAGS := -Wall -I"./include/"
# Add architecture flag
  ARCH := $(shell uname -m)
  ifeq ($(ARCH), x86_64)
    CFLAGS := $(CFLAGS) -m64 -DALIGN=8
  else
    CFLAGS := $(CFLAGS) -m32 -DALIGN=4
  endif
# Add debug flags
  ifneq ($(RELEASE), yes)
    CFLAGS := $(CFLAGS) -g -O0 -DDEBUG
  else
    CFLAGS := $(CFLAGS) -O3
  endif
# Set flags required by OS
  ifeq ($(OS), Win)
    CFLAGS := $(CFLAGS) -I"/d/windows/mingw/include" -I/c/GFraMe/include
  else
    CFLAGS := $(CFLAGS) -fPIC
  endif
#=======================================================================

#=======================================================================
# Define LFLAGS (linker flags)
#=======================================================================
# Add the framework
  ifeq ($(RELEASE), yes)
    LFLAGS := -lGFraMe
  else
    LFLAGS := -lGFraMe_dbg
  endif
# Required in some OSs to link with tan and whatnot
  LFLAGS := $(LFLAGS) -lm
# Add libs and paths required by an especific OS
  ifeq ($(OS), Win)
    LFLAGS := -mwindows -lmingw32 $(LFLAGS) -lSDL2main
    LFLAGS := -L/d/windows/mingw/mingw32/lib $(LFLAGS)
# Prepend the framework search path
    LFLAGS := -L/c/GFraMe/lib/ $(LFLAGS)
  else
# Prepend the framework search path
    LFLAGS := -L/usr/lib/GFraMe/ $(LFLAGS)
  endif
#=======================================================================

#=======================================================================
# Define where source files can be found and where objects & binary are output
#=======================================================================
 VPATH := src
 OBJDIR := obj/$(OS)
 BINDIR := bin/$(OS)
#=======================================================================

#=======================================================================
# Make the objects list constant (and the icon, if any)
#=======================================================================
 OBJS := $(OBJS)
#=======================================================================

#=======================================================================
# Define default compilation rule
#=======================================================================
all: MAKEDIRS $(BINDIR)/$(TARGET)
	date
#=======================================================================

#=======================================================================
# Define a rule to generated the icon
#=======================================================================
$(WINICON):
	windres assets/icon.rc $(WINICON)
#=======================================================================

#=======================================================================
# Rule for actually building the game
#=======================================================================
$(BINDIR)/$(TARGET): MAKEDIRS $(OBJS)  $(ICON)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(ICON) $(LFLAGS)
#=======================================================================

#=======================================================================
# Rule for compiling any .c in its object
#=======================================================================
$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<
#=======================================================================

#=======================================================================
# Rule for creating every directory
#=======================================================================
MAKEDIRS: | $(OBJDIR)
#=======================================================================

#=======================================================================
# Rule for actually creating every directory
#=======================================================================
$(OBJDIR):
	mkdir -p $(OBJDIR)
	mkdir -p $(BINDIR)
#=======================================================================

#=======================================================================
# Removes all built objects
#=======================================================================
clean:
	rm -f $(OBJS)
	rm -f $(BINDIR)/$(TARGET)
#=======================================================================

