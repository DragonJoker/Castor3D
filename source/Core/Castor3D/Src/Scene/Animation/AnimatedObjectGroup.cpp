#include "AnimatedObjectGroup.hpp"

#include "AnimatedObject.hpp"
#include "AnimatedSkeleton.hpp"
#include "AnimatedMesh.hpp"

#include "Animation/Animation.hpp"
#include "Scene/Geometry.hpp"
#include "Mesh/Mesh.hpp"
#include "Mesh/Skeleton/Skeleton.hpp"
#include "Scene/Animation/AnimationInstance.hpp"

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
		Logger::LogInfo( m_tabs + cuT( "Writing AnimatedObjectGroup " ) + p_group.GetName() );
		bool l_return = p_file.WriteText( cuT( "\n" ) + m_tabs + cuT( "animated_object_group \"" ) + p_group.GetName() + cuT( "\"\n" ) ) > 0
			&& p_file.WriteText( m_tabs + cuT( "{\n" ) ) > 0;

		StrSet l_written;

		for ( auto l_it : p_group.GetObjects() )
		{
			auto l_name = l_it.first;
			bool l_write{ true };
			size_t l_skel = l_name.find( cuT( "_Skeleton" ) );
			size_t l_mesh = l_name.find( cuT( "_Mesh" ) );

			// Only add objects, and not skeletons or meshes
			if ( l_skel != String::npos )
			{
				l_name = l_name.substr( 0, l_skel );
				l_write = p_group.GetObjects().find( l_name ) == p_group.GetObjects().end()
					&& l_written.find( l_name ) == l_written.end();
			}
			else if ( l_mesh != String::npos )
			{
				l_name = l_name.substr( 0, l_mesh );
				l_write = p_group.GetObjects().find( l_name ) == p_group.GetObjects().end()
					&& l_written.find( l_name ) == l_written.end();
			}

			if ( l_write )
			{
				l_return &= p_file.WriteText( m_tabs + cuT( "\tanimated_object \"" ) + l_name + cuT( "\"\n" ) ) > 0;
				l_written.insert( l_name );
			}
		}

		if ( !p_group.GetAnimations().empty() )
		{
			l_return &= p_file.WriteText( cuT( "\n" ) ) > 0;

			for ( auto l_it : p_group.GetAnimations() )
			{
				l_return &= p_file.WriteText( m_tabs + cuT( "\tanimation \"" ) + l_it.first + cuT( "\"\n" ) ) > 0
					&& p_file.WriteText( m_tabs + cuT( "\t{\n" ) ) > 0
					&& p_file.WriteText( m_tabs + cuT( "\t\tlooped " ) + String{ l_it.second.m_looped ? cuT( "true" ) : cuT( "false" ) } +cuT( "\n" ) ) > 0
					&& p_file.WriteText( m_tabs + cuT( "\t\tscale " ) + string::to_string( l_it.second.m_scale ) + cuT( "\n" ) ) > 0
					&& p_file.WriteText( m_tabs + cuT( "\t}\n" ) ) > 0;
			}
		}

		if ( !p_group.GetAnimations().empty() )
		{
			l_return &= p_file.WriteText( cuT( "\n" ) ) > 0;

			for ( auto l_it : p_group.GetAnimations() )
			{
				if ( l_it.second.m_state == AnimationState::Playing )
				{
					l_return &= p_file.WriteText( m_tabs + cuT( "\tstart_animation \"" ) + l_it.first + cuT( "\"\n" ) ) > 0;
				}
			}
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

	AnimatedObjectSPtr AnimatedObjectGroup::AddObject( MovableObject & p_object )
	{
		return nullptr;
	}

	AnimatedObjectSPtr AnimatedObjectGroup::AddObject( Mesh & p_object, String const & p_name )
	{
		auto l_object = std::make_shared< AnimatedMesh >( p_name, p_object );

		if ( !AddObject( l_object ) )
		{
			l_object.reset();
		}

		return l_object;
	}

	AnimatedObjectSPtr AnimatedObjectGroup::AddObject( Skeleton & p_object, String const & p_name )
	{
		auto l_object = std::make_shared< AnimatedSkeleton >( p_name, p_object );

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
			m_objects.insert( { p_object->GetName(), p_object } );
		}

		for ( auto l_it : m_animations )
		{
			p_object->AddAnimation( l_it.first );
			auto & l_animation = p_object->GetAnimation( l_it.first );
			l_animation.SetLooped( l_it.second.m_looped );
			l_animation.SetScale( l_it.second.m_scale );
		}

		return l_return;
	}

	void AnimatedObjectGroup::AddAnimation( String const & p_name )
	{
		if ( m_animations.find( p_name ) == m_animations.end() )
		{
			m_animations.insert( { p_name, { AnimationState::Stopped, false, 1.0f } } );

			for ( auto l_it : m_objects )
			{
				l_it.second->AddAnimation( p_name );
			}
		}
	}

	void AnimatedObjectGroup::SetAnimationLooped( Castor::String const & p_name, bool p_looped )
	{
		auto l_itAnim = m_animations.find( p_name );

		if ( l_itAnim != m_animations.end() )
		{
			for ( auto l_it : m_objects )
			{
				if ( l_it.second->HasAnimation( p_name ) )
				{
					auto & l_animation = l_it.second->GetAnimation( p_name );
					l_animation.SetLooped( p_looped );
				}
			}

			l_itAnim->second.m_looped = p_looped;
		}
	}

	void AnimatedObjectGroup::SetAnimationScale( Castor::String const & p_name, float p_scale )
	{
		auto l_itAnim = m_animations.find( p_name );

		if ( l_itAnim != m_animations.end() )
		{
			for ( auto l_it : m_objects )
			{
				if ( l_it.second->HasAnimation( p_name ) )
				{
					auto & l_animation = l_it.second->GetAnimation( p_name );
					l_animation.SetScale( p_scale );
				}
			}

			l_itAnim->second.m_scale = p_scale;
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

	void AnimatedObjectGroup::StartAnimation( String const & p_name )
	{
		auto l_itAnim = m_animations.find( p_name );

		if ( l_itAnim != m_animations.end() )
		{
			for ( auto l_it : m_objects )
			{
				l_it.second->StartAnimation( p_name );
			}

			l_itAnim->second.m_state = AnimationState::Playing;
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

			l_itAnim->second.m_state = AnimationState::Stopped;
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

			l_itAnim->second.m_state = AnimationState::Paused;
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
			l_it.second.m_state = AnimationState::Playing;
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
			l_it.second.m_state = AnimationState::Stopped;
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
			l_it.second.m_state = AnimationState::Paused;
		}
	}
}
