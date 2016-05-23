#include "AnimatedObjectGroup.hpp"

#include "AnimatedObject.hpp"
#include "Animation.hpp"

#include "Scene/Geometry.hpp"

#include <Logger.hpp>

using namespace Castor;

#ifdef GetObject
#	undef GetObject
#endif

namespace Castor3D
{
	AnimatedObjectGroup::TextLoader::TextLoader( String const & p_tabs, File::eENCODING_MODE p_encodingMode )
		: Castor::TextLoader< AnimatedObjectGroup >( p_tabs, p_encodingMode )
	{
	}

	bool AnimatedObjectGroup::TextLoader::operator()( AnimatedObjectGroup const & p_group, TextFile & p_file )
	{
		bool l_return = p_file.WriteText( m_tabs + cuT( "animated_object_group " ) + p_group.GetName() + cuT( "\n{\n" ) ) > 0;

		for ( auto l_it : p_group.GetAnimations() )
		{
			l_return &= p_file.WriteText( m_tabs + cuT( "\tanimation " ) + l_it.first + cuT( "\n" ) ) > 0;
		}

		for ( auto l_it : p_group.GetObjects() )
		{
			if ( l_it.second->GetGeometry() )
			{
				l_return &= p_file.WriteText( m_tabs + cuT( "\tanimated_object " ) + l_it.second->GetName() + cuT( "\n" ) ) > 0;
			}
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "}\n\n" ) ) > 0;
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

	AnimatedObjectSPtr AnimatedObjectGroup::AddObject( GeometrySPtr p_object )
	{
		AnimatedObjectSPtr l_return;

		if ( p_object && m_objects.find( p_object->GetName() ) == m_objects.end() )
		{
			l_return = std::make_shared< AnimatedObject >( p_object->GetName() );
			l_return->SetGeometry( p_object );
			p_object->SetAnimatedObject( l_return );
			m_objects.insert( std::make_pair( p_object->GetName(), l_return ) );
		}

		return l_return;
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
			m_animations.insert( std::make_pair( p_name, eANIMATION_STATE_STOPPED ) );
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

			l_itAnim->second = eANIMATION_STATE_PLAYING;
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

			l_itAnim->second = eANIMATION_STATE_STOPPED;
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

			l_itAnim->second = eANIMATION_STATE_PAUSED;
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
			l_it.second = eANIMATION_STATE_PLAYING;
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
			l_it.second = eANIMATION_STATE_STOPPED;
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
			l_it.second = eANIMATION_STATE_PAUSED;
		}
	}
}
