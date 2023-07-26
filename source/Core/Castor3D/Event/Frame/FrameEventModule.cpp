#include "Castor3D/Event/Frame/FrameEventModule.hpp"

#include "Castor3D/Event/Frame/CpuFrameEvent.hpp"
#include "Castor3D/Event/Frame/CpuFunctorEvent.hpp"
#include "Castor3D/Event/Frame/FrameListener.hpp"
#include "Castor3D/Event/Frame/GpuFrameEvent.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"

CU_ImplementSmartPtr( castor3d, CpuFrameEvent )
CU_ImplementSmartPtr( castor3d, FrameListener )
CU_ImplementSmartPtr( castor3d, FrameListenerCache )
CU_ImplementSmartPtr( castor3d, GpuFrameEvent )
CU_ImplementSmartPtr( castor3d, CpuFunctorEvent )
CU_ImplementSmartPtr( castor3d, GpuFunctorEvent )

namespace castor3d
{
	const castor::String PtrCacheTraitsT< castor3d::FrameListener, castor::String >::Name = cuT( "FrameListener" );

	castor::String getName( CpuEventType value )
	{
		switch ( value )
		{
		case CpuEventType::ePreGpuStep:
			return cuT( "pre_gpu_step" );
		case CpuEventType::ePreCpuStep:
			return cuT( "pre_cpu_step" );
		case CpuEventType::ePostCpuStep:
			return cuT( "post_cpu_step" );
		default:
			CU_Failure( "Unsupported CpuEventType" );
			return castor::cuEmptyString;
		}
	}

	castor::String getName( GpuEventType value )
	{
		switch ( value )
		{
		case GpuEventType::ePreUpload:
			return cuT( "pre_upload" );
		case GpuEventType::ePreRender:
			return cuT( "pre_render" );
		case GpuEventType::ePostRender:
			return cuT( "post_render" );
		default:
			CU_Failure( "Unsupported GpuEventType" );
			return castor::cuEmptyString;
		}
	}
}
