#include "AnimatedObject.hpp"

#include "Animable.hpp"
#include "Animation.hpp"

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
		DoFillShader( p_variable );
	}

	void AnimatedObject::StartAnimation( String const & p_name )
	{
		AnimationSPtr l_animation;
		auto l_it = m_animations.find( p_name );

		if ( l_it != m_animations.end() )
		{
			auto l_animation = l_it->second;

			if ( l_animation->GetState() != AnimationState::Playing
					&& l_animation->GetState() != AnimationState::Paused )
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

			if ( l_animation->GetState() != AnimationState::Stopped )
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
}
