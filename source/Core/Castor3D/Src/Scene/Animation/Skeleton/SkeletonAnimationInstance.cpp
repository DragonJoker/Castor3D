#include "SkeletonAnimationInstance.hpp"

#include "Engine.hpp"

#include "SkeletonAnimationInstanceBone.hpp"
#include "SkeletonAnimationInstanceNode.hpp"

#include "Animation/Skeleton/SkeletonAnimation.hpp"
#include "Animation/Skeleton/SkeletonAnimationNode.hpp"
#include "Animation/Skeleton/SkeletonAnimationBone.hpp"
#include "Mesh/Skeleton/Bone.hpp"
#include "Scene/Animation/AnimatedSkeleton.hpp"

using namespace castor;

namespace castor3d
{
	//*************************************************************************************************

	namespace
	{
		castor::String const & getObjectTypeName( SkeletonAnimationObjectType p_type )
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
			switch ( moving->getType() )
			{
			case SkeletonAnimationObjectType::eNode:
				m_arrayMoving.push_back( std::make_shared< SkeletonAnimationInstanceNode >( *this, *std::static_pointer_cast< SkeletonAnimationNode >( moving ), m_toMove ) );
				m_toMove.insert( { getObjectTypeName( moving->getType() ) + moving->getName(), m_arrayMoving.back() } );
				break;

			case SkeletonAnimationObjectType::eBone:
				m_arrayMoving.push_back( std::make_shared< SkeletonAnimationInstanceBone >( *this, *std::static_pointer_cast< SkeletonAnimationBone >( moving ), m_toMove ) );
				m_toMove.insert( { getObjectTypeName( moving->getType() ) + moving->getName(), m_arrayMoving.back() } );
				break;
			}
		}
	}

	SkeletonAnimationInstance::~SkeletonAnimationInstance()
	{
	}

	SkeletonAnimationInstanceObjectSPtr SkeletonAnimationInstance::getObject( Bone const & p_bone )const
	{
		return getObject( SkeletonAnimationObjectType::eBone, p_bone.getName() );
	}

	SkeletonAnimationInstanceObjectSPtr SkeletonAnimationInstance::getObject( castor::String const & p_name )const
	{
		return getObject( SkeletonAnimationObjectType::eNode, p_name );
	}

	SkeletonAnimationInstanceObjectSPtr SkeletonAnimationInstance::getObject( SkeletonAnimationObjectType p_type, castor::String const & p_name )const
	{
		SkeletonAnimationInstanceObjectSPtr result;
		auto it = m_toMove.find( getObjectTypeName( p_type ) + p_name );

		if ( it != m_toMove.end() )
		{
			result = it->second;
		}
		else
		{
			Logger::logWarning( cuT( "No object named " ) + p_name + cuT( " for this animation instance" ) );
		}

		return result;
	}

	void SkeletonAnimationInstance::doUpdate()
	{
		for ( auto moving : m_arrayMoving )
		{
			moving->update( m_currentTime, Matrix4x4r{ 1 } );
		}
	}

	//*************************************************************************************************
}
