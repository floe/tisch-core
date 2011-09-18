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



std::vector<Tile*> node_list;
std::map<Tile*,int> adj_list;


class GraphWin: public Window {

public:

	GraphWin( int w, int h, std::string title, int use_mouse ): Window(w,h,title,use_mouse) {
		shadow = false;
	}

	void draw() {

		enter();

		Widget::paint();

		glLineWidth(2.0);
		glColor4f(0.0,0.0,1.0,1.0);

		for (std::map<Tile*,int>::iterator it = adj_list.begin(); it != adj_list.end(); it++) {
			if (it->second != -1) {
				glBegin(GL_LINES);
					glVertex2f(it->first->x,it->first->y);
					glVertex2f(node_list[it->second]->x,node_list[it->second]->y);
				glEnd();
			}
		}

		MasterContainer::paint();

		leave();
	}
};


int main( int argc, char* argv[] ) {

	std::cout << "graph - libTISCH 2.0 widget layer demo" << std::endl;
	std::cout << "(c) 2011 by Florian Echtler <floe@butterbrot.org>" << std::endl;

	int mouse = ((argc > 1) && (std::string("-m") == argv[1]));
	GraphWin* win = new GraphWin( 800, 480, "Graph", mouse );
	win->texture(0);

	srandom(45890);

	RGBATexture* tmp = new RGBATexture( TISCH_PREFIX "Dial.png" );

	for (int i = 0; i < 20; i++) {
		Tile* foo = new Tile( 50, 50,
			(int)(((double)random()/(double)RAND_MAX)*700-350),
			(int)(((double)random()/(double)RAND_MAX)*450-225),
			0, tmp, TISCH_TILE_MOVE|TISCH_TILE_SLIDE|TISCH_TILE_BOUNCE
		);
		foo->shadow = false;
		win->add( foo );
		node_list.push_back(foo);
		adj_list[foo] = -1;
	}
	
	for (int i = 0; i < 20; i++) {
		int pair = (int)(((double)random()/(double)RAND_MAX)*19);
		if (pair >= 0) adj_list[node_list[i]] = pair;
	}

	win->update();
	win->run();
}

