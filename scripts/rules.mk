# TISCH Framework: common make rules

SELF = $(lastword $(subst /, ,$(CURDIR)))

ifeq (${MAKELEVEL},0)
all: build ; 
%::
	make -C $(TOPDIR) $(SELF) COMMAND=$@
else

build: all
all: $(APPS) $(LIBS)

%.oo: %.mm *.h
	$(CXX) -c $(CFLAGS) $< -o $@

%.o: %.cc *.h
	$(CXX) -c $(CFLAGS) $< -o $@

$(APPS): %: %.o $(OBJS) $(LIBS)
	$(CXX) $(LDFLAGS) $^ -o $@

$(LIBS): $(OBJS)
	$(CXX) $(SLFLAGS) $(LDFLAGS) $^ -o $@

clean:
	-rm $(APPS) $(LIBS) $(OBJS) *.o gmon.out

install: all
	mkdir -p $(BINDIR)
	mkdir -p $(LIBDIR)
	-ldid -S $(APPS) $(LIBS)
	-cp $(APPS) $(BINDIR)
	-cp $(MORE) $(BINDIR)
	-cp $(LIBS) $(LIBDIR)

endif

