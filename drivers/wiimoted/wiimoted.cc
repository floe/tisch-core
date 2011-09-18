/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#ifndef _MSC_VER
#include <unistd.h>

#else
#include <winsock2.h>
#include <windows.h>
#endif

#include <wiiuse.h>
#include <tisch.h>
#include <BasicBlob.h>
#include <Socket.h>


#define MAX_WIIMOTES 4

wiimote** wiimotes;

UDPSocket out( INADDR_ANY, 0 );
BasicBlob blobs[16];


/*void handle_event(struct wiimote_t* wm) {

	// if a button is pressed, report it 
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_A))		printf("A pressed\n");
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_B))		printf("B pressed\n");
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_UP))		printf("UP pressed\n");
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_DOWN))	printf("DOWN pressed\n");
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_LEFT))	printf("LEFT pressed\n");
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_RIGHT))	printf("RIGHT pressed\n");
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_MINUS))	printf("MINUS pressed\n");
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_PLUS))	printf("PLUS pressed\n");
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_ONE))		printf("ONE pressed\n");
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_TWO))		printf("TWO pressed\n");
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_HOME))	printf("HOME pressed\n");

	// Pressing minus will tell the wiimote we are no longer interested in movement.
	if (IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_MINUS))
		wiiuse_motion_sensing(wm, 0);

	// Pressing plus will tell the wiimote we are interested in movement.
	if (IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_PLUS))
		wiiuse_motion_sensing(wm, 1); 

	// Pressing B will toggle the rumble
	if (IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_B))
		wiiuse_toggle_rumble(wm);

	if (IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_UP))
		wiiuse_set_ir(wm, 1);
	if (IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_DOWN))
		wiiuse_set_ir(wm, 0);

	// if the accelerometer is turned on then print angles
	if (WIIUSE_USING_ACC(wm)) {
		printf("wiimote roll  = %f [%f]\n", wm->orient.roll, wm->orient.a_roll);
		printf("wiimote pitch = %f [%f]\n", wm->orient.pitch, wm->orient.a_pitch);
		printf("wiimote yaw   = %f\n", wm->orient.yaw);
	}

	if (WIIUSE_USING_IR(wm)) {
		// go through each of the 4 possible IR sources
		for (int i = 0; i < 4; ++i) {
			// check if the source is visible
			if (wm->ir.dot[i].visible)
				printf("IR source %i: (%u, %u)\n", i, wm->ir.dot[i].x, wm->ir.dot[i].y);
		}
	}
}*/


void handle_event( wiimote_t* wm, int num ) {

	int send_finger = 0;
	int source_cnt = 0;
	BasicBlob& foo = blobs[wm->unid];

	foo.size  = 1;
	foo.value = 1;
	foo.axis1 = Vector(2,0);
	foo.axis2 = Vector(0,1);

	if (IS_PRESSED( wm, WIIMOTE_BUTTON_A )) send_finger += 1;
	if (IS_PRESSED( wm, WIIMOTE_BUTTON_B ))	send_finger += 1;

	ir_dot_t ir1 = { 0, 0, 0, 0, 0, 0, 0 };
	ir_dot_t ir2 = { 0, 0, 0, 0, 0, 0, 0 };

	// go through each of the 4 possible IR sources, find the two biggest ones
	for (int j = 0; j < 4; ++j) {
		if ((wm->ir.dot[j].visible) && (wm->ir.dot[j].size > ir1.size)) {
			ir2 = ir1;
			ir1 = wm->ir.dot[j];
			source_cnt++;
		}
	}

	if (!ir1.visible) { foo.size = 0; return; } // no points
	if (!ir2.visible) ir2 = ir1; // only one point

	double x = (ir1.rx+ir2.rx)/2.0;
	double y = 767-(ir1.ry+ir2.ry)/2.0;

	foo.id = wm->unid*4;
	foo.pid = 0;
	foo.size = source_cnt;
	foo.pos.x = 1400*x/1024;
	foo.pos.y = 1050*y/768;
	foo.peak = foo.pos;
	foo.value = send_finger;
	foo.tracked = 20;
}


int main( int argc, char* argv[] ) {

	std::cout << "wiimoted - libTISCH 2.0 Wiimote adapter" << std::endl;
	std::cout << "(c) 2011 by Florian Echtler <floe@butterbrot.org>\n" << std::endl;

	out.target( INADDR_LOOPBACK, TISCH_PORT_CALIB );

	// connect wiimotes
	wiimotes = wiiuse_init( MAX_WIIMOTES );

	int found = wiiuse_find( wiimotes, MAX_WIIMOTES, 10 ); // 10 = timeout
	if (!found) {
		std::cout << "\nNo Wiimote(s) found." << std::endl;
		return 0;
	}

	int connected = wiiuse_connect( wiimotes, MAX_WIIMOTES );
	if (!connected) {
		std::cout << "\nFailed to connect to any Wiimote." << std::endl;
		return 0;
	}

	//std::cout << "\nConnected " << connected << " Wiimote(s) out of " << found << " total." << std::endl;
	std::cout << std::endl;

	// set LEDs
	wiiuse_set_leds( wiimotes[0], WIIMOTE_LED_1 );
	wiiuse_set_leds( wiimotes[1], WIIMOTE_LED_2 );
	wiiuse_set_leds( wiimotes[2], WIIMOTE_LED_3 );
	wiiuse_set_leds( wiimotes[3], WIIMOTE_LED_4 );

	// enable IR 
	wiiuse_set_ir( wiimotes[0], 1 );
	wiiuse_set_ir( wiimotes[1], 1 );
	wiiuse_set_ir( wiimotes[2], 1 );
	wiiuse_set_ir( wiimotes[3], 1 );

	// enable accelerometer
	wiiuse_motion_sensing( wiimotes[0], 1 );
	wiiuse_motion_sensing( wiimotes[1], 1 );
	wiiuse_motion_sensing( wiimotes[2], 1 );
	wiiuse_motion_sensing( wiimotes[3], 1 );

	// disable thresholds, receive all updates at maximum speed
	wiiuse_set_accel_threshold( wiimotes[0], 0 ); wiiuse_set_orient_threshold( wiimotes[0], 0 );
	wiiuse_set_accel_threshold( wiimotes[1], 0 ); wiiuse_set_orient_threshold( wiimotes[1], 0 );
	wiiuse_set_accel_threshold( wiimotes[2], 0 ); wiiuse_set_orient_threshold( wiimotes[2], 0 );
	wiiuse_set_accel_threshold( wiimotes[3], 0 ); wiiuse_set_orient_threshold( wiimotes[3], 0 );

	// main loop
	int frame = 0;
	//int idle_frame = 0;

	while (1) {

		int res = wiiuse_poll( wiimotes, MAX_WIIMOTES );
		if (!res) continue;

		/*if (!res) {
			idle_frame++;
			// idle frames sent at approx. 20 Hz, experimentally determined to be slow
			// enough to avoid tracking dropouts due to prematurely inserted frames
			if (idle_frame >= 100) {
				idle_frame = 0;
				out << "frame " << frame++ << std::endl;
			}
			continue;
		}*/

		for (int i = 0; i < MAX_WIIMOTES; ++i) {

			wiimote_t* wm = wiimotes[i];

			switch (wm->event) {

				case WIIUSE_EVENT:
					handle_event( wm, i );			
					break;

				case WIIUSE_STATUS:
					// a status event occured - happens once at startup
					std::cout << "Wiimote " << wm->unid << ": battery level " << wm->battery_level*100.0 << "%." << std::endl; 
					break;

				case WIIUSE_DISCONNECT:
				case WIIUSE_UNEXPECTED_DISCONNECT:
					// the wiimote disconnected
					//handle_disconnect(wiimotes[i]);
					std::cout << "Wiimote " << wm->unid << " disconnected." << std::endl;
					break;

				default:
					break;
			}

			// check base blob & timeout
			BasicBlob& foo = blobs[i];
			foo.tracked--; if (!foo.tracked) foo.size = 0;
			if (!foo.size) continue;
			out << "shadow " << foo << std::endl;

			// send optional finger blobs
			BasicBlob tmp = foo;

			if (tmp.value == 0) continue;
			tmp.pid = tmp.id; tmp.id++;
			out << "finger " << tmp << std::endl;

			if (tmp.value <= 1) continue;
			tmp.id++;
			out << "finger " << tmp << std::endl;
		}

		out << "frame " << frame++ << std::endl;
		//idle_frame = 0;
	}

	wiiuse_cleanup( wiimotes, MAX_WIIMOTES );

	return 0;
}

