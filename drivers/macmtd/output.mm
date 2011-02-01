/**
 * macmtdd
 * 
 * (c) 2011 Martin Kleinhans <mail@mkleinhans.de>
 */

#include <tisch.h>

#include <libs/tools/TUIOOutStream.h>

extern float g_fWidth;
extern float g_fHeight;

TUIOOutStream *g_pOutput = NULL;

void out_init(const char *target) {
	g_pOutput = new TUIOOutStream(TISCH_TUIO2, target, TISCH_PORT_CALIB);
}

void out_deinit() {
	delete g_pOutput;
}

void out_blob(int id, int type, float x, float y) {
	BasicBlob b;
	b.id = id;
	b.pos.x = x*g_fWidth;
	b.pos.y = y*g_fHeight;
	b.peak = b.pos;
	b.type = type;

	*g_pOutput << b;
}

void out_startFrame() {
	g_pOutput->start();
}

void out_endFrame() {
	g_pOutput->send();
}
