#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationObject.hpp"

namespace castor3d
{
	SkeletonAnimationObject::SkeletonAnimationObject( SkeletonAnimation & animation
		, SkeletonAnimationObjectType type )
		:castor::OwnedBy< SkeletonAnimation >{ animation }
		, m_type{ type }
	{
	}

	void SkeletonAnimationObject::addChild( SkeletonAnimationObjectSPtr object )
	{
		CU_Require( object.get() != this );
		object->m_parent = shared_from_this();
		m_children.push_back( object );
	}
}
