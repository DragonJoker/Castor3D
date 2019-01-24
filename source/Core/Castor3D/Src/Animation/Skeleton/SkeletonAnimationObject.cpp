#include "SkeletonAnimationObject.hpp"

#include "SkeletonAnimation.hpp"

#include "SkeletonAnimationBone.hpp"
#include "SkeletonAnimationKeyFrame.hpp"
#include "SkeletonAnimationNode.hpp"

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
