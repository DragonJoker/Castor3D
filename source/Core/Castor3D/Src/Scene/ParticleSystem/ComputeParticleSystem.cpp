#include "ComputeParticleSystem.hpp"

#include "ParticleSystem.hpp"
#include "Particle.hpp"

#include "Engine.hpp"

#include "Mesh/Buffer/GeometryBuffers.hpp"
#include "Mesh/Buffer/VertexBuffer.hpp"
#include "Miscellaneous/ComputePipeline.hpp"
#include "Scene/BillboardList.hpp"
#include "Scene/Scene.hpp"
#include "Shader/AtomicCounterBuffer.hpp"
#include "Shader/UniformBuffer.hpp"
#include "Shader/ShaderObject.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Shader/ShaderStorageBuffer.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <Graphics/PixelBuffer.hpp>

#include <random>

using namespace Castor;

namespace Castor3D
{
	ComputeParticleSystem::ComputeParticleSystem( ParticleSystem & p_parent )
		: ParticleSystemImpl{ ParticleSystemImpl::Type::eComputeShader, p_parent }
	{
	}

	ComputeParticleSystem::~ComputeParticleSystem()
	{
	}

	bool ComputeParticleSystem::Initialise()
	{
		bool l_return = m_updateProgram != nullptr;

		if ( l_return )
		{
			l_return = DoCreateRandomStorage();
		}

		if ( l_return )
		{
			l_return = DoInitialiseParticleStorage();
		}

		if ( l_return )
		{
			m_maxParticleCount->SetValue( uint32_t( m_parent.GetMaxParticlesCount() ) );
			l_return = m_updateProgram->Initialise();
		}

		if ( l_return )
		{
			l_return = DoInitialisePipeline();
		}

		return l_return;
	}

	void ComputeParticleSystem::Cleanup()
	{
		m_deltaTime.reset();
		m_time.reset();
		m_maxParticleCount.reset();
		m_currentParticleCount.reset();
		m_launcherLifetime.reset();
		m_shellLifetime.reset();
		m_secondaryShellLifetime.reset();
		m_randomStorage->Cleanup();

		for ( auto & l_storage : m_particlesStorages )
		{
			if ( l_storage )
			{
				l_storage->Cleanup();
			}
		}

		if ( m_generatedCountBuffer )
		{
			m_generatedCountBuffer->Cleanup();
		}

		if ( m_updateProgram )
		{
			m_updateProgram->Cleanup();
			m_updateProgram.reset();
		}
	}

	uint32_t ComputeParticleSystem::Update( float p_time, float p_totalTime )
	{
		m_deltaTime->SetValue( p_time );
		m_time->SetValue( p_totalTime );
		auto l_particlesCount = std::max( 1u, m_particlesCount );
		m_currentParticleCount->SetValue( l_particlesCount );
		m_emitterPosition->SetValue( m_parent.GetParent()->GetDerivedPosition() );

		uint32_t l_counts[]{ 0u, 0u };
		m_generatedCountBuffer->Upload( 0u, 2u, l_counts );

		m_particlesStorages[m_in]->BindTo( 2u );
		m_particlesStorages[m_out]->BindTo( 3u );

		m_computePipeline->Run(
			Point3ui{ std::max( 1u, uint32_t( std::ceil( float( l_particlesCount ) / m_worgGroupSize ) ) ), 1u, 1u },
			Point3ui{ std::min( m_worgGroupSize, l_particlesCount ), 1u, 1u },
			MemoryBarrier::eAtomicCounterBuffer | MemoryBarrier::eShaderStorageBuffer | MemoryBarrier::eVertexBuffer
		);

		m_generatedCountBuffer->Download( 0u, 1u, &l_particlesCount );
		m_particlesCount = std::min( l_particlesCount, uint32_t( m_parent.GetMaxParticlesCount() ) );

		m_parent.GetBillboards()->GetVertexBuffer().Copy( m_particlesStorages[m_out]->GetGpuBuffer(), m_particlesCount * m_inputs.stride() );
		std::swap( m_in, m_out );
		return m_particlesCount;
	}

	void ComputeParticleSystem::AddParticleVariable( Castor::String const & p_name, ElementType p_type, Castor::String const & p_defaultValue )
	{
		m_inputs.push_back( BufferElementDeclaration{ p_name, 0u, p_type, m_inputs.stride() } );
	}

	void ComputeParticleSystem::SetUpdateProgram( ShaderProgramSPtr p_program )
	{
		m_updateProgram = p_program;
		auto & l_ubo = m_updateProgram->CreateUniformBuffer( cuT( "ParticleSystem" ), ShaderTypeFlag::eCompute );
		m_deltaTime = l_ubo.CreateVariable< Uniform1f >( cuT( "c3d_fDeltaTime" ) );
		m_time = l_ubo.CreateVariable< Uniform1f >( cuT( "c3d_fTotalTime" ) );
		m_maxParticleCount = l_ubo.CreateVariable< Uniform1ui >( cuT( "c3d_uiMaxParticlesCount" ) );
		m_currentParticleCount = l_ubo.CreateVariable< Uniform1ui >( cuT( "c3d_uiCurrentParticlesCount" ) );
		m_emitterPosition = l_ubo.CreateVariable< Uniform3f >( cuT( "c3d_v3EmitterPosition" ) );
		m_ubo = m_updateProgram->FindUniformBuffer( cuT( "ParticleSystem" ) );

		auto & l_atomic = m_updateProgram->CreateAtomicCounterBuffer( cuT( "Counters" ), ShaderTypeFlag::eCompute );
		m_generatedCountBuffer = m_updateProgram->FindAtomicCounterBuffer( cuT( "Counters" ) );

		auto & l_random = m_updateProgram->CreateStorageBuffer( cuT( "Random" ), ShaderTypeFlag::eCompute );
		m_randomStorage = m_updateProgram->FindStorageBuffer( cuT( "Random" ) );

		auto & l_particlesIn = m_updateProgram->CreateStorageBuffer( cuT( "ParticlesIn" ), ShaderTypeFlag::eCompute );
		m_particlesStorages[m_in] = m_updateProgram->FindStorageBuffer( cuT( "ParticlesIn" ) );

		auto & l_particlesOut = m_updateProgram->CreateStorageBuffer( cuT( "ParticlesOut" ), ShaderTypeFlag::eCompute );
		m_particlesStorages[m_out] = m_updateProgram->FindStorageBuffer( cuT( "ParticlesOut" ) );
	}

	bool ComputeParticleSystem::DoInitialiseParticleStorage()
	{
		auto l_size = uint32_t( m_parent.GetMaxParticlesCount() * m_inputs.stride() );
		bool l_return = m_generatedCountBuffer->Initialise( uint32_t( sizeof( uint32_t ) * 2u ), 0u );

		if ( l_return )
		{
			l_return = m_particlesStorages[m_in]->Initialise( l_size, 2u, BufferAccessType::eDynamic, BufferAccessNature::eDraw );
		}

		if ( l_return )
		{
			l_return = m_particlesStorages[m_out]->Initialise( l_size, 3u, BufferAccessType::eDynamic, BufferAccessNature::eDraw );
		}

		if ( l_return )
		{
			std::vector< uint8_t > l_particles;
			l_particles.resize( m_parent.GetMaxParticlesCount() * m_inputs.stride() );
			auto l_buffer = l_particles.data();
			Particle l_particle{ m_inputs, m_parent.GetDefaultValues() };

			for ( uint32_t i = 0u; i < m_parent.GetMaxParticlesCount(); ++i )
			{
				std::memcpy( l_buffer, l_particle.GetData(), m_inputs.stride() );
				l_buffer += m_inputs.stride();
			}

			m_particlesStorages[m_in]->Upload( 0u, uint32_t( l_particles.size() ), l_particles.data() );
		}

		return l_return;
	}

	bool ComputeParticleSystem::DoCreateRandomStorage()
	{
		auto & l_engine = *m_parent.GetScene()->GetEngine();
		auto & l_renderSystem = *l_engine.GetRenderSystem();
		auto l_size = uint32_t( 1024u * 4u * sizeof( float ) );
		bool l_return = m_randomStorage->Initialise( l_size, 1u, BufferAccessType::eStatic, BufferAccessNature::eRead );

		if ( l_return )
		{
			std::array< float, 1024u * 4u > l_buffer;
			std::random_device l_device;
			std::uniform_real_distribution< float > l_distribution{ -1.0f, 1.0f };

			for ( auto & l_value : l_buffer )
			{
				l_value = l_distribution( l_device );
			}

			m_randomStorage->Upload( 0u, l_size, reinterpret_cast< uint8_t * >( l_buffer.data() ) );
		}

		return l_return;
	}

	bool ComputeParticleSystem::DoInitialisePipeline()
	{
		m_computePipeline = m_parent.GetScene()->GetEngine()->GetRenderSystem()->CreateComputePipeline( *m_updateProgram );
		return m_computePipeline != nullptr;
	}
}
