#include "SkeletonAnimationInstance.hpp"

#include "Engine.hpp"

#include "SkeletonAnimationInstanceBone.hpp"
#include "SkeletonAnimationInstanceNode.hpp"

#include "Animation/Skeleton/SkeletonAnimation.hpp"
#include "Animation/Skeleton/SkeletonAnimationNode.hpp"
#include "Animation/Skeleton/SkeletonAnimationBone.hpp"
#include "Event/Frame/InitialiseEvent.hpp"
#include "Mesh/Skeleton/Bone.hpp"
#include "Scene/Animation/AnimatedSkeleton.hpp"

#include "Scene/Geometry.hpp"

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	namespace
	{
		Castor::String const & GetObjectTypeName( AnimationObjectType p_type )
		{
			static std::map< AnimationObjectType, String > Names
			{
				{ AnimationObjectType::Node, cuT( "Node_" ) },
				{ AnimationObjectType::Bone, cuT( "Bone_" ) },
			};

			return Names[p_type];
		}
	}

	//*************************************************************************************************

	SkeletonAnimationInstance::SkeletonAnimationInstance( AnimatedSkeleton & p_object, SkeletonAnimation const & p_animation )
		: AnimationInstance{ p_object, p_animation }
	{
		for ( auto l_moving : p_animation.m_arrayMoving )
		{
			switch ( l_moving->GetType() )
			{
			case AnimationObjectType::Node:
				m_arrayMoving.push_back( std::make_shared< SkeletonAnimationInstanceNode >( *this, *std::static_pointer_cast< SkeletonAnimationNode >( l_moving ), m_toMove ) );
				m_toMove.insert( { GetObjectTypeName( l_moving->GetType() ) + l_moving->GetName(), m_arrayMoving.back() } );
				break;

			case AnimationObjectType::Bone:
				m_arrayMoving.push_back( std::make_shared< SkeletonAnimationInstanceBone >( *this, *std::static_pointer_cast< SkeletonAnimationBone >( l_moving ), m_toMove ) );
				m_toMove.insert( { GetObjectTypeName( l_moving->GetType() ) + l_moving->GetName(), m_arrayMoving.back() } );
				break;
			}
		}
	}

	SkeletonAnimationInstance::~SkeletonAnimationInstance()
	{
	}

	SkeletonAnimationInstanceObjectSPtr SkeletonAnimationInstance::GetObject( Bone const & p_bone )const
	{
		return GetObject( AnimationObjectType::Bone, p_bone.GetName() );
	}

	SkeletonAnimationInstanceObjectSPtr SkeletonAnimationInstance::GetObject( Castor::String const & p_name )const
	{
		return GetObject( AnimationObjectType::Node, p_name );
	}

	SkeletonAnimationInstanceObjectSPtr SkeletonAnimationInstance::GetObject( AnimationObjectType p_type, Castor::String const & p_name )const
	{
		SkeletonAnimationInstanceObjectSPtr l_return;
		auto l_it = m_toMove.find( GetObjectTypeName( p_type ) + p_name );

		if ( l_it != m_toMove.end() )
		{
			l_return = l_it->second;
		}

		return l_return;
	}

	void SkeletonAnimationInstance::SetInterpolationMode( InterpolatorType p_mode )
	{
		for ( auto l_moving : m_arrayMoving )
		{
			l_moving->SetInterpolationMode( p_mode );
		}
	}

	void SkeletonAnimationInstance::DoUpdate( real p_tslf )
	{
		for ( auto l_moving : m_arrayMoving )
		{
			l_moving->Update( m_currentTime, Matrix4x4r{ 1 } );
		}
	}

	//*************************************************************************************************
}
