TOPDIR  = ../../../
PACKAGE = libtisch

WRAP = $(PACKAGE)_java.$(SUFFIX) $(PACKAGE).jar
OBJS = $(subst .java,.class,$(wildcard $(PACKAGE)/*.java)) libtisch/Loader.class
DOCS = example.java example.pde helpers.pde

SWIGFLAGS = -c++ -java -package $(PACKAGE) -outdir $(PACKAGE)

ifndef JAVA_HOME
  $(warning JAVA_HOME not set - trying to guess a default value.)
endif

ifndef COREJAR
  $(warning COREJAR not set - trying to guess a default value.)
endif

ifeq ($(OS),linux)
  COREJAR ?= /usr/share/processing/lib/core.jar
  JAVA_HOME ?= /usr/lib/jvm/default-java/ /usr/lib/jvm/java/
  CFLAGS += $(addsuffix include/,$(addprefix -I,$(JAVA_HOME))) $(addsuffix include/linux/,$(addprefix -I,$(JAVA_HOME)))
else
  COREJAR ?= /Applications/Processing.app/Contents/Resources/Java/core.jar
  JAVA_HOME ?= /System/Library/Frameworks/JavaVM.framework/Versions/Current/
  CFLAGS += -I$(JAVA_HOME)/Headers/ -framework OpenGL -framework GLUT -dynamiclib
  SWIGFLAGS += -I/System/Library/Frameworks/OpenGL.framework/Versions/Current/Headers
endif

ifeq ($(wildcard $(COREJAR)),)
  $(warning COREJAR ($(COREJAR)) not found - disabling Processing component.)
else
  JAVA_PARAM = -cp $(COREJAR) Loader.java
endif

include $(TOPDIR)/scripts/rules.mk

%_java.$(SUFFIX): %_wrap.cxx
	g++ $(CFLAGS) $(SLFLAGS) -I$(TOPDIR) -I$(TOPDIR)/libs/osc -I$(TOPDIR)/libs/tools -I$(TOPDIR)/gestured -I$(TOPDIR)/widgets -I$(TOPDIR)/libs/simplecv -I$(TOPDIR)/libs/simplegl -I$(TOPDIR)/gestured/features -L$(TOPDIR)/libs/tools -L$(TOPDIR)/gestured -L$(TOPDIR)/widgets -L$(TOPDIR)/libs/simplecv -L$(TOPDIR)/libs/simplegl $< -ltools -loscpack -lgestures -lsimplecv -lsimplegl -lwidgets -lGL -lGLU -o $@

%.jar: $(wildcard $(PACKAGE)/*.java) Loader.java
	javac -d . $(PACKAGE)/*.java $(JAVA_PARAM)
	jar -cf $@ $(PACKAGE)

distclean: clean
	-rm $(PACKAGE)/*.java $(PACKAGE)_wrap.*

