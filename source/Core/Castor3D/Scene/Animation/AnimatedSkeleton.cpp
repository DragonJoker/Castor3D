#include "AnimatedSkeleton.hpp"

#include "Animation/Animable.hpp"
#include "Animation/Skeleton/SkeletonAnimation.hpp"
#include "Mesh/Skeleton/Bone.hpp"
#include "Mesh/Skeleton/Skeleton.hpp"
#include "Scene/Animation/Skeleton/SkeletonAnimationInstance.hpp"
#include "Scene/Animation/Skeleton/SkeletonAnimationInstanceObject.hpp"

using namespace castor;

namespace castor3d
{
	AnimatedSkeleton::AnimatedSkeleton( String const & name
		, Skeleton & skeleton
		, Mesh & mesh
		, Geometry & geometry )
		: AnimatedObject{ AnimationType::eSkeleton, name }
		, m_skeleton{ skeleton }
		, m_mesh{ mesh }
		, m_geometry{ geometry }
	{
	}

	AnimatedSkeleton::~AnimatedSkeleton()
	{
	}

	void AnimatedSkeleton::update( Milliseconds const & elapsed )
	{
		for ( auto & animation : m_playingAnimations )
		{
			animation.get().update( elapsed );
		}
	}

	void AnimatedSkeleton::fillShader( castor::Matrix4x4f * variable )const
	{
		Skeleton & skeleton = m_skeleton;
		uint32_t i{ 0u };

		if ( m_playingAnimations.empty() )
		{
			for ( auto bone : skeleton )
			{
				variable[i++] = skeleton.getGlobalInverseTransform();
			}
		}
		else
		{
			for ( auto bone : skeleton )
			{
				Matrix4x4r final{ 1.0_r };

				for ( auto & animation : m_playingAnimations )
				{
					auto object = animation.get().getObject( *bone );

					if ( object )
					{
						final *= object->getFinalTransform();
					}
				}

				variable[i++] = final;
			}
		}
	}

	void AnimatedSkeleton::fillBuffer( uint8_t * buffer )const
	{
		Skeleton & skeleton = m_skeleton;
		auto stride = 16u * sizeof( float );

		if ( m_playingAnimations.empty() )
		{
			for ( auto bone : skeleton )
			{
				std::memcpy( buffer, skeleton.getGlobalInverseTransform().constPtr(), stride );
				buffer += stride;
			}
		}
		else
		{
			for ( auto bone : skeleton )
			{
				Matrix4x4r final{ 1.0_r };

				for ( auto & animation : m_playingAnimations )
				{
					auto object = animation.get().getObject( *bone );

					if ( object )
					{
						final *= object->getFinalTransform();
					}
				}

				std::memcpy( buffer, final.constPtr (), stride );
				buffer += stride;
			}
		}
	}

	void AnimatedSkeleton::doAddAnimation( String const & name )
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
	}

	void AnimatedSkeleton::doClearAnimations()
	{
		m_playingAnimations.clear();
	}
}
