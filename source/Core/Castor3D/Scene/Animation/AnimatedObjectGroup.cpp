#include "Castor3D/Scene/Animation/AnimatedObjectGroup.hpp"

#include "Castor3D/Scene/Animation/AnimatedObject.hpp"
#include "Castor3D/Scene/Animation/AnimatedSkeleton.hpp"
#include "Castor3D/Scene/Animation/AnimatedMesh.hpp"

#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Skeleton/Skeleton.hpp"

using namespace castor;

#ifdef getObject
#	undef getObject
#endif

namespace castor3d
{
	AnimatedObjectGroup::TextWriter::TextWriter( String const & tabs )
		: castor::TextWriter< AnimatedObjectGroup >{ tabs }
	{
	}

	bool AnimatedObjectGroup::TextWriter::operator()( AnimatedObjectGroup const & group, TextFile & file )
	{
		log::info << m_tabs << cuT( "Writing AnimatedObjectGroup " ) << group.getName() << std::endl;
		bool result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "animated_object_group \"" ) + group.getName() + cuT( "\"\n" ) ) > 0
						&& file.writeText( m_tabs + cuT( "{\n" ) ) > 0;
		castor::TextWriter< AnimatedObjectGroup >::checkError( result, "AnimatedObjectGroup name" );

		StrSet written;

		for ( auto it : group.getObjects() )
		{
			auto name = it.first;
			bool write{ true };
			size_t skel = name.find( cuT( "_Skeleton" ) );
			size_t mesh = name.find( cuT( "_Mesh" ) );

			// Only add objects, and not skeletons or meshes
			if ( skel != String::npos )
			{
				name = name.substr( 0, skel );
				write = group.getObjects().find( name ) == group.getObjects().end()
						  && written.find( name ) == written.end();
			}
			else if ( mesh != String::npos )
			{
				name = name.substr( 0, mesh );
				write = group.getObjects().find( name ) == group.getObjects().end()
						  && written.find( name ) == written.end();
			}

			if ( write )
			{
				result = result && ( file.writeText( m_tabs + cuT( "\tanimated_object \"" ) + name + cuT( "\"\n" ) ) > 0 );
				written.insert( name );
				castor::TextWriter< AnimatedObjectGroup >::checkError( result, "AnimatedObjectGroup object name" );
			}
		}

		if ( !group.getAnimations().empty() )
		{
			result = result && ( file.writeText( cuT( "\n" ) ) > 0 );

			for ( auto it : group.getAnimations() )
			{
				result = result
					&& ( file.writeText( m_tabs + cuT( "\tanimation \"" ) + it.first + cuT( "\"\n" ) ) > 0 )
					&& ( file.writeText( m_tabs + cuT( "\t{\n" ) ) > 0 )
					&& ( file.writeText( m_tabs + cuT( "\t\tlooped " ) + String{ it.second.m_looped ? cuT( "true" ) : cuT( "false" ) } +cuT( "\n" ) ) > 0 )
					&& ( file.writeText( m_tabs + cuT( "\t\tscale " ) + string::toString( it.second.m_scale, std::locale{ "C" } ) + cuT( "\n" ) ) > 0 )
					&& ( file.writeText( m_tabs + cuT( "\t}\n" ) ) > 0 );
				castor::TextWriter< AnimatedObjectGroup >::checkError( result, "AnimatedObjectGroup animation" );
			}
		}

		if ( !group.getAnimations().empty() )
		{
			result = result && file.writeText( cuT( "\n" ) ) > 0;

			for ( auto it : group.getAnimations() )
			{
				if ( it.second.m_state == AnimationState::ePlaying )
				{
					result = result && ( file.writeText( m_tabs + cuT( "\tstart_animation \"" ) + it.first + cuT( "\"\n" ) ) > 0 );
					castor::TextWriter< AnimatedObjectGroup >::checkError( result, "AnimatedObjectGroup started animation" );
				}
			}
		}

		if ( result )
		{
			result = file.writeText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return result;
	}

	//*************************************************************************************************

	AnimatedObjectGroup::AnimatedObjectGroup( String const & name, Scene & scene )
		: Named( name )
		, OwnedBy< Scene >( scene )
	{
		m_timer.getElapsed();
	}

	AnimatedObjectGroup::~AnimatedObjectGroup()
	{
		m_objects.clear();
		m_animations.clear();
	}

	AnimatedObjectSPtr AnimatedObjectGroup::addObject( MovableObject & object
		, String const & name )
	{
		return nullptr;
	}

	AnimatedObjectSPtr AnimatedObjectGroup::addObject( Mesh & mesh
		, Geometry & geometry
		, String const & name )
	{
		auto object = std::make_shared< AnimatedMesh >( name, mesh, geometry );

		if ( !addObject( object ) )
		{
			object.reset();
		}

		return object;
	}

	AnimatedObjectSPtr AnimatedObjectGroup::addObject( Skeleton & skeleton
		, Mesh & mesh
		, Geometry & geometry
		, String const & name )
	{
		auto object = std::make_shared< AnimatedSkeleton >( name, skeleton, mesh, geometry );

		if ( !addObject( object ) )
		{
			object.reset();
		}

		return object;
	}

	bool AnimatedObjectGroup::addObject( AnimatedObjectSPtr object )
	{
		bool result = object && m_objects.find( object->getName() ) == m_objects.end();

		if ( result )
		{
			m_objects.insert( { object->getName(), object } );

			switch ( object->getKind() )
			{
			case AnimationType::eMesh:
				onMeshAdded( *this, static_cast< AnimatedMesh const & >( *object ) );
				break;

			case AnimationType::eSkeleton:
				onSkeletonAdded( *this, static_cast< AnimatedSkeleton const & >( *object ) );
				break;

			default:
				break;
			}
		}

		for ( auto it : m_animations )
		{
			object->addAnimation( it.first );
			auto & animation = object->getAnimation( it.first );
			animation.setLooped( it.second.m_looped );
			animation.setScale( it.second.m_scale );
		}

		return result;
	}

	void AnimatedObjectGroup::addAnimation( String const & name )
	{
		if ( m_animations.find( name ) == m_animations.end() )
		{
			m_animations.insert( { name, { AnimationState::eStopped, false, 1.0f } } );

			for ( auto it : m_objects )
			{
				it.second->addAnimation( name );
			}
		}
	}

	void AnimatedObjectGroup::setAnimationLooped( castor::String const & name
		, bool looped )
	{
		auto itAnim = m_animations.find( name );

		if ( itAnim != m_animations.end() )
		{
			for ( auto it : m_objects )
			{
				if ( it.second->hasAnimation( name ) )
				{
					auto & animation = it.second->getAnimation( name );
					animation.setLooped( looped );
				}
			}

			itAnim->second.m_looped = looped;
		}
	}

	void AnimatedObjectGroup::setAnimationscale( castor::String const & name
		, float scale )
	{
		auto itAnim = m_animations.find( name );

		if ( itAnim != m_animations.end() )
		{
			for ( auto it : m_objects )
			{
				if ( it.second->hasAnimation( name ) )
				{
					auto & animation = it.second->getAnimation( name );
					animation.setScale( scale );
				}
			}

			itAnim->second.m_scale = scale;
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

	void AnimatedObjectGroup::startAnimation( String const & name )
	{
		auto itAnim = m_animations.find( name );

		if ( itAnim != m_animations.end() )
		{
			for ( auto it : m_objects )
			{
				it.second->startAnimation( name );
			}

			itAnim->second.m_state = AnimationState::ePlaying;
		}
	}

	void AnimatedObjectGroup::stopAnimation( String const & name )
	{
		auto itAnim = m_animations.find( name );

		if ( itAnim != m_animations.end() )
		{
			for ( auto it : m_objects )
			{
				it.second->stopAnimation( name );
			}

			itAnim->second.m_state = AnimationState::eStopped;
		}
	}

	void AnimatedObjectGroup::pauseAnimation( String const & name )
	{
		auto itAnim = m_animations.find( name );

		if ( itAnim != m_animations.end() )
		{
			for ( auto it : m_objects )
			{
				it.second->pauseAnimation( name );
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
