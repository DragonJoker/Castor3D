#include "Castor3D/Event/Frame/FrameEventModule.hpp"

#include "Castor3D/Event/Frame/CpuFrameEvent.hpp"
#include "Castor3D/Event/Frame/CpuFunctorEvent.hpp"
#include "Castor3D/Event/Frame/FrameListener.hpp"
#include "Castor3D/Event/Frame/GpuFrameEvent.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"

CU_ImplementSmartPtr( c3d, CpuFrameEvent )
CU_ImplementSmartPtr( c3d, FrameListener )
CU_ImplementSmartPtr( c3d, FrameListenerCache )
CU_ImplementSmartPtr( c3d, GpuFrameEvent )
CU_ImplementSmartPtr( c3d, CpuFunctorEvent )
CU_ImplementSmartPtr( c3d, GpuFunctorEvent )

namespace c3d
{
	const String PtrCacheTraitsT< FrameListener, String >::Name = cuT( "FrameListener" );

	String getName( CpuEventType value )
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
			return cuEmptyString;
		}
	}

	String getName( GpuEventType value )
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
			return cuEmptyString;
		}
	}
}
