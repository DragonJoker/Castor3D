#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationObject.hpp"

CU_ImplementSmartPtr( castor3d, SkeletonAnimationObject )

namespace castor3d
{
	SkeletonAnimationObject::SkeletonAnimationObject( SkeletonAnimation & animation
		, SkeletonNodeType type )
		:castor::OwnedBy< SkeletonAnimation >{ animation }
		, m_type{ type }
	{
	}

	void SkeletonAnimationObject::addChild( SkeletonAnimationObjectRPtr object )
	{
		CU_Require( object != this );
		object->m_parent = this;
		m_children.push_back( object );
	}
}
