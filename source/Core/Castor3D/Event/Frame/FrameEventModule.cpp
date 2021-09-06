#include "Castor3D/Event/Frame/FrameEventModule.hpp"

#include "Castor3D/Event/Frame/CpuFrameEvent.hpp"
#include "Castor3D/Event/Frame/CpuFunctorEvent.hpp"
#include "Castor3D/Event/Frame/FrameListener.hpp"
#include "Castor3D/Event/Frame/GpuFrameEvent.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"

CU_ImplementCUSmartPtr( castor3d, CpuFrameEvent )
CU_ImplementCUSmartPtr( castor3d, FrameListener )
CU_ImplementCUSmartPtr( castor3d, FrameListenerCache )
CU_ImplementCUSmartPtr( castor3d, GpuFrameEvent )
CU_ImplementCUSmartPtr( castor3d, CpuFunctorEvent )
CU_ImplementCUSmartPtr( castor3d, GpuFunctorEvent )

namespace castor3d
{
	const castor::String PtrCacheTraitsT< castor3d::FrameListener, castor::String >::Name = cuT( "FrameListener" );

	castor::String getName( EventType value )
	{
		switch ( value )
		{
		case EventType::ePreRender:
			return cuT( "pre_render" );
		case EventType::eQueueRender:
			return cuT( "queue_render" );
		case EventType::ePostRender:
			return cuT( "post_render" );
		default:
			CU_Failure( "Unsupported EventType" );
			return castor::cuEmptyString;
		}
	}
}
