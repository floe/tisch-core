/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <stdlib.h>
#include <nanolibc.h>

#include "Window.h"
#include "Tile.h"
#include "Textbox.h"
#include <Motion.h>
#include <BlobCount.h>

// #define ADD_KBD

class MyContainer: public Container{
	public:

		MyContainer( int _w, int _h, int _x = 0, int _y = 0, double angle = 0.0, RGBATexture* _tex = 0, int mode = 0xFF):
			Container( _w,_h,_x,_y,angle,_tex,mode) 
		{ 
			
			alpha = 1.0;
			for(std::vector<Gesture>::iterator it = region.gestures.begin(); it != region.gestures.end(); it++)
			{
				if(it->name() == "move")
				{
					region.gestures.erase(it);
					break;
				}
			}

			Gesture move( "move" , GESTURE_FLAGS_DEFAULT|GESTURE_FLAGS_STICKY);

			move.push_back(new Motion());
			std::vector<int> b2;
			b2.push_back( 0 );
			b2.push_back( 10000 );
			BlobCount* bcnt = new BlobCount(1<<INPUT_TYPE_FINGER);
			bcnt->bounds( b2 );
			move.push_back( bcnt );
			region.gestures.push_back( move );	
		}

		void action( Gesture* gesture ) {
			if ( gesture->name() == "move" ) {
				
					FeatureBase* f = (*gesture)[1];
					BlobCount* p = dynamic_cast<BlobCount*>(f);
					if (p->result() == 3)
					{
						f = (*gesture)[0];
						Motion* m = dynamic_cast<Motion*>(f);
						Vector tmp = m->result();

						if(tmp.y < 0) alpha -= 0.01f;
						else alpha += 0.01f;
						if(alpha > 1.0) alpha = 1.0;
						else if(alpha < 0.2) alpha = 0.2;
						color(1,1,1,alpha);
						return;
					}
			}
			Container::action(gesture);
		}
		double alpha;
};


int main( int argc, char* argv[] ) {

	std::cout << "picview - libTISCH 2.0 widget layer demo" << std::endl;
	std::cout << "(c) 2011 by Florian Echtler <floe@butterbrot.org>" << std::endl;

	int mouse = ((argc > 1) && (std::string("-m") == argv[1]));
	if (argc <= mouse+1) {
		std::cout << "\nUsage: picview [-m] pic1.png pic2.png ..." << std::endl;
		return 1;
	}

	Window* win = new Window( 800, 480, "PicView", mouse );
	win->texture(0);

	srandom(45890);

	// load the textures
	for (int i = mouse+1; i < argc; i++) {
		RGBATexture* tmp = new RGBATexture( argv[i] );
		MyContainer* foo = new MyContainer( 
			tmp->width(1)/5, 
			tmp->height(1)/5,
			(int)(((double)random()/(double)RAND_MAX)*700-350),
			(int)(((double)random()/(double)RAND_MAX)*450-225),
			(int)(((double)random()/(double)RAND_MAX)*360),
			tmp, 0xFF
		);

		#ifdef ADD_KBD
			double tbw = tmp->width(1)/15;
			foo->add( new Textbox(tbw,75*tbw/500,275*tbw/500, 0, tmp->height()/20) );
		#endif
		win->add( foo );
	}

	win->update();
	win->run();
}

