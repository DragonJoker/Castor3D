#include "Castor3D/Scene/Animation/AnimatedSkeleton.hpp"

#include "Castor3D/Animation/Animable.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimation.hpp"
#include "Castor3D/Model/Skeleton/BoneNode.hpp"
#include "Castor3D/Model/Skeleton/Skeleton.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/Animation/Skeleton/SkeletonAnimationInstance.hpp"
#include "Castor3D/Scene/Animation/Skeleton/SkeletonAnimationInstanceObject.hpp"

namespace castor3d
{
	AnimatedSkeleton::AnimatedSkeleton( castor::String const & name
		, Skeleton & skeleton
		, Mesh & mesh
		, Geometry & geometry )
		: AnimatedObject{ AnimationType::eSkeleton, name }
		, m_skeleton{ skeleton }
		, m_mesh{ mesh }
		, m_geometry{ geometry }
	{
	}

	void AnimatedSkeleton::update( castor::Milliseconds const & elapsed )
	{
		if ( !m_playingAnimations.empty() )
		{
			for ( auto & animation : m_playingAnimations )
			{
				animation.get().update( elapsed );
			}

			m_geometry.markDirty();
		}
	}

	uint32_t AnimatedSkeleton::fillBuffer( SkinningTransformsConfiguration * buffer )const
	{
		Skeleton & skeleton = m_skeleton;

		if ( m_playingAnimations.empty() )
		{
			if ( m_reinit )
			{
				m_reinit = false;

				for ( auto bone : skeleton.getBones() )
				{
					buffer->bonesMatrix[bone->getId()] = skeleton.getGlobalInverseTransform();
				}
			}
		}
		else
		{
			for ( auto bone : skeleton.getBones() )
			{
				castor::Matrix4x4f final{ skeleton.getGlobalInverseTransform() };

				for ( auto & animation : m_playingAnimations )
				{
					auto object = animation.get().getObject( *bone );

					if ( object )
					{
						final *= object->getFinalTransform();
					}
				}

				buffer->bonesMatrix[bone->getId()] = final;
			}
		}

		return uint32_t( skeleton.getBonesCount() );
	}

	void AnimatedSkeleton::doAddAnimation( castor::String const & name )
	{
		auto it = m_animations.find( name );

		if ( it == m_animations.end() )
		{
			auto & animation = static_cast< SkeletonAnimation & >( m_skeleton.getAnimation( name ) );
			auto instance = std::make_unique< SkeletonAnimationInstance >( *this, animation );
			m_animations.emplace( name, std::move( instance ) );
		}
	}

	void AnimatedSkeleton::doStartAnimation( AnimationInstance & animation )
	{
		m_playingAnimations.emplace_back( static_cast< SkeletonAnimationInstance & >( animation ) );
	}

	void AnimatedSkeleton::doStopAnimation( AnimationInstance & animation )
	{
		m_playingAnimations.erase( std::find_if( m_playingAnimations.begin()
			, m_playingAnimations.end()
			, [&animation]( std::reference_wrapper< SkeletonAnimationInstance > & instance )
			{
				return &instance.get() == &static_cast< SkeletonAnimationInstance & >( animation );
			} ) );
		m_reinit = m_playingAnimations.empty();
	}

	void AnimatedSkeleton::doClearAnimations()
	{
		m_reinit = true;
		m_playingAnimations.clear();
	}
}
