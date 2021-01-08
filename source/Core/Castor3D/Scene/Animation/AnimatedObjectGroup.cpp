#include "Castor3D/Scene/Animation/AnimatedObjectGroup.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Skeleton/Skeleton.hpp"
#include "Castor3D/Render/RenderLoop.hpp"
#include "Castor3D/Scene/Animation/AnimatedObject.hpp"
#include "Castor3D/Scene/Animation/AnimatedSkeleton.hpp"
#include "Castor3D/Scene/Animation/AnimatedMesh.hpp"
#include "Castor3D/Scene/Geometry.hpp"

using namespace castor;

#ifdef getObject
#	undef getObject
#endif

namespace castor3d
{
	namespace
	{
		template< typename FuncT, typename ParamT >
		bool applyAnimationFunc( GroupAnimationMap & animations
			, AnimatedObjectPtrStrMap & objects
			, castor::String const & name
			, FuncT func
			, ParamT const & value
			, size_t outputOffset )
		{
			auto itAnim = animations.find( name );
			auto result = itAnim != animations.end();

			if ( result )
			{
				for ( auto it : objects )
				{
					if ( it.second->hasAnimation( name ) )
					{
						auto & animation = it.second->getAnimation( name );
						( animation.*func )( value );
					}
				}

				auto buffer = reinterpret_cast< uint8_t * >( &itAnim->second );
				*reinterpret_cast< ParamT * >( buffer + outputOffset ) = value;
			}

			return result;
		}
	}

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
					&& ( file.writeText( m_tabs + cuT( "\t\tlooped " ) + String{ it.second.looped ? cuT( "true" ) : cuT( "false" ) } +cuT( "\n" ) ) > 0 )
					&& ( file.writeText( m_tabs + cuT( "\t\tscale " ) + string::toString( it.second.scale, std::locale{ "C" } ) + cuT( "\n" ) ) > 0 )
					&& ( it.second.startingPoint == 0_ms ? true : file.writeText( m_tabs + cuT( "\t\tstart_at " ) + string::toString( it.second.startingPoint.count() / 1000.0, std::locale{ "C" } ) + cuT( "\n" ) ) > 0 )
					&& ( it.second.stoppingPoint == 0_ms ? true : file.writeText( m_tabs + cuT( "\t\tstop_at " ) + string::toString( it.second.stoppingPoint.count() / 1000.0, std::locale{ "C" } ) + cuT( "\n" ) ) > 0 )
					&& ( file.writeText( m_tabs + cuT( "\t}\n" ) ) > 0 );
				castor::TextWriter< AnimatedObjectGroup >::checkError( result, "AnimatedObjectGroup animation" );
			}
		}

		if ( !group.getAnimations().empty() )
		{
			result = result && file.writeText( cuT( "\n" ) ) > 0;

			for ( auto it : group.getAnimations() )
			{
				if ( it.second.state == AnimationState::ePlaying )
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
			animation.setLooped( it.second.looped );
			animation.setScale( it.second.scale );
			animation.setStartingPoint( it.second.startingPoint );
			animation.setStoppingPoint( it.second.stoppingPoint );
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
		applyAnimationFunc( m_animations
			, m_objects
			, name
			, &AnimationInstance::setLooped
			, looped
			, offsetof( GroupAnimation, looped ) );
	}

	void AnimatedObjectGroup::setAnimationScale( castor::String const & name
		, float scale )
	{
		applyAnimationFunc( m_animations
			, m_objects
			, name
			, &AnimationInstance::setScale
			, scale
			, offsetof( GroupAnimation, scale ) );
	}

	void AnimatedObjectGroup::setAnimationStartingPoint( castor::String const & name
		, castor::Milliseconds value )
	{
		applyAnimationFunc( m_animations
			, m_objects
			, name
			, &AnimationInstance::setStartingPoint
			, value
			, offsetof( GroupAnimation, startingPoint ) );
	}

	void AnimatedObjectGroup::setAnimationStoppingPoint( castor::String const & name
		, castor::Milliseconds value )
	{
		applyAnimationFunc( m_animations
			, m_objects
			, name
			, &AnimationInstance::setStoppingPoint
			, value
			, offsetof( GroupAnimation, stoppingPoint ) );
	}

	void AnimatedObjectGroup::update( CpuUpdater & updater )
	{
#if defined( NDEBUG )

		auto tslf = updater.tslf > 0_ms
			? updater.tslf
			: std::chrono::duration_cast< Milliseconds >( m_timer.getElapsed() );

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

			itAnim->second.state = AnimationState::ePlaying;
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

			itAnim->second.state = AnimationState::eStopped;
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

			itAnim->second.state = AnimationState::ePaused;
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
			it.second.state = AnimationState::ePlaying;
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
			it.second.state = AnimationState::eStopped;
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
			it.second.state = AnimationState::ePaused;
		}
	}
}
