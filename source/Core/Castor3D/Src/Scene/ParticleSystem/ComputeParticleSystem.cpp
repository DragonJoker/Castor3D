#include "ComputeParticleSystem.hpp"

#include "ParticleSystem.hpp"
#include "Particle.hpp"

#include "Engine.hpp"

#include "Mesh/Buffer/GeometryBuffers.hpp"
#include "Miscellaneous/ComputePipeline.hpp"
#include "Scene/BillboardList.hpp"
#include "Scene/Scene.hpp"
#include "Shader/AtomicCounterBuffer.hpp"
#include "Shader/UniformBufferBinding.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Shader/ShaderStorageBuffer.hpp"
#include "Texture/TextureLayout.hpp"

#include <Graphics/PixelBuffer.hpp>

#include <random>

using namespace Castor;

namespace Castor3D
{
	ComputeParticleSystem::ComputeParticleSystem( ParticleSystem & p_parent )
		: ParticleSystemImpl{ ParticleSystemImpl::Type::eComputeShader, p_parent }
		, m_ubo{ cuT( "ParticleSystem" )
			, *p_parent.GetScene()->GetEngine()->GetRenderSystem()
			, 1u }
	{
		m_deltaTime = m_ubo.CreateUniform< UniformType::eFloat >( cuT( "c3d_fDeltaTime" ) );
		m_time = m_ubo.CreateUniform< UniformType::eFloat >( cuT( "c3d_fTotalTime" ) );
		m_maxParticleCount = m_ubo.CreateUniform< UniformType::eUInt >( cuT( "c3d_uiMaxParticlesCount" ) );
		m_currentParticleCount = m_ubo.CreateUniform< UniformType::eUInt >( cuT( "c3d_uiCurrentParticlesCount" ) );
		m_emitterPosition = m_ubo.CreateUniform< UniformType::eVec3f >( cuT( "c3d_v3EmitterPosition" ) );
	}

	ComputeParticleSystem::~ComputeParticleSystem()
	{
	}

	bool ComputeParticleSystem::Initialise()
	{
		bool result = m_updateProgram != nullptr;

		if ( result )
		{
			result = DoCreateRandomStorage();
		}

		if ( result )
		{
			result = DoInitialiseParticleStorage();
		}

		if ( result )
		{
			m_maxParticleCount->SetValue( uint32_t( m_parent.GetMaxParticlesCount() ) );
			result = m_updateProgram->Initialise();
		}

		if ( result )
		{
			m_binding = &m_ubo.CreateBinding( *m_updateProgram );
			result = m_binding != nullptr;
		}

		if ( result )
		{
			result = DoInitialisePipeline();
		}

		return result;
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

		if ( m_randomStorage )
		{
			m_randomStorage->Cleanup();
		}

		for ( auto & storage : m_particlesStorages )
		{
			if ( storage )
			{
				storage->Cleanup();
			}
		}

		if ( m_generatedCountBuffer )
		{
			m_generatedCountBuffer->Cleanup();
		}

		m_ubo.Cleanup();

		if ( m_updateProgram )
		{
			m_updateProgram->Cleanup();
			m_updateProgram.reset();
		}
	}

	uint32_t ComputeParticleSystem::Update( Milliseconds const & p_time
		, Milliseconds const & p_totalTime )
	{
		m_deltaTime->SetValue( float( p_time.count() ) );
		m_time->SetValue( float( p_totalTime.count() ) );
		auto particlesCount = std::max( 1u, m_particlesCount );
		m_currentParticleCount->SetValue( particlesCount );
		m_emitterPosition->SetValue( m_parent.GetParent()->GetDerivedPosition() );
		m_ubo.Update();
		m_binding->Bind( 1u );

		uint32_t counts[]{ 0u, 0u };
		m_generatedCountBuffer->Upload( 0u, 2u, counts );

		m_randomStorage->BindTo( 1u );
		m_particlesStorages[m_in]->BindTo( 2u );
		m_particlesStorages[m_out]->BindTo( 3u );
		m_binding->Bind( 5u );

		m_computePipeline->Run(
			Point3ui{ std::max( 1u, uint32_t( std::ceil( float( particlesCount ) / m_worgGroupSize ) ) ), 1u, 1u },
			Point3ui{ std::min( m_worgGroupSize, particlesCount ), 1u, 1u },
			MemoryBarrier::eAtomicCounterBuffer | MemoryBarrier::eShaderStorageBuffer | MemoryBarrier::eVertexBuffer
		);

		m_generatedCountBuffer->Download( 0u, 1u, &particlesCount );
		m_particlesCount = std::min( particlesCount, uint32_t( m_parent.GetMaxParticlesCount() ) );

		if ( m_particlesCount )
		{
			m_parent.GetBillboards()->GetVertexBuffer().Copy( m_particlesStorages[m_out]->GetGpuBuffer()
				, m_particlesCount * m_inputs.stride());
		}

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

		auto & atomic = m_updateProgram->CreateAtomicCounterBuffer( cuT( "Counters" ), ShaderTypeFlag::eCompute );
		m_generatedCountBuffer = m_updateProgram->FindAtomicCounterBuffer( cuT( "Counters" ) );

		m_randomStorage = std::make_shared< ShaderStorageBuffer >( *m_parent.GetScene()->GetEngine() );
		m_particlesStorages[m_in] = std::make_shared< ShaderStorageBuffer >( *m_parent.GetScene()->GetEngine() );
		m_particlesStorages[m_out] = std::make_shared< ShaderStorageBuffer >( *m_parent.GetScene()->GetEngine() );;
	}

	bool ComputeParticleSystem::DoInitialiseParticleStorage()
	{
		auto size = uint32_t( m_parent.GetMaxParticlesCount() * m_inputs.stride() );
		bool result = m_generatedCountBuffer->Initialise( uint32_t( sizeof( uint32_t ) * 2u ), 0u );

		if ( result )
		{
			m_particlesStorages[m_in]->Resize( size );
			result = m_particlesStorages[m_in]->Initialise( BufferAccessType::eDynamic, BufferAccessNature::eDraw );
		}

		if ( result )
		{
			m_particlesStorages[m_out]->Resize( size );
			result = m_particlesStorages[m_out]->Initialise( BufferAccessType::eDynamic, BufferAccessNature::eDraw );
		}

		if ( result )
		{
			std::vector< uint8_t > particles;
			particles.resize( m_parent.GetMaxParticlesCount() * m_inputs.stride() );
			auto buffer = particles.data();
			Particle particle{ m_inputs, m_parent.GetDefaultValues() };

			for ( uint32_t i = 0u; i < m_parent.GetMaxParticlesCount(); ++i )
			{
				std::memcpy( buffer, particle.GetData(), m_inputs.stride() );
				buffer += m_inputs.stride();
			}

			m_particlesStorages[m_in]->Upload( 0u, uint32_t( particles.size() ), particles.data() );
		}

		return result;
	}

	bool ComputeParticleSystem::DoCreateRandomStorage()
	{
		auto & engine = *m_parent.GetScene()->GetEngine();
		auto & renderSystem = *engine.GetRenderSystem();
		auto size = uint32_t( 1024u * 4u * sizeof( float ) );
		m_randomStorage->Resize( size );
		bool result = m_randomStorage->Initialise( BufferAccessType::eStatic, BufferAccessNature::eRead );

		if ( result )
		{
			std::array< float, 1024u * 4u > buffer;
			std::random_device device;
			std::uniform_real_distribution< float > distribution{ -1.0f, 1.0f };

			for ( auto & value : buffer )
			{
				value = distribution( device );
			}

			m_randomStorage->Upload( 0u, size, reinterpret_cast< uint8_t * >( buffer.data() ) );
		}

		return result;
	}

	bool ComputeParticleSystem::DoInitialisePipeline()
	{
		m_computePipeline = m_parent.GetScene()->GetEngine()->GetRenderSystem()->CreateComputePipeline( *m_updateProgram );
		return m_computePipeline != nullptr;
	}
}
