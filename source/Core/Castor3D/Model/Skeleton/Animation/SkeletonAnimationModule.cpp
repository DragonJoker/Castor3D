#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationModule.hpp"

namespace castor3d
{
	castor::String getName( SkeletonNodeType value )
	{
		switch ( value )
		{
		case SkeletonNodeType::eNode:
			return cuT( "node" );
		case SkeletonNodeType::eBone:
			return cuT( "bone" );
		default:
			CU_Failure( "Unsupported SkeletonNodeType" );
			return castor::cuEmptyString;
		}
	}
}
