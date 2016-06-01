#include "AnimatedSkeleton.hpp"

#include "Animation/Animable.hpp"
#include "Animation/Skeleton/SkeletonAnimation.hpp"

#include "Mesh/Mesh.hpp"
#include "Mesh/Skeleton/Bone.hpp"
#include "Mesh/Skeleton/Skeleton.hpp"
#include "Scene/Geometry.hpp"
#include "Scene/MovableObject.hpp"
#include "Shader/MatrixFrameVariable.hpp"

using namespace Castor;

namespace Castor3D
{
	AnimatedSkeleton::AnimatedSkeleton( String const & p_name )
		: AnimatedObject{ p_name }
	{
	}

	AnimatedSkeleton :: ~AnimatedSkeleton()
	{
	}

	void AnimatedSkeleton::SetSkeleton( SkeletonSPtr p_skeleton )
	{
		m_animations.clear();

		if ( p_skeleton )
		{
			DoCopyAnimations( p_skeleton );
		}

		m_skeleton = p_skeleton;
	}

	void AnimatedSkeleton::DoFillShader( Matrix4x4rFrameVariable & p_variable )
	{
		SkeletonSPtr l_skeleton = GetSkeleton();

		if ( l_skeleton )
		{
			uint32_t i{ 0u };

			if ( m_playingAnimations.empty() )
			{
				for ( auto l_bone : *l_skeleton )
				{
					p_variable.SetValue( l_skeleton->GetGlobalInverseTransform(), i++ );
				}
			}
			else
			{
				for ( auto l_bone : *l_skeleton )
				{
					Matrix4x4r l_final{ 1.0_r };

					for ( auto l_animation : m_playingAnimations )
					{
						auto l_moving = std::static_pointer_cast< SkeletonAnimation >( l_animation )->GetObject( l_bone );

						if ( l_moving )
						{
							l_final *= l_moving->GetFinalTransform();
						}
					}

					p_variable.SetValue( l_final, i++ );
				}
			}
		}
	}

	void AnimatedSkeleton::DoCopyAnimations( AnimableSPtr p_object )
	{
		for ( auto l_itAnim : p_object->GetAnimations() )
		{
			SkeletonAnimationSPtr l_animation = std::static_pointer_cast< SkeletonAnimation >( l_itAnim.second );
			auto l_it = m_animations.find( l_animation->GetName() );

			if ( l_it == m_animations.end() )
			{
				m_animations.insert( { l_animation->GetName(), l_animation->Clone( *p_object ) } );
			}
		}
	}
}
