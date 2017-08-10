#include "AnimatedObjectGroup.hpp"

#include "AnimatedObject.hpp"
#include "AnimatedSkeleton.hpp"
#include "AnimatedMesh.hpp"

#include "Scene/Geometry.hpp"
#include "Mesh/Mesh.hpp"
#include "Mesh/Skeleton/Skeleton.hpp"

using namespace castor;

#ifdef getObject
#	undef getObject
#endif

namespace castor3d
{
	AnimatedObjectGroup::TextWriter::TextWriter( String const & p_tabs )
		: castor::TextWriter< AnimatedObjectGroup >{ p_tabs }
	{
	}

	bool AnimatedObjectGroup::TextWriter::operator()( AnimatedObjectGroup const & p_group, TextFile & p_file )
	{
		Logger::logInfo( m_tabs + cuT( "Writing AnimatedObjectGroup " ) + p_group.getName() );
		bool result = p_file.writeText( cuT( "\n" ) + m_tabs + cuT( "animated_object_group \"" ) + p_group.getName() + cuT( "\"\n" ) ) > 0
						&& p_file.writeText( m_tabs + cuT( "{\n" ) ) > 0;
		castor::TextWriter< AnimatedObjectGroup >::checkError( result, "AnimatedObjectGroup name" );

		StrSet written;

		for ( auto it : p_group.getObjects() )
		{
			auto name = it.first;
			bool write{ true };
			size_t skel = name.find( cuT( "_Skeleton" ) );
			size_t mesh = name.find( cuT( "_Mesh" ) );

			// Only add objects, and not skeletons or meshes
			if ( skel != String::npos )
			{
				name = name.substr( 0, skel );
				write = p_group.getObjects().find( name ) == p_group.getObjects().end()
						  && written.find( name ) == written.end();
			}
			else if ( mesh != String::npos )
			{
				name = name.substr( 0, mesh );
				write = p_group.getObjects().find( name ) == p_group.getObjects().end()
						  && written.find( name ) == written.end();
			}

			if ( write )
			{
				result &= p_file.writeText( m_tabs + cuT( "\tanimated_object \"" ) + name + cuT( "\"\n" ) ) > 0;
				written.insert( name );
				castor::TextWriter< AnimatedObjectGroup >::checkError( result, "AnimatedObjectGroup object name" );
			}
		}

		if ( !p_group.getAnimations().empty() )
		{
			result &= p_file.writeText( cuT( "\n" ) ) > 0;

			for ( auto it : p_group.getAnimations() )
			{
				result &= p_file.writeText( m_tabs + cuT( "\tanimation \"" ) + it.first + cuT( "\"\n" ) ) > 0
							&& p_file.writeText( m_tabs + cuT( "\t{\n" ) ) > 0
							&& p_file.writeText( m_tabs + cuT( "\t\tlooped " ) + String{ it.second.m_looped ? cuT( "true" ) : cuT( "false" ) } +cuT( "\n" ) ) > 0
							&& p_file.writeText( m_tabs + cuT( "\t\tscale " ) + string::toString( it.second.m_scale ) + cuT( "\n" ) ) > 0
							&& p_file.writeText( m_tabs + cuT( "\t}\n" ) ) > 0;
				castor::TextWriter< AnimatedObjectGroup >::checkError( result, "AnimatedObjectGroup animation" );
			}
		}

		if ( !p_group.getAnimations().empty() )
		{
			result &= p_file.writeText( cuT( "\n" ) ) > 0;

			for ( auto it : p_group.getAnimations() )
			{
				if ( it.second.m_state == AnimationState::ePlaying )
				{
					result &= p_file.writeText( m_tabs + cuT( "\tstart_animation \"" ) + it.first + cuT( "\"\n" ) ) > 0;
					castor::TextWriter< AnimatedObjectGroup >::checkError( result, "AnimatedObjectGroup started animation" );
				}
			}
		}

		if ( result )
		{
			result = p_file.writeText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return result;
	}

	//*************************************************************************************************

	AnimatedObjectGroup::AnimatedObjectGroup( String const & p_name, Scene & p_scene )
		: Named( p_name )
		, OwnedBy< Scene >( p_scene )
	{
		m_timer.getElapsed();
	}

	AnimatedObjectGroup::~AnimatedObjectGroup()
	{
		m_objects.clear();
		m_animations.clear();
	}

	AnimatedObjectSPtr AnimatedObjectGroup::addObject( MovableObject & p_object, String const & p_name )
	{
		return nullptr;
	}

	AnimatedObjectSPtr AnimatedObjectGroup::addObject( Mesh & p_object, String const & p_name )
	{
		auto object = std::make_shared< AnimatedMesh >( p_name, p_object );

		if ( !addObject( object ) )
		{
			object.reset();
		}

		return object;
	}

	AnimatedObjectSPtr AnimatedObjectGroup::addObject( Skeleton & p_object, String const & p_name )
	{
		auto object = std::make_shared< AnimatedSkeleton >( p_name, p_object );

		if ( !addObject( object ) )
		{
			object.reset();
		}

		return object;
	}

	bool AnimatedObjectGroup::addObject( AnimatedObjectSPtr p_object )
	{
		bool result = p_object && m_objects.find( p_object->getName() ) == m_objects.end();

		if ( result )
		{
			m_objects.insert( { p_object->getName(), p_object } );
		}

		for ( auto it : m_animations )
		{
			p_object->addAnimation( it.first );
			auto & animation = p_object->getAnimation( it.first );
			animation.setLooped( it.second.m_looped );
			animation.setScale( it.second.m_scale );
		}

		return result;
	}

	void AnimatedObjectGroup::addAnimation( String const & p_name )
	{
		if ( m_animations.find( p_name ) == m_animations.end() )
		{
			m_animations.insert( { p_name, { AnimationState::eStopped, false, 1.0f } } );

			for ( auto it : m_objects )
			{
				it.second->addAnimation( p_name );
			}
		}
	}

	void AnimatedObjectGroup::setAnimationLooped( castor::String const & p_name, bool p_looped )
	{
		auto itAnim = m_animations.find( p_name );

		if ( itAnim != m_animations.end() )
		{
			for ( auto it : m_objects )
			{
				if ( it.second->hasAnimation( p_name ) )
				{
					auto & animation = it.second->getAnimation( p_name );
					animation.setLooped( p_looped );
				}
			}

			itAnim->second.m_looped = p_looped;
		}
	}

	void AnimatedObjectGroup::setAnimationscale( castor::String const & p_name, float p_scale )
	{
		auto itAnim = m_animations.find( p_name );

		if ( itAnim != m_animations.end() )
		{
			for ( auto it : m_objects )
			{
				if ( it.second->hasAnimation( p_name ) )
				{
					auto & animation = it.second->getAnimation( p_name );
					animation.setScale( p_scale );
				}
			}

			itAnim->second.m_scale = p_scale;
		}
	}

	void AnimatedObjectGroup::update()
	{
#if defined( NDEBUG )

		auto tslf = std::chrono::duration_cast< Milliseconds >( m_timer.getElapsed() );

#else

		auto tslf = 25_ms;

#endif

		for ( auto it : m_objects )
		{
			it.second->update( tslf );
		}
	}

	void AnimatedObjectGroup::startAnimation( String const & p_name )
	{
		auto itAnim = m_animations.find( p_name );

		if ( itAnim != m_animations.end() )
		{
			for ( auto it : m_objects )
			{
				it.second->startAnimation( p_name );
			}

			itAnim->second.m_state = AnimationState::ePlaying;
		}
	}

	void AnimatedObjectGroup::stopAnimation( String const & p_name )
	{
		auto itAnim = m_animations.find( p_name );

		if ( itAnim != m_animations.end() )
		{
			for ( auto it : m_objects )
			{
				it.second->stopAnimation( p_name );
			}

			itAnim->second.m_state = AnimationState::eStopped;
		}
	}

	void AnimatedObjectGroup::pauseAnimation( String const & p_name )
	{
		auto itAnim = m_animations.find( p_name );

		if ( itAnim != m_animations.end() )
		{
			for ( auto it : m_objects )
			{
				it.second->pauseAnimation( p_name );
			}

			itAnim->second.m_state = AnimationState::ePaused;
		}
	}

	void AnimatedObjectGroup::startAllAnimations()
	{
		for ( auto it : m_objects )
		{
			it.second->startAllAnimations();
		}

		for ( auto it : m_animations )
		{
			it.second.m_state = AnimationState::ePlaying;
		}
	}

	void AnimatedObjectGroup::stopAllAnimations()
	{
		for ( auto it : m_objects )
		{
			it.second->stopAllAnimations();
		}

		for ( auto it : m_animations )
		{
			it.second.m_state = AnimationState::eStopped;
		}
	}

	void AnimatedObjectGroup::pauseAllAnimations()
	{
		for ( auto it : m_objects )
		{
			it.second->pauseAllAnimations();
		}

		for ( auto it : m_animations )
		{
			it.second.m_state = AnimationState::ePaused;
		}
	}
}
