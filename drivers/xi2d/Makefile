all: xi2d xicursor

xi2d: xi2d.o
	g++ -o $@ $^ -loscpack -ltools -lX11 -lXi -L ../../build/lib/

xicursor: xicursor.o
	g++ -o $@ $^ -lsimplecv -lXrender -lXi -L ../../build/lib/

%.o: %.cc
	g++ -DTISCH_SHARED="" -c -ggdb -Wall -o $@ $< -I ../../ -I ../../libs/tools/ -I ../../libs/simplecv/ -I ../../libs/osc/

clean:
	rm *.o xi2d
