#include "Castor3D/Scene/Animation/Skeleton/SkeletonAnimationInstance.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimation.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationBone.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationKeyFrame.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationNode.hpp"
#include "Castor3D/Model/Skeleton/Bone.hpp"
#include "Castor3D/Scene/Animation/AnimatedSkeleton.hpp"
#include "Castor3D/Scene/Animation/Skeleton/SkeletonAnimationInstanceBone.hpp"
#include "Castor3D/Scene/Animation/Skeleton/SkeletonAnimationInstanceNode.hpp"

using namespace castor;

namespace castor3d
{
	//*************************************************************************************************

	namespace
	{
		castor::String const & getObjectTypeName( SkeletonAnimationObjectType type )
		{
			static std::map< SkeletonAnimationObjectType, String > Names
			{
				{ SkeletonAnimationObjectType::eNode, cuT( "Node_" ) },
				{ SkeletonAnimationObjectType::eBone, cuT( "Bone_" ) },
			};

			return Names[type];
		}
	}

	//*************************************************************************************************

	SkeletonAnimationInstance::SkeletonAnimationInstance( AnimatedSkeleton & object
		, SkeletonAnimation & animation )
		: AnimationInstance{ object, animation }
	{
		for ( auto moving : animation.m_arrayMoving )
		{
			switch ( moving->getType() )
			{
			case SkeletonAnimationObjectType::eNode:
				{
					auto instance = std::make_shared< SkeletonAnimationInstanceNode >( *this
						, *std::static_pointer_cast< SkeletonAnimationNode >( moving )
						, m_toMove );
					m_toMove.push_back( instance );
				}
				break;

			case SkeletonAnimationObjectType::eBone:
				{
					auto instance = std::make_shared< SkeletonAnimationInstanceBone >( *this
						, *std::static_pointer_cast< SkeletonAnimationBone >( moving )
						, m_toMove );
					m_toMove.push_back( instance );
				}
				break;

			default:
				break;
			}
		}

		for ( auto & keyFrame : animation )
		{
			m_keyFrames.emplace_back( *this
				, static_cast< SkeletonAnimationKeyFrame const & >( *keyFrame )
				, object );
		}

		m_curr = m_keyFrames.empty()
			? m_keyFrames.end()
			: m_keyFrames.begin();
	}

	SkeletonAnimationInstanceObjectSPtr SkeletonAnimationInstance::getObject( Bone const & bone )const
	{
		return getObject( SkeletonAnimationObjectType::eBone, bone.getName() );
	}

	SkeletonAnimationInstanceObjectSPtr SkeletonAnimationInstance::getObject( castor::String const & name )const
	{
		return getObject( SkeletonAnimationObjectType::eNode, name );
	}

	SkeletonAnimationInstanceObjectSPtr SkeletonAnimationInstance::getObject( SkeletonAnimationObjectType type
		, castor::String const & name )const
	{
		SkeletonAnimationInstanceObjectSPtr result;
		auto fullName = getObjectTypeName( type ) + name;
		auto it = std::find_if( m_toMove.begin()
			, m_toMove.end()
			, [&fullName]( SkeletonAnimationInstanceObjectSPtr lookup )
			{
				return getObjectTypeName( lookup->getObject().getType() ) + lookup->getObject().getName() == fullName;
			} );

		if ( it != m_toMove.end() )
		{
			result = *it;
		}
		else
		{
			log::warn << cuT( "No object named " ) << name << cuT( " for this animation instance" ) << std::endl;
		}

		return result;
	}

	void SkeletonAnimationInstance::doUpdate()
	{
		if ( !m_keyFrames.empty() )
		{
			auto limit = m_keyFrames.begin();

			while ( m_curr != limit && m_curr->getTimeIndex() >= m_currentTime )
			{
				// Time has gone too fast backward.
				--m_curr;
			}

			limit = ( m_keyFrames.end() - 1 );

			while ( m_curr != limit && m_curr->getTimeIndex() < m_currentTime )
			{
				// Time has gone too fast forward.
				++m_curr;
			}

			m_curr->apply();
		}
	}

	//*************************************************************************************************
}
