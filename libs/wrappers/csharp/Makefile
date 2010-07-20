TOPDIR  = ../../../
PACKAGE = libtisch

WRAP = $(PACKAGE)_csharp.so $(PACKAGE).dll
DOCS = example.cs

SWIGFLAGS = -c++ -csharp -dllimport $(PACKAGE)_csharp -outdir $(PACKAGE)

include $(TOPDIR)/scripts/rules.mk

%_csharp.so: %_wrap.cxx
	g++ $(CFLAGS) $(SLFLAGS) -I$(TOPDIR) -I$(TOPDIR)/libs/tools -I$(TOPDIR)/gestured -I$(TOPDIR)/widgets -I$(TOPDIR)/libs/simplecv -I$(TOPDIR)/libs/simplegl -I$(TOPDIR)/gestured/features -L$(TOPDIR)/libs/tools -L$(TOPDIR)/gestured -L$(TOPDIR)/widgets -L$(TOPDIR)/libs/simplecv -L$(TOPDIR)/libs/simplegl -ltools -lgestures -lsimplecv -lsimplegl -lwidgets $< -o $@

%.dll: $(wildcard $(PACKAGE)/*.cs)
	cd $(PACKAGE); gmcs -t:library -out:../$@ -warn:4 *.cs

distclean: clean
	-rm $(PACKAGE)/*.cs $(PACKAGE)_wrap.*

