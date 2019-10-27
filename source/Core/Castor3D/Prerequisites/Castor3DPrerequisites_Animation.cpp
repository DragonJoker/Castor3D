#include "Castor3D/Castor3DPrerequisites.hpp"

namespace castor3d
{
	castor::String getName( AnimationType value )
	{
		switch ( value )
		{
		case AnimationType::eMovable:
			return cuT( "movable" );
		case AnimationType::eSkeleton:
			return cuT( "skeleton" );
		case AnimationType::eMesh:
			return cuT( "mesh" );
		default:
			CU_Failure( "Unsupported AnimationType" );
			return castor::cuEmptyString;
		}
	}

	castor::String getName( InterpolatorType value )
	{
		switch ( value )
		{
		case InterpolatorType::eNearest:
			return cuT( "nearest" );
		case InterpolatorType::eLinear:
			return cuT( "linear" );
		default:
			CU_Failure( "Unsupported InterpolatorType" );
			return castor::cuEmptyString;
		}
	}

	castor::String getName( AnimationState value )
	{
		switch ( value )
		{
		case AnimationState::ePlaying:
			return cuT( "playing" );
		case AnimationState::eStopped:
			return cuT( "stopped" );
		case AnimationState::ePaused:
			return cuT( "paused" );
		default:
			CU_Failure( "Unsupported AnimationState" );
			return castor::cuEmptyString;
		}
	}

	castor::String getName( SkeletonAnimationObjectType value )
	{
		switch ( value )
		{
		case SkeletonAnimationObjectType::eNode:
			return cuT( "node" );
		case SkeletonAnimationObjectType::eBone:
			return cuT( "bone" );
		default:
			CU_Failure( "Unsupported SkeletonAnimationObjectType" );
			return castor::cuEmptyString;
		}
	}
}
