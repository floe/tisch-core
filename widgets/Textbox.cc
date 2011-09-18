/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "Textbox.h"
#include <algorithm>
#include <sstream>

#include <BlobCount.h>
#include <BlobPos.h>
#include <BlobID.h>


// (global) keyboard descriptors
// TODO: should be part of the class

// codes:
// \b backspace
// \001 .. \006 switch to layout #x
// \n close

typedef struct {
	int size,slots;
	//double offset;
	const char* keys;
} kbd_row;

typedef struct {
	RGBATexture* kbd_tex;
	kbd_row kbd_rows[4];
} keyboard;

static keyboard lower = {
	0,
	{
		{ 10, 10, "qwertyuiop" },
		{  9, 10, "asdfghjkl"  },
		{  9, 10, "\002zxcvbnm\b" },
		{ 10, 10, "\003\003,    .\n\n" }
	}
};

static keyboard upper = {
	0,
	{
		{ 10, 10, "QWERTYUIOP" },
		{  9, 10, "ASDFGHJKL"  },
		{  9, 10, "\001ZXCVBNM\b" },
		{ 10, 10, "\003\003,    .\n\n" }
	}
};

static keyboard num = {
	0,
	{
		{ 10, 10, "1234567890" },
		{  9, 10, "!\"$%&/()?" },
		{  9, 10, "\003-_+*=:;\b" },
		{ 10, 10, "\001\001,    .\n\n" }
	}
};

static keyboard keyboards[] = { lower, upper, num };


Textbox::Textbox( int _w, int _h0, int _h1, int _x, int _y, double _angle, RGBATexture* _tex ):
	Widget(_w,_h0,_x,_y,_angle,_tex),
	h0(_h0), h1(_h1), active(0), kbd(0),
	text("")
{
	getDefaultTex( _tex, "Textbox.png" );

	for (unsigned int i = 0; i < (sizeof(keyboards)/sizeof(keyboard)); i++)
		if (keyboards[i].kbd_tex == 0) {
			std::ostringstream os; os << "Keyboard" << i << ".png";
			keyboards[i].kbd_tex = new RGBATexture( (std::string(TISCH_PREFIX).append(os.str())).c_str() );
			//std::cout << "loaded kbd " << i << ": " << keyboards[i].kbd_tex << std::endl;
		}

	Gesture tap( "tap" ); region.gestures.push_back( tap );
	// Gesture release( "release" ); region.gestures.push_back( release );

	region.flags( REGION_FLAGS_VOLATILE | (1<<INPUT_TYPE_FINGER) );

	if (h0 > h1) std::swap(h0,h1);
	diff = h1-h0;
}

Textbox::~Textbox() { }


void Textbox::draw() {

	double h_old = h;

	enter();

	// draw the keyboard
	if (active) {
		h = h1-h0;
		glTranslated( 0, -h0/2, 0 );
		paint_tex( keyboards[kbd].kbd_tex );
		glTranslated( 0, h1/2, 0 );
	}

	// draw the box itself
	h = h0; paint_tex( mytex ); h = h_old;

	glTranslated( 0, 1.0, 0 ); // TODO: why offset +1 needed?
	text.append("|");
	glutDrawString( text, w*0.95, h0*0.5, 0, 1 );
	text.erase( text.end()-1 );

	leave();
}


void Textbox::set( std::string _text ) { text = _text; }
std::string Textbox::get() { return text; }


void Textbox::action( Gesture* gesture ) {

	if (gesture->name() == "tap" ) {

		FeatureBase* f = (*gesture)[1];
		BlobPos* p = dynamic_cast<BlobPos*>(f);

		Vector pos = p->result();
		transform( pos, 1 );

		if ((pos.y + h0/2 - active*diff/2) > 0) {

			// click in the text field: toggle keyboard
			if (active) { h -= diff; y += diff/2; }
			       else { h += diff; y -= diff/2; }

			active = !active;

		} else {

			// click on the keyboard: find key
			pos.x += w/2;
			pos.y += h1/2;

			// find row
			kbd_row* kbd_rows = keyboards[kbd].kbd_rows;
			int rowcount  = 4; //sizeof(*kbd_rows)/sizeof(kbd_row);
			double rowheight = diff/rowcount;

			int row = (rowcount-1) - (int)floor(pos.y/rowheight);
			if ((row < 0) || (row >= rowcount)) return;

			// find column
			int colcount = kbd_rows[row].slots;
			int keycount = kbd_rows[row].size;
			double colwidth = w/colcount;

			int key = (int)floor( pos.x/colwidth - 0.5*(colcount-keycount) );
			if ((key < 0) || (key >= keycount)) return;

			// process key
			char tmp[2] = { 0, 0 };
			tmp[0] = kbd_rows[row].keys[key];

			switch (tmp[0]) {

				case 1: kbd = 0; break;
				case 2: kbd = 1; break;
				case 3: kbd = 2; break;

				case '\b': // backspace
					if (!text.empty()) text.erase( text.end()-1 );
					break;

				case '\n': // close
					if (active) {
						active = 0;
						h -= diff;
						y += diff/2; 
					}
					break;

				default:
					text.append(tmp);
					break;
			}
		}
	}

	/*if (gesture->name() == "release" ) {
		release();
	}*/
}

