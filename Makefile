# TISCH Framework: master Makefile

# check if global config already defined
ifndef TISCH_CONFIG

  TOPDIR = $(shell pwd)

  ifeq ($(DESTDIR),)
    DESTDIR = $(TOPDIR)/build/
    PREFIX ?= $(DESTDIR)
  else
    PREFIX ?= /usr
  endif

  BINDIR   = $(DESTDIR)/bin/
  LIBDIR   = $(DESTDIR)/lib/
  INCDIR   = $(DESTDIR)/include/libtisch/
  DOCDIR   = $(DESTDIR)/share/doc/libtisch/
  SHARED   = $(DESTDIR)/share/libtisch/

  CFLAGS  += -Wall -ggdb -fPIC -DTISCH_SHARED="" -DTIXML_USE_STL -I. -DTISCH_PREFIX='"$(PREFIX)/share/libtisch/"'
  LDFLAGS += -Wall -ggdb -L$(LIBDIR) 
  SLFLAGS += -Wall -ggdb -fPIC -L$(LIBDIR)

  TARGET = $(subst -, ,$(shell $(CXX) -dumpmachine))
  OS   = $(word 2,$(TARGET))
  ABI  = $(word 3,$(TARGET))
  ARCH = $(word 1,$(TARGET))

  TISCH_CONFIG = 1

  ifeq ($(OS),linux)

    ifeq ($(ABI),androideabi)

      LDFLAGS += -lGLESv1_CM -Wl,-rpath-link=$(LIBDIR)
      CFLAGS  += -DTISCH_OPENGLES
      ABI = opengles
 
    else

      LDFLAGS += -lpthread -lGL -lGLU -lglut -Wl,-rpath-link=$(LIBDIR)

      LDFLAGS += $(shell pkg-config --libs libdc1394-2 2> /dev/null)
      LDFLAGS += $(shell pkg-config --libs libfreenect 2> /dev/null)
      LDFLAGS += $(shell pkg-config --libs libv4l2 2> /dev/null)

      ifeq (dc1394,$(findstring dc1394,$(LDFLAGS)))
        CFLAGS += -DHAS_DC1394
      endif
      ifeq (v4l2,$(findstring v4l2,$(LDFLAGS)))
        CFLAGS += -DHAS_LIBV4L
      endif
      ifeq (freenect,$(findstring freenect,$(LDFLAGS)))
        CFLAGS += -DHAS_FREENECT
      endif

    endif

    SLFLAGS += -shared
    CFLAGS  += -O2 -fno-strict-aliasing
    SUFFIX = so
    LSF = so.2.0

  else ifeq ($(OS),apple)

    ifeq ($(ARCH),arm)

      LDFLAGS += -lobjc -framework Foundation -framework CoreFoundation -framework CoreGraphics -framework CoreSurface -framework GraphicsServices -framework QuartzCore -framework OpenGLES -framework UIKit -bind_at_load -multiply_defined suppress -F/System/Library/PrivateFrameworks
      CFLAGS  += -DTISCH_GLES
      ABI = opengles

    else

      LDFLAGS += -framework OpenGL -framework GLUT
      CFLAGS  += -O2 -fno-strict-aliasing

    endif

    SLFLAGS += -dynamiclib
    SUFFIX = dylib
    LSF = dylib

  endif

endif

# export global config variables
export TOPDIR BINDIR LIBDIR INCDIR DOCDIR SHARED OS ABI ARCH SUFFIX LSF CFLAGS LDFLAGS SLFLAGS TISCH_CONFIG


# info blurb
default:
	@echo
	@echo TISCH Framework - http://tisch.sourceforge.net/
	@echo
	@echo Copyright '(c)' 2006 - 2011 by Florian Echtler '<floe@butterbrot.org>'
	@echo Licensed under GNU Lesser General Public License '(LGPL)' 3 or later  
	@echo
	@echo Please choose one of the following targets:
	@echo 
	@echo "  all      - build all binaries"
	@echo "  libtisch - build libTISCH targets"
	@echo "  wrappers - build C#/Java/Python wrappers"
	@echo "  install  - install all binaries into $(DESTDIR)"
	@echo "  clean    - remove all binaries"
	@echo
	@echo "  separate libTISCH targets: osc tools simplecv simplegl touchd calibd gestured widgets"
	@echo "  separate wrapper targets:  csharp java python"
	@echo
	@echo If you need special include/library paths, put them into the 
	@echo CFLAGS and LDFLAGS environment variables. To choose a different
	@echo installation target, set the DESTDIR variable. To change the 
	@echo runtime prefix separately, set the PREFIX variable. Example:
	@echo
	@echo "  CFLAGS=-I/opt/include PREFIX=/usr/local/ make install"
	@echo
	@echo "current install target (DESTDIR): $(DESTDIR)"
	@echo "current runtime prefix (PREFIX):  $(PREFIX)"
	@echo
	@echo "(note: detected target ARCH-OS-ABI is $(ARCH)-$(OS)-$(ABI))"
	@echo


# target directories
TARGETS = libs/osc libs/tools libs/simplecv libs/simplegl gestured widgets

ifneq ($(ABI),opengles)
  TARGETS += drivers/touchd calibd
endif

ifeq ($(OS),linux)
  TARGETS += drivers/evdevd
endif

ifeq ($(OS),apple)
  TARGETS += drivers/macmtd
endif

ifneq ($(shell which gmcs),)
  WRAPPERS += libs/wrappers/csharp
endif

ifneq ($(shell which javac),)
  WRAPPERS += libs/wrappers/java
endif

ifneq ($(shell which python),)
  WRAPPERS += libs/wrappers/python
endif


# phony declarations
.PHONY: default all install wrappers clean $(TARGETS) $(WRAPPERS)


# dependencies
ifneq ($(COMMAND),clean)
  libs/tools: libs/osc
  libs/simplecv: libs/tools
  libs/simplegl: libs/simplecv
  drivers/touchd calibd: libs/simplegl
  drivers/evdevd drivers/macmtd: libs/tools
  gestured: libs/tools
  widgets: libs/simplegl gestured
  $(WRAPPERS): widgets
endif


# aliases
simplecv: libs/simplecv
simplegl: libs/simplegl
touchd: drivers/touchd
evdevd: drivers/evdevd
macmtd: drivers/macmtd
osc:   libs/osc 
tools: libs/tools

csharp: libs/wrappers/csharp
java: libs/wrappers/java
python: libs/wrappers/python


# top-level targets
all: libtisch wrappers
libtisch: $(TARGETS)
wrappers: $(WRAPPERS)

install:
	$(MAKE) COMMAND=install all

clean:
	$(MAKE) COMMAND=clean all

$(TARGETS) $(WRAPPERS):
	$(MAKE) -C $@ $(COMMAND)

