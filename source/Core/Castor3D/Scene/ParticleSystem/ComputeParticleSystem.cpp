#include "Castor3D/Scene/ParticleSystem/ComputeParticleSystem.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/ParticleSystem/ParticleSystem.hpp"
#include "Castor3D/Scene/ParticleSystem/Particle.hpp"
#include "Castor3D/Shader/Program.hpp"

#include <ashespp/Buffer/UniformBuffer.hpp>

#include <CastorUtils/Graphics/PixelBuffer.hpp>

#include <random>

using namespace castor;

namespace castor3d
{
	namespace
	{
		static uint32_t constexpr IndexBufferBinding = 0u;
		static uint32_t constexpr RandomBufferBinding = 1u;
		static uint32_t constexpr InParticlesBufferBinding = 2u;
		static uint32_t constexpr OutParticlesBufferBinding = 3u;
		static uint32_t constexpr ParticleSystemBufferBinding = 4u;

		Point3ui doDispatch( uint32_t count, Point3i const & sizes )
		{
			uint32_t blockSize = sizes[0] * sizes[1] * sizes[2];
			uint32_t numBlocks = ( count + blockSize - 1 ) / blockSize;
			return Point3ui{ numBlocks, sizes[1] > 1 ? numBlocks : 1, sizes[2] > 1 ? numBlocks : 1 };
		}
	}

	ComputeParticleSystem::ComputeParticleSystem( ParticleSystem & parent )
		: ParticleSystemImpl{ ParticleSystemImpl::Type::eComputeShader, parent }
	{
	}

	ComputeParticleSystem::~ComputeParticleSystem()
	{
	}

	bool ComputeParticleSystem::initialise()
	{
		bool result = m_program != nullptr;

		if ( result )
		{
			result = m_program->initialise();
		}

		if ( result )
		{
			auto & device = getCurrentRenderDevice( getParent() );
			m_ubo = makeUniformBuffer< Configuration >( *getParent().getScene()->getEngine()->getRenderSystem()
				, 1u
				, VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				, "ComputeParticleSystemUbo" );
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

		if ( result )
		{
			doPrepareCommandBuffers();
		}

		return result;
	}

	void ComputeParticleSystem::cleanup()
	{
		m_fence.reset();
		m_commandBuffer.reset();

		for ( auto & descriptorSet : m_descriptorSets )
		{
			descriptorSet.reset();
		}

		m_descriptorPool.reset();
		m_pipeline.reset();
		m_pipelineLayout.reset();
		m_descriptorLayout.reset();

		m_randomStorage.reset();

		for ( auto & storage : m_particlesStorages )
		{
			storage.reset();
		}

		m_generatedCountBuffer.reset();
		m_ubo.reset();
	}

	uint32_t ComputeParticleSystem::update( RenderPassTimer & timer
		, Milliseconds const & time
		, Milliseconds const & totalTime
		, uint32_t index )
	{
		auto & device = getCurrentRenderDevice( *this );
		auto & data = m_ubo->getData( 0u );
		data.deltaTime = float( time.count() );
		data.time = float( totalTime.count() );
		auto particlesCount = std::max( 1u, m_particlesCount );
		data.currentParticleCount = particlesCount;
		data.emitterPosition = m_parent.getParent()->getDerivedPosition();
		m_ubo->upload( 0u );

		uint32_t counts[]{ 0u, 0u };

		if ( auto buffer = m_generatedCountBuffer->lock( 0u, 2u, 0u ) )
		{
			buffer[0] = counts[0];
			buffer[1] = counts[1];
			m_generatedCountBuffer->flush( 0u, 2u );
			m_generatedCountBuffer->unlock();
		}

		m_commandBuffer->begin( VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT );
		timer.beginPass( *m_commandBuffer, index + 0u );
		// Put buffers in appropriate state for compute
		m_commandBuffer->memoryBarrier( m_generatedCountBuffer->getBuffer().getCompatibleStageFlags()
			, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT
			, m_generatedCountBuffer->getBuffer().makeMemoryTransitionBarrier( VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT ) );
		m_commandBuffer->memoryBarrier( m_particlesStorages[m_in]->getBuffer().getCompatibleStageFlags()
			, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT
			, m_particlesStorages[m_in]->getBuffer().makeMemoryTransitionBarrier( VK_ACCESS_SHADER_READ_BIT ) );
		m_commandBuffer->memoryBarrier( m_particlesStorages[m_out]->getBuffer().getCompatibleStageFlags()
			, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT
			, m_particlesStorages[m_out]->getBuffer().makeMemoryTransitionBarrier( VK_ACCESS_SHADER_WRITE_BIT ) );
		// Dispatch compute
		m_commandBuffer->bindPipeline( *m_pipeline, VK_PIPELINE_BIND_POINT_COMPUTE );
		m_commandBuffer->bindDescriptorSet( *m_descriptorSets[m_in]
			, *m_pipelineLayout
			, VK_PIPELINE_BIND_POINT_COMPUTE );
		auto dispatch = doDispatch( particlesCount, m_worgGroupSizes );
		m_commandBuffer->dispatch( dispatch[0], dispatch[1], dispatch[2] );
		// Put counts buffer to host visible state
		m_commandBuffer->memoryBarrier( m_generatedCountBuffer->getBuffer().getCompatibleStageFlags()
			, VK_PIPELINE_STAGE_HOST_BIT
			, m_generatedCountBuffer->getBuffer().makeMemoryTransitionBarrier( VK_ACCESS_HOST_READ_BIT ) );
		m_commandBuffer->memoryBarrier( m_particlesStorages[m_in]->getBuffer().getCompatibleStageFlags()
			, VK_PIPELINE_STAGE_TRANSFER_BIT
			, m_particlesStorages[m_in]->getBuffer().makeTransferSource() );
		m_commandBuffer->memoryBarrier( m_particlesStorages[m_out]->getBuffer().getCompatibleStageFlags()
			, VK_PIPELINE_STAGE_TRANSFER_BIT
			, m_particlesStorages[m_out]->getBuffer().makeTransferSource() );
		timer.endPass( *m_commandBuffer, index + 0u );
		m_commandBuffer->end();

		device.computeQueue->submit( *m_commandBuffer, m_fence.get() );
		m_fence->wait( ashes::MaxTimeout );
		m_fence->reset();
		m_commandBuffer->reset();

		// Retrieve counts
		if ( auto buffer = m_generatedCountBuffer->lock( 0u, 1u, 0u ) )
		{
			particlesCount = buffer[0];
			m_generatedCountBuffer->unlock();
			m_particlesCount = std::min( particlesCount, uint32_t( m_parent.getMaxParticlesCount() ) );
		}

		if ( m_particlesCount )
		{
			m_commandBuffer->begin( VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT );
			timer.beginPass( *m_commandBuffer, index + 1u );
			// Copy output storage to billboard's vertex buffer
			m_commandBuffer->memoryBarrier( m_parent.getBillboards()->getVertexBuffer().getBuffer().getCompatibleStageFlags()
				, VK_PIPELINE_STAGE_TRANSFER_BIT
				, m_parent.getBillboards()->getVertexBuffer().getBuffer().makeTransferDestination() );
			m_commandBuffer->copyBuffer( m_particlesStorages[m_out]->getBuffer()
				, m_parent.getBillboards()->getVertexBuffer().getBuffer()
				, m_particlesCount * m_inputs.stride() );
			m_commandBuffer->memoryBarrier( m_parent.getBillboards()->getVertexBuffer().getBuffer().getCompatibleStageFlags()
				, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT
				, m_parent.getBillboards()->getVertexBuffer().getBuffer().makeVertexShaderInputResource() );
			timer.endPass( *m_commandBuffer, index + 1u );
			m_commandBuffer->end();

			m_fence->reset();
			device.computeQueue->submit( *m_commandBuffer, m_fence.get() );
			m_fence->wait( ashes::MaxTimeout );
			m_fence->reset();

			m_commandBuffer->reset();
		}

		std::swap( m_in, m_out );
		return m_particlesCount;
	}

	void ComputeParticleSystem::addParticleVariable( castor::String const & name
		, ParticleFormat type
		, castor::String const & defaultValue )
	{
		m_inputs.push_back( ParticleElementDeclaration{ name, 0u, type, m_inputs.stride() } );
	}

	void ComputeParticleSystem::setUpdateProgram( ShaderProgramSPtr program )
	{
		m_program = program;
	}

	bool ComputeParticleSystem::doInitialiseParticleStorage()
	{
		auto size = uint32_t( m_parent.getMaxParticlesCount() * m_inputs.stride() );
		auto & device = getCurrentRenderDevice( getParent() );
		m_generatedCountBuffer = makeBuffer< uint32_t >( device
			, 2u
			, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, "ComputeParticleSystemCountBuffer" );

		m_particlesStorages[0] = makeBuffer< uint8_t >( device
			, size
			, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, "ComputeParticleSystemParticles0" );
		m_particlesStorages[1] = makeBuffer< uint8_t >( device
			, size
			, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, "ComputeParticleSystemParticles1" );
		Particle particle{ m_inputs, m_parent.getDefaultValues() };

		auto initialise = [this, &size, &particle]( ashes::Buffer< uint8_t > & buffer )
		{
			if ( auto data = buffer.lock( 0u, size, 0u ) )
			{
				for ( uint32_t i = 0u; i < m_parent.getMaxParticlesCount(); ++i )
				{
					std::memcpy( data, particle.getData(), m_inputs.stride() );
					data += m_inputs.stride();
				}

				buffer.getBuffer().flush( 0u, size );
				buffer.getBuffer().unlock();
			}
		};
		initialise( *m_particlesStorages[0] );
		initialise( *m_particlesStorages[1] );
		return true;
	}

	bool ComputeParticleSystem::doCreateRandomStorage()
	{
		auto & device = getCurrentRenderDevice( getParent() );
		uint32_t size = 1024u;
		m_randomStorage = makeBuffer< castor::Point4f >( device
			, 1024
			, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, "ComputeParticleSystemRandom" );
		std::random_device rddevice;
		std::uniform_real_distribution< float > distribution{ -1.0f, 1.0f };

		if ( auto buffer = m_randomStorage->lock( 0u
			, size
			, 0u ) )
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
		auto & device = getCurrentRenderDevice( getParent() );
		ashes::VkDescriptorSetLayoutBindingArray bindings
		{
			makeDescriptorSetLayoutBinding( IndexBufferBinding
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_COMPUTE_BIT ),
			makeDescriptorSetLayoutBinding( RandomBufferBinding
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_COMPUTE_BIT ),
			makeDescriptorSetLayoutBinding( InParticlesBufferBinding
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_COMPUTE_BIT ),
			makeDescriptorSetLayoutBinding( OutParticlesBufferBinding
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_COMPUTE_BIT ),
			makeDescriptorSetLayoutBinding( ParticleSystemBufferBinding
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, VK_SHADER_STAGE_COMPUTE_BIT ),
		};
		m_descriptorLayout = device->createDescriptorSetLayout( std::move( bindings ) );
		m_pipelineLayout = device->createPipelineLayout( *m_descriptorLayout );

		m_pipeline = device->createPipeline( ashes::ComputePipelineCreateInfo
			{
				0u,
				m_program->getStates()[0],
				*m_pipelineLayout
			} );

		auto initialiseDescriptor = [this]( ashes::DescriptorSet & descriptorSet
			, uint32_t inIndex
			, uint32_t outIndex )
		{
			descriptorSet.createBinding( m_descriptorLayout->getBinding( IndexBufferBinding )
				, *m_generatedCountBuffer
				, 0u
				, uint32_t( m_generatedCountBuffer->getCount() ) );
			descriptorSet.createBinding( m_descriptorLayout->getBinding( RandomBufferBinding )
				, *m_randomStorage
				, 0u
				, uint32_t( m_randomStorage->getCount() ) );
			descriptorSet.createBinding( m_descriptorLayout->getBinding( InParticlesBufferBinding )
				, *m_particlesStorages[inIndex]
				, 0u
				, uint32_t( m_particlesStorages[inIndex]->getCount() ) );
			descriptorSet.createBinding( m_descriptorLayout->getBinding( OutParticlesBufferBinding )
				, *m_particlesStorages[outIndex]
				, 0u
				, uint32_t( m_particlesStorages[outIndex]->getCount() ) );
			descriptorSet.createSizedBinding( m_descriptorLayout->getBinding( ParticleSystemBufferBinding )
				, m_ubo->getBuffer()
				, 0u
				, 1u );
			descriptorSet.update();
		};

		m_descriptorPool = m_descriptorLayout->createPool( 2u );
		m_descriptorSets[0] = m_descriptorPool->createDescriptorSet( 0u );
		m_descriptorSets[1] = m_descriptorPool->createDescriptorSet( 0u );
		initialiseDescriptor( *m_descriptorSets[0], 0u, 1u );
		initialiseDescriptor( *m_descriptorSets[1], 1u, 0u );
		return true;
	}

	void ComputeParticleSystem::doPrepareCommandBuffers()
	{
		auto & device = getCurrentRenderDevice( *this );
		m_commandBuffer = device.computeCommandPool->createCommandBuffer();
		m_fence = device->createFence();
	}
}
