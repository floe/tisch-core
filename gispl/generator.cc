/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "Gesture.h"
#include "Factory.h"
#include "InputState.h"


int main( int argc, char* argv[] ) {

	Gesture everything("Everything");
	InputState inputstate;

	for (std::map<std::string,FeatureConstructor>::iterator feature = g_factory().begin(); feature != g_factory().end(); feature++) {
		//std::cout << feature->first << std::endl;
		FeatureBase* fb = createFeature( feature->first );
		everything.push_back( fb );
	}

	BasicBlob blob;
	blob.id = 123;
	inputstate[INPUT_TYPE_FINGER][123].add(blob);

	//while (true)
	{

		everything.load( inputstate );
		std::cout << everything << std::endl;

	}

}
