#include "SkeletonAnimationInstanceObject.hpp"

#include "SkeletonAnimationInstanceBone.hpp"
#include "SkeletonAnimationInstanceNode.hpp"

#include "Animation/Skeleton/SkeletonAnimationBone.hpp"
#include "Animation/Skeleton/SkeletonAnimationNode.hpp"

using namespace castor;

namespace castor3d
{
	//*************************************************************************************************

	namespace
	{
	}

	//*************************************************************************************************

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
		//if ( m_animationObject.hasKeyFrames() )
		//{
		//	if ( m_animationObject.getKeyFrames().size() == 1 )
		//	{
		//		m_cumulativeTransform = p_transformations * m_prev->getTransform();
		//	}
		//	else
		//	{
		//		doFind( p_time, m_animationObject.getKeyFrames().begin(), m_animationObject.getKeyFrames().end() - 1, m_prev, m_curr );
		//		m_cumulativeTransform = p_transformations * m_curr->getTransform();
		//	}
		//}
		//else
		//{
		//	m_cumulativeTransform = p_transformations * m_animationObject.getNodeTransform();
		//}

		//doApply();

		//for ( auto object : m_children )
		//{
		//	object->update( p_time, m_cumulativeTransform );
		//}

		m_cumulativeTransform = current;
		doApply();
	}

	//*************************************************************************************************
}
