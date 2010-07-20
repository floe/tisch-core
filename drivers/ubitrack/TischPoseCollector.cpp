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
 * Multiplication component.
 * This file contains a multiplication of two inputs implemented as a \c TriggerComponent.
 *
 * @author Daniel Pustka <daniel.pustka@in.tum.de>
 */
#include <Ubitrack/Dataflow/TriggerComponent.h>
#include <Ubitrack/Dataflow/TriggerInPort.h>
#include <Ubitrack/Dataflow/TriggerOutPort.h>
#include <Ubitrack/Dataflow/ComponentFactory.h>
#include <Ubitrack/Measurement/Measurement.h>

namespace Ubitrack { namespace Components {

class TischPoseCollector
	: public Dataflow::TriggerComponent
{
public:
	/**
	 * UTQL component constructor.
	 *
	 * @param sName Unique name of the component.
	 * @param subgraph UTQL subgraph
	 */
	TischPoseCollector( const std::string& sName, boost::shared_ptr< Graph::UTQLSubgraph > pConfig )
		: Dataflow::TriggerComponent( sName, pConfig )
		, m_inPortA( "AB", *this )
		, m_inPortB( "AC", *this )
		, m_inPortC( "AD", *this )
		, m_outPort( "AE", *this )

    {
    }

	/** Method that computes the result. */
	void compute( Measurement::Timestamp t )
	{
		std::vector< Math::Pose > list;
		list.push_back( *m_inPortA.get() );
		list.push_back( *m_inPortB.get() );
		list.push_back( *m_inPortC.get() );
		m_outPort.send( Measurement::PoseList( t, list ) );
	}

protected:
	/** Input port A of the component. */
	Dataflow::TriggerInPort< Measurement::Pose > m_inPortA;

	/** Input port B of the component. */
	Dataflow::TriggerInPort< Measurement::Pose > m_inPortB;

	/** Input port C of the component. */
	Dataflow::TriggerInPort< Measurement::Pose > m_inPortC;

	/** Output port of the component. */
	Dataflow::TriggerOutPort< Measurement::PoseList > m_outPort;
};


UBITRACK_REGISTER_COMPONENT( Dataflow::ComponentFactory* const cf ) {
	cf->registerComponent< TischPoseCollector > ( "TischPoseCollector" );
}

} } // namespace Ubitrack::Components
