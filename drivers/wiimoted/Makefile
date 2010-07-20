wiimoted: wiimoted.o
	g++ -o $@ $^ -ltools -lsimplecv -lwidgets -lgestures -L ../../framework/build/lib/ -lwiiuse

%.o: %.cc
	g++ -DTISCH_SHARED="" -c -ggdb -Wall -o $@ $< -I ../../framework -I ../../framework/gestured/features/ -I ../../framework/gestured/ -I ../../framework/libs/tools/ -I ../../framework/widgets/ -I ../../framework/libs/simplecv/ -I /home/echtler/sources/external/wiiuse_v0.12/src/

clean:
	rm *.o wiimoted 
