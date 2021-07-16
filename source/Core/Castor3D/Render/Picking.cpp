#include "Castor3D/Render/Picking.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Culling/SceneCuller.hpp"
#include "Castor3D/Render/Node/QueueCulledRenderNodes.hpp"
#include "Castor3D/Render/Node/BillboardRenderNode.hpp"
#include "Castor3D/Render/Node/SubmeshRenderNode.hpp"
#include "Castor3D/Render/Passes/PickingPass.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Geometry.hpp"

#include <ashespp/Image/Image.hpp>
#include <ashespp/Sync/Fence.hpp>
#include <ashespp/Sync/Queue.hpp>

CU_ImplementCUSmartPtr( castor3d, Picking )

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		static std::string const Picking = "Picking";
		static std::string const DrawIndex = "c3d_iDrawIndex";
		static std::string const NodeIndex = "c3d_iNodeIndex";

		static int constexpr PickingOffset = int( Picking::PickingWidth / 2 );
		static int constexpr BufferOffset = ( PickingOffset * Picking::PickingWidth ) + PickingOffset - 1;

		castor::Position convertToTopDown( castor::Position const & position
			, castor::Size const & size )
		{
			return
			{
				position.x(),
				int32_t( size.getHeight() - position.y() )
			};
		}

		template< typename MapType, typename NodeType, typename SubNodeType >
		inline void pickFromList( MapType const & map
			, castor::Point4f const & index
			, std::weak_ptr< NodeType > & node
			, std::weak_ptr< SubNodeType > & subnode
			, uint32_t & face )
		{
			uint32_t pipelineIndex{ ( uint32_t( index[0] ) >> 8 ) - 1 };

			if ( map.size() > pipelineIndex )
			{
				auto itPipeline = map.begin();

				while ( pipelineIndex )
				{
					++itPipeline;
					--pipelineIndex;
				}

				uint32_t nodeIndex{ uint32_t( index[1] ) };

				if ( itPipeline->second.size() > nodeIndex )
				{
					auto itNode = itPipeline->second.begin() + nodeIndex;

					subnode = std::static_pointer_cast< SubNodeType >( ( *itNode )->data.shared_from_this() );
					node = std::static_pointer_cast< NodeType >( ( *itNode )->instance.shared_from_this() );
					face = uint32_t( index[3] );
				}
			}
		}

		template< typename MapType, typename NodeType, typename SubNodeType >
		inline void pickFromInstantiatedList( MapType const & map
			, castor::Point4f const & index
			, std::weak_ptr< NodeType > & node
			, std::weak_ptr< SubNodeType > & subnode
			, uint32_t & face )
		{
			uint32_t pipelineIndex{ ( uint32_t( index[0] ) >> 8 ) - 1 };

			if ( map.size() > pipelineIndex )
			{
				auto itPipeline = map.begin();

				while ( pipelineIndex )
				{
					++itPipeline;
					--pipelineIndex;
				}

				auto itPass = itPipeline->second.begin();

				if ( !itPass->second.empty() )
				{
					auto itMesh = itPass->second.begin();
					uint32_t nodeIndex{ uint32_t( index[1] ) };

					while ( nodeIndex && itPass != itPipeline->second.end() )
					{
						while ( itMesh != itPass->second.end() && nodeIndex )
						{
							++itMesh;
							--nodeIndex;
						}

						if ( nodeIndex || itMesh == itPass->second.end() )
						{
							++itPass;

							if ( itPass != itPipeline->second.end() )
							{
								itMesh = itPass->second.begin();
							}
						}
					}

					if ( itPass != itPipeline->second.end()
						&& itMesh != itPass->second.end() )
					{
						if ( !itMesh->second.empty() )
						{
							uint32_t instanceIndex{ uint32_t( index[2] ) };
							uint32_t faceIndex{ uint32_t( index[3] ) };
							auto itNode = itMesh->second.begin() + instanceIndex;

							subnode = ( *itNode )->data.shared_from_this();
							node = std::static_pointer_cast< Geometry >( ( *itNode )->instance.shared_from_this() );
							face = faceIndex;
						}
					}
				}
			}
		}

		std::vector< VkBufferImageCopy > createPickDisplayRegions()
		{
			std::vector< VkBufferImageCopy > result;

			for ( int i = 0; i < Picking::PickingWidth; ++i )
			{
				for ( int j = 0; j < Picking::PickingWidth; ++j )
				{
					result.push_back( { BufferOffset * ashes::getMinimalSize( VK_FORMAT_R32G32B32A32_SFLOAT )
						, 0u
						, 0u
						, { VK_IMAGE_ASPECT_COLOR_BIT, 0u, 0u, 1u }
						, { int( Picking::PickingWidth ) * 2 + i, j, 0 }
						, { 1u, 1u, 1u } } );
				}
			}

			return result;
		}
	}

	//*********************************************************************************************

	uint32_t const Picking::UboBindingPoint = 7u;

	Picking::Picking( crg::ResourceHandler & handler
		, RenderDevice const & device
		, castor::Size const & size
		, MatrixUbo & matrixUbo
		, SceneCuller & culler )
		: castor::OwnedBy< Engine >{ *device.renderSystem.getEngine() }
		, m_device{ device }
		, m_size{ size }
		, m_graph{ handler, "PickingGraph" }
		, m_colourImage{ m_graph.createImage( { "PickingColour"
			, 0u
			, VK_IMAGE_TYPE_2D
			, VK_FORMAT_R32G32B32A32_SFLOAT
			, makeExtent3D( size )
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
			, makeExtent3D( size )
			, ( VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_SAMPLED_BIT ) } ) }
		, m_depthImageView{ m_graph.createView( { "PickingDepth"
			, m_depthImage
			, 0u
			, VK_IMAGE_VIEW_TYPE_2D
			, m_depthImage.data->info.format
			, { VK_IMAGE_ASPECT_DEPTH_BIT, 0u, 1u, 0u, 1u } } ) }
		, m_pickingPassDesc{ doCreatePickingPass( matrixUbo, culler ) }
		, m_copyRegion{ 0u
			, 0u
			, 0u
			, { VK_IMAGE_ASPECT_COLOR_BIT, 0u, 0u, 1u }
			, { 0, 0, 0 }
			, { PickingWidth, PickingWidth, 1u } }
		, m_transferDisplayRegion{ 0u
			, 0u
			, 0u
			, { VK_IMAGE_ASPECT_COLOR_BIT, 0u, 0u, 1u }
			, { 0, 0, 0 }
			, { PickingWidth, PickingWidth, 1u } }
		, m_pickDisplayRegions{ createPickDisplayRegions() }
		, m_commandBuffer{ m_device.graphicsCommandPool->createCommandBuffer( "PickingPass" ) }
		, m_transferFence{ m_device->createFence( "PickingPass" ) }
		, m_stagingBuffer{ makeBuffer< castor::Point4f >( m_device
			, PickingWidth * PickingWidth
			, ( VK_BUFFER_USAGE_TRANSFER_DST_BIT
				| VK_BUFFER_USAGE_TRANSFER_SRC_BIT )
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, "PickingPassStagingBuffer" ) }
		, m_buffer{ PickingWidth * PickingWidth }
	{
		m_runnable = m_graph.compile( device.makeContext() );
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
		m_scenes.clear();
		m_stagingBuffer.reset();
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
			position = convertToTopDown( position, m_size );
			m_pickNodeType = PickNodeType::eNone;
			m_geometry.reset();
			m_submesh.reset();
			m_face = 0u;
			auto & myCamera = m_pickingPass->getCuller().getCamera();
			int32_t offsetX = std::clamp( position.x() - PickingOffset
				, 0
				, int32_t( m_size.getWidth() - PickingWidth ) );
			int32_t offsetY = std::clamp( position.y() - PickingOffset
				, 0
				, int32_t( m_size.getHeight() - PickingWidth ) );
			VkRect2D scissor =
			{
				{ offsetX, offsetY },
				{ PickingWidth, PickingWidth },
			};

			if ( m_pickingPass->updateNodes( scissor ) )
			{
				auto pixel = doFboPick( position, myCamera );
				m_pickNodeType = doPick( pixel, m_pickingPass->getCulledRenderNodes() );
			}

			m_picking = false;
		}

		return m_pickNodeType;
	}

	crg::FramePass Picking::doCreatePickingPass( MatrixUbo & matrixUbo
		, SceneCuller & culler )
	{
		auto & result = m_graph.createPass( "PickingPass"
			, [this, &matrixUbo, &culler]( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = std::make_unique< PickingPass >( pass
					, context
					, graph
					, m_device
					, m_size
					, matrixUbo
					, culler );
				m_pickingPass = result.get();
				return result;
			} );
		result.addOutputDepthView( m_depthImageView
			, defaultClearDepthStencil );
		result.addOutputColourView( m_colourImageView
			, makeClearValue( 1.0f, 0.5f, 0.5f, 1.0f ) );
		return result;
	}

	castor::Point4f Picking::doFboPick( castor::Position const & position
		, Camera const & camera )
	{
		auto toWait = m_runnable->run( crg::SemaphoreWait{}, *m_device.graphicsQueue );

		m_copyRegion.imageOffset.x = std::clamp( position.x() - PickingOffset
			, 0
			, int32_t( m_colourImage.data->info.extent.width - PickingWidth ) );
		m_copyRegion.imageOffset.y = std::clamp( position.y() - PickingOffset
			, 0
			, int32_t( m_colourImage.data->info.extent.height - PickingWidth ) );

#if !C3D_DebugPicking
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
			, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
			, m_colourView.makeShaderInputResource( layout ) );
		m_commandBuffer->endDebugBlock();
#endif

		m_commandBuffer->end();

		m_device.graphicsQueue->submit( *m_commandBuffer
			, toWait.semaphore
			, toWait.dstStageMask
			, {}
			, *m_transferFence );
		m_transferFence->wait( ashes::MaxTimeout );
		m_transferFence->reset();

#if !C3D_DebugPicking
		if ( auto * data = m_stagingBuffer->lock( 0u, m_stagingBuffer->getCount(), 0u ) )
		{
			std::copy( data, data + m_stagingBuffer->getCount(), m_buffer.begin() );
			m_stagingBuffer->unlock();
		}
#endif

		auto & result = *( m_buffer.begin() + BufferOffset );
		log::trace << cuT( "Picked" )
			<< cuT( ": " ) << result->x
			<< cuT( ", " ) << result->y
			<< cuT( ", " ) << result->z
			<< cuT( ", " ) << result->w
			<< cuT( ", at " ) << position[0]
			<< cuT( "x" ) << position[1] << std::endl;
		return result;
	}

	PickNodeType Picking::doPick( castor::Point4f const & pixel
		, QueueCulledRenderNodes const & nodes )
	{
		PickNodeType result{ PickNodeType::eNone };

		if ( castor::point::lengthSquared( castor::Point3f{ pixel } ) )
		{
			result = PickNodeType( uint32_t( pixel[0] ) & 0xFF );

			switch ( result )
			{
			case PickNodeType::eStatic:
				pickFromList( nodes.staticNodes.backCulled, pixel, m_geometry, m_submesh, m_face );
				break;

			case PickNodeType::eInstantiatedStatic:
				pickFromInstantiatedList( nodes.instancedStaticNodes.backCulled, pixel, m_geometry, m_submesh, m_face );
				break;

			case PickNodeType::eSkinning:
				pickFromList( nodes.skinnedNodes.backCulled, pixel, m_geometry, m_submesh, m_face );
				break;

			case PickNodeType::eInstantiatedSkinning:
				pickFromInstantiatedList( nodes.instancedSkinnedNodes.backCulled, pixel, m_geometry, m_submesh, m_face );
				break;

			case PickNodeType::eMorphing:
				pickFromList( nodes.morphingNodes.backCulled, pixel, m_geometry, m_submesh, m_face );
				break;

			case PickNodeType::eBillboard:
				pickFromList( nodes.billboardNodes.backCulled, pixel, m_billboard, m_billboard, m_face );
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
