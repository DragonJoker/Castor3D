#include "Castor3D/Scene/ParticleSystem/ComputeParticleSystem.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/ParticleSystem/ParticleSystem.hpp"
#include "Castor3D/Scene/ParticleSystem/Particle.hpp"
#include "Castor3D/Shader/Program.hpp"

#include <ashespp/Buffer/UniformBuffer.hpp>

#include <CastorUtils/Graphics/PixelBuffer.hpp>

#include <RenderGraph/FramePassTimer.hpp>

#include <random>

CU_ImplementSmartPtr( castor3d, ComputeParticleSystem )

namespace castor3d
{
	namespace compptcl
	{
		static uint32_t constexpr IndexBufferBinding = 0u;
		static uint32_t constexpr RandomBufferBinding = 1u;
		static uint32_t constexpr InParticlesBufferBinding = 2u;
		static uint32_t constexpr OutParticlesBufferBinding = 3u;
		static uint32_t constexpr ParticleSystemBufferBinding = 4u;

		static castor::Point3ui doDispatch( uint32_t count
			, castor::Point3i const & sizes )
		{
			auto blockSize = uint32_t( sizes[0] * sizes[1] * sizes[2] );
			auto numBlocks = ( count + blockSize - 1 ) / blockSize;
			return castor::Point3ui{ numBlocks
				, sizes[1] > 1 ? numBlocks : 1
				, sizes[2] > 1 ? numBlocks : 1 };
		}
	}

	ComputeParticleSystem::ComputeParticleSystem( ParticleSystem & parent )
		: ParticleSystemImpl{ ParticleSystemImpl::Type::eComputeShader, parent }
	{
	}

	bool ComputeParticleSystem::initialise( RenderDevice const & device )
	{
		bool result = m_program != nullptr;

		if ( result )
		{
			m_ubo = device.uboPool->getBuffer< Configuration >( 0u );
			auto & data = m_ubo.getData();
			data.maxParticleCount = m_parent.getMaxParticlesCount();
		}

		if ( result )
		{
			result = doInitialiseParticleStorage( device );
		}

		if ( result )
		{
			result = doInitialisePipeline( device );
		}

		if ( result )
		{
			doPrepareCommandBuffers( device );
		}

		return result;
	}

	void ComputeParticleSystem::cleanup( RenderDevice const & device )
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

		for ( auto & storage : m_particlesStorages )
		{
			storage.reset();
		}

		m_generatedCountBuffer.reset();

		if ( m_ubo )
		{
			device.uboPool->putBuffer( m_ubo );
		}
	}

	void ComputeParticleSystem::update( CpuUpdater & updater )
	{
	}

	uint32_t ComputeParticleSystem::update( GpuUpdater & updater )
	{
		auto & device = updater.device;
		auto & data = m_ubo.getData();
		data.deltaTime = float( updater.time.count() );
		data.time = float( updater.total.count() );
		auto particlesCount = std::max( 1u, m_particlesCount );
		data.currentParticleCount = particlesCount;
		data.emitterPosition = m_parent.getParent()->getDerivedPosition();
		auto dispatch = compptcl::doDispatch( particlesCount, m_worgGroupSizes );

		if ( dispatch->x )
		{
			auto align = device.renderSystem.getValue( GpuMin::eBufferMapSize );
			auto size = ashes::getAlignedSize( m_parent.getMaxParticlesCount() * m_inputs.stride(), align );

			m_commandBuffer->begin( VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT );
			updater.timer->beginPass( *m_commandBuffer );

			// Initialise counts buffer to 0.
			auto flags = m_generatedCountBuffer->getBuffer().getCompatibleStageFlags();
			m_commandBuffer->memoryBarrier( flags
				, VK_PIPELINE_STAGE_TRANSFER_BIT
				, m_generatedCountBuffer->getBuffer().makeTransferDestination() );
			m_commandBuffer->fillBuffer( m_generatedCountBuffer->getBuffer()
				, 0u
				, m_generatedCountBuffer->getBuffer().getSize()
				, 0u );
			m_commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
				, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT
				, m_generatedCountBuffer->getBuffer().makeMemoryTransitionBarrier( VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT ) );

			// Put In and Out buffers to compute state.
			flags = m_particlesStorages[m_in]->getBuffer().getCompatibleStageFlags();
			m_commandBuffer->memoryBarrier( flags
				, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT
				, m_particlesStorages[m_in]->getBuffer().makeMemoryTransitionBarrier( VK_ACCESS_SHADER_READ_BIT ) );
			flags = m_particlesStorages[m_out]->getBuffer().getCompatibleStageFlags();
			m_commandBuffer->memoryBarrier( flags
				, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT
				, m_particlesStorages[m_out]->getBuffer().makeMemoryTransitionBarrier( VK_ACCESS_SHADER_WRITE_BIT ) );

			// Dispatch compute
			m_commandBuffer->bindPipeline( *m_pipeline, VK_PIPELINE_BIND_POINT_COMPUTE );
			m_commandBuffer->bindDescriptorSet( *m_descriptorSets[m_in]
				, *m_pipelineLayout
				, VK_PIPELINE_BIND_POINT_COMPUTE );
			m_commandBuffer->dispatch( dispatch[0], dispatch[1], dispatch[2] );

			// Put In and Out buffers to transfer state.
			flags = m_particlesStorages[m_in]->getBuffer().getCompatibleStageFlags();
			m_commandBuffer->memoryBarrier( flags
				, VK_PIPELINE_STAGE_TRANSFER_BIT
				, m_particlesStorages[m_in]->getBuffer().makeTransferSource() );
			flags = m_particlesStorages[m_out]->getBuffer().getCompatibleStageFlags();
			m_commandBuffer->memoryBarrier( flags
				, VK_PIPELINE_STAGE_TRANSFER_BIT
				, m_particlesStorages[m_out]->getBuffer().makeTransferSource() );

			// Copy output storage to billboard's vertex buffer
			flags = m_parent.getBillboards()->getVertexBuffer().getBuffer().getBuffer().getCompatibleStageFlags();
			m_commandBuffer->memoryBarrier( flags
				, VK_PIPELINE_STAGE_TRANSFER_BIT
				, m_parent.getBillboards()->getVertexBuffer().getBuffer().getBuffer().makeTransferDestination() );
			m_commandBuffer->copyBuffer( m_particlesStorages[m_out]->getBuffer()
				, m_parent.getBillboards()->getVertexBuffer().getBuffer()
				, size
				, 0u
				, m_parent.getBillboards()->getVertexBuffer().getOffset() );
			m_commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
				, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT
				, m_parent.getBillboards()->getVertexBuffer().getBuffer().getBuffer().makeVertexShaderInputResource() );

			// Put counts buffer to host visible state
			flags = m_generatedCountBuffer->getBuffer().getCompatibleStageFlags();
			m_commandBuffer->memoryBarrier( flags
				, VK_PIPELINE_STAGE_HOST_BIT
				, m_generatedCountBuffer->getBuffer().makeMemoryTransitionBarrier( VK_ACCESS_HOST_READ_BIT ) );

			updater.timer->endPass( *m_commandBuffer );
			m_commandBuffer->end();
			device.graphicsData()->queue->submit( *m_commandBuffer, m_fence.get() );
			updater.timer->notifyPassRender( updater.index );
			m_fence->wait( ashes::MaxTimeout );
			m_fence->reset();
			m_commandBuffer->reset();

			// Retrieve counts
			if ( auto buffer = m_generatedCountBuffer->lock( 0u, 1u, 0u ) )
			{
				particlesCount = buffer[0];
				m_generatedCountBuffer->unlock();
				m_particlesCount = std::min( particlesCount, m_parent.getMaxParticlesCount() );
			}

			std::swap( m_in, m_out );
		}

		return m_particlesCount;
	}

	void ComputeParticleSystem::addParticleVariable( castor::String const & name
		, ParticleFormat type
		, castor::String const & defaultValue )
	{
		m_inputs.push_back( ParticleElementDeclaration{ name, ElementUsage::eUnknown, type, m_inputs.stride() } );
	}

	void ComputeParticleSystem::setUpdateProgram( ShaderProgramRPtr program )
	{
		m_program = program;
	}

	bool ComputeParticleSystem::doInitialiseParticleStorage( RenderDevice const & device )
	{
		auto align = device.renderSystem.getValue( GpuMin::eBufferMapSize );
		auto size = ashes::getAlignedSize( m_parent.getMaxParticlesCount() * m_inputs.stride(), align );
		m_generatedCountBuffer = makeBuffer< uint32_t >( device
			, ashes::getAlignedSize( 2u * sizeof( uint32_t ), align ) / sizeof( uint32_t )
			, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, "ComputeParticleSystemCountBuffer" );
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

	bool ComputeParticleSystem::doInitialisePipeline( RenderDevice const & device )
	{
		ashes::VkDescriptorSetLayoutBindingArray bindings
		{
			makeDescriptorSetLayoutBinding( compptcl::IndexBufferBinding
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_COMPUTE_BIT ),
			makeDescriptorSetLayoutBinding( compptcl::RandomBufferBinding
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_COMPUTE_BIT ),
			makeDescriptorSetLayoutBinding( compptcl::InParticlesBufferBinding
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_COMPUTE_BIT ),
			makeDescriptorSetLayoutBinding( compptcl::OutParticlesBufferBinding
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_COMPUTE_BIT ),
			makeDescriptorSetLayoutBinding( compptcl::ParticleSystemBufferBinding
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, VK_SHADER_STAGE_COMPUTE_BIT ),
		};
		m_descriptorLayout = device->createDescriptorSetLayout( m_parent.getName() + "/Compute"
			, std::move( bindings ) );
		m_pipelineLayout = device->createPipelineLayout( m_parent.getName() + "/Compute" 
			, *m_descriptorLayout );

		m_pipeline = device->createPipeline( m_parent.getName() + "/Compute"
			, ashes::ComputePipelineCreateInfo
			{
				0u,
				m_program->getStates()[0],
				*m_pipelineLayout
			} );

		auto initialiseDescriptor = [this, &device]( ashes::DescriptorSet & descriptorSet
			, uint32_t inIndex
			, uint32_t outIndex )
		{
			auto & randomStorage = device.renderSystem.getRandomStorage();
			descriptorSet.createBinding( m_descriptorLayout->getBinding( compptcl::IndexBufferBinding )
				, *m_generatedCountBuffer
				, 0u
				, uint32_t( m_generatedCountBuffer->getCount() ) );
			descriptorSet.createBinding( m_descriptorLayout->getBinding( compptcl::RandomBufferBinding )
				, randomStorage
				, 0u
				, uint32_t( randomStorage.getCount() ) );
			descriptorSet.createBinding( m_descriptorLayout->getBinding( compptcl::InParticlesBufferBinding )
				, *m_particlesStorages[inIndex]
				, 0u
				, uint32_t( m_particlesStorages[inIndex]->getCount() ) );
			descriptorSet.createBinding( m_descriptorLayout->getBinding( compptcl::OutParticlesBufferBinding )
				, *m_particlesStorages[outIndex]
				, 0u
				, uint32_t( m_particlesStorages[outIndex]->getCount() ) );
			m_ubo.createSizedBinding( descriptorSet
				, m_descriptorLayout->getBinding( compptcl::ParticleSystemBufferBinding ) );
			descriptorSet.update();
		};

		m_descriptorPool = m_descriptorLayout->createPool( m_parent.getName() + "/Compute", 2u );
		m_descriptorSets[0] = m_descriptorPool->createDescriptorSet( m_parent.getName() + "/Compute", 0u );
		m_descriptorSets[1] = m_descriptorPool->createDescriptorSet( m_parent.getName() + "/Compute", 0u );
		initialiseDescriptor( *m_descriptorSets[0], 0u, 1u );
		initialiseDescriptor( *m_descriptorSets[1], 1u, 0u );
		return true;
	}

	void ComputeParticleSystem::doPrepareCommandBuffers( RenderDevice const & device )
	{
		m_commandBuffer = device.graphicsData()->commandPool->createCommandBuffer( m_parent.getName() + "/Compute" );
		m_fence = device->createFence( m_parent.getName() + "/Compute" );
	}
}
