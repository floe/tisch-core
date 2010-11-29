#include "Pipeline2.h"
#include "BlobList.h"
#include <GLUTWindow.h>

#include "osc/OscOutboundPacketStream.h"
#include "ip/UdpSocket.h"

int width  = 640;
int height = 480;

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

UdpTransmitSocket transmitSocket( IpEndpointName( ADDRESS, TISCH_PORT_RAW ) );

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

	glColor4f( 1.0, 0.0, 0.0, 1.0 ); 
	win->print( "foobar", 5, 5 );

	if (tmp) tmp->draw( win );
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

	if (mypipe->process() != 0) curframe++;
	if (curframe == 2) mypipe->reset();

	TimeTag current_time = TimeTag(time(NULL));

	oscOut  << osc::BeginBundleImmediate;
	oscOut	<< osc::BeginMessage( "/tuio2/frm" )
			<< curframe
			<< current_time
			<< osc::EndMessage;

	for (std::vector<Filter*>::iterator filter = mypipe->begin(); filter != mypipe->end(); filter++) {
		BlobList* bl;
		if ((bl = dynamic_cast<BlobList*>(*filter)) != 0) bl->sendBlobs(oscOut);
	}

	oscOut << osc::BeginMessage( "/tuio2/alv" ) << 0 << osc::EndMessage;

	transmitSocket.Send( oscOut.Data(), oscOut.Size() );
	oscOut.Clear();

	glutPostRedisplay();
}

int main( int argc, char* argv[] ) {

	win = new GLUTWindow( width, height, "mypipeline test" );

	TiXmlDocument doc("test.xml");
	doc.LoadFile();

	mypipe = new Pipeline2( doc.FirstChildElement() );

	glutIdleFunc(idle);
	glutDisplayFunc(disp);
	glutKeyboardFunc(keyb);

	win->run();

}

