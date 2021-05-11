#include "Castor3D/Render/PickingPass.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/PoolUniformBuffer.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Cache/GeometryCache.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Culling/SceneCuller.hpp"
#include "Castor3D/Render/Node/QueueCulledRenderNodes.hpp"
#include "Castor3D/Render/Node/BillboardRenderNode.hpp"
#include "Castor3D/Render/Node/SubmeshRenderNode.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Shader/ShaderModule.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/BillboardUbo.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelUbo.hpp"
#include "Castor3D/Shader/Ubos/MorphingUbo.hpp"
#include "Castor3D/Shader/Ubos/PickingUbo.hpp"
#include "Castor3D/Shader/Ubos/SkinningUbo.hpp"

#include <CastorUtils/Graphics/RgbaColour.hpp>

#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Image/Image.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/Sync/Fence.hpp>

#include <ShaderWriter/Source.hpp>

CU_ImplementCUSmartPtr( castor3d, PickingPass )

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		static std::string const Picking = "Picking";
		static std::string const DrawIndex = "c3d_iDrawIndex";
		static std::string const NodeIndex = "c3d_iNodeIndex";

		static int constexpr PickingOffset = int( PickingPass::PickingWidth / 2 );
		static int constexpr BufferOffset = ( PickingOffset * PickingPass::PickingWidth ) + PickingOffset - 1;

		castor::Position convertToTopDown( castor::Position const & position
			, castor::Size const & size )
		{
			return
			{
				position.x(),
				int32_t( size.getHeight() - position.y() )
			};
		}

		template< typename NodeT, typename FuncT >
		inline void traverseNodes( PickingPass & pass
			, std::unordered_map< SubmeshRenderNode const *, UniformBufferOffsetT< PickingUboConfiguration > > & ubos
			, ObjectNodesPtrByPipelineMapT< NodeT > & nodes
			, PickNodeType type
			, FuncT function )
		{
			uint32_t count{ 1u };

			for ( auto itPipelines : nodes )
			{
				pass.updatePipeline( *itPipelines.first );
				auto drawIndex = uint8_t( type ) + ( ( count & 0x00FFFFFF ) << 8 );
				uint32_t index{ 0u };

				for ( auto itPass : itPipelines.second )
				{
					for ( auto itSubmeshes : itPass.second )
					{
						if ( !itSubmeshes.second.empty() )
						{
							auto renderNode = itSubmeshes.second.front();
							auto it = ubos.find( renderNode );
							CU_Require( it != ubos.end() );
							PickingUbo::update( it->second.getData()
								, drawIndex
								, index++ );
							function( *itPipelines.first
								, *itPass.first
								, *itSubmeshes.first
								, itSubmeshes.first->getInstantiation()
								, itSubmeshes.second );
						}
					}
				}

				count++;
			}
		}

		template< typename NodeT >
		inline void updateNonInstanced( PickingPass & pass
			, std::unordered_map< NodeT const *, UniformBufferOffsetT< PickingUboConfiguration > > & ubos
			, PickNodeType type
			, NodePtrByPipelineMapT< NodeT > & nodes )
		{
			uint32_t count{ 1u };

			for ( auto itPipelines : nodes )
			{
				pass.updatePipeline( *itPipelines.first );
				auto drawIndex = uint8_t( type ) + ( ( count & 0x00FFFFFF ) << 8 );
				uint32_t index{ 0u };

				for ( auto & renderNode : itPipelines.second )
				{
					auto it = ubos.find( renderNode );
					CU_Require( it != ubos.end() );
					PickingUbo::update( it->second.getData()
						, drawIndex
						, index++ );
				}

				count++;
			}
		}

		template< typename MapType, typename NodeType, typename SubNodeType >
		inline void pickFromList( MapType const & map
			, Point4f const & index
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
			, Point4f const & index
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

		ashes::ImagePtr createTexture( RenderDevice const & device
			, Size const & size
			, VkFormat format
			, VkImageUsageFlags usage
			, VkMemoryPropertyFlagBits memory
			, std::string const & name )
		{
			ashes::ImageCreateInfo image
			{
				0u,
				VK_IMAGE_TYPE_2D,
				format,
				{ size[0], size[1], 1u },
				1u,
				1u,
				VK_SAMPLE_COUNT_1_BIT,
				VK_IMAGE_TILING_OPTIMAL,
				usage,
			};
			return makeImage( device
				, std::move( image )
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, "PickingPass" + name );
		}

		ashes::ImageView createView( RenderDevice const & device
			, ashes::Image const & texture
			, std::string const & name )
		{
			ashes::ImageViewCreateInfo view
			{
				0u,
				texture,
				VK_IMAGE_VIEW_TYPE_2D,
				texture.getFormat(),
				VkComponentMapping{},
				VkImageSubresourceRange
				{
					ashes::getAspectMask( texture.getFormat() ),
					0u,
					1u,
					0u,
					1u
				}
			};
			auto result = texture.createView( "PickingPass" + name + "View"
				, view );
			return result;
		}

		ashes::RenderPassPtr createRenderPass( RenderDevice const & device
			, VkFormat colourFormat
			, VkFormat depthFormat )
		{
			// Create the render pass.
			ashes::VkAttachmentDescriptionArray attaches
			{
				{ 0u
					, colourFormat
					, VK_SAMPLE_COUNT_1_BIT
					, VK_ATTACHMENT_LOAD_OP_CLEAR
					, VK_ATTACHMENT_STORE_OP_STORE
					, VK_ATTACHMENT_LOAD_OP_DONT_CARE
					, VK_ATTACHMENT_STORE_OP_DONT_CARE
					, VK_IMAGE_LAYOUT_UNDEFINED
#if C3D_DebugPicking
					, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL }
#else
					, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL }
#endif
				, { 0u
					, depthFormat
					, VK_SAMPLE_COUNT_1_BIT
					, VK_ATTACHMENT_LOAD_OP_CLEAR
					, VK_ATTACHMENT_STORE_OP_STORE
					, VK_ATTACHMENT_LOAD_OP_DONT_CARE
					, VK_ATTACHMENT_STORE_OP_DONT_CARE
					, VK_IMAGE_LAYOUT_UNDEFINED
					, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL } };
			ashes::SubpassDescriptionArray subpasses;
			subpasses.emplace_back( ashes::SubpassDescription{ 0u
				, VK_PIPELINE_BIND_POINT_GRAPHICS
				, {}
				, { { 0u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } }
				, {}
				, VkAttachmentReference{ 1u, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL }
				, {} } );
			ashes::VkSubpassDependencyArray dependencies
			{
				{ VK_SUBPASS_EXTERNAL
					, 0u
					, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT
					, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
					, VK_ACCESS_MEMORY_READ_BIT
					, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
					, VK_DEPENDENCY_BY_REGION_BIT }
				, { 0u
					, VK_SUBPASS_EXTERNAL
					, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
					, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
					, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
					, VK_ACCESS_SHADER_READ_BIT
					, VK_DEPENDENCY_BY_REGION_BIT } };
			ashes::RenderPassCreateInfo createInfo{ 0u
				, std::move( attaches )
				, std::move( subpasses )
				, std::move( dependencies ) };
			return device->createRenderPass( "PickingPass"
				, std::move( createInfo ) );
		}

		ashes::FrameBufferPtr createFramebuffer( ashes::RenderPass const & renderPass
			, Size const & size
			, ashes::ImageView const & colourView
			, ashes::ImageView const & depthView )
		{
			ashes::ImageViewCRefArray attachments;
			attachments.emplace_back( colourView );
			attachments.emplace_back( depthView );
			return renderPass.createFrameBuffer( "PickingPass"
				, { size.getWidth(), size.getHeight() }
				, std::move( attachments ) );
		}

		std::vector< VkBufferImageCopy > createPickDisplayRegions()
		{
			std::vector< VkBufferImageCopy > result;

			for ( int i = 0; i < PickingPass::PickingWidth; ++i )
			{
				for ( int j = 0; j < PickingPass::PickingWidth; ++j )
				{
					result.push_back( { BufferOffset * ashes::getMinimalSize( VK_FORMAT_R32G32B32A32_SFLOAT )
						, 0u
						, 0u
						, { VK_IMAGE_ASPECT_COLOR_BIT, 0u, 0u, 1u }
						, { int( PickingPass::PickingWidth ) * 2 + i, j, 0 }
						, { 1u, 1u, 1u } } );
				}
			}

			return result;
		}
	}

	//*********************************************************************************************

	uint32_t const PickingPass::UboBindingPoint = 7u;

	PickingPass::PickingPass( RenderDevice const & device
		, castor::Size const & size
		, MatrixUbo & matrixUbo
		, SceneCuller & culler )
		: SceneRenderPass{ device
			, cuT( "Picking" )
			, cuT( "Picking" )
			, SceneRenderPassDesc{ { size.getWidth(), size.getHeight(), 1u }, matrixUbo, culler, RenderMode::eBoth, true, false }
			, createRenderPass( device, VK_FORMAT_R32G32B32A32_SFLOAT, VK_FORMAT_D32_SFLOAT ) }
		, m_colourTexture{ createTexture( m_device
			, m_size
			, VK_FORMAT_R32G32B32A32_SFLOAT
			, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_TRANSFER_SRC_BIT
				| VK_IMAGE_USAGE_TRANSFER_DST_BIT
				| VK_IMAGE_USAGE_SAMPLED_BIT )
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, "Colour" ) }
		, m_depthTexture{ createTexture( m_device
			, m_size
			, VK_FORMAT_D32_SFLOAT
			, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, "Depth" ) }
		, m_colourView{ createView( m_device
			, *m_colourTexture
			, "Colour" ) }
		, m_depthView{ createView( m_device
			, *m_depthTexture
			, "Depth" ) }
		, m_frameBuffer{ createFramebuffer( *m_renderPass
			, m_size
			, m_colourView
			, m_depthView ) }
		, m_copyRegion{ 0u
			, 0u
			, 0u
			, { m_colourView->subresourceRange.aspectMask, 0u, 0u, 1u }
			, { 0, 0, 0 }
			, { PickingWidth, PickingWidth, 1u } }
		, m_transferDisplayRegion{ 0u
			, 0u
			, 0u
			, { m_colourView->subresourceRange.aspectMask, 0u, 0u, 1u }
			, { 0, 0, 0 }
			, { PickingWidth, PickingWidth, 1u } }
		, m_pickDisplayRegions{ createPickDisplayRegions() }
		, m_commandBuffer{ m_device.graphicsCommandPool->createCommandBuffer( "PickingPass" ) }
		, m_transferFence{ m_device->createFence( "PickingPass" ) }
		, m_stagingBuffer{ makeBuffer< Point4f >( m_device
			, PickingWidth * PickingWidth
			, ( VK_BUFFER_USAGE_TRANSFER_DST_BIT
				| VK_BUFFER_USAGE_TRANSFER_SRC_BIT )
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, "PickingPassStagingBuffer" ) }
		, m_buffer{ PickingWidth * PickingWidth }
	{
	}

	PickingPass::~PickingPass()
	{
		m_commandBuffer.reset();

		for ( auto & ubo : m_submeshBuffers )
		{
			m_device.uboPools->putBuffer( ubo.second );
		}

		for ( auto & ubo : m_billboardBuffers )
		{
			m_device.uboPools->putBuffer( ubo.second );
		}

		m_submeshBuffers.clear();
		m_billboardBuffers.clear();
		m_scenes.clear();
		m_stagingBuffer.reset();
		m_frameBuffer.reset();
		m_depthTexture.reset();
		m_colourTexture.reset();
	}

	void PickingPass::addScene( Scene & scene, Camera & camera )
	{
	}

	PickNodeType PickingPass::pick( RenderDevice const & device
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
			auto & myCamera = getCuller().getCamera();
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

			ShadowMapLightTypeArray shadowMaps;
			m_renderQueue.update( shadowMaps, scissor );

			if ( m_renderQueue.getCulledRenderNodes().hasNodes() )
			{
				doUpdateNodes( m_renderQueue.getCulledRenderNodes() );
				auto pixel = doFboPick( device, position, myCamera, m_renderQueue.getCommandBuffer() );
				m_pickNodeType = doPick( pixel, m_renderQueue.getCulledRenderNodes() );
			}

			m_picking = false;
		}

		return m_pickNodeType;
	}

	bool PickingPass::doIsValidPass( PassFlags const & passFlags )const
	{
		if ( !checkFlag( passFlags, PassFlag::ePickable ) )
		{
			return false;
		}

		return SceneRenderPass::doIsValidPass( passFlags );
	}

	void PickingPass::doUpdateNodes( QueueCulledRenderNodes & nodes )
	{
		auto & myCamera = getCuller().getCamera();
		auto & myScene = getCuller().getScene();
		m_matrixUbo.cpuUpdate( myCamera.getView()
			, myCamera.getProjection() );
		doUpdate( nodes.instancedStaticNodes.backCulled );
		doUpdate( nodes.staticNodes.backCulled );
		doUpdate( nodes.skinnedNodes.backCulled );
		doUpdate( nodes.instancedSkinnedNodes.backCulled );
		doUpdate( nodes.morphingNodes.backCulled );
		doUpdate( nodes.billboardNodes.backCulled );
	}

	Point4f PickingPass::doFboPick( RenderDevice const & device
		, Position const & position
		, Camera const & camera
		, ashes::CommandBuffer const & commandBuffer )
	{
		static ashes::VkClearValueArray clearValues
		{
			makeClearValue( 1.0f, 0.5f, 0.5f, 1.0f ),
			//transparentBlackClearColor,
			defaultClearDepthStencil,
		};

		m_commandBuffer->begin();
		m_commandBuffer->beginDebugBlock(
			{
				"PickingPass Render",
				makeFloatArray( getEngine()->getNextRainbowColour() ),
			} );
		m_commandBuffer->beginRenderPass( *m_renderPass
			, *m_frameBuffer
			, clearValues
			, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS );
		m_commandBuffer->executeCommands( { commandBuffer } );
		m_commandBuffer->endRenderPass();
		m_commandBuffer->endDebugBlock();

		m_copyRegion.imageOffset.x = std::clamp( position.x() - PickingOffset
			, 0
			, int32_t( m_colourTexture->getDimensions().width - PickingWidth ) );
		m_copyRegion.imageOffset.y = std::clamp( position.y() - PickingOffset
			, 0
			, int32_t( m_colourTexture->getDimensions().height - PickingWidth ) );

#if !C3D_DebugPicking
		m_commandBuffer->beginDebugBlock(
			{
				"PickingPass Copy",
				makeFloatArray( getEngine()->getNextRainbowColour() ),
			} );
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

		device.graphicsQueue->submit( *m_commandBuffer, m_transferFence.get() );
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

	PickNodeType PickingPass::doPick( Point4f const & pixel
		, QueueCulledRenderNodes & nodes )
	{
		PickNodeType result{ PickNodeType::eNone };

		if ( castor::point::lengthSquared( Point3f{ pixel } ) )
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

	void PickingPass::doUpdate( SubmeshRenderNodePtrByPipelineMap & nodes )
	{
		updateNonInstanced( *this
			, m_submeshBuffers
			, PickNodeType::eStatic
			, nodes );
	}

	void PickingPass::doUpdate( BillboardRenderNodePtrByPipelineMap & nodes )
	{
		updateNonInstanced( *this
			, m_billboardBuffers
			, PickNodeType::eBillboard
			, nodes );
	}

	void PickingPass::doUpdate( SubmeshRenderNodesPtrByPipelineMap & nodes )
	{
		traverseNodes( *this
			, m_submeshBuffers
			, nodes
			, PickNodeType::eInstantiatedStatic
			, [this]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, InstantiationComponent & component
				, SubmeshRenderNodePtrArray & renderNodes )
			{
				auto it = component.find( pass.getOwner()->shared_from_this() );

				if ( it != component.end()
					&& it->second[0].buffer )
				{
					doCopyNodesMatrices( renderNodes
						, it->second[0].data );

					if ( renderNodes.front()->skeleton )
					{
						auto & instantiatedBones = submesh.getInstantiatedBones();

						if ( instantiatedBones.hasInstancedBonesBuffer() )
						{
							doCopyNodesBones( renderNodes, instantiatedBones.getInstancedBonesBuffer() );
						}
					}
				}
			} );
	}

	void PickingPass::doFillAdditionalBindings( PipelineFlags const & flags
		, ashes::VkDescriptorSetLayoutBindingArray & bindings )const
	{
		bindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( PassUboIdx::eCount )
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
	}

	void PickingPass::doFillAdditionalDescriptor( RenderPipeline const & pipeline
		, ashes::WriteDescriptorSetArray & descriptorWrites
		, BillboardRenderNode & node
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		auto ires = m_billboardBuffers.insert( { &node, {} } );

		if ( ires.second )
		{
			ires.first->second = m_device.uboPools->getBuffer< PickingUboConfiguration >( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
		}

		descriptorWrites.push_back( ires.first->second.getDescriptorWrite( uint32_t( PassUboIdx::eCount ) ) );
	}

	void PickingPass::doFillAdditionalDescriptor( RenderPipeline const & pipeline
		, ashes::WriteDescriptorSetArray & descriptorWrites
		, SubmeshRenderNode & node
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		auto ires = m_submeshBuffers.insert( { &node, {} } );

		if ( ires.second )
		{
			ires.first->second = m_device.uboPools->getBuffer< PickingUboConfiguration >( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
		}

		descriptorWrites.push_back( ires.first->second.getDescriptorWrite( uint32_t( PassUboIdx::eCount ) ) );
	}

	void PickingPass::doUpdate( RenderQueueArray & CU_UnusedParam( queues ) )
	{
	}

	ShaderPtr PickingPass::doGetVertexShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		VertexWriter writer;
		bool hasTextures = !flags.textures.empty();

		// Vertex inputs
		shader::VertexSurface inSurface{ writer
			, flags.programFlags
			, getShaderFlags()
			, hasTextures };
		auto in = writer.getIn();

		UBO_MODEL( writer
			, uint32_t( NodeUboIdx::eModel )
			, RenderPipeline::eBuffers );
		auto skinningData = SkinningUbo::declare( writer
			, uint32_t( NodeUboIdx::eSkinningUbo )
			, uint32_t( NodeUboIdx::eSkinningSsbo )
			, RenderPipeline::eBuffers
			, flags.programFlags );
		UBO_MORPHING( writer
			, uint32_t( NodeUboIdx::eMorphing )
			, RenderPipeline::eBuffers
			, flags.programFlags );

		UBO_MATRIX( writer
			, uint32_t( PassUboIdx::eMatrix )
			, RenderPipeline::eAdditional );

		// Outputs
		shader::OutFragmentSurface outSurface{ writer
			, getShaderFlags()
			, hasTextures };
		auto out = writer.getOut();

		writer.implementFunction< sdw::Void >( "main"
			,[&] ()
			{
				auto curPosition = writer.declLocale( "curPosition"
					, inSurface.position );
				outSurface.texture = inSurface.texture;
				inSurface.morph( c3d_morphingData
					, curPosition
					, outSurface.texture );
				outSurface.material = c3d_modelData.getMaterialIndex( flags.programFlags
					, inSurface.material );
				outSurface.instance = writer.cast< UInt >( in.instanceIndex );

				auto mtxModel = writer.declLocale< Mat4 >( "mtxModel"
					, c3d_modelData.getCurModelMtx( flags.programFlags, skinningData, inSurface ) );
				curPosition = mtxModel * curPosition;
				out.vtx.position = c3d_matrixData.worldToCurProj( curPosition );
			} );
		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr PickingPass::doGetGeometryShaderSource( PipelineFlags const & flags )const
	{
		return ShaderPtr{};
	}

	ShaderPtr PickingPass::doGetPhongPixelShaderSource( PipelineFlags const & flags )const
	{
		return doGetPixelShaderSource( flags );
	}

	ShaderPtr PickingPass::doGetPbrMRPixelShaderSource( PipelineFlags const & flags )const
	{
		return doGetPixelShaderSource( flags );
	}

	ShaderPtr PickingPass::doGetPbrSGPixelShaderSource( PipelineFlags const & flags )const
	{
		return doGetPixelShaderSource( flags );
	}

	ShaderPtr PickingPass::doGetPixelShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		FragmentWriter writer;
		auto textures = filterTexturesFlags( flags.textures );

		// UBOs
		auto & renderSystem = *getEngine()->getRenderSystem();
		auto materials = shader::createMaterials( writer, flags.passFlags );
		materials->declare( renderSystem.getGpuInformations().hasShaderStorageBuffers()
			, uint32_t( NodeUboIdx::eMaterials )
			, RenderPipeline::eBuffers );
		shader::TextureConfigurations textureConfigs{ writer };
		bool hasTextures = !flags.textures.empty();

		if ( hasTextures )
		{
			textureConfigs.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers()
				, uint32_t( NodeUboIdx::eTextures )
				, RenderPipeline::eBuffers );
		}

		UBO_PICKING( writer
			, uint32_t( PassUboIdx::eCount )
			, RenderPipeline::eAdditional );

		// Fragment Intputs
		shader::InFragmentSurface inSurface{ writer
			, getShaderFlags()
			, hasTextures };
		auto in = writer.getIn();
		auto c3d_maps( writer.declSampledImageArray< FImg2DRgba32 >( "c3d_maps"
			, 0u
			, RenderPipeline::eTextures
			, std::max( 1u, uint32_t( flags.textures.size() ) )
			, hasTextures ) );

		// Fragment Outputs
		auto pxl_fragColor( writer.declOutput< Vec4 >( "pxl_fragColor", 0 ) );
		shader::Utils utils{ writer };

		writer.implementFunction< sdw::Void >( "main"
			, [&]()
			{
				auto material = materials->getBaseMaterial( inSurface.material );
				auto alpha = writer.declLocale( "alpha"
					, material->m_opacity );
				utils.computeOpacityMapContribution( textures
					, textureConfigs
					, c3d_maps
					, inSurface.texture
					, alpha );
				utils.applyAlphaFunc( flags.alphaFunc
					, alpha
					, material->m_alphaRef );
				pxl_fragColor = c3d_pickingData.getIndex( inSurface.instance );
#if C3D_DebugPicking
				pxl_fragColor /= 255.0_f;
#endif
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	TextureFlags PickingPass::getTexturesMask()const
	{
		return TextureFlags{ TextureFlag::eOpacity };
	}

	void PickingPass::doUpdateFlags( PipelineFlags & flags )const
	{
		remFlag( flags.programFlags, ProgramFlag::eLighting );
		remFlag( flags.passFlags, PassFlag::eAlphaBlending );
		addFlag( flags.programFlags, ProgramFlag::ePicking );
	}

	void PickingPass::doUpdatePipeline( RenderPipeline & pipeline )
	{
	}

	ashes::PipelineDepthStencilStateCreateInfo PickingPass::doCreateDepthStencilState( PipelineFlags const & flags )const
	{
		return ashes::PipelineDepthStencilStateCreateInfo{ 0u, true, true };
	}

	ashes::PipelineColorBlendStateCreateInfo PickingPass::doCreateBlendState( PipelineFlags const & flags )const
	{
		return SceneRenderPass::createBlendState( BlendMode::eNoBlend, BlendMode::eNoBlend, 1u );
	}

	//*********************************************************************************************
}
