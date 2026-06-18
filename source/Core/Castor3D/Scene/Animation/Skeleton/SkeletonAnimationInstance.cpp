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

CU_ImplementSmartPtr( c3d, SkeletonAnimationInstance )

namespace c3d
{
	//*************************************************************************************************

	namespace sklanminst
	{
		static String const & getObjectTypeName( SkeletonNodeType type )
		{
			static Map< SkeletonNodeType, String > Names
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
		: AnimationInstance{ object, animation, InterpolatorType::eNearest }
	{
		for ( auto moving : animation.getRootObjects() )
		{
			switch ( moving->getType() )
			{
			case SkeletonNodeType::eNode:
				{
					m_toMove.push_back( makeUniqueDerived< SkeletonAnimationInstanceObject, SkeletonAnimationInstanceNode >( *this
						, static_cast< SkeletonAnimationNode & >( *moving )
						, m_toMove ) );
				}
				break;

			case SkeletonNodeType::eBone:
				{
					m_toMove.push_back( makeUniqueDerived< SkeletonAnimationInstanceObject, SkeletonAnimationInstanceBone >( *this
						, static_cast< SkeletonAnimationBone & >( *moving )
						, m_toMove ) );
				}
				break;

			default:
				break;
			}
		}

		// Build object lookup cache for O(1) access
		for ( auto const & obj : m_toMove )
		{
			auto fullName = sklanminst::getObjectTypeName( obj->getObject().getType() ) + obj->getObject().getName();
			m_objectCache.try_emplace( fullName, obj.get() );
		}

		for ( auto const & keyFrame : animation )
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
		, String const & name )const
	{
		SkeletonAnimationInstanceObjectRPtr result{};
		auto fullName = sklanminst::getObjectTypeName( type ) + name;

		if ( auto it = m_objectCache.find( fullName );
			it != m_objectCache.end() )
		{
			result = it->second;
		}
		else
		{
			log::warn << cuT( "No object named " ) << name << cuT( " for this animation instance" ) << std::endl;
		}

		return result;
	}

	void SkeletonAnimationInstance::doUpdate()
	{
		if ( m_keyFrames.size() == 1u )
		{
			m_keyFrames.begin()->apply();
		}
		else if ( !m_keyFrames.empty() )
		{
			auto lowerBound = m_keyFrames.begin();
			auto upperBound = std::prev( m_keyFrames.end() );

			while ( m_curr != lowerBound && m_curr->getTimeIndex() >= m_currentTime )
				// Time has gone too fast backward.
				--m_curr;

			while ( m_curr != upperBound && m_curr->getTimeIndex() < m_currentTime )
				// Time has gone too fast forward.
				++m_curr;

			if ( getInterpolation() != InterpolatorType::eNearest )
			{
				auto prev = m_curr;
				Milliseconds timeMs{};

				if ( m_curr == lowerBound )
				{
					if ( m_looped )
					{
						prev = upperBound;
						timeMs = ( m_currentTime + getAnimation().getLength() ) - prev->getTimeIndex();
					}
				}
				else
				{
					prev = std::prev( m_curr );
					timeMs = m_currentTime - prev->getTimeIndex();
				}

				if ( prev == m_curr )
					m_curr->apply();
				else
					doInterpolate( *prev, *m_curr, timeMs );
			}
			else
			{
				m_curr->apply();
			}
		}
	}

	void SkeletonAnimationInstance::doInterpolate( SkeletonAnimationInstanceKeyFrame const & prev, SkeletonAnimationInstanceKeyFrame const & curr, Milliseconds const & timeMs )
	{
		if ( !m_floatInterpolator
			|| m_floatInterpolator->getType() != getInterpolation() )
			m_floatInterpolator = makeInterpolator< float >( getInterpolation() );
		if ( !m_vecInterpolator
			|| m_vecInterpolator->getType() != getInterpolation() )
			m_vecInterpolator = makeInterpolator< Point3f >( getInterpolation() );
		curr.interpolate( prev, timeMs, *m_floatInterpolator, *m_vecInterpolator );
	}

	//*************************************************************************************************
}
