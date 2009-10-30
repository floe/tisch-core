TOPDIR = ../
CFLAGS += -I$(TOPDIR)/libs/tools/ -I$(TOPDIR)/libs/simplecv/ -I$(TOPDIR)/gestured/ -I$(TOPDIR)/gestured/features/ -I$(TOPDIR)
LDFLAGS += -ltools -lsimplecv -lgestures -L$(TOPDIR)/libs/tools/ -L$(TOPDIR)/libs/simplecv/ -L$(TOPDIR)/gestured/

APPS = picview demo
MORE = *.png
LIBS = libwidgets.$(LSF)
OBJS = Window.o Container.o Button.o Tile.o MasterContainer.o Dial.o Textbox.o Slider.o Checkbox.o Label.o

include $(TOPDIR)/scripts/rules.mk

