#include "Castor3D/Animation/Skeleton/SkeletonAnimationObject.hpp"

#include "Castor3D/Animation/Skeleton/SkeletonAnimation.hpp"

#include "Castor3D/Animation/Skeleton/SkeletonAnimationBone.hpp"
#include "Castor3D/Animation/Skeleton/SkeletonAnimationKeyFrame.hpp"
#include "Castor3D/Animation/Skeleton/SkeletonAnimationNode.hpp"

using namespace castor;

namespace castor3d
{
	SkeletonAnimationObject::SkeletonAnimationObject( SkeletonAnimation & animation
		, SkeletonAnimationObjectType type )
		: OwnedBy< SkeletonAnimation >{ animation }
		, m_type{ type }
	{
	}

	SkeletonAnimationObject::~SkeletonAnimationObject()
	{
	}

	void SkeletonAnimationObject::addChild( SkeletonAnimationObjectSPtr object )
	{
		CU_Require( object.get() != this );
		object->m_parent = shared_from_this();
		m_children.push_back( object );
	}
}
