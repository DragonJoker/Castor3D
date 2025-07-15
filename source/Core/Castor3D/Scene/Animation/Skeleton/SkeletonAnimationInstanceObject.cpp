#include "Castor3D/Scene/Animation/Skeleton/SkeletonAnimationInstanceObject.hpp"

#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationBone.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationNode.hpp"
#include "Castor3D/Scene/Animation/Skeleton/SkeletonAnimationInstanceBone.hpp"
#include "Castor3D/Scene/Animation/Skeleton/SkeletonAnimationInstanceNode.hpp"

CU_ImplementSmartPtr( c3d, SkeletonAnimationInstanceObject )

namespace c3d
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
			case SkeletonNodeType::eNode:
				{
					auto instance = makeUniqueDerived< SkeletonAnimationInstanceObject, SkeletonAnimationInstanceNode >( animationInstance
						, static_cast< SkeletonAnimationNode & >( *moving )
						, allObjects );
					addChild( *instance );
					allObjects.push_back( c3d::move( instance ) );
				}
				break;

			case SkeletonNodeType::eBone:
				{
					auto instance = makeUniqueDerived< SkeletonAnimationInstanceObject, SkeletonAnimationInstanceBone >( animationInstance
						, static_cast< SkeletonAnimationBone & >( *moving )
						, allObjects );
					addChild( *instance );
					allObjects.push_back( c3d::move( instance ) );
				}
				break;

			default:
				break;
			}
		}
	}

	void SkeletonAnimationInstanceObject::addChild( SkeletonAnimationInstanceObject & object )
	{
		m_children.push_back( &object );
	}

	void SkeletonAnimationInstanceObject::update( Matrix4x4f const & current )
	{
		m_cumulativeTransform = current;
		doApply();
	}
}
