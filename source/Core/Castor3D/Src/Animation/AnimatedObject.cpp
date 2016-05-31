#include "AnimatedObject.hpp"

#include "Animable.hpp"
#include "Animation.hpp"
#include "AnimationObject.hpp"

#include "Mesh/Mesh.hpp"
#include "Mesh/Skeleton/Bone.hpp"
#include "Mesh/Skeleton/Skeleton.hpp"
#include "Scene/Geometry.hpp"
#include "Scene/MovableObject.hpp"
#include "Shader/MatrixFrameVariable.hpp"

using namespace Castor;

namespace Castor3D
{
	AnimatedObject::TextWriter::TextWriter( String const & p_tabs )
		: Castor::TextWriter< AnimatedObject >{ p_tabs }
	{
	}

	bool AnimatedObject::TextWriter::operator()( AnimatedObject const & p_object, TextFile & p_file )
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

	AnimatedObject::AnimatedObject( String const & p_name )
		: Named( p_name )
	{
	}

	AnimatedObject :: ~AnimatedObject()
	{
	}

	void AnimatedObject::Update( real p_tslf )
	{
		for ( auto l_animation : m_playingAnimations )
		{
			l_animation->Update( p_tslf );
		}
	}

	void AnimatedObject::FillShader( Matrix4x4rFrameVariable & p_variable )
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
						auto l_moving = l_animation->GetObject( l_bone );

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

	void AnimatedObject::StartAnimation( String const & p_name )
	{
		AnimationSPtr l_animation;
		auto l_it = m_animations.find( p_name );

		if ( l_it != m_animations.end() )
		{
			auto l_animation = l_it->second;

			if ( l_animation->GetState() != eANIMATION_STATE_PLAYING
					&& l_animation->GetState() != eANIMATION_STATE_PAUSED )
			{
				l_animation->Play();
				m_playingAnimations.push_back( l_animation );
			}
		}
	}

	void AnimatedObject::StopAnimation( String const & p_name )
	{
		AnimationSPtr l_animation;
		auto l_it = m_animations.find( p_name );

		if ( l_it != m_animations.end() )
		{
			auto l_animation = l_it->second;

			if ( l_animation->GetState() != eANIMATION_STATE_STOPPED )
			{
				l_animation->Stop();
				m_playingAnimations.erase( std::find( m_playingAnimations.begin(), m_playingAnimations.end(), l_animation ) );
			}
		}
	}

	void AnimatedObject::PauseAnimation( String const & p_name )
	{
		AnimationSPtr l_animation;
		auto l_it = m_animations.find( p_name );

		if ( l_it != m_animations.end() )
		{
			l_it->second->Pause();
		}
	}

	void AnimatedObject::StartAllAnimations()
	{
		m_playingAnimations.clear();

		for ( auto l_it : m_animations )
		{
			l_it.second->Play();
			m_playingAnimations.push_back( l_it.second );
		}
	}

	void AnimatedObject::StopAllAnimations()
	{
		m_playingAnimations.clear();

		for ( auto l_it : m_animations )
		{
			l_it.second->Stop();
		}
	}

	void AnimatedObject::PauseAllAnimations()
	{
		for ( auto l_it : m_animations )
		{
			l_it.second->Pause();
		}
	}

	AnimationSPtr AnimatedObject::GetAnimation( Castor::String const & p_name )
	{
		AnimationSPtr l_return;
		auto l_it = m_animations.find( p_name );

		if ( l_it != m_animations.end() )
		{
			l_return = l_it->second;
		}

		return l_return;
	}

	void AnimatedObject::SetGeometry( GeometrySPtr p_object )
	{
		m_animations.clear();
		DoSetGeometry( p_object );
	}

	void AnimatedObject::SetMesh( MeshSPtr p_mesh )
	{
		m_animations.clear();
		DoSetMesh( p_mesh );
	}

	void AnimatedObject::SetSkeleton( SkeletonSPtr p_pSkeleton )
	{
		m_animations.clear();
		DoSetSkeleton( p_pSkeleton );
	}

	void AnimatedObject::DoSetGeometry( GeometrySPtr p_object )
	{
		if ( p_object )
		{
			DoSetMesh( p_object->GetMesh() );
		}

		m_geometry = p_object;
	}

	void AnimatedObject::DoSetMesh( MeshSPtr p_mesh )
	{
		if ( p_mesh )
		{
			DoCopyAnimations( p_mesh );
			DoSetSkeleton( p_mesh->GetSkeleton() );
		}

		m_mesh = p_mesh;
	}

	void AnimatedObject::DoSetSkeleton( SkeletonSPtr p_skeleton )
	{
		if ( p_skeleton )
		{
			DoCopyAnimations( p_skeleton );
		}

		m_skeleton = p_skeleton;
	}

	void AnimatedObject::DoCopyAnimations( AnimableSPtr p_object )
	{
		for ( auto l_itAnim : p_object->GetAnimations() )
		{
			AnimationSPtr l_animation = l_itAnim.second;
			auto l_it = m_animations.find( l_animation->GetName() );

			if ( l_it == m_animations.end() )
			{
				m_animations.insert( { l_animation->GetName(), l_animation->Clone( *p_object ) } );
			}
		}
	}
}
