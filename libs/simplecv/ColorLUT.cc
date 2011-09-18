/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

// based on code from GspcaGui, Copyright (C) 2004 2005 2006 Michel Xhaard 

#include "ColorLUT.h"
#include <stdlib.h>

int *LutYr = NULL;
int *LutYg = NULL;;
int *LutYb = NULL;;
int *LutVr = NULL;;
int *LutVrY = NULL;;
int *LutUb = NULL;;
int *LutUbY = NULL;;
int *LutRv = NULL;
int *LutGu = NULL;
int *LutGv = NULL;
int *LutBu = NULL;


void initLUT() {

	#define Rcoef 299 
	#define Gcoef 587 
	#define Bcoef 114 
	#define Vrcoef 711 //656 //877 
	#define Ubcoef 560 //500 //493 564
	
	#define CoefRv 1402
	#define CoefGu 714 // 344
	#define CoefGv 344 // 714
	#define CoefBu 1772
	
	LutYr = (int*)malloc(256*sizeof(int));
	LutYg = (int*)malloc(256*sizeof(int));
	LutYb = (int*)malloc(256*sizeof(int));
	LutVr = (int*)malloc(256*sizeof(int));
	LutVrY = (int*)malloc(256*sizeof(int));
	LutUb = (int*)malloc(256*sizeof(int));
	LutUbY = (int*)malloc(256*sizeof(int));
	
	LutRv = (int*)malloc(256*sizeof(int));
	LutGu = (int*)malloc(256*sizeof(int));
	LutGv = (int*)malloc(256*sizeof(int));
	LutBu = (int*)malloc(256*sizeof(int));

	for (int i = 0; i < 256; i++) {

		LutYr[i] = i*Rcoef/1000 ;
		LutYg[i] = i*Gcoef/1000 ;
		LutYb[i] = i*Bcoef/1000 ;
		LutVr[i] = i*Vrcoef/1000;
		LutUb[i] = i*Ubcoef/1000;

		LutVrY[i] = 128 -(i*Vrcoef/1000);
		LutUbY[i] = 128 -(i*Ubcoef/1000);
		LutRv[i] = (i-128)*CoefRv/1000;
		LutBu[i] = (i-128)*CoefBu/1000;
		LutGu[i] = (128-i)*CoefGu/1000;
		LutGv[i] = (128-i)*CoefGv/1000;
	}
}


void freeLUT() {

	free(LutYr);
	free(LutYg);
	free(LutYb);
	free(LutVr);
	free(LutVrY);
	free(LutUb);
	free(LutUbY);
	
	free(LutRv);
	free(LutGu);
	free(LutGv);
	free(LutBu);
}

