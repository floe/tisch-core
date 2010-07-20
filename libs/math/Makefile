test: test.o
	g++ -ggdb -Wall -o $@ $+ -llapack -lgfortran

%.o: %.cc homography.h normalize.h
	g++ -ggdb -c -o $@ -I tnt -I jama -I clapack -Wall $<

clean:
	-rm *.o test

