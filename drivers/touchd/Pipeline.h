/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006,07,08 by Florian Echtler, TUM <echtler@in.tum.de>   *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _PIPELINE_H_
#define _PIPELINE_H_

#include "Settings.h"
#include "ImageSet.h"
#include "Camera.h"
#include "GLUTWindow.h"
#include "BlobList.h"
#include <sstream>
#include "osc/OscOutboundPacketStream.h"
#include "ip/UdpSocket.h"

#define OUTPUT_BUFFER_SIZE 8196

class Pipeline {

	public:

		 Pipeline( PipelineParameters* _pipe );
		~Pipeline();

		void acquire( int& intensity, unsigned long long int &timestamp );
		void process();
		void draw( GLUTWindow* win, int num );
		void send( osc::OutboundPacketStream& oscOut );
		void update();
		void correlate( Pipeline* parents );
		void swap( Pipeline* other );

	private:

		PipelineParameters* pipe;

		ImageSet* imageset;
		BlobList* bloblist;
};

#endif // _PIPELINE_H_

