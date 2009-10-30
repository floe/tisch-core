TOPDIR=../../../
LIBDIR=$(TOPDIR)/build/lib/

PACKAGE=libtisch
TARGETS=_$(PACKAGE).so $(PACKAGE).py

install: $(TARGETS)
	-cp $(TARGETS) $(LIBDIR)

all: $(TARGETS)

.SECONDARY:

%_wrap.cxx %.py: ../%.i
	swig -I/usr/include -c++ -python -DTISCH_SHARED="" -o $(PACKAGE)_wrap.cxx -oh $(PACKAGE)_wrap.h $<

_%.o: %_wrap.cxx
	g++ -DTISCH_SHARED="" -Wall -c $< -I$(TOPDIR) -I$(TOPDIR)/libs/tools -I$(TOPDIR)/gestured -I$(TOPDIR)/widgets -I$(TOPDIR)/libs/simplecv -I$(TOPDIR)/gestured/features -I/usr/include/python2.5/ -o $@

%.so: %.o
	ld -G $< -o $@ -L$(LIBDIR) -ltools -lgestures -lsimplecv -lwidgets

clean:
	-rm -r $(PACKAGE) $(TARGETS) $(PACKAGE)_wrap.* _$(PACKAGE).*

