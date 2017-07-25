#include "FireworksParticle.hpp"

#include <Engine.hpp>

#include <Mesh/Buffer/VertexBuffer.hpp>
#include <Scene/ParticleSystem/ParticleSystem.hpp>
#include <Scene/BillboardList.hpp>

#include <random>

using namespace Castor;
using namespace Castor3D;

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

		inline float GetRandomFloat()
		{
			static std::minstd_rand device;
			std::uniform_real_distribution< float > distribution{ -1.0f, 1.0f };
			return distribution( device );
		}

		inline Point3f DoGetRandomDirection()
		{
			return Point3f{ GetRandomFloat(), GetRandomFloat(), GetRandomFloat() };
		}

		inline void DoUpdateLauncher( ParticleSystem & p_system
			, Milliseconds const & p_time
			, float & p_type
			, Coords3f & p_position
			, Coords3f & p_velocity
			, float & p_age )
		{
			if ( p_age >= g_launcherCooldown.count() )
			{
				Point3f velocity{ DoGetRandomDirection() * 5.0f };
				velocity[1] = std::max( velocity[1] * 7.0f, 10.0f );
				p_system.EmitParticle( g_shell, p_position, velocity, 0.0f );
				p_age = 0.0f;
			}

			auto position = p_system.GetParent().GetParent()->GetDerivedPosition();
			p_position[0] = float( position[0] );
			p_position[1] = float( position[1] );
			p_position[2] = float( position[2] );
		}

		inline void DoUpdateShell( ParticleSystem & p_system
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
					p_system.EmitParticle( g_secondaryShell, p_position, ( DoGetRandomDirection() * 5.0f ) + p_velocity / 2.0f, 0.0f );
				}

				// Turn this shell to a secondary shell, to decrease the holes in buffer
				p_type = g_secondaryShell;
				p_velocity = ( DoGetRandomDirection() * 5.0f ) + p_velocity / 2.0f;
				p_age = 0.0f;
			}
		}

		inline void DoUpdateSecondaryShell( ParticleSystem & p_system
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

		inline void DoUpdateParticle( ParticleSystem & p_system
			, Milliseconds const & p_time
			, float & p_type
			, Coords3f & p_position
			, Coords3f & p_velocity
			, float & p_age )
		{
			p_age += p_time.count();

			if ( p_type == g_launcher )
			{
				DoUpdateLauncher( p_system, p_time, p_type, p_position, p_velocity, p_age );
			}
			else if ( p_type == g_shell )
			{
				DoUpdateShell( p_system, p_time, p_type, p_position, p_velocity, p_age );
			}
			else
			{
				DoUpdateSecondaryShell( p_system, p_time, p_type, p_position, p_velocity, p_age );
			}
		}

		inline void DoPackParticles( ParticleArray & p_particles, uint32_t & p_firstUnused )
		{
			for ( auto i = 1u; i < p_firstUnused && p_firstUnused > 1u; ++i )
			{
				auto & particle = p_particles[i];

				if ( particle.GetValue< ElementType::eFloat >( 0u ) == 0.0f )
				{
					particle = std::move( p_particles[p_firstUnused - 1] );
					--p_firstUnused;
				}
			}
		}
	}

	String const ParticleSystem::Type = cuT( "fireworks" );
	String const ParticleSystem::Name = cuT( "Fireworks Particle" );

	ParticleSystem::ParticleSystem( Castor3D::ParticleSystem & p_parent )
		: CpuParticleSystem( p_parent )
	{
	}

	ParticleSystem::~ParticleSystem()
	{
	}

	CpuParticleSystemUPtr ParticleSystem::Create( Castor3D::ParticleSystem & p_parent )
	{
		return std::make_unique< ParticleSystem >( p_parent );
	}

	void ParticleSystem::EmitParticle( float p_type, Castor::Point3f const & p_position, Castor::Point3f const & p_velocity, float p_age )
	{
		Particle particle{ m_inputs };
		particle.SetValue< ElementType::eFloat >( 0u, p_type );
		particle.SetValue< ElementType::eVec3 >( 1u, p_position );
		particle.SetValue< ElementType::eVec3 >( 2u, p_velocity );
		particle.SetValue< ElementType::eFloat >( 3u, p_age );
		m_particles[m_firstUnused++] = particle;
	}

	uint32_t ParticleSystem::Update( Milliseconds const & p_time
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
			Coords3f pos{ reinterpret_cast< float * >( particle.GetData() + position->m_offset ) };
			Coords3f vel{ reinterpret_cast< float * >( particle.GetData() + velocity->m_offset ) };
			DoUpdateParticle(
				*this,
				p_time,
				*reinterpret_cast< float * >( particle.GetData() + type->m_offset ),
				pos,
				vel,
				*reinterpret_cast< float * >( particle.GetData() + age->m_offset ) );
		}

		DoPackParticles( m_particles, m_firstUnused );
		auto & vbo = m_parent.GetBillboards()->GetVertexBuffer();

		vbo.Bind();
		auto stride = m_inputs.stride();
		auto dst = m_parent.GetBillboards()->GetVertexBuffer().Lock( 0, m_firstUnused * stride, AccessType::eWrite );

		if ( dst )
		{
			for ( auto i = 0u; i < m_firstUnused; ++i )
			{
				std::memcpy( dst, m_particles[i].GetData(), stride );
				dst += stride;
			}

			vbo.Unlock();
		}

		vbo.Unbind();

		return m_firstUnused;
	}

	bool ParticleSystem::DoInitialise()
	{
		return m_inputs.stride() == m_parent.GetBillboards()->GetVertexBuffer().GetDeclaration().stride();
	}

	void ParticleSystem::DoCleanup()
	{
		m_firstUnused = 1u;
	}
}
