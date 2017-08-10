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
		inline void doFind( Milliseconds const & p_time
			, typename KeyFrameArray::const_iterator const & p_first
			, typename KeyFrameArray::const_iterator const & p_last
			, typename KeyFrameArray::const_iterator & p_prv
			, typename KeyFrameArray::const_iterator & p_cur )
		{
			while ( p_prv != p_first && p_prv->getTimeIndex() >= p_time )
			{
				// Time has gone too fast backward.
				--p_prv;
				--p_cur;
			}

			while ( p_cur != p_last && p_cur->getTimeIndex() < p_time )
			{
				// Time has gone too fast forward.
				++p_prv;
				++p_cur;
			}

			ENSURE( p_prv != p_cur );
		}

		String const & getObjectTypeName( SkeletonAnimationObjectType p_type )
		{
			static std::map< SkeletonAnimationObjectType, String > Names
			{
				{ SkeletonAnimationObjectType::eNode, cuT( "Node_" ) },
				{ SkeletonAnimationObjectType::eBone, cuT( "Bone_" ) },
			};

			return Names[p_type];
		}
	}

	//*************************************************************************************************

	SkeletonAnimationInstanceObject::SkeletonAnimationInstanceObject( SkeletonAnimationInstance & p_animationInstance
		, SkeletonAnimationObject & p_animationObject
		, SkeletonAnimationInstanceObjectPtrStrMap & p_allObjects )
		: OwnedBy< SkeletonAnimationInstance >{ p_animationInstance }
		, m_animationObject{ p_animationObject }
		, m_prev{ p_animationObject.getKeyFrames().empty() ? p_animationObject.getKeyFrames().end() : p_animationObject.getKeyFrames().begin() }
		, m_curr{ p_animationObject.getKeyFrames().empty() ? p_animationObject.getKeyFrames().end() : p_animationObject.getKeyFrames().begin() + 1 }
	{
		for ( auto moving : p_animationObject.m_children )
		{
			switch ( moving->getType() )
			{
			case SkeletonAnimationObjectType::eNode:
				m_children.push_back( std::make_shared< SkeletonAnimationInstanceNode >( p_animationInstance, *std::static_pointer_cast< SkeletonAnimationNode >( moving ), p_allObjects ) );
				p_allObjects.insert( { getObjectTypeName( moving->getType() ) + moving->getName(), m_children.back() } );
				break;

			case SkeletonAnimationObjectType::eBone:
				m_children.push_back( std::make_shared< SkeletonAnimationInstanceBone >( p_animationInstance, *std::static_pointer_cast< SkeletonAnimationBone >( moving ), p_allObjects ) );
				p_allObjects.insert( { getObjectTypeName( moving->getType() ) + moving->getName(), m_children.back() } );
				break;
			}
		}
	}

	SkeletonAnimationInstanceObject::~SkeletonAnimationInstanceObject()
	{
	}

	void SkeletonAnimationInstanceObject::addChild( SkeletonAnimationInstanceObjectSPtr p_object )
	{
		m_children.push_back( p_object );
	}

	void SkeletonAnimationInstanceObject::update( Milliseconds const & p_time
		, Matrix4x4r const & p_transformations )
	{
		if ( m_animationObject.hasKeyFrames() )
		{
			if ( m_animationObject.getKeyFrames().size() == 1 )
			{
				m_cumulativeTransform = p_transformations * m_prev->getTransform();
			}
			else
			{
				doFind( p_time, m_animationObject.getKeyFrames().begin(), m_animationObject.getKeyFrames().end() - 1, m_prev, m_curr );
				m_cumulativeTransform = p_transformations * m_curr->getTransform();
			}
		}
		else
		{
			m_cumulativeTransform = p_transformations * m_animationObject.getNodeTransform();
		}

		doApply();

		for ( auto object : m_children )
		{
			object->update( p_time, m_cumulativeTransform );
		}
	}

	//*************************************************************************************************
}
