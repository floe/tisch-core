# TISCH Framework: common make rules

SELF = $(lastword $(subst /, ,$(CURDIR)))

ifeq (${MAKELEVEL},0)
all: build ; 
%::
	make -C $(TOPDIR) $(SELF) COMMAND=$@
else

build: all
all: $(APPS) $(LIBS) $(WRAP)

%.oo: %.mm *.h
	$(CXX) -c $(CFLAGS) $< -o $@

%.o: %.sx
	gcc -Wall -c $(ASFLAGS) $< -o $@

%.o: %.cc *.h
	$(CXX) -c $(CFLAGS) $< -o $@

$(APPS): %: %.o $(OBJS) $(LIBS)
	$(CXX) $^ $(LDFLAGS) -o $@

ifeq ($(OS),linux)
  SLFLAGS += -Wl,-soname,$(subst .2.0,.2,$@)
endif

$(LIBS): $(OBJS)
	$(CXX) $^ $(SLFLAGS) $(LDFLAGS) -o $@
	-ln -s $@ $(subst .2.0,.2,$@)
	-ln -s $(subst .2.0,.2,$@) $(subst .2.0,,$@)

.PRECIOUS: %_wrap.cxx

%_wrap.cxx: ../%.i
	mkdir -p $*
	swig $(SWIGFLAGS) -I/usr/include -DTISCH_SHARED="" -o $@ -oh $*_wrap.h $<

clean:
	-rm $(APPS) $(LIBS) $(OBJS) $(WRAP) *.o *.so* gmon.out

install: all
	mkdir -p $(BINDIR)
	mkdir -p $(LIBDIR)
	mkdir -p $(INCDIR)
	mkdir -p $(DOCDIR)
	mkdir -p $(SHARED)
	-ldid -S $(APPS) $(LIBS)
	-cp $(APPS) $(BINDIR)
	-cp $(MORE) $(SHARED)
	-cp $(LIBS) $(LIBDIR)
	-cp $(WRAP) $(LIBDIR)
	-cp -a *.so* $(LIBDIR)
	-cp $(INCS) $(INCDIR)
	-cp $(DOCS) $(DOCDIR)

endif

