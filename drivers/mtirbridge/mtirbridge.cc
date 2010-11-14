#include <iostream>
#include <unistd.h>

#include <mtirdefs.h>
#include <mtirctldll.h>

int main ( int argc, char* argv[] ) {

	std::cout << "MtirctlEnum: " << std::flush;
	int num = MtirctlEnum();
	std::cout << num << std::endl;
	if (num == 0) return 0;

	int dev = 0;
	int run = 1;

	bool res = MtirctlOpen( dev );
	MtirctlStartScan( dev, NULL );

	MTIR_GEOMETRY geo;
	res = MtirctlGetGeometry( dev, &geo );

	std::cout << "size: " << geo.sx << " " << geo.sy << std::endl;

	MTIR_POSITION pos[64];
	MTIR_SCANRES scanres;
	scanres.pPosition = pos;
	scanres.pMap = NULL;
	scanres.pReserved1 = NULL;

	while (run) {

		res = MtirctlGetScanResult( dev, &scanres );

		if (res)
			for (int i = 0; i < scanres.nPositions; i++)
				std::cout << pos[i].id << " " << pos[i].x << " " << pos[i].y << std::endl;
	}

	res = MtirctlStopScan( dev );
	res = MtirctlClose( dev );

	return 0;
}

