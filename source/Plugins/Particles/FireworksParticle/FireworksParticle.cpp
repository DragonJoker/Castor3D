#include "FireworksParticle/FireworksParticle.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderDevice.hpp>
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
			: public castor3d::ParticleEmitter
		{
		protected:
			ParticleEmitter( castor3d::ParticleDeclaration const & decl
				, float type );

		public:
			castor3d::Particle emit( castor::Point3f const & position
				, castor::Point3f const & velocity
				, float age );

		private:
			float m_type;
		};

		class PrimaryParticleEmitter
			: public ParticleEmitter
		{
		public:
			explicit PrimaryParticleEmitter( castor3d::ParticleDeclaration const & decl );
		};

		class SecondaryParticleEmitter
			: public ParticleEmitter
		{
		public:
			explicit SecondaryParticleEmitter( castor3d::ParticleDeclaration const & decl );
		};

		class ParticleUpdater
			: public castor3d::ParticleUpdater
		{
		public:
			ParticleUpdater( castor3d::ParticleSystem const & system
				, castor3d::ParticleDeclaration const & inputs
				, castor3d::ParticleEmitterArray & emitters );
			void update( castor::Milliseconds const & time
				, castor3d::Particle & particle );

		private:
			castor3d::ParticleDeclaration::const_iterator m_type;
			castor3d::ParticleDeclaration::const_iterator m_position;
			castor3d::ParticleDeclaration::const_iterator m_velocity;
			castor3d::ParticleDeclaration::const_iterator m_age;
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

		constexpr castor::Milliseconds g_launcherCooldown = 100_ms;
		constexpr castor::Milliseconds g_shellLifetime = 10000_ms;
		constexpr castor::Milliseconds g_secondaryShellLifetime = 2500_ms;

		inline float getRandomFloat()
		{
			static std::minstd_rand device;
			std::uniform_real_distribution< float > distribution{ -1.0f, 1.0f };
			return distribution( device );
		}

		inline castor::Point3f doGetRandomDirection()
		{
			return castor::Point3f{ getRandomFloat(), getRandomFloat(), getRandomFloat() };
		}

		inline void doUpdateLauncher( ParticleEmitter & emitter
			, castor::Coords3f & position
			, float & age )
		{
			if ( age >= g_launcherCooldown.count() )
			{
				castor::Point3f velocity{ doGetRandomDirection() * 5.0f };
				velocity[1] = std::max( velocity[1] * 7.0f, 10.0f );
				emitter.emit( castor::Point3f{ position }
					, velocity
					, 0.0f );
				age = 0.0f;
			}
		}

		inline void doUpdateShell( ParticleEmitter & emitter
			, castor::Milliseconds const & time
			, float & type
			, castor::Coords3f & position
			, castor::Coords3f & velocity
			, float & age )
		{
			castor::Point3f delta{ time.count() / 1000.0, time.count() / 1000.0, time.count() / 1000.0 };
			castor::Point3f deltaP = delta * velocity;
			castor::Point3f deltaV = delta * castor::Point3f{ 0.0f, -0.981f, 0.0f };

			if ( age < g_shellLifetime.count() )
			{
				position += deltaP;
				velocity += deltaV;
			}
			else
			{
				for ( int i = 1; i < 10; ++i )
				{
					emitter.emit( castor::Point3f{ position }
						, ( doGetRandomDirection() * 5.0f ) + velocity / 2.0f
						, 0.0f );
				}

				// Turn this shell to a secondary shell, to decrease the holes in buffer
				type = g_secondaryShell;
				velocity = ( doGetRandomDirection() * 5.0f ) + velocity / 2.0f;
				age = 0.0f;
			}
		}

		inline void doUpdateSecondaryShell( castor::Milliseconds const & time
			, float & type
			, castor::Coords3f & position
			, castor::Coords3f & velocity
			, float & age )
		{
			if ( age < g_secondaryShellLifetime.count() )
			{
				castor::Point3f delta{ time.count() / 1000.0, time.count() / 1000.0, time.count() / 1000.0 };
				castor::Point3f deltaP = delta * velocity;
				castor::Point3f deltaV = delta * castor::Point3f{ 0.0f, -0.981f, 0.0f };
				position += deltaP;
				velocity += deltaV;
			}
			else
			{
				type = g_launcher;
			}
		}

		inline void doPackParticles( castor3d::ParticleArray & particles
			, uint32_t & firstUnused )
		{
			for ( auto i = 1u; i < firstUnused && firstUnused > 1u; ++i )
			{
				auto & particle = particles[i];

				if ( particle.getValue< castor3d::ParticleFormat::eFloat >( eType ) == 0.0f )
				{
					particle = std::move( particles[firstUnused - 1] );
					--firstUnused;
				}
			}
		}

		//*****************************************************************************************

		ParticleEmitter::ParticleEmitter( castor3d::ParticleDeclaration const & decl
			, float type )
			: castor3d::ParticleEmitter{ decl }
			, m_type{ type }
		{
		}

		castor3d::Particle ParticleEmitter::emit( castor::Point3f const & position
			, castor::Point3f const & velocity
			, float age )
		{
			castor3d::ParticleValues particle;
			particle.add( size_t( ePosition ), position );
			particle.add( size_t( eType ), m_type );
			particle.add( size_t( eVelocity ), velocity );
			particle.add( size_t( eAge ), age );
			return castor3d::ParticleEmitter::emit( particle );
		}

		//*****************************************************************************************

		PrimaryParticleEmitter::PrimaryParticleEmitter( castor3d::ParticleDeclaration const & decl )
			: ParticleEmitter{ decl, g_shell }
		{
		}

		//*****************************************************************************************

		SecondaryParticleEmitter::SecondaryParticleEmitter( castor3d::ParticleDeclaration const & decl )
			: ParticleEmitter{ decl, g_secondaryShell }
		{
		}

		//*****************************************************************************************

		ParticleUpdater::ParticleUpdater( castor3d::ParticleSystem const & system
			, castor3d::ParticleDeclaration const & inputs
			, castor3d::ParticleEmitterArray & emitters )
			: castor3d::ParticleUpdater{ system, inputs, emitters }
			, m_type{ std::find_if( inputs.begin()
				, inputs.end()
				, []( castor3d::ParticleElementDeclaration const & element )
				{
					return element.m_name == cuT( "type" );
				} ) }
			, m_position{ std::find_if( inputs.begin()
				, inputs.end()
				, []( castor3d::ParticleElementDeclaration const & element )
				{
					return element.m_name == cuT( "position" );
				} ) }
			, m_velocity{ std::find_if( inputs.begin()
				, inputs.end()
				, []( castor3d::ParticleElementDeclaration const & element )
				{
					return element.m_name == cuT( "velocity" );
				} ) }
			, m_age{ std::find_if( inputs.begin()
				, inputs.end()
				, []( castor3d::ParticleElementDeclaration const & element )
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

		void ParticleUpdater::update( castor::Milliseconds const & time
			, castor3d::Particle & particle )
		{
			castor::Coords3f pos{ reinterpret_cast< float * >( particle.getData() + m_position->m_offset ) };
			castor::Coords3f vel{ reinterpret_cast< float * >( particle.getData() + m_velocity->m_offset ) };
			float * age{ reinterpret_cast< float * >( particle.getData() + m_age->m_offset ) };
			float * type{ reinterpret_cast< float * >( particle.getData() + m_type->m_offset ) };
			*age += time.count();

			if ( *type == g_launcher )
			{
				doUpdateLauncher( static_cast< ParticleEmitter & >( *m_emitters[size_t( g_shell )] )
					, pos
					, *age );
				auto worldPosition = m_system.getParent()->getDerivedPosition();
				pos[0] = float( worldPosition[0] );
				pos[1] = float( worldPosition[1] );
				pos[2] = float( worldPosition[2] );
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

	castor::String const ParticleSystem::Type = cuT( "fireworks" );
	castor::String const ParticleSystem::Name = cuT( "Fireworks Particle" );

	ParticleSystem::ParticleSystem( castor3d::ParticleSystem & parent )
		: CpuParticleSystem( parent )
	{
	}

	ParticleSystem::~ParticleSystem()
	{
	}

	castor3d::CpuParticleSystemUPtr ParticleSystem::create( castor3d::ParticleSystem & parent )
	{
		return std::make_unique< ParticleSystem >( parent );
	}

	uint32_t ParticleSystem::update( castor3d::RenderPassTimer & timer
		, castor::Milliseconds const & time
		, castor::Milliseconds const & total
		, uint32_t index )
	{
		auto firstUnused = m_firstUnused;

		for ( auto i = 0u; i < firstUnused; ++i )
		{
			m_updaters.front()->update( time
				, m_particles[i] );
		}

		doPackParticles( m_particles, m_firstUnused );
		auto & device = getCurrentRenderDevice( *m_parent.getParent()->getScene() );
		auto & vbo = m_parent.getBillboards()->getVertexBuffer();
		auto stride = m_inputs.stride();
		auto mappedSize = ashes::getAlignedSize( VkDeviceSize( m_firstUnused * stride )
			, device.properties.limits.nonCoherentAtomSize );

		if ( auto dst = vbo.getBuffer().lock( 0u, mappedSize, 0u ) )
		{
			for ( auto i = 0u; i < m_firstUnused; ++i )
			{
				std::memcpy( dst, m_particles[i].getData(), stride );
				dst += stride;
			}

			vbo.getBuffer().flush( 0u, mappedSize );
			vbo.getBuffer().unlock();
		}

		return m_firstUnused;
	}

	bool ParticleSystem::doInitialise()
	{
		m_firstUnused = 1u;
		addEmitter( nullptr );
		addEmitter( std::make_unique< PrimaryParticleEmitter >( getParent().getParticleVariables() ) );
		addEmitter( std::make_unique< SecondaryParticleEmitter >( getParent().getParticleVariables() ) );
		addUpdater( std::make_unique< ParticleUpdater >( getParent(), m_inputs, m_emitters ) );
		return true;
	}

	void ParticleSystem::doCleanup()
	{
		m_firstUnused = 1u;
	}

	void ParticleSystem::doOnEmit( castor3d::Particle const & particle )
	{
		m_particles[m_firstUnused++] = particle;
	}

	//*********************************************************************************************
}
