#include <osc/OscOutboundPacketStream.h>
#include <ip/UdpSocket.h>

#include <tisch.h>
#include <BasicBlob.h>
#include <UDPSocket.h>

#include <string>
#include <stdlib.h>


#define ADDRESS "127.0.0.1"
#define PORT 3333

#define OUTPUT_BUFFER_SIZE 8196

#define MAX_BLOBS 100

int current_frame;
int currentBlobIDs[MAX_BLOBS];
float currentBlobXCoords[MAX_BLOBS];
float currentBlobYCoords[MAX_BLOBS];


void clearBlobIDArray() {
	int i;
	
	for(i = 0; i < MAX_BLOBS; i++) {
		currentBlobIDs[i] = -1;	
	}
}

void updateBlobs(int currentID, float x, float y) {
	int i;
	bool idFound = false;

//	std::cout << "---" << std::endl;
	for(i = 0; currentBlobIDs[i] != -1 && i < MAX_BLOBS; i++) {
	//	std::cout << currentBlobIDs[i] << " ";
		if(currentBlobIDs[i] == currentID) {
			idFound = true;
		}

		//std::cout << std::endl;
	}

	if(!idFound) {
		currentBlobIDs[i] = currentID;
		
		if(y < 0.0) {
			y = 0.0;
		} else if(y > 1.0) {
			y = 1.0;	
		}

		if(x < 0.0) {
			x = 0.0;
		} else if(x > 1.0) {
			x = 1.0;	
		}

		currentBlobYCoords[i] = y;
		currentBlobXCoords[i] = x;
	}
}

void dumpBlobsToOSC(osc::OutboundPacketStream &oscOut, UdpTransmitSocket &transmitSocket) {
	int i;

	bool blobsAlive = false;

   for(i = 0; currentBlobIDs[i] != -1 && i < MAX_BLOBS; i++) {	
				blobsAlive = true;
				current_frame++;
				oscOut  << osc::BeginBundleImmediate;
								
				oscOut	<< osc::BeginMessage( "/tuio/2Dcur" )
								<< "alive";
								int j;
								for(j = 0; currentBlobIDs[j] != -1 && j < MAX_BLOBS; j++) {
										oscOut << currentBlobIDs[j];
										std::cout << "alive : " << currentBlobIDs[j] << std::endl; 
								}
								oscOut << osc::EndMessage;
								std::cout << std::endl;
				oscOut	<< osc::BeginMessage( "/tuio/2Dcur" )
								<< "set"
								<< currentBlobIDs[i] 
								<< currentBlobXCoords[i] 
								<< currentBlobYCoords[i]
								<< 0.0 << 0.0 << 0.0
								<< osc::EndMessage;
								std::cout << currentBlobIDs[i] 
													<< " x: " << currentBlobXCoords[i] 
													<< " y: " << currentBlobYCoords[i] 
													<< std::endl; 

				oscOut 	<< osc::BeginMessage( "/tuio/2Dcur" )
								<< "fseq"
								<< current_frame
								<< osc::EndMessage;
			
				oscOut  << osc::EndBundle;
	
			transmitSocket.Send( oscOut.Data(), oscOut.Size() );
			oscOut.Clear();						
		}

		if(!blobsAlive) {
				current_frame++;
				//std::cout << "all dead" << std::endl;
				oscOut  << osc::BeginBundleImmediate;
				oscOut	<< osc::BeginMessage( "/tuio/2Dcur" )
								<< "alive";
								oscOut << osc::EndMessage;
				
				oscOut 	<< osc::BeginMessage( "/tuio/2Dcur" )
							<< "fseq"
							<< current_frame
							<< osc::EndMessage;
				oscOut << osc::EndBundle;

				transmitSocket.Send( oscOut.Data(), oscOut.Size() );
				oscOut.Clear();						
		}
}

int main(int argc, char* argv[])
{
	std::cout << "TISCHtoTUIO - TISCH library TUIO adapter 1.0" << std::endl;
	std::cout << "(c) 2009 by Andreas Dippon <dippona@in.tum.de>" << std::endl;

	if (argc >= 2) 
	if ((std::string(argv[1]) == "-h") || (std::string(argv[1]) == "-?")) {
		std::cout << "\nUsage: TISCHtoTUIO [target_host [target_port]]\n";
		std::cout << "Defaults: 127.0.0.1:3333\n" << std::endl;
		exit(1);
	}

	//bool lastframe = false;
	//bool cleared = false;
	BasicBlob blob;

	const char* addr = (argc >= 2 ? argv[1] : ADDRESS );
	int port = (argc >= 3 ? atoi(argv[2]) : PORT );

	struct timeval tv = { 1, 0 };
	UDPSocket input( INADDR_ANY, TISCH_PORT_CALIB, &tv );
	UdpTransmitSocket transmitSocket( IpEndpointName( addr, port ) );
	std::cout << "Sending TUIO data to " << addr << ":" << port << "." << std::endl;
    
  char buffer[OUTPUT_BUFFER_SIZE];
	osc::OutboundPacketStream oscOut( buffer, OUTPUT_BUFFER_SIZE );

	current_frame = 0;    

	clearBlobIDArray();

	while(1)
	{
		std::string type;
		input >> type;

		if (!input) {
			input.flush();
			continue;
		}
		
		if(type == "finger")
		{
			float tuio_x;
			float tuio_y;
			int tuio_id;

			input >> blob;

			tuio_x = (blob.pos.x / 1024);
			tuio_y = 1.0  - (blob.pos.y / 768);
			tuio_id = blob.id;

			updateBlobs(tuio_id, tuio_x, tuio_y);
			
		} else if(type == "frame") {
			dumpBlobsToOSC(oscOut, transmitSocket);
			clearBlobIDArray();
		}
	}
}

