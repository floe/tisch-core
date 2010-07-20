/*
 * Ubitrack - Library for Ubiquitous Tracking
 * Copyright 2006, Technische Universitaet Muenchen, and individual
 * contributors as indicated by the @authors tag. See the
 * copyright.txt in the distribution for a full listing of individual
 * contributors.
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA, or see the FSF site: http://www.fsf.org.
 */


/**
 * @ingroup dataflow_components
 * @file
 * TISCHAdapter component.
 * 
 * @author Florian Echtler <echtler@in.tum.de>
 */

#include <tisch.h>

#include <boost/bind.hpp>
//#include <log4cpp/Category.hh>

#include <Ubitrack/Dataflow/PullConsumer.h>
#include <Ubitrack/Dataflow/PushConsumer.h>
#include <Ubitrack/Dataflow/PushSupplier.h>
#include <Ubitrack/Dataflow/Component.h>
#include <Ubitrack/Dataflow/ComponentFactory.h>
#include <Ubitrack/Measurement/Measurement.h>

#include <Ubitrack/Math/Vector.h>
#include <BasicBlob.h>
#include <Vector.h>
#include <Socket.h>

// currently unused
// static log4cpp::Category& logger( log4cpp::Category::getInstance( "Ubitrack.Components.TISCHAdapter" ) );


namespace Ubitrack { namespace Components {

using namespace Dataflow;


template< class EventType > class TISCHAdapter : public Component
{

public:

	TISCHAdapter( const std::string& nm, boost::shared_ptr< Graph::UTQLSubgraph > pCfg )
		: Ubitrack::Dataflow::Component( nm )
	{
		frame = 1;

		std::string targetIP = "127.0.0.1";

		pCfg->m_DataflowAttributes.getAttributeData( "triggerDistance", triggerDistance);
		pCfg->m_DataflowAttributes.getAttributeData( "targetIP", targetIP);

		output = new UDPSocket( INADDR_ANY, 0 );
		output->target( targetIP.c_str(), TISCH_PORT_RAW );

		int portnum = 0;

		// create input ports
		for ( Graph::UTQLSubgraph::EdgeMap::iterator it = pCfg->m_Edges.begin(); it != pCfg->m_Edges.end(); it++ )
			if ( it->second->isInput() ) 
				m_inPorts.push_back(
					boost::shared_ptr< Dataflow::PushConsumer< EventType > >( 
						new Dataflow::PushConsumer< EventType >(
							it->first, *this, boost::bind( &TISCHAdapter::receive, this, _1, portnum++ )
						)
					)
				);

		// initialize blobs
		for (int i = 0; i < portnum; i++) {

			BasicBlob blob;

			blob.size  = 1;
			blob.value = 0;

			blob.id  = i*2+1;
			blob.pid = 0;

			blob.pos   = Vector();
			blob.peak  = Vector();
			blob.axis1 = Vector();
			blob.axis2 = Vector();

			blobs.push_back( blob );
		}
	}

protected:

	typedef std::vector< boost::shared_ptr< Dataflow::PushConsumer< EventType > > > PortList;

	void send()
	{
		// check for waiting events that have not been handled yet
		for (typename PortList::iterator port = m_inPorts.begin(); port != m_inPorts.end(); port++)
			if ((*port)->getQueuedEvents() > 0) return;

		// cycle through all blobs
		for (std::vector<BasicBlob>::iterator blob = blobs.begin(); blob != blobs.end(); blob++) {
			switch (blob->value) {
				case  0: continue;
				case  1: *output << "shadow "; break;
				case  2: *output << "shadow "; *output << *blob << std::endl; *output << "finger "; break;
				case  3: *output << "hand "; break;
				default: break;
			}
			*output << *blob << std::endl;
			blob->value = 0;
		}
		*output << "frame " << frame++ << std::endl;
	}


	/** called when a new item arrives */
	void receive( const EventType& event, int num )
	{
		// default implementation - does nothing
	}

	/** Ports of the component */
	PortList m_inPorts;

	UDPSocket* output;

	double triggerDistance;
	int frame;

	std::vector<BasicBlob> blobs;

};

template<> void TISCHAdapter<Measurement::Pose>::receive( const Measurement::Pose& event, int num )
{	
	Math::Vector<3> pos = event->translation();
	BasicBlob& blob = blobs[num];
	blob.pos.x = pos(0);
	blob.pos.y = pos(1);
	blob.peak  = blob.pos;
	blob.value = 1;
	send();
}

template<> void TISCHAdapter<Measurement::PoseList>::receive( const Measurement::PoseList& event, int num )
{	
	Measurement::PoseList curlist = event;
	Math::Vector<3> blob0 = ((Measurement::Pose)(*curlist)[0])->translation();
	Math::Vector<3> blob1 = ((Measurement::Pose)(*curlist)[1])->translation();

	BasicBlob& blob = blobs[num];
	blob.pos.x = 0.5*(blob0(0) + blob1(0));
	blob.pos.y = 0.5*(blob0(1) + blob1(1));
	blob.peak  = blob.pos;

	double x_dist = fabs(blob0(0) - blob1(0));
	double y_dist = fabs(blob0(1) - blob1(1));
	if(sqrt(x_dist*x_dist + y_dist*y_dist) < triggerDistance)
		blob.value = 2;
	else
		blob.value = 1;

	send();
}

template<> void TISCHAdapter<Measurement::PositionList>::receive( const Measurement::PositionList& event, int num )
{	
	/*Measurement::PositionList curlist = event;
	blobpos[0] = (*curlist)[0];
	blobpos[1] = (*curlist)[1];

	sendBlob();*/
}

UBITRACK_REGISTER_COMPONENT( ComponentFactory* const cf ) 
{
	cf->registerComponent< TISCHAdapter< Measurement::Pose          > > ( "PoseTISCHAdapter"          );
	cf->registerComponent< TISCHAdapter< Measurement::PoseList      > > ( "PoseListTISCHAdapter"      );
	cf->registerComponent< TISCHAdapter< Measurement::Position      > > ( "PositionTISCHAdapter"      );
	cf->registerComponent< TISCHAdapter< Measurement::PositionList  > > ( "PositionListTISCHAdapter"  );
	cf->registerComponent< TISCHAdapter< Measurement::Position2D    > > ( "Position2DTISCHAdapter"    );
	cf->registerComponent< TISCHAdapter< Measurement::PositionList2 > > ( "PositionList2TISCHAdapter" );
}

} } // namespace Ubitrack::Components

