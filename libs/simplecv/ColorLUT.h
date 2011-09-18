/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

// based on code from GspcaGui, Copyright (C) 2004 2005 2006 Michel Xhaard 

extern int *LutYr;
extern int *LutYg;
extern int *LutYb;
extern int *LutVr;
extern int *LutVrY;
extern int *LutUb;
extern int *LutUbY;
extern int *LutRv;
extern int *LutGu;
extern int *LutGv;
extern int *LutBu;

void initLUT();
void freeLUT();

#define CLIP(color) (unsigned char)(((color)>0xFF)?0xff:(((color)<0)?0:(color)))

#define RGB24_TO_Y(r,g,b) LutYr[(r)] + LutYg[(g)] + LutYb[(b)]
#define YR_TO_V(r,y) LutVr[(r)] + LutVrY[(y)]
#define YB_TO_U(b,y) LutUb[(b)] + LutUbY[(y)]

#define R_FROMYV(y,v)  CLIP((y) + LutRv[(v)])
#define G_FROMYUV(y,u,v) CLIP((y) + LutGu[(u)] + LutGv[(v)])
#define B_FROMYU(y,u) CLIP((y) + LutBu[(u)])

#define YfromRGB(r,g,b) CLIP((77*(r)+150*(g)+29*(b))>>8)
#define UfromRGB(r,g,b) CLIP(((128*(b)-85*(g)-43*(r))>>8 )+128)
#define VfromRGB(r,g,b) CLIP(((128*(r)-107*(g)-21*(b))>>8) +128)

#define PACKRGB16(r,g,b) (__u16) ((((b) & 0xF8) << 8 ) | (((g) & 0xFC) << 3 ) | (((r) & 0xF8) >> 3 ))
#define UNPACK16(pixel,r,g,b) r=((pixel)&0xf800) >> 8; 	g=((pixel)&0x07e0) >> 3; b=(((pixel)&0x001f) << 3)

