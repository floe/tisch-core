TOPDIR = ../../

CFLAGS += -I$(TOPDIR)/libs/tools/ -I$(TOPDIR)/libs/osc/ -I$(TOPDIR)
LDFLAGS += -ltools -loscpack -L$(TOPDIR)/libs/tools/ -L$(TOPDIR)/libs/osc/ -F/System/Library/PrivateFrameworks -framework MultitouchSupport -lIOKit -framework CoreFoundation -framework ApplicationServices -framework AppKit -lobjc

APPS=macmtd
OBJS=output.oo

include $(TOPDIR)/scripts/rules.mk

macmtd.o: macmtd.oo
	cp $^ $@ 

#macmtd: macmtd.mm output.mm
#	g++ $^ -o $@ -I ../../ -I ../../libs/osc/ -I ../../libs/tools/ -L../../build/lib -loscpack -ltools -F/System/Library/PrivateFrameworks -framework MultitouchSupport -lIOKit -framework CoreFoundation -framework ApplicationServices -framework AppKit -lobjc 
