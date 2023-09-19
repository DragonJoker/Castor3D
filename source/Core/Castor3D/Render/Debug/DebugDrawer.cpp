#include "Castor3D/Render/Debug/DebugDrawer.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/DirectUploadData.hpp"
#include "Castor3D/Buffer/InstantUploadData.hpp"
#include "Castor3D/Buffer/ObjectBufferPool.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderQueue.hpp"
#include "Castor3D/Render/RenderTarget.hpp"

#include <CastorUtils/Miscellaneous/Hash.hpp>

#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>
#include <ashespp/Pipeline/GraphicsPipeline.hpp>
#include <ashespp/Pipeline/PipelineShaderStageCreateInfo.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/Sync/Fence.hpp>
#include <ashespp/Sync/Queue.hpp>

CU_ImplementSmartPtr( castor3d, DebugDrawer )

namespace castor3d
{
	namespace dbgdrw
	{
		static ashes::RenderPassPtr createRenderPass( RenderDevice const & device
			, std::string const & name
			, Texture const & colour
			, Texture const & depth )
		{
			ashes::VkAttachmentDescriptionArray attaches{ VkAttachmentDescription{ 0u
					, depth.getFormat()
					, VK_SAMPLE_COUNT_1_BIT
					, VK_ATTACHMENT_LOAD_OP_LOAD
					, VK_ATTACHMENT_STORE_OP_STORE
					, VK_ATTACHMENT_LOAD_OP_DONT_CARE
					, VK_ATTACHMENT_STORE_OP_DONT_CARE
					, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
					, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL }
				, VkAttachmentDescription{ 1u
					, colour.getFormat()
					, VK_SAMPLE_COUNT_1_BIT
					, VK_ATTACHMENT_LOAD_OP_LOAD
					, VK_ATTACHMENT_STORE_OP_STORE
					, VK_ATTACHMENT_LOAD_OP_DONT_CARE
					, VK_ATTACHMENT_STORE_OP_DONT_CARE
					, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
					, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } };
			ashes::SubpassDescription subpassesDesc{ 0u
				, VK_PIPELINE_BIND_POINT_GRAPHICS
				, {}
				, { { 1u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } }
				, {}
				, VkAttachmentReference{ 0u, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL }
				, {} };
			ashes::SubpassDescriptionArray subpasses;
			subpasses.push_back( std::move( subpassesDesc ) );
			ashes::VkSubpassDependencyArray dependencies{ { VK_SUBPASS_EXTERNAL
					, 0u
					, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
					, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
					, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
					, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
					, VK_DEPENDENCY_BY_REGION_BIT }
				, { 0u
					, VK_SUBPASS_EXTERNAL
					, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
					, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
					, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
					, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
					, VK_DEPENDENCY_BY_REGION_BIT } };
			ashes::RenderPassCreateInfo createInfo{ 0u
				, std::move( attaches )
				, std::move( subpasses )
				, std::move( dependencies ) };
			return device->createRenderPass( name + "/Debug"
				, std::move( createInfo ) );
		}

		static ashes::FrameBufferPtr createFrameBuffer( ashes::RenderPass const & renderPass
			, std::string const & name
			, Texture const & colour
			, Texture const & depth )
		{
			ashes::VkImageViewArray fbAttaches;
			auto extent = colour.getExtent();
			fbAttaches.emplace_back( depth.targetView );
			fbAttaches.emplace_back( colour.targetView );
			return renderPass.createFrameBuffer( name + "/Debug"
				, makeVkStruct< VkFramebufferCreateInfo >( 0u
					, renderPass
					, uint32_t( fbAttaches.size() )
					, fbAttaches.data()
					, extent.width
					, extent.height
					, 1u ) );
		}

		size_t hash( VkPipelineShaderStageCreateInfo const & shader )
		{
			size_t result = std::hash< VkFlags >{}( shader.flags );
			castor::hashCombine( result, shader.module );
			castor::hashCombine( result, shader.stage );
			return result;
		}

		size_t hash( VkBuffer buffer
			, VkDeviceSize offset
			, VkDeviceSize size
			, ashes::PipelineShaderStageCreateInfoArray const & shaders )
		{
			size_t result{};

			for ( auto & shader : shaders )
			{
				castor::hashCombine( result, hash( shader ) );
			}
			
			castor::hashCombine( result, buffer );
			castor::hashCombine( result, offset );
			castor::hashCombine( result, size );
			return result;
		}
	}

	//*********************************************************************************************

	DebugDrawer::DebugDrawer( RenderTarget & parent
		, RenderDevice const & device
		, Texture const & colour
		, Texture const & depth )
		: castor::OwnedBy< RenderTarget >{ parent }
		, m_device{ device }
		, m_renderPass{ dbgdrw::createRenderPass( m_device, parent.getName(), colour, depth ) }
		, m_framebuffer{ dbgdrw::createFrameBuffer( *m_renderPass, parent.getName(), colour, depth ) }
		, m_commandPool{ m_device->createCommandPool( parent.getName() + "/Debug", m_device.getGraphicsQueueFamilyIndex(), 0u ) }
		, m_commandBuffers{ CommandsSemaphore{ m_commandPool->createCommandBuffer( parent.getName() + "/Debug/0" ), m_device->createSemaphore( parent.getName() + "/Debug/0" ) }
			, CommandsSemaphore{ m_commandPool->createCommandBuffer( parent.getName() + "/Debug/1" ), m_device->createSemaphore( parent.getName() + "/Debug/1" ) } }
	{
	}

	void DebugDrawer::addAabbs( VkBuffer buffer
		, VkDeviceSize offset
		, VkDeviceSize size
		, VkDeviceSize count
		, ashes::PipelineShaderStageCreateInfoArray shader )
	{
		auto hash = dbgdrw::hash( buffer, offset, size, shader );
		auto ires = m_pipelines.emplace( hash, nullptr );

		if ( ires.second )
		{
			auto name = getOwner()->getName() + "/Debug/AABB/" + std::to_string( hash );
			auto & extent = m_framebuffer->getDimensions();
			ashes::PipelineVertexInputStateCreateInfo vertexState{ 0u
				, ashes::VkVertexInputBindingDescriptionArray{ VkVertexInputBindingDescription{ 0u, 16u, VK_VERTEX_INPUT_RATE_VERTEX } }
				, ashes::VkVertexInputAttributeDescriptionArray{ VkVertexInputAttributeDescription{ 0u, 0u, VK_FORMAT_R32G32B32A32_SFLOAT, 0u } } };
			ashes::PipelineViewportStateCreateInfo viewportState{ 0u
				, { makeViewport( castor::Point2ui{ extent.width, extent.height } ) }
				, { makeScissor( castor::Point2ui{ extent.width, extent.height } ) } };
			ires.first->second->descriptorLayout = m_device->createDescriptorSetLayout( name
				, ashes::VkDescriptorSetLayoutBindingArray{ VkDescriptorSetLayoutBinding{ 0u, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1u, VK_SHADER_STAGE_VERTEX_BIT, nullptr } } );
			ires.first->second->pipelineLayout = m_device->createPipelineLayout( name
				, *ires.first->second->descriptorLayout );
			ashes::GraphicsPipelineCreateInfo graphics{ 0u
				, std::move( shader )
				, std::move( vertexState )
				, ashes::PipelineInputAssemblyStateCreateInfo{ 0u, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST }
				, ashes::nullopt
				, std::move( viewportState )
				, ashes::PipelineRasterizationStateCreateInfo{ 0u, VK_FALSE, VK_FALSE, VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE }
				, ashes::PipelineMultisampleStateCreateInfo{}
				, ashes::PipelineDepthStencilStateCreateInfo{ 0u, VK_TRUE, VK_FALSE, VK_COMPARE_OP_GREATER }
				, ashes::PipelineColorBlendStateCreateInfo{}
				, ashes::nullopt
				, static_cast< VkPipelineLayout >( *ires.first->second->pipelineLayout )
				, static_cast< VkRenderPass >( *m_renderPass ) };
			ires.first->second->pipeline = m_device->createPipeline( name, std::move( graphics ) );

			ires.first->second->descriptorPool = ires.first->second->descriptorLayout->createPool( name, 1u );
			ires.first->second->descriptorSet = ires.first->second->descriptorPool->createDescriptorSet( name );
			ires.first->second->vertices = m_device.vertexPools->getBuffer< castor::Point4f >( 36u );			
			{
				std::vector< castor::Point4f > vertexData
				{
					castor::Point4f{ -1, +1, -1, +1 }, castor::Point4f{ +1, -1, -1, +1 }, castor::Point4f{ -1, -1, -1, +1 }, castor::Point4f{ +1, -1, -1, +1 }, castor::Point4f{ -1, +1, -1, +1 }, castor::Point4f{ +1, +1, -1, +1 },// Back
					castor::Point4f{ -1, -1, +1, +1 }, castor::Point4f{ -1, +1, -1, +1 }, castor::Point4f{ -1, -1, -1, +1 }, castor::Point4f{ -1, +1, -1, +1 }, castor::Point4f{ -1, -1, +1, +1 }, castor::Point4f{ -1, +1, +1, +1 },// Left
					castor::Point4f{ +1, -1, -1, +1 }, castor::Point4f{ +1, +1, +1, +1 }, castor::Point4f{ +1, -1, +1, +1 }, castor::Point4f{ +1, +1, +1, +1 }, castor::Point4f{ +1, -1, -1, +1 }, castor::Point4f{ +1, +1, -1, +1 },// Right
					castor::Point4f{ -1, -1, +1, +1 }, castor::Point4f{ +1, +1, +1, +1 }, castor::Point4f{ -1, +1, +1, +1 }, castor::Point4f{ +1, +1, +1, +1 }, castor::Point4f{ -1, -1, +1, +1 }, castor::Point4f{ +1, -1, +1, +1 },// Front
					castor::Point4f{ -1, +1, -1, +1 }, castor::Point4f{ +1, +1, +1, +1 }, castor::Point4f{ +1, +1, -1, +1 }, castor::Point4f{ +1, +1, +1, +1 }, castor::Point4f{ -1, +1, -1, +1 }, castor::Point4f{ -1, +1, +1, +1 },// Top
					castor::Point4f{ -1, -1, -1, +1 }, castor::Point4f{ +1, -1, -1, +1 }, castor::Point4f{ -1, -1, +1, +1 }, castor::Point4f{ +1, -1, -1, +1 }, castor::Point4f{ +1, -1, +1, +1 }, castor::Point4f{ -1, -1, +1, +1 },// Bottom
				};
				auto queue = m_device.graphicsData();
				InstantDirectUploadData uploader{ *queue->queue
					, m_device
					, "RenderCube"
					, *m_commandPool };
				uploader->pushUpload( vertexData.data()
					, ires.first->second->vertices.getAskedSize( SubmeshFlag::ePositions )
					, ires.first->second->vertices.getBuffer( SubmeshFlag::ePositions )
					, ires.first->second->vertices.getOffset( SubmeshFlag::ePositions )
					, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT
					, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT );
			}
		}
		
		m_aabbs.emplace_back( buffer, offset, size, count, ires.first->second.get() );
	}

	crg::SemaphoreWaitArray DebugDrawer::render( ashes::Queue const & queue
			, crg::SemaphoreWaitArray toWait )
	{
		if ( m_aabbs.empty() )
		{
			return toWait;
		}

		auto & commands = m_commandBuffers[m_index];
		commands.commandBuffer->begin();
		commands.commandBuffer->beginDebugBlock( { "Staging Texture Upload"
			, makeFloatArray( getOwner()->getEngine()->getNextRainbowColour() ) } );
		commands.commandBuffer->beginRenderPass( *m_renderPass
			, *m_framebuffer
			, { defaultClearDepthStencil, transparentBlackClearColor }
			, VK_SUBPASS_CONTENTS_INLINE);

		for ( auto & aabb : m_aabbs )
		{
			commands.commandBuffer->bindPipeline( *aabb.pipeline->pipeline );
			commands.commandBuffer->bindDescriptorSet( *aabb.pipeline->descriptorSet, *aabb.pipeline->pipelineLayout );
			commands.commandBuffer->bindVertexBuffer( 0u, aabb.pipeline->vertices.getBuffer( SubmeshFlag::ePositions ), aabb.pipeline->vertices.getOffset( SubmeshFlag::ePositions ) );

			if ( aabb.pipeline->indices )
			{
				commands.commandBuffer->bindIndexBuffer( aabb.pipeline->indices.getBuffer( SubmeshFlag::eIndex ), aabb.pipeline->indices.getOffset( SubmeshFlag::eIndex ), VK_INDEX_TYPE_UINT16 );
				commands.commandBuffer->drawIndexed( aabb.pipeline->indices.getAskedSize( SubmeshFlag::eIndex ) / sizeof( uint16_t ), uint32_t( aabb.count ), 0u, 0u, 0u );
			}
			else
			{
				commands.commandBuffer->draw( aabb.pipeline->indices.getAskedSize( SubmeshFlag::ePositions ) / sizeof( castor::Point4f ), uint32_t( aabb.count ), 0u, 0u );
			}
		}

		commands.commandBuffer->endRenderPass();
		commands.commandBuffer->endDebugBlock();
		commands.commandBuffer->end();

		m_index = 1u - m_index;
		return { crg::SemaphoreWait{ commands.submit( queue, toWait ), VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT } };
	}

	//*********************************************************************************************
}
