#include "Castor3D/Render/Debug/DebugDrawer.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/DirectUploadData.hpp"
#include "Castor3D/Buffer/InstantUploadData.hpp"
#include "Castor3D/Buffer/ObjectBufferPool.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include <Castor3D/Model/Mesh/Submesh/Component/LinesMapping.hpp>
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderQueue.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"

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
		static size_t hash( VkPipelineShaderStageCreateInfo const & value )
		{
			size_t result = std::hash< VkFlags >{}( value.flags );
			castor::hashCombine( result, value.module );
			castor::hashCombine( result, value.stage );
			return result;
		}

		static size_t hash( VkDescriptorImageInfo const & value )
		{
			size_t result = std::hash< VkSampler >{}( value.sampler );
			castor::hashCombine( result, value.imageView );
			castor::hashCombine( result, value.imageLayout );
			return result;
		}

		static size_t hash( VkDescriptorBufferInfo const & value )
		{
			size_t result = std::hash< VkBuffer >{}( value.buffer );
			castor::hashCombine( result, value.offset );
			castor::hashCombine( result, value.range );
			return result;
		}

		static size_t hash( VkVertexInputAttributeDescription const & value )
		{
			size_t result = std::hash< uint32_t >{}( value.binding );
			castor::hashCombine( result, value.format );
			castor::hashCombine( result, value.location );
			castor::hashCombine( result, value.offset );
			return result;
		}

		static size_t hash( VkVertexInputBindingDescription const & value )
		{
			size_t result = std::hash< uint32_t >{}( value.binding );
			castor::hashCombine( result, value.stride );
			castor::hashCombine( result, value.inputRate );
			return result;
		}

		static size_t hash( VkWriteDescriptorSet const & value )
		{
			size_t result = std::hash< uint32_t >{}( value.dstBinding );
			castor::hashCombine( result, value.dstArrayElement );
			castor::hashCombine( result, value.descriptorCount );
			castor::hashCombine( result, value.descriptorType );

			if ( value.pImageInfo )
			{
				for ( auto & info : castor::makeArrayView( value.pImageInfo, value.descriptorCount ) )
				{
					castor::hashCombine( result, hash( info ) );
				}
			}

			if ( value.pBufferInfo )
			{
				for ( auto & info : castor::makeArrayView( value.pBufferInfo, value.descriptorCount ) )
				{
					castor::hashCombine( result, hash( info ) );
				}
			}

			if ( value.pTexelBufferView )
			{
				for ( auto & info : castor::makeArrayView( value.pTexelBufferView, value.descriptorCount ) )
				{
					castor::hashCombine( result, info );
				}
			}

			return result;
		}

		static size_t hash( ashes::VkVertexInputAttributeDescriptionArray const & vertexAttributes
			, ashes::VkVertexInputBindingDescriptionArray const & vertexBindings
			, ashes::PipelineShaderStageCreateInfoArray const & shaders
			, bool enableDepthTest )
		{
			size_t result{};

			for ( auto & value : vertexAttributes )
			{
				castor::hashCombine( result, hash( value ) );
			}

			for ( auto & value : vertexBindings )
			{
				castor::hashCombine( result, hash( value ) );
			}

			for ( auto & value : shaders )
			{
				castor::hashCombine( result, hash( value ) );
			}

			castor::hashCombine( result, enableDepthTest );
			return result;
		}

		static size_t hash( DebugIndexBuffer const & buffer )
		{
			size_t result = std::hash< VkBuffer >{}( buffer.buffer );
			castor::hashCombine( result, buffer.offset );
			castor::hashCombine( result, buffer.count );
			return result;
		}

		static size_t hash( DebugVertexBuffers const & vertexBuffers
			, DebugIndexBuffer const & indexBuffer
			, ashes::WriteDescriptorSetArray const & writes )
		{
			size_t result{};

			for ( auto & value : writes )
			{
				castor::hashCombine( result, hash( value ) );
			}

			for ( auto & value : vertexBuffers.buffers )
			{
				castor::hashCombine( result, value );
			}

			for ( auto & value : vertexBuffers.offsets )
			{
				castor::hashCombine( result, value );
			}

			castor::hashCombine( result, vertexBuffers.count );
			castor::hashCombine( result, hash( indexBuffer ) );
			return result;
		}
	}

	//*********************************************************************************************

	void addDebugAabbs( DebugDrawer & drawer
		, ashes::VkDescriptorSetLayoutBindingArray const & bindings
		, ashes::WriteDescriptorSetArray const & writes
		, VkDeviceSize count
		, ashes::PipelineShaderStageCreateInfoArray const & shader
		, bool enableDepthTest )
	{
		drawer.addAabbs( bindings, writes, count, shader, enableDepthTest );
	}

	void addDebugDrawable( DebugDrawer & drawer
		, DebugVertexBuffers vertexBuffers
		, DebugIndexBuffer indexBuffer
		, ashes::VkVertexInputAttributeDescriptionArray const & vertexAttributes
		, ashes::VkVertexInputBindingDescriptionArray const & vertexBindings
		, ashes::VkDescriptorSetLayoutBindingArray const & bindings
		, ashes::WriteDescriptorSetArray const & writes
		, VkDeviceSize count
		, ashes::PipelineShaderStageCreateInfoArray const & shader
		, bool enableDepthTest )
	{
		drawer.addDrawable( vertexBuffers, indexBuffer
			, std::move( vertexAttributes ), std::move( vertexBindings )
			, bindings, writes, count, shader, enableDepthTest );
	}

	//*********************************************************************************************

	DebugDrawer::AABBBuffer::AABBBuffer( DebugVertexBuffers pvertexBuffers
		, DebugIndexBuffer pindexBuffer
		, VkDeviceSize pinstanceCount
		, Pipeline * ppipeline
		, ashes::DescriptorSet * pdescriptorSet
		, ashes::WriteDescriptorSetArray pwrites )
		: vertexBuffers{ std::move( pvertexBuffers ) }
		, indexBuffer{ std::move( pindexBuffer ) }
		, instanceCount{ pinstanceCount }
		, pipeline{ ppipeline }
		, descriptorSet{ pdescriptorSet }
		, writes{ std::move( pwrites ) }
	{
	}

	//*********************************************************************************************
	
	DebugDrawer::FramePass::FramePass( crg::FramePass const & framePass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, VkExtent2D dimensions
		, uint32_t const * passIndex )
		: crg::RenderPass{ framePass
			, context
			, graph
			, { crg::defaultV< InitialiseCallback >
				, [this]( crg::RecordContext & recContext, VkCommandBuffer cb, uint32_t ){ doSubRecordInto( recContext, cb ); }
				, crg::defaultV< GetSubpassContentsCallback >
				, GetPassIndexCallback( [passIndex](){ return *passIndex; } )
				, IsEnabledCallback( [this](){ return doIsEnabled(); } ) }
			, dimensions
			, crg::ru::Config{ 2u } }
		, m_device{ device }
	{
		m_aabb.vertices = m_device.vertexPools->getBuffer< castor::Point4f >( 8u );
		m_aabb.indices = m_device.indexPools->getBuffer< uint32_t >( 24u );
		{
			static castor::Array< castor::Point4f, 8u > const vertexData{ castor::Point4f{ 0.0f, 0.0f, 0.0f, 1.0f }
				, castor::Point4f{ 1.0f, 0.0f, 0.0f, 1.0f }
				, castor::Point4f{ 0.0f, 1.0f, 0.0f, 1.0f }
				, castor::Point4f{ 1.0f, 1.0f, 0.0f, 1.0f }
				, castor::Point4f{ 0.0f, 0.0f, 1.0f, 1.0f }
				, castor::Point4f{ 1.0f, 0.0f, 1.0f, 1.0f }
				, castor::Point4f{ 0.0f, 1.0f, 1.0f, 1.0f }
				, castor::Point4f{ 1.0f, 1.0f, 1.0f, 1.0f } };
			static castor::Array< castor::Point2ui, 12u > const indexData{ castor::Point2ui{ 0u, 1u }
				, castor::Point2ui{ 1u, 3u }
				, castor::Point2ui{ 3u, 2u }
				, castor::Point2ui{ 2u, 0u }
				, castor::Point2ui{ 4u, 5u }
				, castor::Point2ui{ 5u, 7u }
				, castor::Point2ui{ 7u, 6u }
				, castor::Point2ui{ 6u, 4u }
				, castor::Point2ui{ 0u, 4u }
				, castor::Point2ui{ 1u, 5u }
				, castor::Point2ui{ 2u, 6u }
				, castor::Point2ui{ 3u, 7u } };
			auto queue = m_device.graphicsData();
			InstantDirectUploadData uploader{ *queue->queue
				, m_device
				, cuT( "RenderCube" )
				, *queue->commandPool };
			uploader->pushUpload( vertexData.data()->constPtr()
				, vertexData.size() * sizeof( castor::Point4f )
				, m_aabb.vertices.getBuffer( SubmeshData::ePositions )
				, m_aabb.vertices.getOffset( SubmeshData::ePositions )
				, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT
				, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT );
			uploader->pushUpload( indexData.data()->constPtr()
				, indexData.size() * sizeof( castor::Point2ui )
				, m_aabb.indices.getBuffer( SubmeshData::eIndex )
				, m_aabb.indices.getOffset( SubmeshData::eIndex )
				, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT
				, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT );
		}
	}

	DebugDrawer::FramePass::~FramePass()noexcept
	{
		m_device.indexPools->putBuffer( m_aabb.indices );
		m_device.vertexPools->putBuffer< castor::Point4f >( m_aabb.vertices );
	}

	void DebugDrawer::FramePass::addAabbs( ashes::VkDescriptorSetLayoutBindingArray const & bindings
		, ashes::WriteDescriptorSetArray const & writes
		, VkDeviceSize instanceCount
		, ashes::PipelineShaderStageCreateInfoArray const & shader
		, bool enableDepthTest )
	{
		VkBuffer vertexBuffer = m_aabb.vertices.getBuffer( SubmeshData::ePositions );
		VkDeviceSize vertexOffset = m_aabb.vertices.getOffset( SubmeshData::ePositions );
		VkBuffer indexBuffer = m_aabb.indices.getBuffer( SubmeshData::eIndex );
		VkDeviceSize indexOffset = m_aabb.indices.getOffset( SubmeshData::eIndex );

		addDrawable( { { vertexBuffer }, { vertexOffset }, 8u }
			, { indexBuffer, indexOffset, 24u }
			, { VkVertexInputAttributeDescription{ 0u, 0u, VK_FORMAT_R32G32B32A32_SFLOAT, 0u } }
			, { VkVertexInputBindingDescription{ 0u, 16u, VK_VERTEX_INPUT_RATE_VERTEX } }
			, bindings
			, writes
			, instanceCount
			, shader
			, enableDepthTest );
	}

	void DebugDrawer::FramePass::addDrawable( DebugVertexBuffers vertexBuffers
		, DebugIndexBuffer indexBuffer
		, ashes::VkVertexInputAttributeDescriptionArray const & vertexAttributes
		, ashes::VkVertexInputBindingDescriptionArray const & vertexBindings
		, ashes::VkDescriptorSetLayoutBindingArray const & bindings
		, ashes::WriteDescriptorSetArray const & writes
		, VkDeviceSize instanceCount
		, ashes::PipelineShaderStageCreateInfoArray const & shader
		, bool enableDepthTest )
	{
		auto renderPass = getRenderPass( 0u );
		m_pending = true;

		if ( !renderPass )
		{
			return;
		}

		auto hash = dbgdrw::hash( vertexAttributes, vertexBindings, shader, enableDepthTest );
		auto [it, res] = m_pipelines.try_emplace( hash );

		if ( res )
		{
			auto name = castor::toUtf8( m_pass.getName() ) + "/AABB/" + castor::string::toMbString( hash );
			auto & extent = doGetHolder().getRenderSize();
			ashes::PipelineVertexInputStateCreateInfo vertexState{ 0u
				, vertexBindings
				, vertexAttributes };
			ashes::PipelineViewportStateCreateInfo viewportState{ 0u
				, { makeViewport( castor::Point2ui{ extent.width, extent.height } ) }
				, { makeScissor( castor::Point2ui{ extent.width, extent.height } ) } };
			it->second = std::make_unique< Pipeline >();
			it->second->descriptorLayout = m_device->createDescriptorSetLayout( name, bindings );
			it->second->pipelineLayout = m_device->createPipelineLayout( name
				, *it->second->descriptorLayout );
			ashes::GraphicsPipelineCreateInfo graphics{ 0u
				, shader
				, castor::move( vertexState )
				, ashes::PipelineInputAssemblyStateCreateInfo{ 0u, VK_PRIMITIVE_TOPOLOGY_LINE_LIST }
				, ashes::nullopt
				, castor::move( viewportState )
				, ashes::PipelineRasterizationStateCreateInfo{ 0u, VK_FALSE, VK_FALSE, VK_POLYGON_MODE_LINE, VK_CULL_MODE_NONE }
				, ashes::PipelineMultisampleStateCreateInfo{}
				, ashes::PipelineDepthStencilStateCreateInfo{ 0u, enableDepthTest, VK_FALSE, VK_COMPARE_OP_GREATER }
				, ashes::PipelineColorBlendStateCreateInfo{}
				, ashes::nullopt
				, static_cast< VkPipelineLayout >( *it->second->pipelineLayout )
				, getRenderPass( 0u ) };
			it->second->pipeline = m_device->createPipeline( name, castor::move( graphics ) );
		}

		auto ihash = dbgdrw::hash( vertexBuffers, indexBuffer, writes );
		auto [iit, ires] = it->second->instances.try_emplace( ihash );

		if ( ires )
		{
			auto name = castor::toUtf8( m_pass.getName() ) + "/AABB/" + castor::string::toMbString( hash ) + "/" + castor::string::toMbString( ihash );
			iit->second.descriptorPool = it->second->descriptorLayout->createPool( name, 1U );
			iit->second.descriptorSet = iit->second.descriptorPool->createDescriptorSet( name );
			iit->second.descriptorSet->setBindings( writes );
			iit->second.descriptorSet->update();
		}

		m_aabbs.emplace_back( std::move( vertexBuffers )
			, std::move( indexBuffer )
			, instanceCount
			, it->second.get()
			, iit->second.descriptorSet.get()
			, writes );
	}

	void DebugDrawer::FramePass::doSubRecordInto( crg::RecordContext const & context
		, VkCommandBuffer commandBuffer )
	{
		if ( !m_aabbs.empty() )
		{
			for ( auto const & aabb : m_aabbs )
			{
				VkDescriptorSet descriptorSet = *aabb.descriptorSet;
				context->vkCmdBindPipeline( commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *aabb.pipeline->pipeline );
				context->vkCmdBindDescriptorSets( commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *aabb.pipeline->pipelineLayout, 0u, 1u, &descriptorSet, 0u, nullptr );

				if ( !aabb.vertexBuffers.buffers.empty() )
				{
					context->vkCmdBindVertexBuffers( commandBuffer, 0u, uint32_t( aabb.vertexBuffers.buffers.size() ), aabb.vertexBuffers.buffers.data(), aabb.vertexBuffers.offsets.data() );
				}

				if ( aabb.indexBuffer.buffer )
				{
					context->vkCmdBindIndexBuffer( commandBuffer, aabb.indexBuffer.buffer, aabb.indexBuffer.offset, VK_INDEX_TYPE_UINT32 );
					context->vkCmdDrawIndexed( commandBuffer, uint32_t( aabb.indexBuffer.count ), uint32_t( aabb.instanceCount ), 0u, 0u, 0u );
				}
				else
				{
					context->vkCmdDraw( commandBuffer, uint32_t( aabb.vertexBuffers.count ), uint32_t( aabb.instanceCount ), 0u, 0u );
				}
			}

			m_aabbs.clear();
		}

		m_pending = false;
	}

	//*********************************************************************************************

	DebugDrawer::DebugDrawer( crg::FramePassGroup & graph
		, crg::FramePass const * previous
		, RenderDevice const & device
		, RenderTarget & parent
		, crg::ImageViewIdArray colour
		, Texture const & depth
			, uint32_t const * passIndex )
		: castor::OwnedBy< RenderTarget >{ parent }
	{
		auto extent = makeExtent2D( depth.getExtent() );
		auto & pass = graph.createPass( "DebugDraw"
			, [this, &device, extent, passIndex]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				auto result = castor::make_unique< FramePass >( framePass
					, context
					, runnableGraph
					, device
					, extent
					, passIndex );
				m_framePass = result.get();
				device.renderSystem.getEngine()->registerTimer( castor::makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		pass.addDependency( *previous );
		pass.addInOutColourView( std::move( colour ) );
		pass.addInputDepthStencilView( depth.targetViewId );
		m_lastPass = &pass;
	}

	void DebugDrawer::addAabbs( ashes::VkDescriptorSetLayoutBindingArray const & bindings
		, ashes::WriteDescriptorSetArray const & writes
		, VkDeviceSize count
		, ashes::PipelineShaderStageCreateInfoArray const & shader
		, bool enableDepthTest )
	{
		if ( m_framePass && count > 0u )
		{
			m_framePass->addAabbs( bindings, writes, count, shader, enableDepthTest );
		}
	}

	void DebugDrawer::addDrawable( DebugVertexBuffers vertexBuffers
		, DebugIndexBuffer indexBuffer
		, ashes::VkVertexInputAttributeDescriptionArray const & vertexAttributes
		, ashes::VkVertexInputBindingDescriptionArray const & vertexBindings
		, ashes::VkDescriptorSetLayoutBindingArray const & bindings
		, ashes::WriteDescriptorSetArray const & writes
		, VkDeviceSize instanceCount
		, ashes::PipelineShaderStageCreateInfoArray const & shader
		, bool enableDepthTest )
	{
		if ( m_framePass && instanceCount > 0u )
		{
			m_framePass->addDrawable( vertexBuffers, indexBuffer
				, std::move( vertexAttributes ), std::move( vertexBindings )
				, bindings, writes, instanceCount, shader, enableDepthTest );
		}
	}

	//*********************************************************************************************
}
