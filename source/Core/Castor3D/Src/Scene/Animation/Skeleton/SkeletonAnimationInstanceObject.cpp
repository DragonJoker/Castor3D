#include "SkeletonAnimationInstanceObject.hpp"

#include "SkeletonAnimationInstanceBone.hpp"
#include "SkeletonAnimationInstanceNode.hpp"

#include "Animation/Interpolator.hpp"
#include "Animation/KeyFrame.hpp"
#include "Animation/Skeleton/SkeletonAnimationBone.hpp"
#include "Animation/Skeleton/SkeletonAnimationNode.hpp"

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	namespace
	{
		inline void DoFind( real p_time,
							typename KeyFrameArray::const_iterator const & p_first,
							typename KeyFrameArray::const_iterator const & p_last,
							typename KeyFrameArray::const_iterator & p_prv,
							typename KeyFrameArray::const_iterator & p_cur )
		{
			while ( p_prv != p_first && p_prv->GetTimeIndex() >= p_time )
			{
				// Time has gone too fast backward.
				--p_prv;
				--p_cur;
			}

			while ( p_cur != p_last && p_cur->GetTimeIndex() < p_time )
			{
				// Time has gone too fast forward.
				++p_prv;
				++p_cur;
			}

			ENSURE( p_prv != p_cur );
		}

		String const & GetObjectTypeName( SkeletonAnimationObjectType p_type )
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

	SkeletonAnimationInstanceObject::SkeletonAnimationInstanceObject( SkeletonAnimationInstance & p_animationInstance, SkeletonAnimationObject & p_animationObject, SkeletonAnimationInstanceObjectPtrStrMap & p_allObjects )
		: OwnedBy< SkeletonAnimationInstance >{ p_animationInstance }
		, m_animationObject{ p_animationObject }
		, m_prev{ p_animationObject.GetKeyFrames().empty() ? p_animationObject.GetKeyFrames().end() : p_animationObject.GetKeyFrames().begin() }
		, m_curr{ p_animationObject.GetKeyFrames().empty() ? p_animationObject.GetKeyFrames().end() : p_animationObject.GetKeyFrames().begin() + 1 }
	{
		for ( auto l_moving : p_animationObject.m_children )
		{
			switch ( l_moving->GetType() )
			{
			case SkeletonAnimationObjectType::eNode:
				m_children.push_back( std::make_shared< SkeletonAnimationInstanceNode >( p_animationInstance, *std::static_pointer_cast< SkeletonAnimationNode >( l_moving ), p_allObjects ) );
				p_allObjects.insert( { GetObjectTypeName( l_moving->GetType() ) + l_moving->GetName(), m_children.back() } );
				break;

			case SkeletonAnimationObjectType::eBone:
				m_children.push_back( std::make_shared< SkeletonAnimationInstanceBone >( p_animationInstance, *std::static_pointer_cast< SkeletonAnimationBone >( l_moving ), p_allObjects ) );
				p_allObjects.insert( { GetObjectTypeName( l_moving->GetType() ) + l_moving->GetName(), m_children.back() } );
				break;
			}
		}
	}

	SkeletonAnimationInstanceObject::~SkeletonAnimationInstanceObject()
	{
	}

	void SkeletonAnimationInstanceObject::AddChild( SkeletonAnimationInstanceObjectSPtr p_object )
	{
		m_children.push_back( p_object );
	}

	void SkeletonAnimationInstanceObject::Update( real p_time, Matrix4x4r const & p_transformations )
	{
		if ( m_animationObject.HasKeyFrames() )
		{
			if ( m_animationObject.GetKeyFrames().size() == 1 )
			{
				m_cumulativeTransform = p_transformations * m_prev->GetTransform();
			}
			else
			{
				DoFind( p_time, m_animationObject.GetKeyFrames().begin(), m_animationObject.GetKeyFrames().end() - 1, m_prev, m_curr );
				m_cumulativeTransform = p_transformations * m_curr->GetTransform();
			}
		}
		else
		{
			m_cumulativeTransform = p_transformations * m_animationObject.GetNodeTransform();
		}

		DoApply();

		for ( auto l_object : m_children )
		{
			l_object->Update( p_time, m_cumulativeTransform );
		}
	}

	//*************************************************************************************************
}
