#include "Castor3D/Scene/Animation/Skeleton/SkeletonAnimationInstanceObject.hpp"

#include "Castor3D/Animation/Skeleton/SkeletonAnimationBone.hpp"
#include "Castor3D/Animation/Skeleton/SkeletonAnimationNode.hpp"
#include "Castor3D/Scene/Animation/Skeleton/SkeletonAnimationInstanceBone.hpp"
#include "Castor3D/Scene/Animation/Skeleton/SkeletonAnimationInstanceNode.hpp"

using namespace castor;

namespace castor3d
{
	SkeletonAnimationInstanceObject::SkeletonAnimationInstanceObject( SkeletonAnimationInstance & animationInstance
		, SkeletonAnimationObject & animationObject
		, SkeletonAnimationInstanceObjectPtrArray & allObjects )
		: OwnedBy< SkeletonAnimationInstance >{ animationInstance }
		, m_animationObject{ animationObject }
	{
		for ( auto moving : animationObject.m_children )
		{
			switch ( moving->getType() )
			{
			case SkeletonAnimationObjectType::eNode:
				{
					auto instance = std::make_shared< SkeletonAnimationInstanceNode >( animationInstance
						, *std::static_pointer_cast< SkeletonAnimationNode >( moving )
						, allObjects );
					m_children.push_back( instance );
					allObjects.push_back( instance );
				}
				break;

			case SkeletonAnimationObjectType::eBone:
				{
					auto instance = std::make_shared< SkeletonAnimationInstanceBone >( animationInstance
						, *std::static_pointer_cast< SkeletonAnimationBone >( moving )
						, allObjects );
					m_children.push_back( instance );
					allObjects.push_back( instance );
				}
				break;
			}
		}
	}

	SkeletonAnimationInstanceObject::~SkeletonAnimationInstanceObject()
	{
	}

	void SkeletonAnimationInstanceObject::addChild( SkeletonAnimationInstanceObjectSPtr object )
	{
		m_children.push_back( object );
	}

	void SkeletonAnimationInstanceObject::update( Matrix4x4r const & current )
	{
		m_cumulativeTransform = current;
		doApply();
	}
}
