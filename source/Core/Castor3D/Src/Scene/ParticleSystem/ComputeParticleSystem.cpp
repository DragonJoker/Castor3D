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

using namespace castor;

namespace castor3d
{
	ComputeParticleSystem::ComputeParticleSystem( ParticleSystem & p_parent )
		: ParticleSystemImpl{ ParticleSystemImpl::Type::eComputeShader, p_parent }
		, m_ubo{ cuT( "ParticleSystem" )
			, *p_parent.getScene()->getEngine()->getRenderSystem()
			, 1u }
	{
		m_deltaTime = m_ubo.createUniform< UniformType::eFloat >( cuT( "c3d_fDeltaTime" ) );
		m_time = m_ubo.createUniform< UniformType::eFloat >( cuT( "c3d_fTotalTime" ) );
		m_maxParticleCount = m_ubo.createUniform< UniformType::eUInt >( cuT( "c3d_uiMaxParticlesCount" ) );
		m_currentParticleCount = m_ubo.createUniform< UniformType::eUInt >( cuT( "c3d_uiCurrentParticlesCount" ) );
		m_emitterPosition = m_ubo.createUniform< UniformType::eVec3f >( cuT( "c3d_v3EmitterPosition" ) );
	}

	ComputeParticleSystem::~ComputeParticleSystem()
	{
	}

	bool ComputeParticleSystem::initialise()
	{
		bool result = m_updateProgram != nullptr;

		if ( result )
		{
			result = doCreateRandomStorage();
		}

		if ( result )
		{
			result = doInitialiseParticleStorage();
		}

		if ( result )
		{
			m_maxParticleCount->setValue( uint32_t( m_parent.getMaxParticlesCount() ) );
			result = m_updateProgram->initialise();
		}

		if ( result )
		{
			m_binding = &m_ubo.createBinding( *m_updateProgram );
			result = m_binding != nullptr;
		}

		if ( result )
		{
			result = doInitialisePipeline();
		}

		return result;
	}

	void ComputeParticleSystem::cleanup()
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
			m_randomStorage->cleanup();
		}

		for ( auto & storage : m_particlesStorages )
		{
			if ( storage )
			{
				storage->cleanup();
			}
		}

		if ( m_generatedCountBuffer )
		{
			m_generatedCountBuffer->cleanup();
		}

		m_ubo.cleanup();

		if ( m_updateProgram )
		{
			m_updateProgram->cleanup();
			m_updateProgram.reset();
		}
	}

	uint32_t ComputeParticleSystem::update( Milliseconds const & p_time
		, Milliseconds const & p_totalTime )
	{
		m_deltaTime->setValue( float( p_time.count() ) );
		m_time->setValue( float( p_totalTime.count() ) );
		auto particlesCount = std::max( 1u, m_particlesCount );
		m_currentParticleCount->setValue( particlesCount );
		m_emitterPosition->setValue( m_parent.getParent()->getDerivedPosition() );
		m_ubo.update();
		m_binding->bind( 1u );

		uint32_t counts[]{ 0u, 0u };
		m_generatedCountBuffer->upload( 0u, 2u, counts );

		m_randomStorage->bindTo( 1u );
		m_particlesStorages[m_in]->bindTo( 2u );
		m_particlesStorages[m_out]->bindTo( 3u );
		m_binding->bind( 5u );

		m_computePipeline->run(
			Point3ui{ std::max( 1u, uint32_t( std::ceil( float( particlesCount ) / m_worgGroupSize ) ) ), 1u, 1u },
			Point3ui{ std::min( m_worgGroupSize, particlesCount ), 1u, 1u },
			MemoryBarrier::eAtomicCounterBuffer | MemoryBarrier::eShaderStorageBuffer | MemoryBarrier::eVertexBuffer
		);

		m_generatedCountBuffer->download( 0u, 1u, &particlesCount );
		m_particlesCount = std::min( particlesCount, uint32_t( m_parent.getMaxParticlesCount() ) );

		if ( m_particlesCount )
		{
			m_parent.getBillboards()->getVertexBuffer().copy( *m_particlesStorages[m_out]
				, m_particlesCount * m_inputs.stride());
		}

		std::swap( m_in, m_out );
		return m_particlesCount;
	}

	void ComputeParticleSystem::addParticleVariable( castor::String const & p_name, renderer::AttributeFormat p_type, castor::String const & p_defaultValue )
	{
		m_inputs.push_back( BufferElementDeclaration{ p_name, 0u, p_type, m_inputs.stride() } );
	}

	void ComputeParticleSystem::setUpdateProgram( ShaderProgramSPtr p_program )
	{
		m_updateProgram = p_program;

		m_updateProgram->createAtomicCounterBuffer( cuT( "Counters" ), ShaderTypeFlag::eCompute );
		m_generatedCountBuffer = m_updateProgram->findAtomicCounterBuffer( cuT( "Counters" ) );

		m_randomStorage = std::make_shared< ShaderStorageBuffer >( *m_parent.getScene()->getEngine() );
		m_particlesStorages[m_in] = std::make_shared< ShaderStorageBuffer >( *m_parent.getScene()->getEngine() );
		m_particlesStorages[m_out] = std::make_shared< ShaderStorageBuffer >( *m_parent.getScene()->getEngine() );;
	}

	bool ComputeParticleSystem::doInitialiseParticleStorage()
	{
		auto size = uint32_t( m_parent.getMaxParticlesCount() * m_inputs.stride() );
		bool result = m_generatedCountBuffer->initialise( uint32_t( sizeof( uint32_t ) * 2u ), 0u );

		if ( result )
		{
			m_particlesStorages[m_in]->resize( size );
			result = m_particlesStorages[m_in]->initialise( renderer::MemoryPropertyFlag::eHostVisible );
		}

		if ( result )
		{
			m_particlesStorages[m_out]->resize( size );
			result = m_particlesStorages[m_out]->initialise( renderer::MemoryPropertyFlag::eHostVisible );
		}

		if ( result )
		{
			std::vector< uint8_t > particles;
			particles.resize( m_parent.getMaxParticlesCount() * m_inputs.stride() );
			auto buffer = particles.data();
			Particle particle{ m_inputs, m_parent.getDefaultValues() };

			for ( uint32_t i = 0u; i < m_parent.getMaxParticlesCount(); ++i )
			{
				std::memcpy( buffer, particle.getData(), m_inputs.stride() );
				buffer += m_inputs.stride();
			}

			m_particlesStorages[m_in]->upload( 0u, uint32_t( particles.size() ), particles.data() );
		}

		return result;
	}

	bool ComputeParticleSystem::doCreateRandomStorage()
	{
		auto size = uint32_t( 1024u * 4u * sizeof( float ) );
		m_randomStorage->resize( size );
		bool result = m_randomStorage->initialise( renderer::MemoryPropertyFlag::eHostVisible );

		if ( result )
		{
			std::array< float, 1024u * 4u > buffer;
			std::random_device device;
			std::uniform_real_distribution< float > distribution{ -1.0f, 1.0f };

			for ( auto & value : buffer )
			{
				value = distribution( device );
			}

			m_randomStorage->upload( 0u, size, reinterpret_cast< uint8_t * >( buffer.data() ) );
		}

		return result;
	}

	bool ComputeParticleSystem::doInitialisePipeline()
	{
		m_computePipeline = m_parent.getScene()->getEngine()->getRenderSystem()->createComputePipeline( *m_updateProgram );
		return m_computePipeline != nullptr;
	}
}
