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

		constexpr float g_launcherCooldown = 100;
		constexpr float g_shellLifetime = 10000;
		constexpr float g_secondaryShellLifetime = 2500;

		inline float GetRandomFloat()
		{
			static std::minstd_rand l_device;
			std::uniform_real_distribution< float > l_distribution{ -1.0f, 1.0f };
			return l_distribution( l_device );
		}

		inline Point3f DoGetRandomDirection()
		{
			return Point3f{ GetRandomFloat(), GetRandomFloat(), GetRandomFloat() };
		}

		inline void DoUpdateLauncher( ParticleSystem & p_system, float p_time, float & p_type, Coords3f & p_position, Coords3f & p_velocity, float & p_age )
		{
			if ( p_age >= g_launcherCooldown )
			{
				Point3f l_velocity{ DoGetRandomDirection() * 5.0f };
				l_velocity[1] = std::max( l_velocity[1] * 7.0f, 10.0f );
				p_system.EmitParticle( g_shell, p_position, l_velocity, 0.0f );
				p_age = 0.0f;
			}

			auto l_position = p_system.GetParent().GetParent()->GetDerivedPosition();
			p_position[0] = float( l_position[0] );
			p_position[1] = float( l_position[1] );
			p_position[2] = float( l_position[2] );
		}

		inline void DoUpdateShell( ParticleSystem & p_system, float p_time, float & p_type, Coords3f & p_position, Coords3f & p_velocity, float & p_age )
		{
			Point3f l_delta{ p_time / 1000.0, p_time / 1000.0, p_time / 1000.0 };
			Point3f l_deltaP = l_delta * p_velocity;
			Point3f l_deltaV = l_delta * Point3f{ 0.0f, -0.981f, 0.0f };

			if ( p_age < g_shellLifetime )
			{
				p_position += l_deltaP;
				p_velocity += l_deltaV;
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

		inline void DoUpdateSecondaryShell( ParticleSystem & p_system, float p_time, float & p_type, Coords3f & p_position, Coords3f & p_velocity, float & p_age )
		{
			if ( p_age < g_secondaryShellLifetime )
			{
				Point3f l_delta{ p_time / 1000.0, p_time / 1000.0, p_time / 1000.0 };
				Point3f l_deltaP = l_delta * p_velocity;
				Point3f l_deltaV = l_delta * Point3f{ 0.0f, -0.981f, 0.0f };
				p_position += l_deltaP;
				p_velocity += l_deltaV;
			}
			else
			{
				p_type = g_launcher;
			}
		}

		inline void DoUpdateParticle( ParticleSystem & p_system, float p_time, float & p_type, Coords3f & p_position, Coords3f & p_velocity, float & p_age )
		{
			p_age += p_time;

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
				auto & l_particle = p_particles[i];

				if ( l_particle.GetValue< ElementType::eFloat >( 0u ) == 0.0f )
				{
					l_particle = std::move( p_particles[p_firstUnused - 1] );
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
		Particle l_particle{ m_inputs };
		l_particle.SetValue< ElementType::eFloat >( 0u, p_type );
		l_particle.SetValue< ElementType::eVec3 >( 1u, p_position );
		l_particle.SetValue< ElementType::eVec3 >( 2u, p_velocity );
		l_particle.SetValue< ElementType::eFloat >( 3u, p_age );
		m_particles[m_firstUnused++] = l_particle;
	}

	uint32_t ParticleSystem::Update( float p_time, float p_total )
	{
		auto const l_type = std::find_if( m_inputs.begin(), m_inputs.end(), []( BufferElementDeclaration const & p_element )
		{
			return p_element.m_name == cuT( "type" );
		} );

		auto const l_position = std::find_if( m_inputs.begin(), m_inputs.end(), []( BufferElementDeclaration const & p_element )
		{
			return p_element.m_name == cuT( "position" );
		} );

		auto const l_velocity = std::find_if( m_inputs.begin(), m_inputs.end(), []( BufferElementDeclaration const & p_element )
		{
			return p_element.m_name == cuT( "velocity" );
		} );

		auto const l_age = std::find_if( m_inputs.begin(), m_inputs.end(), []( BufferElementDeclaration const & p_element )
		{
			return p_element.m_name == cuT( "age" );
		} );

		REQUIRE( l_type != m_inputs.end() && l_position != m_inputs.end() && l_velocity != m_inputs.end() && l_age != m_inputs.end() );
		auto l_firstUnused = m_firstUnused;

		for ( auto i = 0u; i < l_firstUnused; ++i )
		{
			auto & l_particle = m_particles[i];
			Coords3f l_pos{ reinterpret_cast< float * >( l_particle.GetData() + l_position->m_offset ) };
			Coords3f l_vel{ reinterpret_cast< float * >( l_particle.GetData() + l_velocity->m_offset ) };
			DoUpdateParticle(
				*this,
				p_time,
				*reinterpret_cast< float * >( l_particle.GetData() + l_type->m_offset ),
				l_pos,
				l_vel,
				*reinterpret_cast< float * >( l_particle.GetData() + l_age->m_offset ) );
		}

		DoPackParticles( m_particles, m_firstUnused );
		auto & l_vbo = m_parent.GetBillboards()->GetVertexBuffer();

		if ( l_vbo.Bind() )
		{
			auto l_stride = m_inputs.stride();
			auto l_dst = m_parent.GetBillboards()->GetVertexBuffer().Lock( 0, m_firstUnused * l_stride, AccessType::eWrite );

			if ( l_dst )
			{
				for ( auto i = 0u; i < m_firstUnused; ++i )
				{
					std::memcpy( l_dst, m_particles[i].GetData(), l_stride );
					l_dst += l_stride;
				}

				l_vbo.Unlock();
			}

			l_vbo.Unbind();
		}

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
