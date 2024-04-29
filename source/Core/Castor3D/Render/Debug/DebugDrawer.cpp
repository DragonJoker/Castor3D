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
		static size_t hash( VkPipelineShaderStageCreateInfo const & shader )
		{
			size_t result = std::hash< VkFlags >{}( shader.flags );
			castor::hashCombine( result, shader.module );
			castor::hashCombine( result, shader.stage );
			return result;
		}

		static size_t hash( VkDescriptorImageInfo const & write )
		{
			size_t result = std::hash< VkSampler >{}( write.sampler );
			castor::hashCombine( result, write.imageView );
			castor::hashCombine( result, write.imageLayout );
			return result;
		}

		static size_t hash( VkDescriptorBufferInfo const & write )
		{
			size_t result = std::hash< VkBuffer >{}( write.buffer );
			castor::hashCombine( result, write.offset );
			castor::hashCombine( result, write.range );
			return result;
		}

		static size_t hash( VkWriteDescriptorSet const & write )
		{
			size_t result = std::hash< uint32_t >{}( write.dstBinding );
			castor::hashCombine( result, write.dstArrayElement );
			castor::hashCombine( result, write.descriptorCount );
			castor::hashCombine( result, write.descriptorType );

			if ( write.pImageInfo )
			{
				for ( auto & imageInfo : castor::makeArrayView( write.pImageInfo, write.descriptorCount ) )
				{
					castor::hashCombine( result, hash( imageInfo ) );
				}
			}

			if ( write.pImageInfo )
			{
				for ( auto & imageInfo : castor::makeArrayView( write.pImageInfo, write.descriptorCount ) )
				{
					castor::hashCombine( result, hash( imageInfo ) );
				}
			}

			if ( write.pBufferInfo )
			{
				for ( auto & bufferInfo : castor::makeArrayView( write.pBufferInfo, write.descriptorCount ) )
				{
					castor::hashCombine( result, hash( bufferInfo ) );
				}
			}

			if ( write.pTexelBufferView )
			{
				for ( auto & texelBufferView : castor::makeArrayView( write.pTexelBufferView, write.descriptorCount ) )
				{
					castor::hashCombine( result, texelBufferView );
				}
			}

			return result;
		}

		static size_t hash( ashes::PipelineShaderStageCreateInfoArray const & shaders
			, bool enableDepthTest )
		{
			size_t result{};

			for ( auto & shader : shaders )
			{
				castor::hashCombine( result, hash( shader ) );
			}

			castor::hashCombine( result, enableDepthTest );
			return result;
		}

		static size_t hash( ashes::WriteDescriptorSetArray const & writes )
		{
			size_t result{};

			for ( auto & write : writes )
			{
				castor::hashCombine( result, hash( write ) );
			}

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

	//*********************************************************************************************

	DebugDrawer::AABBBuffer::AABBBuffer( VkDeviceSize pcount
		, Pipeline * ppipeline
		, ashes::DescriptorSet * pdescriptorSet
		, ashes::WriteDescriptorSetArray pwrites )
		: count{ pcount }
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
		m_device.vertexPools->putBuffer( m_aabb.vertices );
	}

	void DebugDrawer::FramePass::addAabbs( ashes::VkDescriptorSetLayoutBindingArray const & bindings
		, ashes::WriteDescriptorSetArray const & writes
		, VkDeviceSize count
		, ashes::PipelineShaderStageCreateInfoArray const & shader
		, bool enableDepthTest )
	{
		auto renderPass = getRenderPass( 0u );
		m_pending = true;

		if ( !renderPass )
		{
			return;
		}

		auto hash = dbgdrw::hash( shader, enableDepthTest );
		auto [it, res] = m_pipelines.try_emplace( hash );

		if ( res )
		{
			auto name = castor::toUtf8( m_pass.getName() ) + "/AABB/" + castor::string::toMbString( hash );
			auto & extent = doGetHolder().getRenderSize();
			ashes::PipelineVertexInputStateCreateInfo vertexState{ 0u
				, ashes::VkVertexInputBindingDescriptionArray{ VkVertexInputBindingDescription{ 0u, 16u, VK_VERTEX_INPUT_RATE_VERTEX } }
				, ashes::VkVertexInputAttributeDescriptionArray{ VkVertexInputAttributeDescription{ 0u, 0u, VK_FORMAT_R32G32B32A32_SFLOAT, 0u } } };
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

		auto ihash = dbgdrw::hash( writes );
		auto [iit, ires] = it->second->instances.try_emplace( ihash );

		if ( ires )
		{
			auto name = castor::toUtf8( m_pass.getName() ) + "/AABB/" + castor::string::toMbString( hash ) + "/" + castor::string::toMbString( ihash );
			iit->second.descriptorPool = it->second->descriptorLayout->createPool( name, 1U );
			iit->second.descriptorSet = iit->second.descriptorPool->createDescriptorSet( name );
			iit->second.descriptorSet->setBindings( writes );
			iit->second.descriptorSet->update();
		}

		m_aabbs.emplace_back( count, it->second.get(), iit->second.descriptorSet.get(), writes );
	}

	void DebugDrawer::FramePass::doSubRecordInto( crg::RecordContext const & context
		, VkCommandBuffer commandBuffer )
	{
		if ( !m_aabbs.empty() )
		{
			VkBuffer vertexBuffer = m_aabb.vertices.getBuffer( SubmeshData::ePositions );
			VkDeviceSize vertexOffset = m_aabb.vertices.getOffset( SubmeshData::ePositions );
			VkBuffer indexBuffer = m_aabb.indices.getBuffer( SubmeshData::eIndex );
			VkDeviceSize indexOffset = m_aabb.indices.getOffset( SubmeshData::eIndex );

			for ( auto const & aabb : m_aabbs )
			{
				VkDescriptorSet descriptorSet = *aabb.descriptorSet;

				context->vkCmdBindPipeline( commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *aabb.pipeline->pipeline );
				context->vkCmdBindDescriptorSets( commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *aabb.pipeline->pipelineLayout, 0u, 1u, &descriptorSet, 0u, nullptr );
				context->vkCmdBindVertexBuffers( commandBuffer, 0u, 1u, &vertexBuffer, &vertexOffset );
				context->vkCmdBindIndexBuffer( commandBuffer, indexBuffer, indexOffset, VK_INDEX_TYPE_UINT32 );
				context->vkCmdDrawIndexed( commandBuffer, 24u, uint32_t( aabb.count ), 0u, 0u, 0u );
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

	//*********************************************************************************************
}
