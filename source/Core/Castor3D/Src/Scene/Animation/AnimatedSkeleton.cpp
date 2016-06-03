#include "AnimatedSkeleton.hpp"

#include "Animation/Animable.hpp"
#include "Animation/Skeleton/SkeletonAnimation.hpp"
#include "Mesh/Skeleton/Bone.hpp"
#include "Mesh/Skeleton/Skeleton.hpp"
#include "Scene/Animation/Skeleton/SkeletonAnimationInstance.hpp"
#include "Scene/Animation/Skeleton/SkeletonAnimationInstanceObject.hpp"
#include "Shader/MatrixFrameVariable.hpp"

using namespace Castor;

namespace Castor3D
{
	AnimatedSkeleton::AnimatedSkeleton( String const & p_name, Skeleton & p_skeleton )
		: AnimatedObject{ p_name }
		, m_skeleton{ p_skeleton }
	{
	}

	AnimatedSkeleton :: ~AnimatedSkeleton()
	{
	}

	AnimationInstanceSPtr AnimatedSkeleton::DoAddAnimation( String const & p_name )
	{
		SkeletonAnimationInstanceSPtr l_instance;
		auto l_it = m_animations.find( p_name );

		if ( l_it == m_animations.end() )
		{
			auto l_animation{ std::static_pointer_cast< SkeletonAnimation >( m_skeleton.GetAnimation( p_name ) ) };

			if ( l_animation )
			{
				l_instance = std::make_shared< SkeletonAnimationInstance >( *this, *l_animation );
				m_animations.insert( { p_name, l_instance } );
			}
		}

		return l_instance;
	}

	void AnimatedSkeleton::DoFillShader( Matrix4x4rFrameVariable & p_variable )
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

				for ( auto l_animation : m_playingAnimations )
				{
					auto l_object = std::static_pointer_cast< SkeletonAnimationInstance >( l_animation )->GetObject( *l_bone );

					if ( l_object )
					{
						l_final *= l_object->GetFinalTransform();
					}
				}

				p_variable.SetValue( l_final, i++ );
			}
		}
	}
}
