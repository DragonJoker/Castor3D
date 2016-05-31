#include "AnimatedObjectGroup.hpp"

#include "AnimatedObject.hpp"
#include "Animation.hpp"

#include "Animation/Skeleton/AnimatedSkeleton.hpp"
#include "Scene/Geometry.hpp"
#include "Mesh/Mesh.hpp"
#include "Mesh//Skeleton/Skeleton.hpp"

#include <Logger.hpp>

using namespace Castor;

#ifdef GetObject
#	undef GetObject
#endif

namespace Castor3D
{
	AnimatedObjectGroup::TextWriter::TextWriter( String const & p_tabs )
		: Castor::TextWriter< AnimatedObjectGroup >{ p_tabs }
	{
	}

	bool AnimatedObjectGroup::TextWriter::operator()( AnimatedObjectGroup const & p_group, TextFile & p_file )
	{
		bool l_return = p_file.WriteText( m_tabs + cuT( "animated_object_group \"" ) + p_group.GetName() + cuT( "\"\n" ) ) > 0
			&& p_file.WriteText( m_tabs + cuT( "{\n" ) ) > 0;

		std::set< String > l_animations;

		for ( auto l_it : p_group.GetAnimations() )
		{
			l_return &= p_file.WriteText( m_tabs + cuT( "\tanimation \"" ) + l_it.first + cuT( "\"\n" ) ) > 0;
			l_animations.insert( l_it.first );
		}

		StringArray l_playing;

		for ( auto l_it : p_group.GetObjects() )
		{
			l_return &= p_file.WriteText( m_tabs + cuT( "\tanimated_object \"" ) + l_it.first + cuT( "\"\n" ) ) > 0
				&& p_file.WriteText( m_tabs + cuT( "\t{\n" ) ) > 0;

			for ( auto l_it : l_it.second->GetAnimations() )
			{
				if ( l_animations.end() != l_animations.find( l_it.first ) )
				{
					l_return &= p_file.WriteText( m_tabs + cuT( "\t\tanimation \"" ) + l_it.first + cuT( "\"\n" ) ) > 0
						&& p_file.WriteText( m_tabs + cuT( "\t\t{\n" ) ) > 0
						&& p_file.WriteText( m_tabs + cuT( "\t\t\tlooped " ) + String{ l_it.second->IsLooped() ? cuT( "true" ) : cuT( "false" ) } +cuT( "\n" ) ) > 0
						&& p_file.WriteText( m_tabs + cuT( "\t\t\tscale " ) + string::to_string( l_it.second->GetScale() ) + cuT( "\n" ) ) > 0
						&& p_file.WriteText( m_tabs + cuT( "\t\t}\n" ) ) > 0;
				}
			}

			if ( l_return )
			{
				l_return = p_file.WriteText( m_tabs + cuT( "\t}\n" ) ) > 0;
			}

			for ( auto l_anim : l_it.second->GetPlayingAnimations() )
			{
				l_playing.push_back( l_anim->GetName() );
			}
		}

		for ( auto l_it : l_playing )
		{
			l_return &= p_file.WriteText( m_tabs + cuT( "\tstart_animation \"" ) + l_it + cuT( "\"\n" ) ) > 0;
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return l_return;
	}

	//*************************************************************************************************

	AnimatedObjectGroup::AnimatedObjectGroup( String const & p_name, Scene & p_scene )
		: Named( p_name )
		, OwnedBy< Scene >( p_scene )
	{
		m_timer.TimeS();
	}

	AnimatedObjectGroup::~AnimatedObjectGroup()
	{
		m_objects.clear();
		m_animations.clear();
	}

	AnimatedObjectSPtr AnimatedObjectGroup::AddObject( MovableObjectSPtr p_object )
	{
		return nullptr;
	}

	AnimatedObjectSPtr AnimatedObjectGroup::AddObject( MeshSPtr p_object, String const & p_name )
	{
		return nullptr;
	}

	AnimatedObjectSPtr AnimatedObjectGroup::AddObject( SkeletonSPtr p_object, String const & p_name )
	{
		auto l_object = std::make_shared< AnimatedSkeleton >( p_name );
		l_object->SetSkeleton( p_object );

		if ( !AddObject( l_object ) )
		{
			l_object.reset();
		}

		return l_object;
	}

	bool AnimatedObjectGroup::AddObject( AnimatedObjectSPtr p_object )
	{
		bool l_return = p_object && m_objects.find( p_object->GetName() ) == m_objects.end();

		if ( l_return )
		{
			m_objects.insert( std::make_pair( p_object->GetName(), p_object ) );
		}

		return l_return;
	}

	void AnimatedObjectGroup::AddAnimation( String const & p_name )
	{
		if ( m_animations.find( p_name ) == m_animations.end() )
		{
			m_animations.insert( std::make_pair( p_name, AnimationState::Stopped ) );
		}
	}

	void AnimatedObjectGroup::Update()
	{
		real l_tslf = real( m_timer.TimeS() );

		for ( auto l_it : m_objects )
		{
			l_it.second->Update( l_tslf );
		}
	}

	void AnimatedObjectGroup::SetAnimationLooped( Castor::String const & p_name, bool p_looped )
	{
		if ( m_animations.find( p_name ) != m_animations.end() )
		{
			for ( auto l_it : m_objects )
			{
				AnimationSPtr l_animation = l_it.second->GetAnimation( p_name );

				if ( l_animation )
				{
					l_animation->SetLooped( p_looped );
				}
			}
		}
	}

	void AnimatedObjectGroup::SetAnimationScale( Castor::String const & p_name, float p_scale )
	{
		if ( m_animations.find( p_name ) != m_animations.end() )
		{
			for ( auto l_it : m_objects )
			{
				AnimationSPtr l_animation = l_it.second->GetAnimation( p_name );

				if ( l_animation )
				{
					l_animation->SetScale( p_scale );
				}
			}
		}
	}

	void AnimatedObjectGroup::StartAnimation( String const & p_name )
	{
		auto l_itAnim = m_animations.find( p_name );

		if ( l_itAnim != m_animations.end() )
		{
			for ( auto l_it : m_objects )
			{
				l_it.second->StartAnimation( p_name );
			}

			l_itAnim->second = AnimationState::Playing;
		}
	}

	void AnimatedObjectGroup::StopAnimation( String const & p_name )
	{
		auto l_itAnim = m_animations.find( p_name );

		if ( l_itAnim != m_animations.end() )
		{
			for ( auto l_it : m_objects )
			{
				l_it.second->StopAnimation( p_name );
			}

			l_itAnim->second = AnimationState::Stopped;
		}
	}

	void AnimatedObjectGroup::PauseAnimation( String const & p_name )
	{
		auto l_itAnim = m_animations.find( p_name );

		if ( l_itAnim != m_animations.end() )
		{
			for ( auto l_it : m_objects )
			{
				l_it.second->PauseAnimation( p_name );
			}

			l_itAnim->second = AnimationState::Paused;
		}
	}

	void AnimatedObjectGroup::StartAllAnimations()
	{
		for ( auto l_it : m_objects )
		{
			l_it.second->StartAllAnimations();
		}

		for ( auto l_it : m_animations )
		{
			l_it.second = AnimationState::Playing;
		}
	}

	void AnimatedObjectGroup::StopAllAnimations()
	{
		for ( auto l_it : m_objects )
		{
			l_it.second->StopAllAnimations();
		}

		for ( auto l_it : m_animations )
		{
			l_it.second = AnimationState::Stopped;
		}
	}

	void AnimatedObjectGroup::PauseAllAnimations()
	{
		for ( auto l_it : m_objects )
		{
			l_it.second->PauseAllAnimations();
		}

		for ( auto l_it : m_animations )
		{
			l_it.second = AnimationState::Paused;
		}
	}
}
