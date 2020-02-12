#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationModule.hpp"

namespace castor3d
{
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
