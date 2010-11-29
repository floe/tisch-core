#include "Pipeline2.h"
#include "BlobList.h"
#include <GLUTWindow.h>

#include "osc/OscOutboundPacketStream.h"
#include "ip/UdpSocket.h"


GLUTWindow* win;
Filter* tmp = 0;
Pipeline2* mypipe = 0;


int curframe = 0;
int frame = 0;
int lasttime = 0;
int lastframe = 0;


#define OUTPUT_BUFFER_SIZE 0x10000
#define ADDRESS "127.0.0.1"
#define TISCH_PORT_RAW 3333

using namespace osc;

char buffer[OUTPUT_BUFFER_SIZE];
osc::OutboundPacketStream oscOut( buffer, OUTPUT_BUFFER_SIZE );

UdpTransmitSocket* transmitSocket; // ( IpEndpointName( ADDRESS, TISCH_PORT_RAW ) );


void disp() {

	int curtime = glutGet( GLUT_ELAPSED_TIME );
	if ((curtime - lasttime) >= 1000) {
		double fps = (1000.0*(curframe-lastframe))/((double)(curtime-lasttime));
		lasttime  = curtime;
		lastframe = curframe;
		std::cout << "fps: " << fps << std::endl;
	}

	win->clear( );
	win->mode2D();

	tmp->draw( win );
	win->swap( );
}

void keyb( unsigned char c, int, int ) {

	if (c == 'q') exit(0);
	if (c == ' ') mypipe->reset();

	if ((c >= '0') && (c <= '9')) {
		c = c - '0';
		if (c < mypipe->size())
			tmp = (*mypipe)[c];
	}

	glutPostRedisplay();
}

void idle() {

	std::vector<int> alive;

	if (mypipe->process() != 0) curframe++;
	if (curframe == 2) mypipe->reset();

	oscOut << osc::BeginBundleImmediate;

	// frame message
	oscOut << osc::BeginMessage( "/tuio2/frm" ) << curframe << TimeTag(time(NULL)) << osc::EndMessage;

	// blob/pointer messages
	for (std::vector<Filter*>::iterator filter = mypipe->begin(); filter != mypipe->end(); filter++) {
		BlobList* bl = dynamic_cast<BlobList*>(*filter);
		if (bl) bl->send( oscOut, alive );
	}

	// alive message
	oscOut << osc::BeginMessage( "/tuio2/alv" );
	for (std::vector<int>::iterator id = alive.begin(); id != alive.end(); id++) oscOut << *id;
	oscOut << osc::EndMessage;

	//oscOut << osc::EndBundle; ?

	transmitSocket->Send( oscOut.Data(), oscOut.Size() );
	oscOut.Clear();

	glutPostRedisplay();
}


int main( int argc, char* argv[] ) {

	TiXmlDocument doc("test.xml");
	doc.LoadFile();

	mypipe = new Pipeline2( doc.FirstChildElement() );
	tmp = (*mypipe)[0];

	transmitSocket = new UdpTransmitSocket( IpEndpointName( ADDRESS, TISCH_PORT_RAW ) );

	int width  = tmp->getImage()->getWidth();
	int height = tmp->getImage()->getHeight();

	win = new GLUTWindow( width, height, "mypipeline test" );

	glutIdleFunc(idle);
	glutDisplayFunc(disp);
	glutKeyboardFunc(keyb);

	win->run();
}

