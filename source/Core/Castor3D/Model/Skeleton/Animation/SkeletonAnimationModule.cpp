#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationModule.hpp"

namespace c3d
{
	String getName( SkeletonNodeType value )
	{
		switch ( value )
		{
		case SkeletonNodeType::eNode:
			return cuT( "node" );
		case SkeletonNodeType::eBone:
			return cuT( "bone" );
		default:
			CU_Failure( "Unsupported SkeletonNodeType" );
			return cuEmptyString;
		}
	}
}
