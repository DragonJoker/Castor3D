#include "SkeletonAnimationInstance.hpp"

#include "Engine.hpp"

#include "SkeletonAnimationInstanceBone.hpp"
#include "SkeletonAnimationInstanceNode.hpp"

#include "Animation/Skeleton/SkeletonAnimation.hpp"
#include "Animation/Skeleton/SkeletonAnimationNode.hpp"
#include "Animation/Skeleton/SkeletonAnimationBone.hpp"
#include "Mesh/Skeleton/Bone.hpp"
#include "Scene/Animation/AnimatedSkeleton.hpp"

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	namespace
	{
		Castor::String const & GetObjectTypeName( SkeletonAnimationObjectType p_type )
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

	SkeletonAnimationInstance::SkeletonAnimationInstance( AnimatedSkeleton & p_object, SkeletonAnimation const & p_animation )
		: AnimationInstance{ p_object, p_animation }
	{
		for ( auto moving : p_animation.m_arrayMoving )
		{
			switch ( moving->GetType() )
			{
			case SkeletonAnimationObjectType::eNode:
				m_arrayMoving.push_back( std::make_shared< SkeletonAnimationInstanceNode >( *this, *std::static_pointer_cast< SkeletonAnimationNode >( moving ), m_toMove ) );
				m_toMove.insert( { GetObjectTypeName( moving->GetType() ) + moving->GetName(), m_arrayMoving.back() } );
				break;

			case SkeletonAnimationObjectType::eBone:
				m_arrayMoving.push_back( std::make_shared< SkeletonAnimationInstanceBone >( *this, *std::static_pointer_cast< SkeletonAnimationBone >( moving ), m_toMove ) );
				m_toMove.insert( { GetObjectTypeName( moving->GetType() ) + moving->GetName(), m_arrayMoving.back() } );
				break;
			}
		}
	}

	SkeletonAnimationInstance::~SkeletonAnimationInstance()
	{
	}

	SkeletonAnimationInstanceObjectSPtr SkeletonAnimationInstance::GetObject( Bone const & p_bone )const
	{
		return GetObject( SkeletonAnimationObjectType::eBone, p_bone.GetName() );
	}

	SkeletonAnimationInstanceObjectSPtr SkeletonAnimationInstance::GetObject( Castor::String const & p_name )const
	{
		return GetObject( SkeletonAnimationObjectType::eNode, p_name );
	}

	SkeletonAnimationInstanceObjectSPtr SkeletonAnimationInstance::GetObject( SkeletonAnimationObjectType p_type, Castor::String const & p_name )const
	{
		SkeletonAnimationInstanceObjectSPtr result;
		auto it = m_toMove.find( GetObjectTypeName( p_type ) + p_name );

		if ( it != m_toMove.end() )
		{
			result = it->second;
		}
		else
		{
			Logger::LogWarning( cuT( "No object named " ) + p_name + cuT( " for this animation instance" ) );
		}

		return result;
	}

	void SkeletonAnimationInstance::DoUpdate()
	{
		for ( auto moving : m_arrayMoving )
		{
			moving->Update( m_currentTime, Matrix4x4r{ 1 } );
		}
	}

	//*************************************************************************************************
}
