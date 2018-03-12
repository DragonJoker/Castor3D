#include "ComputeParticleSystem.hpp"

#include "Engine.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/BillboardList.hpp"
#include "Scene/Scene.hpp"
#include "Scene/ParticleSystem/ParticleSystem.hpp"
#include "Scene/ParticleSystem/Particle.hpp"
#include "Texture/TextureLayout.hpp"

#include <Buffer/UniformBuffer.hpp>

#include <Graphics/PixelBuffer.hpp>

#include <random>

using namespace castor;

namespace castor3d
{
	ComputeParticleSystem::ComputeParticleSystem( ParticleSystem & parent )
		: ParticleSystemImpl{ ParticleSystemImpl::Type::eComputeShader, parent }
	{
	}

	ComputeParticleSystem::~ComputeParticleSystem()
	{
	}

	bool ComputeParticleSystem::initialise()
	{
		bool result = m_updateProgram.module != nullptr;

		if ( result )
		{
			auto & device = *getParent().getScene()->getEngine()->getRenderSystem()->getCurrentDevice();
			m_ubo = renderer::makeUniformBuffer< Configuration >( device
				, 1u
				, renderer::BufferTarget::eTransferDst
				, renderer::MemoryPropertyFlag::eHostVisible );
			m_ubo->getData( 0u ).maxParticleCount = uint32_t( m_parent.getMaxParticlesCount() );
		}

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
			result = doInitialisePipeline();
		}

		return result;
	}

	void ComputeParticleSystem::cleanup()
	{
		if ( m_randomStorage )
		{
			m_randomStorage.reset();
		}

		for ( auto & storage : m_particlesStorages )
		{
			if ( storage )
			{
				storage.reset();
			}
		}

		if ( m_generatedCountBuffer )
		{
			m_generatedCountBuffer.reset();
		}

		m_ubo.reset();
		m_updateProgram.module.reset();
	}

	uint32_t ComputeParticleSystem::update( Milliseconds const & time
		, Milliseconds const & totalTime )
	{
		auto & data = m_ubo->getData( 0u );
		data.deltaTime = float( time.count() );
		data.time = float( totalTime.count() );
		auto particlesCount = std::max( 1u, m_particlesCount );
		data.currentParticleCount = particlesCount;
		data.emitterPosition = m_parent.getParent()->getDerivedPosition();
		m_ubo->upload( 0u );

		uint32_t counts[]{ 0u, 0u };

		if ( auto buffer = m_generatedCountBuffer->lock( 0u
			, 2u
			, renderer::MemoryMapFlag::eWrite | renderer::MemoryMapFlag::eInvalidateRange ) )
		{
			buffer[0] = counts[0];
			buffer[1] = counts[1];
			m_randomStorage->flush( 0u, 2u );
			m_randomStorage->unlock();
		}

		//m_computePipeline->run(
		//	Point3ui{ std::max( 1u, uint32_t( std::ceil( float( particlesCount ) / m_worgGroupSize ) ) ), 1u, 1u },
		//	Point3ui{ std::min( m_worgGroupSize, particlesCount ), 1u, 1u },
		//	MemoryBarrier::eAtomicCounterBuffer | MemoryBarrier::eShaderStorageBuffer | MemoryBarrier::eVertexBuffer
		//);

		//m_generatedCountBuffer->download( 0u, 1u, &particlesCount );
		//m_particlesCount = std::min( particlesCount, uint32_t( m_parent.getMaxParticlesCount() ) );

		//if ( m_particlesCount )
		//{
		//	m_parent.getBillboards()->getVertexBuffer().copy( *m_particlesStorages[m_out]
		//		, m_particlesCount * m_inputs.stride());
		//}

		std::swap( m_in, m_out );
		return m_particlesCount;
	}

	void ComputeParticleSystem::addParticleVariable( castor::String const & name
		, ParticleFormat type
		, castor::String const & defaultValue )
	{
		m_inputs.push_back( ParticleElementDeclaration{ name, 0u, type, m_inputs.stride() } );
	}

	void ComputeParticleSystem::setUpdateProgram( renderer::ShaderStageState const & program )
	{
		m_updateProgram = program;
	}

	bool ComputeParticleSystem::doInitialiseParticleStorage()
	{
		auto size = uint32_t( m_parent.getMaxParticlesCount() * m_inputs.stride() );
		auto & device = *getParent().getScene()->getEngine()->getRenderSystem()->getCurrentDevice();
		m_generatedCountBuffer = renderer::makeBuffer< uint32_t >( device
			, 2u
			, renderer::BufferTarget::eStorageBuffer | renderer::BufferTarget::eTransferDst | renderer::BufferTarget::eTransferSrc
			, renderer::MemoryPropertyFlag::eHostVisible );

		m_particlesStorages[m_in] = renderer::makeBuffer< uint8_t >( device
			, size
			, renderer::BufferTarget::eStorageBuffer | renderer::BufferTarget::eTransferDst
			, renderer::MemoryPropertyFlag::eHostVisible );

		m_particlesStorages[m_in] = renderer::makeBuffer< uint8_t >( device
			, size
			, renderer::BufferTarget::eStorageBuffer | renderer::BufferTarget::eTransferDst
			, renderer::MemoryPropertyFlag::eHostVisible );

		Particle particle{ m_inputs, m_parent.getDefaultValues() };

		if ( auto buffer = m_particlesStorages[m_in]->getBuffer().lock( 0u
			, size
			, renderer::MemoryMapFlag::eWrite | renderer::MemoryMapFlag::eInvalidateRange ) )
		{
			for ( uint32_t i = 0u; i < m_parent.getMaxParticlesCount(); ++i )
			{
				std::memcpy( buffer, particle.getData(), m_inputs.stride() );
				buffer += m_inputs.stride();
			}

			m_particlesStorages[m_in]->getBuffer().flush( 0u, size );
			m_particlesStorages[m_in]->getBuffer().unlock();
		}

		return true;
	}

	bool ComputeParticleSystem::doCreateRandomStorage()
	{
		auto & device = *getParent().getScene()->getEngine()->getRenderSystem()->getCurrentDevice();
		uint32_t size = 1024u;
		m_randomStorage = renderer::makeBuffer< castor::Point4f >( device
			, 1024
			, renderer::BufferTarget::eStorageBuffer | renderer::BufferTarget::eTransferDst
			, renderer::MemoryPropertyFlag::eHostVisible );
		std::random_device rddevice;
		std::uniform_real_distribution< float > distribution{ -1.0f, 1.0f };

		if ( auto buffer = m_randomStorage->lock( 0u
			, size
			, renderer::MemoryMapFlag::eWrite | renderer::MemoryMapFlag::eInvalidateRange ) )
		{
			for ( auto i = 0u; i < size; ++i )
			{
				( *buffer )[0] = distribution( rddevice );
				( *buffer )[1] = distribution( rddevice );
				( *buffer )[2] = distribution( rddevice );
				( *buffer )[3] = distribution( rddevice );
				++buffer;
			}

			m_randomStorage->getBuffer().flush( 0u, size );
			m_randomStorage->getBuffer().unlock();
		}

		return true;
	}

	bool ComputeParticleSystem::doInitialisePipeline()
	{
		auto & device = *getParent().getScene()->getEngine()->getRenderSystem()->getCurrentDevice();
		renderer::DescriptorSetLayoutBindingArray bindings
		{
			{ 0u, renderer::DescriptorType::eStorageBuffer, renderer::ShaderStageFlag::eCompute },
			{ 1u, renderer::DescriptorType::eStorageBuffer, renderer::ShaderStageFlag::eCompute },
			{ 2u, renderer::DescriptorType::eStorageBuffer, renderer::ShaderStageFlag::eCompute },
			{ 3u, renderer::DescriptorType::eStorageBuffer, renderer::ShaderStageFlag::eCompute },
			{ 4u, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eCompute },
		};
		m_descriptorLayout = device.createDescriptorSetLayout( std::move( bindings ) );
		m_descriptorPool = m_descriptorLayout->createPool( 1u );
		m_descriptorSet = m_descriptorPool->createDescriptorSet( 0u );
		m_pipelineLayout = device.createPipelineLayout( *m_descriptorLayout );
		m_pipeline = m_pipelineLayout->createPipeline( renderer::ComputePipelineCreateInfo
		{
			m_updateProgram,
		} );
		return true;
	}
}
