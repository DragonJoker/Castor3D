#include "AnimatedSkeleton.hpp"

#include "Animation/Animable.hpp"
#include "Animation/Skeleton/SkeletonAnimation.hpp"
#include "Mesh/Skeleton/Bone.hpp"
#include "Mesh/Skeleton/Skeleton.hpp"
#include "Scene/Animation/Skeleton/SkeletonAnimationInstance.hpp"
#include "Scene/Animation/Skeleton/SkeletonAnimationInstanceObject.hpp"
#include "Shader/Uniform.hpp"

using namespace Castor;

namespace Castor3D
{
	AnimatedSkeleton::AnimatedSkeleton( String const & p_name, Skeleton & p_skeleton )
		: AnimatedObject{ p_name }
		, m_skeleton{ p_skeleton }
	{
	}

	AnimatedSkeleton::~AnimatedSkeleton()
	{
	}

	void AnimatedSkeleton::Update( Milliseconds const & p_tslf )
	{
		for ( auto & animation : m_playingAnimations )
		{
			animation.get().Update( p_tslf );
		}
	}

	void AnimatedSkeleton::FillShader( Uniform4x4r & p_variable )const
	{
		Skeleton & skeleton = m_skeleton;
		uint32_t i{ 0u };

		if ( m_playingAnimations.empty() )
		{
			for ( auto bone : skeleton )
			{
				p_variable.SetValue( skeleton.GetGlobalInverseTransform(), i++ );
			}
		}
		else
		{
			for ( auto bone : skeleton )
			{
				Matrix4x4r final{ 1.0_r };

				for ( auto & animation : m_playingAnimations )
				{
					auto object = animation.get().GetObject( *bone );

					if ( object )
					{
						final *= object->GetFinalTransform();
					}
				}

				p_variable.SetValue( final, i++ );
			}
		}
	}

	void AnimatedSkeleton::FillBuffer( uint8_t * p_buffer )const
	{
		Skeleton & skeleton = m_skeleton;
		uint32_t i{ 0u };
		auto buffer = p_buffer;
		auto stride = 16u * sizeof( float );

		if ( m_playingAnimations.empty() )
		{
			for ( auto bone : skeleton )
			{
				std::memcpy( buffer, skeleton.GetGlobalInverseTransform().const_ptr(), stride );
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
					auto object = animation.get().GetObject( *bone );

					if ( object )
					{
						final *= object->GetFinalTransform();
					}
				}

				std::memcpy( buffer, final.const_ptr (), stride );
				buffer += stride;
			}
		}
	}

	void AnimatedSkeleton::DoAddAnimation( String const & p_name )
	{
		auto it = m_animations.find( p_name );

		if ( it == m_animations.end() )
		{
			auto & animation = static_cast< SkeletonAnimation const & >( m_skeleton.GetAnimation( p_name ) );
			auto instance = std::make_unique< SkeletonAnimationInstance >( *this, animation );
			m_animations.emplace( p_name, std::move( instance ) );
		}
	}

	void AnimatedSkeleton::DoStartAnimation( AnimationInstance & p_animation )
	{
		m_playingAnimations.push_back( static_cast< SkeletonAnimationInstance & >( p_animation ) );
	}

	void AnimatedSkeleton::DoStopAnimation( AnimationInstance & p_animation )
	{
		m_playingAnimations.erase( std::find_if( m_playingAnimations.begin()
			, m_playingAnimations.end()
			, [&p_animation]( std::reference_wrapper< SkeletonAnimationInstance > & p_instance )
			{
				return &p_instance.get() == &static_cast< SkeletonAnimationInstance & >( p_animation );
			} ) );
	}

	void AnimatedSkeleton::DoClearAnimations()
	{
		m_playingAnimations.clear();
	}
}
