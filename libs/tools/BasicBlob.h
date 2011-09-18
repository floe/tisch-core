/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _BASICBLOB_H_
#define _BASICBLOB_H_

#include <iostream>
#include <osc/OscTypes.h>
#include "Vector.h"


// generic names
#define INPUT_TYPE_FINGER INPUT_TYPE_FINGER_RI
#define INPUT_TYPE_HAND   INPUT_TYPE_HAND_RP
#define INPUT_TYPE_FOOT   INPUT_TYPE_FOOT_R
#define INPUT_TYPE_ANY    (((unsigned int)1<<INPUT_TYPE_COUNT)-1)

enum InputType {

	INPUT_TYPE_UNKNOWN,

	INPUT_TYPE_FINGER_RI, // right index finger
	INPUT_TYPE_FINGER_RM, // right middle finger
	INPUT_TYPE_FINGER_RR, // right ring finger
	INPUT_TYPE_FINGER_RL, // right little finger
	INPUT_TYPE_FINGER_RT, // right thumb

	INPUT_TYPE_FINGER_LI, // left index finger
	INPUT_TYPE_FINGER_LM, // left middle finger
	INPUT_TYPE_FINGER_LR, // left ring finger
	INPUT_TYPE_FINGER_LL, // left little finger
	INPUT_TYPE_FINGER_LT, // left thumb

	INPUT_TYPE_STYLUS,
	INPUT_TYPE_LASER,
	INPUT_TYPE_MOUSE,
	INPUT_TYPE_TRACKBALL,
	INPUT_TYPE_JOYSTICK,
	INPUT_TYPE_WIIMOTE,
	INPUT_TYPE_EYETRACKER,

	INPUT_TYPE_OBJECT,
	INPUT_TYPE_TOKEN,

	INPUT_TYPE_HAND_RP = 21, // right hand pointing
	INPUT_TYPE_HAND_RO,      // right hand open
	INPUT_TYPE_HAND_RC,      // right hand closed

	INPUT_TYPE_HAND_LP,      // left hand pointing
	INPUT_TYPE_HAND_LO,      // left hand open
	INPUT_TYPE_HAND_LC,      // left hand closed

	INPUT_TYPE_FOOT_R,
	INPUT_TYPE_FOOT_L,

	INPUT_TYPE_HEAD,
	INPUT_TYPE_PERSON,

	// always keep as last - must be <= 31
	INPUT_TYPE_COUNT 
};


class TISCH_SHARED BasicBlob {

	friend TISCH_SHARED std::istream& operator>>( std::istream& s, BasicBlob& b );
	friend TISCH_SHARED std::ostream& operator<<( std::ostream& s, BasicBlob& b );

	public:

		BasicBlob();

		osc::int32 id, pid, type;
		int size, tracked;
		unsigned char value;

		::Vector pos, speed;
		::Vector peak, axis1, axis2;

};

#endif // _BASICBLOB_H_

