#include "Castor3D/Scene/Animation/Skeleton/SkeletonAnimationInstance.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimation.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationBone.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationKeyFrame.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationNode.hpp"
#include "Castor3D/Model/Skeleton/BoneNode.hpp"
#include "Castor3D/Scene/Animation/AnimatedSkeleton.hpp"
#include "Castor3D/Scene/Animation/Skeleton/SkeletonAnimationInstanceBone.hpp"
#include "Castor3D/Scene/Animation/Skeleton/SkeletonAnimationInstanceNode.hpp"

CU_ImplementSmartPtr( castor3d, SkeletonAnimationInstance )

namespace castor3d
{
	//*************************************************************************************************

	namespace sklanminst
	{
		static castor::String const & getObjectTypeName( SkeletonNodeType type )
		{
			static std::map< SkeletonNodeType, castor::String > Names
			{
				{ SkeletonNodeType::eNode, cuT( "Node_" ) },
				{ SkeletonNodeType::eBone, cuT( "Bone_" ) },
			};

			return Names[type];
		}
	}

	//*************************************************************************************************

	SkeletonAnimationInstance::SkeletonAnimationInstance( AnimatedSkeleton & object
		, SkeletonAnimation & animation )
		: AnimationInstance{ object, animation }
	{
		for ( auto moving : animation.getRootObjects() )
		{
			switch ( moving->getType() )
			{
			case SkeletonNodeType::eNode:
				{
					m_toMove.push_back( castor::makeUniqueDerived< SkeletonAnimationInstanceObject, SkeletonAnimationInstanceNode >( *this
						, static_cast< SkeletonAnimationNode & >( *moving )
						, m_toMove ) );
				}
				break;

			case SkeletonNodeType::eBone:
				{
					m_toMove.push_back( castor::makeUniqueDerived< SkeletonAnimationInstanceObject, SkeletonAnimationInstanceBone >( *this
						, static_cast< SkeletonAnimationBone & >( *moving )
						, m_toMove ) );
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

	SkeletonAnimationInstanceObjectRPtr SkeletonAnimationInstance::getObject( BoneNode const & bone )const
	{
		return getObject( SkeletonNodeType::eBone, bone.getName() );
	}

	SkeletonAnimationInstanceObjectRPtr SkeletonAnimationInstance::getObject( SkeletonNode const & node )const
	{
		return getObject( SkeletonNodeType::eNode, node.getName() );
	}

	SkeletonAnimationInstanceObjectRPtr SkeletonAnimationInstance::getObject( SkeletonNodeType type
		, castor::String const & name )const
	{
		SkeletonAnimationInstanceObjectRPtr result{};
		auto fullName = sklanminst::getObjectTypeName( type ) + name;
		auto it = std::find_if( m_toMove.begin()
			, m_toMove.end()
			, [&fullName]( SkeletonAnimationInstanceObjectUPtr const & lookup )
			{
				return sklanminst::getObjectTypeName( lookup->getObject().getType() ) + lookup->getObject().getName() == fullName;
			} );

		if ( it != m_toMove.end() )
		{
			result = it->get();
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
