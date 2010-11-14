all: mtirbridge

mtirbridge.o: mtirbridge.cc
	g++ -m32 -Wall -I. -I/usr/include/qt4/QtCore/ -I/usr/include/qt4/ -c mtirbridge.cc -o mtirbridge.o

mtirbridge: mtirbridge.o
	g++ -m32 -o mtirbridge mtirbridge.o -L. -lmtirctl
