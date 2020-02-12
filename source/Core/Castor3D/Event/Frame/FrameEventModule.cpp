#include "Castor3D/Event/Frame/FrameEventModule.hpp"

namespace castor3d
{
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
