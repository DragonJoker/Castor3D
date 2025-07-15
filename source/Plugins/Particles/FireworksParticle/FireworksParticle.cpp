#include "FireworksParticle/FireworksParticle.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderDevice.hpp>
#include <Castor3D/Render/RenderLoop.hpp>
#include <Castor3D/Scene/BillboardList.hpp>
#include <Castor3D/Scene/SceneNode.hpp>
#include <Castor3D/Scene/ParticleSystem/ParticleSystem.hpp>

#include <ashespp/Buffer/VertexBuffer.hpp>

#include <random>

namespace fireworks
{
	//*********************************************************************************************

	namespace
	{
		class ParticleEmitter
			: public c3d::ParticleEmitter
		{
		protected:
			ParticleEmitter( c3d::ParticleDeclaration const & decl
				, float type );

		public:
			c3d::Particle emit( c3d::Point3f const & position
				, c3d::Point3f const & velocity
				, float age );

		private:
			float m_type;
		};

		class PrimaryParticleEmitter
			: public ParticleEmitter
		{
		public:
			explicit PrimaryParticleEmitter( c3d::ParticleDeclaration const & decl );
		};

		class SecondaryParticleEmitter
			: public ParticleEmitter
		{
		public:
			explicit SecondaryParticleEmitter( c3d::ParticleDeclaration const & decl );
		};

		class ParticleUpdater
			: public c3d::ParticleUpdater
		{
		public:
			ParticleUpdater( c3d::ParticleSystem const & system
				, c3d::ParticleDeclaration const & inputs
				, c3d::ParticleEmitterArray & emitters );
			void update( c3d::Milliseconds const & time
				, c3d::Particle & particle )override;

		private:
			c3d::ParticleDeclaration::const_iterator m_type;
			c3d::ParticleDeclaration::const_iterator m_position;
			c3d::ParticleDeclaration::const_iterator m_velocity;
			c3d::ParticleDeclaration::const_iterator m_age;
		};

		//*****************************************************************************************

		constexpr float g_launcher = 0.0f;
		constexpr float g_shell = 1.0f;
		constexpr float g_secondaryShell = 2.0f;

		enum
		{
			ePosition,
			eType,
			eVelocity,
			eAge,
		};

		constexpr c3d::Milliseconds g_launcherCooldown = 100_ms;
		constexpr c3d::Milliseconds g_shellLifetime = 10000_ms;
		constexpr c3d::Milliseconds g_secondaryShellLifetime = 2500_ms;

		inline float getRandomFloat()
		{
			static std::minstd_rand device;
			std::uniform_real_distribution< float > distribution{ -1.0f, 1.0f };
			return distribution( device );
		}

		inline c3d::Point3f doGetRandomDirection()
		{
			return c3d::Point3f{ getRandomFloat(), getRandomFloat(), getRandomFloat() };
		}

		inline void doUpdateLauncher( ParticleEmitter & emitter
			, c3d::Coords3f & position
			, float & age )
		{
			if ( age >= float( g_launcherCooldown.count() ) )
			{
				c3d::Point3f velocity{ doGetRandomDirection() * 5.0f };
				velocity[1] = std::max( velocity[1] * 7.0f, 10.0f );
				emitter.emit( c3d::Point3f{ position }
					, velocity
					, 0.0f );
				age = 0.0f;
			}
		}

		inline void doUpdateShell( ParticleEmitter & emitter
			, c3d::Milliseconds const & time
			, float & type
			, c3d::Coords3f & position
			, c3d::Coords3f & velocity
			, float & age )
		{
			auto deltaS = float( time.count() ) / 1000.0f;
			c3d::Point3f delta{ deltaS, deltaS, deltaS };
			c3d::Point3f deltaP = delta * velocity;
			c3d::Point3f deltaV = delta * c3d::Point3f{ 0.0f, -0.981f, 0.0f };

			if ( age < float( g_shellLifetime.count() ) )
			{
				position += deltaP;
				velocity += deltaV;
			}
			else
			{
				for ( int i = 1; i < 10; ++i )
				{
					emitter.emit( c3d::Point3f{ position }
						, ( doGetRandomDirection() * 5.0f ) + velocity / 2.0f
						, 0.0f );
				}

				// Turn this shell to a secondary shell, to decrease the holes in buffer
				type = g_secondaryShell;
				velocity = ( doGetRandomDirection() * 5.0f ) + velocity / 2.0f;
				age = 0.0f;
			}
		}

		inline void doUpdateSecondaryShell( c3d::Milliseconds const & time
			, float & type
			, c3d::Coords3f & position
			, c3d::Coords3f & velocity
			, float & age )
		{
			if ( age < float( g_secondaryShellLifetime.count() ) )
			{
				auto deltaS = float( time.count() ) / 1000.0f;
				c3d::Point3f delta{ deltaS, deltaS, deltaS };
				c3d::Point3f deltaP = delta * velocity;
				c3d::Point3f deltaV = delta * c3d::Point3f{ 0.0f, -0.981f, 0.0f };
				position += deltaP;
				velocity += deltaV;
			}
			else
			{
				type = g_launcher;
			}
		}

		//*****************************************************************************************

		ParticleEmitter::ParticleEmitter( c3d::ParticleDeclaration const & decl
			, float type )
			: c3d::ParticleEmitter{ decl }
			, m_type{ type }
		{
		}

		c3d::Particle ParticleEmitter::emit( c3d::Point3f const & position
			, c3d::Point3f const & velocity
			, float age )
		{
			c3d::ParticleValues particle;
			particle.add( size_t( ePosition ), position );
			particle.add( size_t( eType ), m_type );
			particle.add( size_t( eVelocity ), velocity );
			particle.add( size_t( eAge ), age );
			return c3d::ParticleEmitter::emit( particle );
		}

		//*****************************************************************************************

		PrimaryParticleEmitter::PrimaryParticleEmitter( c3d::ParticleDeclaration const & decl )
			: ParticleEmitter{ decl, g_shell }
		{
		}

		//*****************************************************************************************

		SecondaryParticleEmitter::SecondaryParticleEmitter( c3d::ParticleDeclaration const & decl )
			: ParticleEmitter{ decl, g_secondaryShell }
		{
		}

		//*****************************************************************************************

		ParticleUpdater::ParticleUpdater( c3d::ParticleSystem const & system
			, c3d::ParticleDeclaration const & inputs
			, c3d::ParticleEmitterArray & emitters )
			: c3d::ParticleUpdater{ system, inputs, emitters }
			, m_type{ std::find_if( inputs.begin()
				, inputs.end()
				, []( c3d::ParticleElementDeclaration const & element )
				{
					return element.m_name == cuT( "type" );
				} ) }
			, m_position{ std::find_if( inputs.begin()
				, inputs.end()
				, []( c3d::ParticleElementDeclaration const & element )
				{
					return element.m_name == cuT( "position" );
				} ) }
			, m_velocity{ std::find_if( inputs.begin()
				, inputs.end()
				, []( c3d::ParticleElementDeclaration const & element )
				{
					return element.m_name == cuT( "velocity" );
				} ) }
			, m_age{ std::find_if( inputs.begin()
				, inputs.end()
				, []( c3d::ParticleElementDeclaration const & element )
				{
					return element.m_name == cuT( "age" );
				} ) }
		{
			if ( m_type == inputs.end()
				|| m_position == inputs.end()
				|| m_velocity == inputs.end()
				|| m_age == inputs.end() )
			{
				CU_Exception( "All particle data offsets couldn't be found." );
			}
		}

		void ParticleUpdater::update( c3d::Milliseconds const & time
			, c3d::Particle & particle )
		{
			c3d::Coords3f pos{ reinterpret_cast< float * >( particle.getData() + m_position->m_offset ) };
			c3d::Coords3f vel{ reinterpret_cast< float * >( particle.getData() + m_velocity->m_offset ) };
			float * age{ reinterpret_cast< float * >( particle.getData() + m_age->m_offset ) };
			float * type{ reinterpret_cast< float * >( particle.getData() + m_type->m_offset ) };
			*age += float( time.count() );

			if ( *type == g_launcher )
			{
				doUpdateLauncher( static_cast< ParticleEmitter & >( *m_emitters[size_t( g_shell )] )
					, pos
					, *age );
				auto worldPosition = m_system.getParent()->getDerivedPosition();
				pos[0] = worldPosition[0];
				pos[1] = worldPosition[1];
				pos[2] = worldPosition[2];
			}
			else if ( *type == g_shell )
			{
				doUpdateShell( static_cast< ParticleEmitter & >( *m_emitters[size_t( g_secondaryShell )] )
					, time
					, *type
					, pos
					, vel
					, *age );
			}
			else
			{
				doUpdateSecondaryShell( time
					, *type
					, pos
					, vel
					, *age );
			}
		}
	}

	//*********************************************************************************************

	c3d::String const ParticleSystem::Type = cuT( "fireworks" );
	c3d::MbString const ParticleSystem::Name = "Fireworks Particle";

	ParticleSystem::ParticleSystem( c3d::ParticleSystem & parent )
		: CpuParticleSystem( parent )
	{
	}

	c3d::CpuParticleSystemUPtr ParticleSystem::create( c3d::ParticleSystem & parent )
	{
		return c3d::makeUniqueDerived< c3d::CpuParticleSystem, ParticleSystem >( parent );
	}

	bool ParticleSystem::doInitialise()
	{
		addEmitter( nullptr );
		addEmitter( c3d::makeUniqueDerived< c3d::ParticleEmitter, PrimaryParticleEmitter >( getParent().getParticleVariables() ) );
		addEmitter( c3d::makeUniqueDerived< c3d::ParticleEmitter, SecondaryParticleEmitter >( getParent().getParticleVariables() ) );
		addUpdater( c3d::makeUniqueDerived< c3d::ParticleUpdater, ParticleUpdater >( getParent(), m_inputs, m_emitters ) );
		return true;
	}

	void ParticleSystem::doPackParticles()
	{
		for ( auto i = 1u; i < m_firstUnused && m_firstUnused > 1u; ++i )
		{
			auto & particle = m_particles[i];

			if ( particle.getValue< c3d::ParticleFormat::eFloat >( eType ) == 0.0f )
			{
				particle = c3d::move( m_particles[m_firstUnused - 1] );
				--m_firstUnused;
			}
		}
	}

	//*********************************************************************************************
}
