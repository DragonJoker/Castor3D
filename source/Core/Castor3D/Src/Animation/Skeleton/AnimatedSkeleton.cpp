#include "AnimatedSkeleton.hpp"

#include "Animation/Animable.hpp"
#include "SkeletonAnimation.hpp"

#include "Mesh/Mesh.hpp"
#include "Mesh/Skeleton/Bone.hpp"
#include "Mesh/Skeleton/Skeleton.hpp"
#include "Scene/Geometry.hpp"
#include "Scene/MovableObject.hpp"
#include "Shader/MatrixFrameVariable.hpp"

using namespace Castor;

namespace Castor3D
{
	AnimatedSkeleton::TextWriter::TextWriter( String const & p_tabs )
		: Castor::TextWriter< AnimatedSkeleton >{ p_tabs }
	{
	}

	bool AnimatedSkeleton::TextWriter::operator()( AnimatedSkeleton const & p_object, TextFile & p_file )
	{
		bool l_return = p_file.WriteText( m_tabs + cuT( "animated_object \"" ) + p_object.GetName() + cuT( "\"\n" ) ) > 0
			&& p_file.WriteText( m_tabs + cuT( "{\n" ) ) > 0;

		for ( auto l_it : p_object.GetAnimations() )
		{
			l_return &= p_file.WriteText( m_tabs + cuT( "\tanimation \"" ) + l_it.first + cuT( "\"\n" ) ) > 0
				&& p_file.WriteText( m_tabs + cuT( "\t{\n" ) ) > 0
				&& p_file.WriteText( m_tabs + cuT( "\t\tlooped " ) + String{ l_it.second->IsLooped() ? cuT( "true" ) : cuT( "false" ) } +cuT( "\n" ) ) > 0
				&& p_file.WriteText( m_tabs + cuT( "\t\tscale " ) + string::to_string( l_it.second->GetScale() ) +cuT( "\n" ) ) > 0
				&& p_file.WriteText( m_tabs + cuT( "\t}\n" ) ) > 0;
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return l_return;
	}

	//*************************************************************************************************

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
