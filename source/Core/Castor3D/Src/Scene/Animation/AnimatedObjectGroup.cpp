#include "AnimatedObjectGroup.hpp"

#include "AnimatedObject.hpp"
#include "AnimatedSkeleton.hpp"
#include "AnimatedMesh.hpp"

#include "Scene/Geometry.hpp"
#include "Mesh/Mesh.hpp"
#include "Mesh/Skeleton/Skeleton.hpp"

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
		bool result = p_file.WriteText( cuT( "\n" ) + m_tabs + cuT( "animated_object_group \"" ) + p_group.GetName() + cuT( "\"\n" ) ) > 0
						&& p_file.WriteText( m_tabs + cuT( "{\n" ) ) > 0;
		Castor::TextWriter< AnimatedObjectGroup >::CheckError( result, "AnimatedObjectGroup name" );

		StrSet written;

		for ( auto it : p_group.GetObjects() )
		{
			auto name = it.first;
			bool write{ true };
			size_t skel = name.find( cuT( "_Skeleton" ) );
			size_t mesh = name.find( cuT( "_Mesh" ) );

			// Only add objects, and not skeletons or meshes
			if ( skel != String::npos )
			{
				name = name.substr( 0, skel );
				write = p_group.GetObjects().find( name ) == p_group.GetObjects().end()
						  && written.find( name ) == written.end();
			}
			else if ( mesh != String::npos )
			{
				name = name.substr( 0, mesh );
				write = p_group.GetObjects().find( name ) == p_group.GetObjects().end()
						  && written.find( name ) == written.end();
			}

			if ( write )
			{
				result &= p_file.WriteText( m_tabs + cuT( "\tanimated_object \"" ) + name + cuT( "\"\n" ) ) > 0;
				written.insert( name );
				Castor::TextWriter< AnimatedObjectGroup >::CheckError( result, "AnimatedObjectGroup object name" );
			}
		}

		if ( !p_group.GetAnimations().empty() )
		{
			result &= p_file.WriteText( cuT( "\n" ) ) > 0;

			for ( auto it : p_group.GetAnimations() )
			{
				result &= p_file.WriteText( m_tabs + cuT( "\tanimation \"" ) + it.first + cuT( "\"\n" ) ) > 0
							&& p_file.WriteText( m_tabs + cuT( "\t{\n" ) ) > 0
							&& p_file.WriteText( m_tabs + cuT( "\t\tlooped " ) + String{ it.second.m_looped ? cuT( "true" ) : cuT( "false" ) } +cuT( "\n" ) ) > 0
							&& p_file.WriteText( m_tabs + cuT( "\t\tscale " ) + string::to_string( it.second.m_scale ) + cuT( "\n" ) ) > 0
							&& p_file.WriteText( m_tabs + cuT( "\t}\n" ) ) > 0;
				Castor::TextWriter< AnimatedObjectGroup >::CheckError( result, "AnimatedObjectGroup animation" );
			}
		}

		if ( !p_group.GetAnimations().empty() )
		{
			result &= p_file.WriteText( cuT( "\n" ) ) > 0;

			for ( auto it : p_group.GetAnimations() )
			{
				if ( it.second.m_state == AnimationState::ePlaying )
				{
					result &= p_file.WriteText( m_tabs + cuT( "\tstart_animation \"" ) + it.first + cuT( "\"\n" ) ) > 0;
					Castor::TextWriter< AnimatedObjectGroup >::CheckError( result, "AnimatedObjectGroup started animation" );
				}
			}
		}

		if ( result )
		{
			result = p_file.WriteText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return result;
	}

	//*************************************************************************************************

	AnimatedObjectGroup::AnimatedObjectGroup( String const & p_name, Scene & p_scene )
		: Named( p_name )
		, OwnedBy< Scene >( p_scene )
	{
		m_timer.Time();
	}

	AnimatedObjectGroup::~AnimatedObjectGroup()
	{
		m_objects.clear();
		m_animations.clear();
	}

	AnimatedObjectSPtr AnimatedObjectGroup::AddObject( MovableObject & p_object, String const & p_name )
	{
		return nullptr;
	}

	AnimatedObjectSPtr AnimatedObjectGroup::AddObject( Mesh & p_object, String const & p_name )
	{
		auto object = std::make_shared< AnimatedMesh >( p_name, p_object );

		if ( !AddObject( object ) )
		{
			object.reset();
		}

		return object;
	}

	AnimatedObjectSPtr AnimatedObjectGroup::AddObject( Skeleton & p_object, String const & p_name )
	{
		auto object = std::make_shared< AnimatedSkeleton >( p_name, p_object );

		if ( !AddObject( object ) )
		{
			object.reset();
		}

		return object;
	}

	bool AnimatedObjectGroup::AddObject( AnimatedObjectSPtr p_object )
	{
		bool result = p_object && m_objects.find( p_object->GetName() ) == m_objects.end();

		if ( result )
		{
			m_objects.insert( { p_object->GetName(), p_object } );
		}

		for ( auto it : m_animations )
		{
			p_object->AddAnimation( it.first );
			auto & animation = p_object->GetAnimation( it.first );
			animation.SetLooped( it.second.m_looped );
			animation.SetScale( it.second.m_scale );
		}

		return result;
	}

	void AnimatedObjectGroup::AddAnimation( String const & p_name )
	{
		if ( m_animations.find( p_name ) == m_animations.end() )
		{
			m_animations.insert( { p_name, { AnimationState::eStopped, false, 1.0f } } );

			for ( auto it : m_objects )
			{
				it.second->AddAnimation( p_name );
			}
		}
	}

	void AnimatedObjectGroup::SetAnimationLooped( Castor::String const & p_name, bool p_looped )
	{
		auto itAnim = m_animations.find( p_name );

		if ( itAnim != m_animations.end() )
		{
			for ( auto it : m_objects )
			{
				if ( it.second->HasAnimation( p_name ) )
				{
					auto & animation = it.second->GetAnimation( p_name );
					animation.SetLooped( p_looped );
				}
			}

			itAnim->second.m_looped = p_looped;
		}
	}

	void AnimatedObjectGroup::SetAnimationScale( Castor::String const & p_name, float p_scale )
	{
		auto itAnim = m_animations.find( p_name );

		if ( itAnim != m_animations.end() )
		{
			for ( auto it : m_objects )
			{
				if ( it.second->HasAnimation( p_name ) )
				{
					auto & animation = it.second->GetAnimation( p_name );
					animation.SetScale( p_scale );
				}
			}

			itAnim->second.m_scale = p_scale;
		}
	}

	void AnimatedObjectGroup::Update()
	{
#if defined( NDEBUG )

		auto tslf = std::chrono::duration_cast< std::chrono::milliseconds >( m_timer.Time() );

#else

		auto tslf = 25_ms;

#endif

		for ( auto it : m_objects )
		{
			it.second->Update( tslf );
		}
	}

	void AnimatedObjectGroup::StartAnimation( String const & p_name )
	{
		auto itAnim = m_animations.find( p_name );

		if ( itAnim != m_animations.end() )
		{
			for ( auto it : m_objects )
			{
				it.second->StartAnimation( p_name );
			}

			itAnim->second.m_state = AnimationState::ePlaying;
		}
	}

	void AnimatedObjectGroup::StopAnimation( String const & p_name )
	{
		auto itAnim = m_animations.find( p_name );

		if ( itAnim != m_animations.end() )
		{
			for ( auto it : m_objects )
			{
				it.second->StopAnimation( p_name );
			}

			itAnim->second.m_state = AnimationState::eStopped;
		}
	}

	void AnimatedObjectGroup::PauseAnimation( String const & p_name )
	{
		auto itAnim = m_animations.find( p_name );

		if ( itAnim != m_animations.end() )
		{
			for ( auto it : m_objects )
			{
				it.second->PauseAnimation( p_name );
			}

			itAnim->second.m_state = AnimationState::ePaused;
		}
	}

	void AnimatedObjectGroup::StartAllAnimations()
	{
		for ( auto it : m_objects )
		{
			it.second->StartAllAnimations();
		}

		for ( auto it : m_animations )
		{
			it.second.m_state = AnimationState::ePlaying;
		}
	}

	void AnimatedObjectGroup::StopAllAnimations()
	{
		for ( auto it : m_objects )
		{
			it.second->StopAllAnimations();
		}

		for ( auto it : m_animations )
		{
			it.second.m_state = AnimationState::eStopped;
		}
	}

	void AnimatedObjectGroup::PauseAllAnimations()
	{
		for ( auto it : m_objects )
		{
			it.second->PauseAllAnimations();
		}

		for ( auto it : m_animations )
		{
			it.second.m_state = AnimationState::ePaused;
		}
	}
}
