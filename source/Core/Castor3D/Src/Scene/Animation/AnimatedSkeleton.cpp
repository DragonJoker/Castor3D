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

	void AnimatedSkeleton::Update( std::chrono::milliseconds const & p_tslf )
	{
		for ( auto & l_animation : m_playingAnimations )
		{
			l_animation.get().Update( p_tslf );
		}
	}

	void AnimatedSkeleton::FillShader( Uniform4x4r & p_variable )const
	{
		Skeleton & l_skeleton = m_skeleton;
		uint32_t i{ 0u };

		if ( m_playingAnimations.empty() )
		{
			for ( auto l_bone : l_skeleton )
			{
				p_variable.SetValue( l_skeleton.GetGlobalInverseTransform(), i++ );
			}
		}
		else
		{
			for ( auto l_bone : l_skeleton )
			{
				Matrix4x4r l_final{ 1.0_r };

				for ( auto & l_animation : m_playingAnimations )
				{
					auto l_object = l_animation.get().GetObject( *l_bone );

					if ( l_object )
					{
						l_final *= l_object->GetFinalTransform();
					}
				}

				p_variable.SetValue( l_final, i++ );
			}
		}
	}

	void AnimatedSkeleton::FillBuffer( uint8_t * p_buffer )const
	{
		Skeleton & l_skeleton = m_skeleton;
		uint32_t i{ 0u };
		auto l_buffer = reinterpret_cast< float * >( p_buffer );
		auto l_stride = 16u;

		if ( m_playingAnimations.empty() )
		{
			for ( auto l_bone : l_skeleton )
			{
				std::memcpy( l_buffer, l_skeleton.GetGlobalInverseTransform().const_ptr(), l_stride );
				l_buffer += l_stride;
			}
		}
		else
		{
			for ( auto l_bone : l_skeleton )
			{
				Matrix4x4r l_final{ 1.0_r };

				for ( auto & l_animation : m_playingAnimations )
				{
					auto l_object = l_animation.get().GetObject( *l_bone );

					if ( l_object )
					{
						l_final *= l_object->GetFinalTransform();
					}
				}

				std::memcpy( l_buffer, l_final.const_ptr (), l_stride );
				l_buffer += l_stride;
			}
		}
	}

	void AnimatedSkeleton::DoAddAnimation( String const & p_name )
	{
		auto l_it = m_animations.find( p_name );

		if ( l_it == m_animations.end() )
		{
			auto & l_animation = static_cast< SkeletonAnimation const & >( m_skeleton.GetAnimation( p_name ) );
			auto l_instance = std::make_unique< SkeletonAnimationInstance >( *this, l_animation );
			m_animations.emplace( p_name, std::move( l_instance ) );
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
