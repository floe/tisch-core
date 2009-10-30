# TISCH Framework: master Makefile

# check if global config already defined
ifndef TISCH_CONFIG

  TOPDIR  = $(shell pwd)
  PREFIX ?= $(TOPDIR)/build/
  BINDIR  = $(PREFIX)/bin/
  LIBDIR  = $(PREFIX)/lib/
  INCDIR  = $(PREFIX)/include/

  CFLAGS  += -Wall -ggdb -fPIC -DTISCH_SHARED="" -DTIXML_USE_STL
  LDFLAGS += -Wall -ggdb -L$(LIBDIR) $(shell pkg-config --libs libdc1394-2 2> /dev/null) $(shell pkg-config --libs libv4l2 2> /dev/null)
  SLFLAGS += -Wall -ggdb -fPIC -L$(LIBDIR)

  OS = $(shell uname)
  TISCH_CONFIG = 1

  ifeq ($(OS),Linux)

    ifeq (dc1394,$(findstring dc1394,$(LDFLAGS)))
      CFLAGS += -DHAS_DC1394
    endif
    ifeq (v4l2,$(findstring v4l2,$(LDFLAGS)))
      CFLAGS += -DHAS_LIBV4L
    endif
    LDFLAGS += -pg -lGL -lGLU -lglut
    SLFLAGS += -pg -shared
    CFLAGS  += -pg -O3 -mmmx
    LSF = so

  else ifeq ($(OS),Darwin)

    ifeq (arm-apple-darwin9,$(shell $(CXX) -dumpmachine))
      LDFLAGS += -lobjc -framework Foundation -framework CoreFoundation -framework CoreGraphics -framework CoreSurface -framework GraphicsServices -framework QuartzCore -framework OpenGLES -framework UIKit -bind_at_load -multiply_defined suppress -F/System/Library/PrivateFrameworks
      CFLAGS  += -DTISCH_IPHONE
      OS = iPhone
    else
      LDFLAGS += -framework OpenGL -framework GLUT
      CFLAGS  += -O3 -mmmx
      OS = MacOSX
    endif
    SLFLAGS += -dynamiclib
    LSF = dylib

  endif

endif

# export global config variables
export TOPDIR PREFIX BINDIR LIBDIR INCDIR OS LSF CFLAGS LDFLAGS SLFLAGS TISCH_CONFIG

# info blurb
default:
	@echo
	@echo TISCH Framework - http://tisch.sourceforge.net/
	@echo
	@echo Copyright '(c)' 2006-2009 by Florian Echtler, TUM '<echtler@in.tum.de>'
	@echo Licensed under GNU Lesser General Public License '(LGPL)' 3 or later  
	@echo
	@echo Please choose one of the following targets:
	@echo 
	@echo "  all     - build all targets"
	@echo "  install - install into $(PREFIX)"
	@echo "  clean   - remove all binaries"
	@echo
	@echo "  additional targets: $(TARGETS)"
	@echo
	@echo If you need special include/library paths, put them into the 
	@echo CFLAGS and LDFLAGS environment variables. To choose a different
	@echo installation target, set the PREFIX variable. Example:
	@echo
	@echo "  make CFLAGS=-I/opt/include PREFIX=/usr/local/ install"
	@echo


# target directories, dependencies & phonies
TARGETS = libs/tools libs/simplegl gestured widgets 
ifneq ($(OS),iPhone)
  TARGETS += libs/simplecv drivers/touchd drivers/tuio calibd
endif

.PHONY: default all install clean $(TARGETS)

ifneq ($(COMMAND),clean)
  libs/simplecv: libs/tools
  libs/simplegl: libs/simplecv
  drivers/touchd calibd: libs/simplegl
  drivers/tuio gestured: libs/tools
  widgets: libs/simplegl gestured
endif

simplecv: libs/simplecv
simplegl: libs/simplegl
touchd: drivers/touchd
tuio:   drivers/tuio
tools: libs/tools


# top-level targets
all: $(TARGETS)

install:
	$(MAKE) COMMAND=install all

clean:
	$(MAKE) COMMAND=clean all

$(TARGETS):
	$(MAKE) -C $@ $(COMMAND)


# target-specific commands
#%-build: % ;

#%-install: %
#	$(MAKE) -C $* install

#%-clean:
#	$(MAKE) -C $* clean

#getobjs=$(filter-out $(1)/main.o $(1)/test.o $(1)/cvdemo.o,$(patsubst %.cc,%.o,$(wildcard $(1)/*.cc)))
#OBJECTS=$(call getobjs,simplecv)

