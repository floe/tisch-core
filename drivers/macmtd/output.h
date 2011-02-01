/**
 * macmtdd
 * 
 * (c) 2011 Martin Kleinhans <mail@mkleinhans.de>
 */
extern void out_init(const char *target);
extern void out_deinit();
extern void out_blob(int id, int type, float x, float y);
extern void out_startFrame();
extern void out_endFrame();
