TOPDIR=../../../
LIBDIR=$(TOPDIR)/build/lib/


PACKAGE=libtisch
TARGETS=$(PACKAGE)_csharp.so $(PACKAGE).dll

install: $(TARGETS)
	-cp $(TARGETS) $(LIBDIR)

all: $(TARGETS)

.SECONDARY:

%_wrap.cxx: ../%.i
	mkdir -p $(PACKAGE)
	swig -I/usr/include -c++ -csharp -dllimport libtisch_csharp -DTISCH_SHARED="" -o $(PACKAGE)_wrap.cxx -oh $(PACKAGE)_wrap.h -outdir $(PACKAGE) $<

%_csharp.o: %_wrap.cxx
	g++ -DTISCH_SHARED="" -Wall -fPIC -c $< -I$(TOPDIR) -I$(TOPDIR)/libs/tools -I$(TOPDIR)/gestured -I$(TOPDIR)/widgets -I$(TOPDIR)/libs/simplecv -I$(TOPDIR)/gestured/features -o $@

%.so: %.o
	ld -G $< -o $@ -L$(LIBDIR) -fPIC -ltools -lgestures -lsimplecv -lwidgets

%.dll: %_wrap.cxx
	cp *.cs $(PACKAGE)
	cd $(PACKAGE); gmcs -t:library -out:../$@ -warn:4 *.cs

clean:
	-rm -r $(PACKAGE) $(TARGETS) $(PACKAGE)_wrap.* $(PACKAGE)_csharp.o

