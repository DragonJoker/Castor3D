﻿#include "Castor3D/Render/Picking.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderQueue.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Culling/SceneCuller.hpp"
#include "Castor3D/Render/Node/QueueRenderNodes.hpp"
#include "Castor3D/Render/Node/BillboardRenderNode.hpp"
#include "Castor3D/Render/Node/SceneRenderNodes.hpp"
#include "Castor3D/Render/Node/SubmeshRenderNode.hpp"
#include "Castor3D/Render/Passes/PickingPass.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <ashespp/Image/Image.hpp>
#include <ashespp/Sync/Fence.hpp>
#include <ashespp/Sync/Queue.hpp>

CU_ImplementCUSmartPtr( castor3d, Picking )

namespace castor3d
{
	//*********************************************************************************************

	namespace rendpick
	{
		static std::string const Picking = "Picking";
		static std::string const DrawIndex = "c3d_iDrawIndex";
		static std::string const NodeIndex = "c3d_iNodeIndex";

		static int constexpr PickingOffset = int( PickingAreaWidth / 2 );
		static int constexpr BufferOffset = ( PickingOffset * PickingAreaWidth ) + PickingOffset - 1;

		inline castor::Position convertToTopDown( castor::Position const & position
			, castor::Size const & size )
		{
			return
			{
				position.x(),
				int32_t( size.getHeight() - position.y() )
			};
		}

		inline void pickSubmesh( GeometryCache const & cache
			, uint32_t nodeId
			, uint32_t primitiveId
			, Geometry const *& node
			, Submesh const *& subnode
			, uint32_t & face )
		{
			auto renderNode = cache.getScene()->getRenderNodes().getSubmeshNode( nodeId );

			if ( renderNode )
			{
				node = &renderNode->instance;
				subnode = &renderNode->data;
				face = primitiveId;
			}
		}

		inline void pickBillboard( BillboardListCache const & cache
			, uint32_t nodeId
			, uint32_t primitiveId
			, BillboardBase const *& node
			, uint32_t & face )
		{
			auto renderNode = cache.getScene()->getRenderNodes().getBillboardNode( nodeId );

			if ( renderNode )
			{
				node = &renderNode->instance;
				face = primitiveId;
			}
		}

		inline std::vector< VkBufferImageCopy > createPickDisplayRegions()
		{
			std::vector< VkBufferImageCopy > result;

			for ( int i = 0; i < int( PickingAreaWidth ); ++i )
			{
				for ( int j = 0; j < int( PickingAreaWidth ); ++j )
				{
					result.push_back( { BufferOffset * ashes::getMinimalSize( VK_FORMAT_R32G32B32A32_UINT )
						, 0u
						, 0u
						, { VK_IMAGE_ASPECT_COLOR_BIT, 0u, 0u, 1u }
						, { int( PickingAreaWidth ) * 2 + i, j, 0 }
						, { 1u, 1u, 1u } } );
				}
			}

			return result;
		}
	}

	//*********************************************************************************************

	uint32_t const Picking::UboBindingPoint = 7u;

	Picking::Picking( crg::ResourcesCache & resources
		, RenderDevice const & device
		, QueueData const & queueData
		, castor::Size const & size
		, MatrixUbo & matrixUbo
		, SceneUbo & sceneUbo
		, SceneCuller & culler )
		: castor::OwnedBy< Engine >{ *device.renderSystem.getEngine() }
		, m_device{ device }
		, m_bandSize{ getSafeBandSize( size ) }
		, m_realSize{ getSafeBandedSize( size ) }
		, m_graph{ resources.getHandler(), "PickingGraph" }
		, m_colourImage{ m_graph.createImage( { "PickingColour"
			, 0u
			, VK_IMAGE_TYPE_2D
			, VK_FORMAT_R32G32B32A32_UINT
			, makeExtent3D( m_realSize )
			, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_TRANSFER_SRC_BIT
				| VK_IMAGE_USAGE_TRANSFER_DST_BIT
				| VK_IMAGE_USAGE_SAMPLED_BIT ) } ) }
		, m_colourImageView{ m_graph.createView( { "PickingColour"
			, m_colourImage
			, 0u
			, VK_IMAGE_VIEW_TYPE_2D
			, m_colourImage.data->info.format
			, { VK_IMAGE_ASPECT_COLOR_BIT, 0u, 1u, 0u, 1u } } ) }
		, m_depthImage{ m_graph.createImage( { "PickingDepth"
			, 0u
			, VK_IMAGE_TYPE_2D
			, VK_FORMAT_D32_SFLOAT
			, makeExtent3D( m_realSize )
			, ( VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_SAMPLED_BIT ) } ) }
		, m_depthImageView{ m_graph.createView( { "PickingDepth"
			, m_depthImage
			, 0u
			, VK_IMAGE_VIEW_TYPE_2D
			, m_depthImage.data->info.format
			, { VK_IMAGE_ASPECT_DEPTH_BIT, 0u, 1u, 0u, 1u } } ) }
		, m_pickingPassDesc{ &doCreatePickingPass( matrixUbo, sceneUbo, culler ) }
		, m_copyRegion{ 0u
			, 0u
			, 0u
			, { VK_IMAGE_ASPECT_COLOR_BIT, 0u, 0u, 1u }
			, { 0, 0, 0 }
			, { PickingAreaWidth, PickingAreaWidth, 1u } }
		, m_pickDisplayRegions{ rendpick::createPickDisplayRegions() }
		, m_commandBuffer{ queueData.commandPool->createCommandBuffer( "PickingPass" ) }
		, m_stagingBuffer{ makeBuffer< castor::Point4ui >( m_device
			, PickingAreaWidth * PickingAreaWidth
			, ( VK_BUFFER_USAGE_TRANSFER_DST_BIT
				| VK_BUFFER_USAGE_TRANSFER_SRC_BIT )
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, "PickingPassStagingBuffer" ) }
		, m_buffer{ PickingAreaWidth * PickingAreaWidth }
		, m_transferFence{ m_device->createFence( "PickingPass" ) }
	{
		m_runnable = m_graph.compile( device.makeContext() );
		printGraph( *m_runnable );
		m_colourTexture = std::make_unique< ashes::Image >( *m_device
			, m_runnable->createImage( m_colourImage )
			, m_colourImage.data->info );
		m_colourView = ashes::ImageView{ m_colourImageView.data->info
			, m_runnable->createImageView( m_colourImageView )
			, m_colourTexture.get() };
	}

	Picking::~Picking()
	{
		m_commandBuffer.reset();
		m_stagingBuffer.reset();
		m_colourTexture.reset();
	}

	void Picking::addScene( Scene & scene, Camera & camera )
	{
	}

	PickNodeType Picking::pick( RenderDevice const & device
		, castor::Position position
		, Camera const & camera )
	{
		if ( !m_picking.exchange( true ) )
		{
			position[0] += m_bandSize;
			position[1] += m_bandSize;
			position = rendpick::convertToTopDown( position, m_realSize );
			m_pickNodeType = PickNodeType::eNone;
			m_geometry = {};
			m_submesh = {};
			m_billboard = {};
			m_face = 0u;
			int32_t offsetX = std::clamp( position.x() - rendpick::PickingOffset
				, 0
				, int32_t( m_realSize.getWidth() - PickingAreaWidth ) );
			int32_t offsetY = std::clamp( position.y() - rendpick::PickingOffset
				, 0
				, int32_t( m_realSize.getHeight() - PickingAreaWidth ) );
			VkRect2D scissor{ { offsetX, offsetY }
				, { PickingAreaWidth, PickingAreaWidth } };

			if ( m_first )
			{
				m_pickingPass->updateArea( scissor );
				doFboPick( position );
				m_first = false;
			}

			m_pickingPass->updateArea( scissor );
			auto pixel = doFboPick( position );
			m_pickNodeType = doPick( pixel
				, m_pickingPass->getCuller().getScene() );
			m_picking = false;
		}

		return m_pickNodeType;
	}

	crg::FramePass & Picking::doCreatePickingPass( MatrixUbo & matrixUbo
		, SceneUbo & sceneUbo
		, SceneCuller & culler )
	{
		auto & result = m_graph.createPass( "PickingPass"
			, [this, &matrixUbo, &sceneUbo, &culler]( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto res = std::make_unique< PickingPass >( pass
					, context
					, graph
					, m_device
					, m_realSize
					, matrixUbo
					, sceneUbo
					, culler );
				m_pickingPass = res.get();
				return res;
			} );
		result.addOutputDepthView( m_depthImageView
			, defaultClearDepthStencil );
#if C3D_DebugPicking
		result.addOutputColourView( m_colourImageView
			, makeClearValue( 0u, 0u, 0u, 0u )
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
#else
		result.addOutputColourView( m_colourImageView
			, makeClearValue( 0u, 0u, 0u, 0u )
			, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL );
#endif
		return result;
	}

	castor::Point4ui Picking::doFboPick( castor::Position const & position )
	{
		auto queueData = m_device.graphicsData();
		m_toWait = m_runnable->run( crg::SemaphoreWaitArray{}, *queueData->queue );

		m_copyRegion.imageOffset.x = std::clamp( position.x() - rendpick::PickingOffset
			, 0
			, int32_t( m_colourImage.data->info.extent.width - PickingAreaWidth ) );
		m_copyRegion.imageOffset.y = std::clamp( position.y() - rendpick::PickingOffset
			, 0
			, int32_t( m_colourImage.data->info.extent.height - PickingAreaWidth ) );

#if !C3D_DebugPicking
		m_commandBuffer->begin();
		m_commandBuffer->beginDebugBlock( { "PickingPass Copy"
			, makeFloatArray( getEngine()->getNextRainbowColour() ) } );
		auto pipelineStageFlags = m_stagingBuffer->getBuffer().getCompatibleStageFlags();
		m_commandBuffer->memoryBarrier( pipelineStageFlags
			, VK_PIPELINE_STAGE_TRANSFER_BIT
			, m_stagingBuffer->getBuffer().makeTransferDestination() );
		m_commandBuffer->copyToBuffer( m_copyRegion
			, *m_colourTexture
			, m_stagingBuffer->getBuffer() );
		VkImageLayout layout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

#	if C3D_DebugPickingTransfer

		m_commandBuffer->endDebugBlock();
		m_commandBuffer->beginDebugBlock(
			{
				"PickingPass Transfer Display",
				makeFloatArray( getEngine()->getNextRainbowColour() ),
			} );
		m_commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
			, VK_PIPELINE_STAGE_TRANSFER_BIT
			, m_stagingBuffer->getBuffer().makeTransferSource() );
		m_commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
			, VK_PIPELINE_STAGE_TRANSFER_BIT
			, m_colourView.makeTransferDestination( VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL ) );
		m_commandBuffer->copyToImage( m_transferDisplayRegion
			, m_stagingBuffer->getBuffer()
			, *m_colourTexture );

		for ( auto & region : m_pickDisplayRegions )
		{
			m_commandBuffer->copyToImage( region
				, m_stagingBuffer->getBuffer()
				, *m_colourTexture );
		}

		layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

#	endif

		m_commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
			, VK_PIPELINE_STAGE_HOST_BIT
			, m_stagingBuffer->getBuffer().makeHostRead() );
		m_commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
			, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			, m_colourView.makeColourAttachment( layout ) );
		m_commandBuffer->endDebugBlock();
		m_commandBuffer->end();

		std::vector< VkSemaphore > semaphores;
		std::vector< VkPipelineStageFlags > dstStageMasks;
		convert( m_toWait, semaphores, dstStageMasks );
		queueData->queue->submit( { *m_commandBuffer }
			, semaphores
			, dstStageMasks
			, ashes::VkSemaphoreArray{}
			, *m_transferFence );
		m_transferFence->wait( ashes::MaxTimeout );
		m_transferFence->reset();
		m_toWait.clear();

#endif
#if !C3D_DebugPicking

		if ( auto * data = m_stagingBuffer->lock( 0u, m_stagingBuffer->getCount(), 0u ) )
		{
			std::copy( data, data + m_stagingBuffer->getCount(), m_buffer.begin() );
			m_stagingBuffer->unlock();
		}

#endif

		auto & result = *( m_buffer.begin() + rendpick::BufferOffset );
		log::trace << cuT( "Picked" )
			<< cuT( ": " ) << std::dec << result->x
			<< cuT( ", " ) << std::dec << result->y
			<< cuT( ", " ) << std::dec << result->z
			<< cuT( ", " ) << std::dec << result->w
			<< cuT( ", at " ) << std::dec << position[0]
			<< cuT( "x" ) << std::dec << position[1] << std::endl;
		return result;
	}

	PickNodeType Picking::doPick( castor::Point4ui const & pixel
		, Scene const & scene )
	{
		auto result{ PickNodeType( pixel->x ) };

		if ( result != PickNodeType::eNone )
		{
			auto nodeId = pixel->y;
			auto primitiveId = pixel->z;

			switch ( result )
			{
			case PickNodeType::eSubmesh:
				rendpick::pickSubmesh( scene.getGeometryCache()
					, nodeId
					, primitiveId
					, m_geometry
					, m_submesh
					, m_face );
				break;

			case PickNodeType::eBillboard:
				rendpick::pickBillboard( scene.getBillboardListCache()
					, nodeId
					, primitiveId
					, m_billboard
					, m_face );
				break;

			default:
				CU_Failure( "Unsupported index" );
				result = PickNodeType::eNone;
				break;
			}
		}

		return result;
	}

	//*********************************************************************************************
}
