#include "FireworksParticle.hpp"

#include <Engine.hpp>

#include <Scene/ParticleSystem/ParticleSystem.hpp>
#include <Scene/BillboardList.hpp>

#include <Buffer/VertexBuffer.hpp>

#include <random>

using namespace castor;
using namespace castor3d;

namespace Fireworks
{
	namespace
	{
		constexpr float g_launcher = 0.0f;
		constexpr float g_shell = 1.0f;
		constexpr float g_secondaryShell = 2.0f;

		constexpr Milliseconds g_launcherCooldown = 100_ms;
		constexpr Milliseconds g_shellLifetime = 10000_ms;
		constexpr Milliseconds g_secondaryShellLifetime = 2500_ms;

		inline float getRandomFloat()
		{
			static std::minstd_rand device;
			std::uniform_real_distribution< float > distribution{ -1.0f, 1.0f };
			return distribution( device );
		}

		inline Point3f doGetRandomDirection()
		{
			return Point3f{ getRandomFloat(), getRandomFloat(), getRandomFloat() };
		}

		inline void doUpdateLauncher( ParticleSystem & system
			, Milliseconds const & time
			, float & type
			, Coords3f & position
			, Coords3f & velocity
			, float & age )
		{
			if ( age >= g_launcherCooldown.count() )
			{
				Point3f velocity{ doGetRandomDirection() * 5.0f };
				velocity[1] = std::max( velocity[1] * 7.0f, 10.0f );
				system.emitParticle( g_shell, Point3f{ position }, velocity, 0.0f );
				age = 0.0f;
			}

			auto absolutePosition = system.getParent().getParent()->getDerivedPosition();
			position[0] = float( absolutePosition[0] );
			position[1] = float( absolutePosition[1] );
			position[2] = float( absolutePosition[2] );
		}

		inline void doUpdateShell( ParticleSystem & system
			, Milliseconds const & time
			, float & type
			, Coords3f & position
			, Coords3f & velocity
			, float & age )
		{
			Point3f delta{ time.count() / 1000.0, time.count() / 1000.0, time.count() / 1000.0 };
			Point3f deltaP = delta * velocity;
			Point3f deltaV = delta * Point3f{ 0.0f, -0.981f, 0.0f };

			if ( age < g_shellLifetime.count() )
			{
				position += deltaP;
				velocity += deltaV;
			}
			else
			{
				for ( int i = 1; i < 10; ++i )
				{
					system.emitParticle( g_secondaryShell, Point3f{ position }, ( doGetRandomDirection() * 5.0f ) + velocity / 2.0f, 0.0f );
				}

				// Turn this shell to a secondary shell, to decrease the holes in buffer
				type = g_secondaryShell;
				velocity = ( doGetRandomDirection() * 5.0f ) + velocity / 2.0f;
				age = 0.0f;
			}
		}

		inline void doUpdateSecondaryShell( ParticleSystem & system
			, Milliseconds const & time
			, float & type
			, Coords3f & position
			, Coords3f & velocity
			, float & age )
		{
			if ( age < g_secondaryShellLifetime.count() )
			{
				Point3f delta{ time.count() / 1000.0, time.count() / 1000.0, time.count() / 1000.0 };
				Point3f deltaP = delta * velocity;
				Point3f deltaV = delta * Point3f{ 0.0f, -0.981f, 0.0f };
				position += deltaP;
				velocity += deltaV;
			}
			else
			{
				type = g_launcher;
			}
		}

		inline void doUpdateParticle( ParticleSystem & system
			, Milliseconds const & time
			, float & type
			, Coords3f & position
			, Coords3f & velocity
			, float & age )
		{
			age += time.count();

			if ( type == g_launcher )
			{
				doUpdateLauncher( system, time, type, position, velocity, age );
			}
			else if ( type == g_shell )
			{
				doUpdateShell( system, time, type, position, velocity, age );
			}
			else
			{
				doUpdateSecondaryShell( system, time, type, position, velocity, age );
			}
		}

		inline void doPackParticles( ParticleArray & particles, uint32_t & firstUnused )
		{
			for ( auto i = 1u; i < firstUnused && firstUnused > 1u; ++i )
			{
				auto & particle = particles[i];

				if ( particle.getValue< ParticleFormat::eFloat >( 0u ) == 0.0f )
				{
					particle = std::move( particles[firstUnused - 1] );
					--firstUnused;
				}
			}
		}
	}

	String const ParticleSystem::Type = cuT( "fireworks" );
	String const ParticleSystem::Name = cuT( "Fireworks Particle" );

	ParticleSystem::ParticleSystem( castor3d::ParticleSystem & parent )
		: CpuParticleSystem( parent )
	{
	}

	ParticleSystem::~ParticleSystem()
	{
	}

	CpuParticleSystemUPtr ParticleSystem::create( castor3d::ParticleSystem & parent )
	{
		return std::make_unique< ParticleSystem >( parent );
	}

	void ParticleSystem::emitParticle( float type
		, castor::Point3f const & position
		, castor::Point3f const & velocity
		, float age )
	{
		Particle particle{ m_inputs };
		particle.setValue< ParticleFormat::eFloat >( 0u, type );
		particle.setValue< ParticleFormat::eVec3f >( 1u, position );
		particle.setValue< ParticleFormat::eVec3f >( 2u, velocity );
		particle.setValue< ParticleFormat::eFloat >( 3u, age );
		m_particles[m_firstUnused++] = particle;
	}

	uint32_t ParticleSystem::update( Milliseconds const & time
		, Milliseconds const & total )
	{
		auto firstUnused = m_firstUnused;

		for ( auto i = 0u; i < firstUnused; ++i )
		{
			auto & particle = m_particles[i];
			Coords3f pos{ reinterpret_cast< float * >( particle.getData() + m_position->m_offset ) };
			Coords3f vel{ reinterpret_cast< float * >( particle.getData() + m_velocity->m_offset ) };
			doUpdateParticle(
				*this,
				time,
				*reinterpret_cast< float * >( particle.getData() + m_type->m_offset ),
				pos,
				vel,
				*reinterpret_cast< float * >( particle.getData() + m_age->m_offset ) );
		}

		doPackParticles( m_particles, m_firstUnused );
		auto & vbo = m_parent.getBillboards()->getVertexBuffer();
		auto stride = m_inputs.stride();

		if ( auto dst = vbo.getBuffer().lock( 0u
			, m_firstUnused * stride
			, renderer::MemoryMapFlag::eWrite | renderer::MemoryMapFlag::eInvalidateRange ) )
		{
			for ( auto i = 0u; i < m_firstUnused; ++i )
			{
				std::memcpy( dst, m_particles[i].getData(), stride );
				dst += stride;
			}

			vbo.getBuffer().flush( 0u, m_firstUnused * stride );
			vbo.getBuffer().unlock();
		}

		return m_firstUnused;
	}

	bool ParticleSystem::doInitialise()
	{
		m_type = std::find_if( m_inputs.begin()
			, m_inputs.end()
			, []( ParticleElementDeclaration const & element )
			{
				return element.m_name == cuT( "type" );
			} );

		m_position = std::find_if( m_inputs.begin()
			, m_inputs.end()
			, []( ParticleElementDeclaration const & element )
			{
				return element.m_name == cuT( "position" );
			} );

		m_velocity = std::find_if( m_inputs.begin()
			, m_inputs.end()
			, []( ParticleElementDeclaration const & element )
			{
				return element.m_name == cuT( "velocity" );
			} );

		m_age = std::find_if( m_inputs.begin()
			, m_inputs.end()
			, []( ParticleElementDeclaration const & element )
			{
				return element.m_name == cuT( "age" );
			} );

		return m_type != m_inputs.end()
			&& m_position != m_inputs.end()
			&& m_velocity != m_inputs.end()
			&& m_age != m_inputs.end();
	}

	void ParticleSystem::doCleanup()
	{
		m_firstUnused = 1u;
	}
}
