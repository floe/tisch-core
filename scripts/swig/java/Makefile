TOPDIR=../../../
LIBDIR=$(TOPDIR)/build/lib/
COREJAR=/usr/share/processing/lib/core.jar

PACKAGE=libtisch
TARGETS=$(PACKAGE)_java.so $(PACKAGE).jar

install: $(TARGETS)
	-cp $(TARGETS) $(LIBDIR)

all: $(TARGETS)

.SECONDARY:

%_wrap.cxx: ../%.i
	mkdir -p $(PACKAGE)
	swig -I/usr/include -c++ -java -DTISCH_SHARED="" -o $(PACKAGE)_wrap.cxx -oh $(PACKAGE)_wrap.h -package $(PACKAGE) -outdir $(PACKAGE) $<

%_java.o: %_wrap.cxx
	g++ -DTISCH_SHARED="" -Wall -c $< -I$(TOPDIR) -I$(TOPDIR)/libs/tools -I$(TOPDIR)/gestured -I$(TOPDIR)/widgets -I$(TOPDIR)/libs/simplecv -I$(TOPDIR)/gestured/features -I/usr/lib/jvm/sun-jdk-1.6/include/ -I/usr/lib/jvm/sun-jdk-1.6/include/linux/ -o $@

%.so: %.o
	ld -G $< -o $@ -L$(LIBDIR) -ltools -lgestures -lsimplecv -lwidgets

%.class: %.java
	javac -cp $(COREJAR) -d . $(PACKAGE)/*.java Loader.java

%.jar: Loader.class
	jar -cf $@ $(PACKAGE)

clean:
	-rm -r $(PACKAGE) $(TARGETS) $(PACKAGE)_wrap.*

