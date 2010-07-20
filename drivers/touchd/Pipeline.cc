/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006,07,08 by Florian Echtler, TUM <echtler@in.tum.de>   *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "Pipeline.h"


Pipeline::Pipeline( PipelineParameters* _pipe ):
	pipe(_pipe)
{
	imageset = new ImageSet( &(pipe->settings) );
	bloblist = new BlobList( &(pipe->settings) );
}


Pipeline::~Pipeline() {
	delete imageset;
	delete bloblist;
}


void Pipeline::acquire( int& intensity, unsigned long long int &timestamp ) {

	IntensityImage* target = imageset->getRaw();
	pipe->camera->apply( &(pipe->settings.camera) );
	pipe->camera->acquire( target );

	//target->histogram( hist );
	//for (int i = 0; i < 256; i++) sum += (hist[i]*i);
	//int hist[256];
	//int sum = target->intensity();
	//sum /= pipe->settings.video->width * pipe->settings.video->height;

	intensity = imageset->analyze();
	timestamp = target->timestamp();
}


void Pipeline::process() {

	BlobList* oldbl = bloblist;

	imageset->process();

	bloblist = new BlobList( &(pipe->settings), imageset->getFinal() );

	bloblist->track( oldbl );
	delete oldbl;
}


void Pipeline::send( ) {
	*(pipe->output) << *bloblist;
}

void Pipeline::update() {
	imageset->update();
}

void Pipeline::correlate( Pipeline* parents ) {
	bloblist->correlate( parents->bloblist );
}

void Pipeline::swap( Pipeline* other ) {
	imageset->swap( other->imageset );
}


void Pipeline::draw( GLUTWindow* win, int num ) {

	std::string info("");

	if (num != 0) {
		info = pipe->settings.name; info += ": ";
		info += imageset->draw( win, num );
	}

	win->mode2D();
	glColor4f( 1.0, 0.0, 0.0, 1.0 ); 
	win->print( info, 5, 5 );

	bloblist->draw( win );
}

