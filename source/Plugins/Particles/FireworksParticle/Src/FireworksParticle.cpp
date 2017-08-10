#include "FireworksParticle.hpp"

#include <Engine.hpp>

#include <Mesh/Buffer/VertexBuffer.hpp>
#include <Scene/ParticleSystem/ParticleSystem.hpp>
#include <Scene/BillboardList.hpp>

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

		inline void doUpdateLauncher( ParticleSystem & p_system
			, Milliseconds const & p_time
			, float & p_type
			, Coords3f & p_position
			, Coords3f & p_velocity
			, float & p_age )
		{
			if ( p_age >= g_launcherCooldown.count() )
			{
				Point3f velocity{ doGetRandomDirection() * 5.0f };
				velocity[1] = std::max( velocity[1] * 7.0f, 10.0f );
				p_system.emitParticle( g_shell, p_position, velocity, 0.0f );
				p_age = 0.0f;
			}

			auto position = p_system.getParent().getParent()->getDerivedPosition();
			p_position[0] = float( position[0] );
			p_position[1] = float( position[1] );
			p_position[2] = float( position[2] );
		}

		inline void doUpdateShell( ParticleSystem & p_system
			, Milliseconds const & p_time
			, float & p_type
			, Coords3f & p_position
			, Coords3f & p_velocity
			, float & p_age )
		{
			Point3f delta{ p_time.count() / 1000.0, p_time.count() / 1000.0, p_time.count() / 1000.0 };
			Point3f deltaP = delta * p_velocity;
			Point3f deltaV = delta * Point3f{ 0.0f, -0.981f, 0.0f };

			if ( p_age < g_shellLifetime.count() )
			{
				p_position += deltaP;
				p_velocity += deltaV;
			}
			else
			{
				for ( int i = 1; i < 10; ++i )
				{
					p_system.emitParticle( g_secondaryShell, p_position, ( doGetRandomDirection() * 5.0f ) + p_velocity / 2.0f, 0.0f );
				}

				// Turn this shell to a secondary shell, to decrease the holes in buffer
				p_type = g_secondaryShell;
				p_velocity = ( doGetRandomDirection() * 5.0f ) + p_velocity / 2.0f;
				p_age = 0.0f;
			}
		}

		inline void doUpdateSecondaryShell( ParticleSystem & p_system
			, Milliseconds const & p_time
			, float & p_type
			, Coords3f & p_position
			, Coords3f & p_velocity
			, float & p_age )
		{
			if ( p_age < g_secondaryShellLifetime.count() )
			{
				Point3f delta{ p_time.count() / 1000.0, p_time.count() / 1000.0, p_time.count() / 1000.0 };
				Point3f deltaP = delta * p_velocity;
				Point3f deltaV = delta * Point3f{ 0.0f, -0.981f, 0.0f };
				p_position += deltaP;
				p_velocity += deltaV;
			}
			else
			{
				p_type = g_launcher;
			}
		}

		inline void doUpdateParticle( ParticleSystem & p_system
			, Milliseconds const & p_time
			, float & p_type
			, Coords3f & p_position
			, Coords3f & p_velocity
			, float & p_age )
		{
			p_age += p_time.count();

			if ( p_type == g_launcher )
			{
				doUpdateLauncher( p_system, p_time, p_type, p_position, p_velocity, p_age );
			}
			else if ( p_type == g_shell )
			{
				doUpdateShell( p_system, p_time, p_type, p_position, p_velocity, p_age );
			}
			else
			{
				doUpdateSecondaryShell( p_system, p_time, p_type, p_position, p_velocity, p_age );
			}
		}

		inline void doPackParticles( ParticleArray & p_particles, uint32_t & p_firstUnused )
		{
			for ( auto i = 1u; i < p_firstUnused && p_firstUnused > 1u; ++i )
			{
				auto & particle = p_particles[i];

				if ( particle.getValue< ElementType::eFloat >( 0u ) == 0.0f )
				{
					particle = std::move( p_particles[p_firstUnused - 1] );
					--p_firstUnused;
				}
			}
		}
	}

	String const ParticleSystem::Type = cuT( "fireworks" );
	String const ParticleSystem::Name = cuT( "Fireworks Particle" );

	ParticleSystem::ParticleSystem( castor3d::ParticleSystem & p_parent )
		: CpuParticleSystem( p_parent )
	{
	}

	ParticleSystem::~ParticleSystem()
	{
	}

	CpuParticleSystemUPtr ParticleSystem::create( castor3d::ParticleSystem & p_parent )
	{
		return std::make_unique< ParticleSystem >( p_parent );
	}

	void ParticleSystem::emitParticle( float p_type, castor::Point3f const & p_position, castor::Point3f const & p_velocity, float p_age )
	{
		Particle particle{ m_inputs };
		particle.setValue< ElementType::eFloat >( 0u, p_type );
		particle.setValue< ElementType::eVec3 >( 1u, p_position );
		particle.setValue< ElementType::eVec3 >( 2u, p_velocity );
		particle.setValue< ElementType::eFloat >( 3u, p_age );
		m_particles[m_firstUnused++] = particle;
	}

	uint32_t ParticleSystem::update( Milliseconds const & p_time
		, Milliseconds const & p_total )
	{
		auto const type = std::find_if( m_inputs.begin(), m_inputs.end(), []( BufferElementDeclaration const & p_element )
		{
			return p_element.m_name == cuT( "type" );
		} );

		auto const position = std::find_if( m_inputs.begin(), m_inputs.end(), []( BufferElementDeclaration const & p_element )
		{
			return p_element.m_name == cuT( "position" );
		} );

		auto const velocity = std::find_if( m_inputs.begin(), m_inputs.end(), []( BufferElementDeclaration const & p_element )
		{
			return p_element.m_name == cuT( "velocity" );
		} );

		auto const age = std::find_if( m_inputs.begin(), m_inputs.end(), []( BufferElementDeclaration const & p_element )
		{
			return p_element.m_name == cuT( "age" );
		} );

		REQUIRE( type != m_inputs.end() && position != m_inputs.end() && velocity != m_inputs.end() && age != m_inputs.end() );
		auto firstUnused = m_firstUnused;

		for ( auto i = 0u; i < firstUnused; ++i )
		{
			auto & particle = m_particles[i];
			Coords3f pos{ reinterpret_cast< float * >( particle.getData() + position->m_offset ) };
			Coords3f vel{ reinterpret_cast< float * >( particle.getData() + velocity->m_offset ) };
			doUpdateParticle(
				*this,
				p_time,
				*reinterpret_cast< float * >( particle.getData() + type->m_offset ),
				pos,
				vel,
				*reinterpret_cast< float * >( particle.getData() + age->m_offset ) );
		}

		doPackParticles( m_particles, m_firstUnused );
		auto & vbo = m_parent.getBillboards()->getVertexBuffer();

		vbo.bind();
		auto stride = m_inputs.stride();
		auto dst = m_parent.getBillboards()->getVertexBuffer().lock( 0, m_firstUnused * stride, AccessType::eWrite );

		if ( dst )
		{
			for ( auto i = 0u; i < m_firstUnused; ++i )
			{
				std::memcpy( dst, m_particles[i].getData(), stride );
				dst += stride;
			}

			vbo.unlock();
		}

		vbo.unbind();

		return m_firstUnused;
	}

	bool ParticleSystem::doInitialise()
	{
		return m_inputs.stride() == m_parent.getBillboards()->getVertexBuffer().getDeclaration().stride();
	}

	void ParticleSystem::doCleanup()
	{
		m_firstUnused = 1u;
	}
}
