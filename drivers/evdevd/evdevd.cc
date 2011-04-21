/* based on evtest.c by Vojtec Pavlik */

#include <BasicBlob.h>
#include <TUIOOutStream.h>
#include <tisch.h>
#include <vector>

#include <linux/input.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>


#define BITS_PER_LONG (sizeof(long) * 8)
#define NBITS(x) ((((x)-1)/BITS_PER_LONG)+1)
#define OFF(x)  ((x)%BITS_PER_LONG)
#define BIT(x)  (1UL<<OFF(x))
#define LONG(x) ((x)/BITS_PER_LONG)
#define test_bit(bit, array)	((array[LONG(bit)] >> OFF(bit)) & 1)


BasicBlob current;
std::vector<BasicBlob> b1,b2;
std::vector<BasicBlob>* blobs = &b1;
std::vector<BasicBlob>* oldblobs = &b2;

//int blobcount = 0;
//int framenum = 0;
int blobid = 0;

TUIOOutStream output( TISCH_TUIO1 | TISCH_TUIO2 );


int main (int argc, char *argv[]) {

	int fd;
	unsigned int rd;
	struct input_event ev[64];

	int version;
	unsigned short id[4];
	char name[256] = "Unknown";

	unsigned long bit[EV_MAX][NBITS(KEY_MAX)];
	int abs[5];

	double maxx = 1.0;
	double maxy = 1.0;

	if (argc < 2) {
		printf("Usage: evdevd /dev/input/eventX\n");
		return 1;
	}

	if ((fd = open(argv[1], O_RDONLY)) < 0) { perror("evtest: open"); return 1; }

	if (ioctl(fd, EVIOCGVERSION, &version) < 0) { perror("evtest: can't get version"); return 1; }
	printf("Input driver version is %d.%d.%d\n", version >> 16, (version >> 8) & 0xff, version & 0xff);
	ioctl(fd, EVIOCGID, id);
	printf("Input device ID: bus 0x%x vendor 0x%x product 0x%x version 0x%x\n", id[ID_BUS], id[ID_VENDOR], id[ID_PRODUCT], id[ID_VERSION]);
	ioctl(fd, EVIOCGNAME(sizeof(name)), name);
	printf("Input device name: \"%s\"\n", name);

	memset(bit, 0, sizeof(bit));
	ioctl(fd, EVIOCGBIT(0, sizeof(bit[0])), bit[0]);

	for (int i = 1; i < EV_MAX; i++)
		if (test_bit(i, bit[0])) {
			ioctl(fd, EVIOCGBIT(i, sizeof(bit[0])), bit[i]);
			for (int j = 0; j < KEY_MAX; j++) 
				if (test_bit(j, bit[i])) {
					if (i == EV_ABS) {
						ioctl(fd, EVIOCGABS(j), abs);
						if (j == ABS_MT_POSITION_X) maxx = abs[2];
						if (j == ABS_MT_POSITION_Y) maxy = abs[2]; 
					}
				}
		}

	printf("Processing...\n");
	output.start();

	while (1) {

		rd = read(fd, ev, sizeof(struct input_event) * 64);

		if (rd < (int) sizeof(struct input_event)) {
			perror("\nevtest: read:");
			return 1;
		}

		for (unsigned int i = 0; i < rd / sizeof(struct input_event); i++)

			if (ev[i].type == EV_SYN) {
				
				if (ev[i].code) { // config sync

					double mindist = 0.05;
					for (std::vector<BasicBlob>::iterator it = oldblobs->begin(); it != oldblobs->end(); it++) {
						double dist = (it->pos - current.pos).length();
						if (dist < mindist) { mindist = dist; current.id = it->id; }
					}

					blobs->push_back(current);
					output << current;

				} else { // report sync

					output.send();
					output.start();

					std::swap(blobs,oldblobs);
					blobs->clear();
				}

			} else if (ev[i].type == EV_ABS) {

				current.id   = blobid++;
				current.type = INPUT_TYPE_FINGER;

				if (ev[i].code == ABS_MT_POSITION_X) {
					double tx = ev[i].value / maxx;
					current.pos.x  = tx;
					current.peak.x = tx;
				}

				if (ev[i].code == ABS_MT_POSITION_Y) {
					double ty = (1.0 - (ev[i].value / maxy));
					current.pos.y  = ty;
					current.peak.y = ty;
				}

			}

		fflush(stdout);

	}
}
