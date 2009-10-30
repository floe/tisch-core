#include "tisch.h"
#include "UDPSocket.h"
#include "BasicBlob.h"
#include <stdlib.h>
#include <stdio.h>

extern "C" {
#ifdef WIN32
__declspec(dllexport) void send_blob(int num, double b_angle, double b_scale, int b_val, int b_x, int b_y)
#else
void send_blob(int num, double b_angle, double b_scale, int b_val, int b_x, int b_y)
#endif
{
    static UDPSocket *s = new UDPSocket(INADDR_ANY, 0);
    s->target(INADDR_LOOPBACK, 0x7AB1);
    BasicBlob *bf = new BasicBlob();
    bf->size = 1;
    bf->value = b_val;
    bf->axis1 = Vector(2,0);
    bf->axis1 = bf->axis1*b_scale;
    bf->axis1.rotate( b_angle );
    bf->axis2 = Vector(0,1);
    bf->axis2 = bf->axis2*b_scale;
    bf->axis2.rotate( b_angle );
    bf->pos.x = b_x;
    bf->pos.y = b_y;
    bf->peak = bf->pos;
    
    if (b_val >= 1) {
        bf->id  = num+1;
        bf->pid = 0;
        //printf("sending1\n");
        //*s << "shadow " << *bf << std::endl;
    }

    if (b_val >= 2) {
        bf->id  = num;
        bf->pid = num+1;
        //printf("sending finger\n");
        *s << "finger " << *bf << std::endl;
    }
    
    //*s << "frame " << framenum << std::endl;
}

#ifdef WIN32
__declspec(dllexport) void send_frame(int num)
#else
void send_frame(int num)
#endif
{
	static UDPSocket *s = new UDPSocket(INADDR_ANY, 0);
	s->target(INADDR_LOOPBACK, 0x7AB1);
	*s << "frame " << num << std::endl;
}
}
