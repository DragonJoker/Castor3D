#include "Castor3D/Render/PickingPass.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/PoolUniformBuffer.hpp"
#include "Castor3D/Cache/GeometryCache.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Node/SceneCulledRenderNodes.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Shader/ShaderModule.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"
#include "Castor3D/Shader/Ubos/BillboardUbo.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelMatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelUbo.hpp"
#include "Castor3D/Shader/Ubos/MorphingUbo.hpp"
#include "Castor3D/Shader/Ubos/PickingUbo.hpp"
#include "Castor3D/Shader/Ubos/SkinningUbo.hpp"
#include "Castor3D/Shader/Ubos/TexturesUbo.hpp"

#include <CastorUtils/Graphics/RgbaColour.hpp>

#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Image/Image.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/Sync/Fence.hpp>

#include <ShaderWriter/Source.hpp>

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		static std::string const Picking = "Picking";
		static std::string const DrawIndex = "c3d_iDrawIndex";
		static std::string const NodeIndex = "c3d_iNodeIndex";

		castor::Position convertToBottomUp( Position const & position
			, castor::Size const & size )
		{
			return
			{
				position.x(),
				int32_t( size.getHeight() - position.y() )
			};
		}

		template< bool Opaque, typename MapType, typename FuncType >
		inline void doTraverseNodes( RenderPass & pass
			, MapType & nodes
			, PickNodeType type
			, FuncType function )
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
							PickingUbo::update( itSubmeshes.second[0]->pickingUbo.getData()
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

		template< bool Opaque, typename MapType >
		inline void doUpdateNonInstanced( RenderPass & pass
			, PickNodeType type
			, MapType & nodes )
		{
			uint32_t count{ 1u };

			for ( auto itPipelines : nodes )
			{
				pass.updatePipeline( *itPipelines.first );
				auto drawIndex = uint8_t( type ) + ( ( count & 0x00FFFFFF ) << 8 );
				uint32_t index{ 0u };

				for ( auto & renderNode : itPipelines.second )
				{
					PickingUbo::update( renderNode->pickingUbo.getData()
						, drawIndex
						, index++ );
				}

				count++;
			}
		}

		template< typename MapType, typename NodeType, typename SubNodeType >
		inline void doPickFromList( MapType const & map
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
		inline void doPickFromInstantiatedList( MapType const & map
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

		static int constexpr PickingOffset = int( PickingPass::PickingWidth / 2 );
		static int constexpr BufferOffset = ( PickingOffset * PickingPass::PickingWidth ) + PickingOffset - 1;
	}

	//*********************************************************************************************

	uint32_t const PickingPass::UboBindingPoint = 7u;

	PickingPass::PickingPass( Engine & engine
		, MatrixUbo & matrixUbo
		, SceneCuller & culler )
		: RenderPass{ cuT( "Picking" )
			, cuT( "Picking" )
			, engine
			, matrixUbo
			, culler
			, nullptr }
	{
		engine.sendEvent( makeGpuFunctorEvent( EventType::ePreRender
			, [this]( RenderDevice const & device )
			{
				m_transferFence = device->createFence( "PickingPass" );
			} ) );
	}

	PickingPass::~PickingPass()
	{
		CU_Assert( m_commandBuffer == nullptr, "Did you forget to call PickingPass::cleanup ?" );
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
			m_pickNodeType = PickNodeType::eNone;
			m_geometry.reset();
			m_submesh.reset();
			m_face = 0u;
			auto & myCamera = getCuller().getCamera();
			int32_t offsetX = std::clamp( position.x() - PickingOffset
				, 0
				, int32_t( m_colourTexture->getDimensions().width - PickingWidth ) );
			int32_t offsetY = std::clamp( position.y() - PickingOffset
				, 0
				, int32_t( m_colourTexture->getDimensions().height - PickingWidth ) );
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

	void PickingPass::doUpdateNodes( SceneCulledRenderNodes & nodes )
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
			<< cuT( ", " ) << result->w << std::endl;
		return result;
	}

	PickNodeType PickingPass::doPick( Point4f const & pixel
		, SceneCulledRenderNodes & nodes )
	{
		PickNodeType result{ PickNodeType::eNone };

		if ( castor::point::lengthSquared( Point3f{ pixel } ) )
		{
			result = PickNodeType( uint32_t( pixel[0] ) & 0xFF );

			switch ( result )
			{
			case PickNodeType::eStatic:
				doPickFromList( nodes.staticNodes.backCulled, pixel, m_geometry, m_submesh, m_face );
				break;

			case PickNodeType::eInstantiatedStatic:
				doPickFromInstantiatedList( nodes.instancedStaticNodes.backCulled, pixel, m_geometry, m_submesh, m_face );
				break;

			case PickNodeType::eSkinning:
				doPickFromList( nodes.skinnedNodes.backCulled, pixel, m_geometry, m_submesh, m_face );
				break;

			case PickNodeType::eInstantiatedSkinning:
				doPickFromInstantiatedList( nodes.instancedSkinnedNodes.backCulled, pixel, m_geometry, m_submesh, m_face );
				break;

			case PickNodeType::eMorphing:
				doPickFromList( nodes.morphingNodes.backCulled, pixel, m_geometry, m_submesh, m_face );
				break;

			case PickNodeType::eBillboard:
				doPickFromList( nodes.billboardNodes.backCulled, pixel, m_billboard, m_billboard, m_face );
				break;

			default:
				CU_Failure( "Unsupported index" );
				result = PickNodeType::eNone;
				break;
			}
		}

		return result;
	}

	void PickingPass::doUpdate( SubmeshStaticRenderNodesPtrByPipelineMap & nodes )
	{
		doTraverseNodes< true >( *this
			, nodes
			, PickNodeType::eInstantiatedStatic
			, [this]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, InstantiationComponent & component
				, StaticRenderNodePtrArray & renderNodes )
			{
				auto it = component.find( pass.getOwner()->shared_from_this() );

				if ( it != component.end()
					&& it->second.buffer )
				{
					doCopyNodesMatrices( renderNodes
						, it->second.data );
				}
			} );
	}

	void PickingPass::doUpdate( StaticRenderNodesPtrByPipelineMap & nodes )
	{
		doUpdateNonInstanced< true >( *this
			, PickNodeType::eStatic
			, nodes );
	}

	void PickingPass::doUpdate( SkinningRenderNodesPtrByPipelineMap & nodes )
	{
		doUpdateNonInstanced< true >( *this
			, PickNodeType::eSkinning
			, nodes );
	}
	
	void PickingPass::doUpdate( SubmeshSkinningRenderNodesPtrByPipelineMap & nodes )
	{
		doTraverseNodes< true >( *this
			, nodes
			, PickNodeType::eInstantiatedSkinning
			, [this]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, InstantiationComponent & component
				, SkinningRenderNodePtrArray & renderNodes )
			{
				auto & instantiatedBones = submesh.getInstantiatedBones();
				auto it = component.find( pass.getOwner()->shared_from_this() );

				if ( !renderNodes.empty()
					&& it != component.end()
					&& it->second.buffer
					&& instantiatedBones.hasInstancedBonesBuffer() )
				{
					doCopyNodesBones( renderNodes, instantiatedBones.getInstancedBonesBuffer() );
				}
			} );
	}

	void PickingPass::doUpdate( MorphingRenderNodesPtrByPipelineMap & nodes )
	{
		doUpdateNonInstanced< true >( *this
			, PickNodeType::eMorphing
			, nodes );
	}

	void PickingPass::doUpdate( BillboardRenderNodesPtrByPipelineMap & nodes )
	{
		doUpdateNonInstanced< true >( *this
			, PickNodeType::eBillboard
			, nodes );
	}

	bool PickingPass::doInitialise( RenderDevice const & device
		, Size const & size )
	{
		m_commandBuffer = device.graphicsCommandPool->createCommandBuffer( "PickingPass" );

		m_colourTexture = createTexture( device
			, size
			, VK_FORMAT_R32G32B32A32_SFLOAT
			, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_TRANSFER_SRC_BIT
				| VK_IMAGE_USAGE_TRANSFER_DST_BIT
				| VK_IMAGE_USAGE_SAMPLED_BIT
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, "Colour" );
		m_colourView = createView( device
			, *m_colourTexture
			, "Colour" );

		m_copyRegion =
		{
			0u,
			0u,
			0u,
			{ m_colourView->subresourceRange.aspectMask, 0u, 0u, 1u },
			{ 0, 0, 0 },
			{ PickingWidth, PickingWidth, 1u },
		};
		m_transferDisplayRegion =
		{
			0u,
			0u,
			0u,
			{ m_colourView->subresourceRange.aspectMask, 0u, 0u, 1u },
			{ 0, 0, 0 },
			{ PickingWidth, PickingWidth, 1u },
		};
		for ( int i = 0; i < PickingWidth; ++i )
		{
			for ( int j = 0; j < PickingWidth; ++j )
			{
				m_pickDisplayRegions.push_back(
					{
						BufferOffset * ashes::getMinimalSize( VK_FORMAT_R32G32B32A32_SFLOAT ),
						0u,
						0u,
						{ m_colourView->subresourceRange.aspectMask, 0u, 0u, 1u },
						{ int( PickingWidth ) * 2 + i, j, 0 },
						{ 1u, 1u, 1u },
					} );
			}
		}

		m_depthTexture = createTexture( device
			, size
			, VK_FORMAT_D32_SFLOAT
			, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, "Depth" );
		m_depthView = createView( device
			, *m_depthTexture
			, "Depth" );

		m_stagingBuffer = makeBuffer< Point4f >( device
			, PickingWidth * PickingWidth
			, ( VK_BUFFER_USAGE_TRANSFER_DST_BIT
				| VK_BUFFER_USAGE_TRANSFER_SRC_BIT )
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, "PickingPassStagingBuffer" );

		m_buffer.resize( PickingWidth * PickingWidth );

		// Create the render pass.
		ashes::VkAttachmentDescriptionArray attaches
		{
			{
				0u,
				m_colourTexture->getFormat(),
				VK_SAMPLE_COUNT_1_BIT,
				VK_ATTACHMENT_LOAD_OP_CLEAR,
				VK_ATTACHMENT_STORE_OP_STORE,
				VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				VK_ATTACHMENT_STORE_OP_DONT_CARE,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			},
			{
				0u,
				m_depthTexture->getFormat(),
				VK_SAMPLE_COUNT_1_BIT,
				VK_ATTACHMENT_LOAD_OP_CLEAR,
				VK_ATTACHMENT_STORE_OP_STORE,
				VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				VK_ATTACHMENT_STORE_OP_DONT_CARE,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			},
		};
		ashes::SubpassDescriptionArray subpasses;
		subpasses.emplace_back( ashes::SubpassDescription
			{
				0u,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				{},
				{ { 0u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } },
				{},
				VkAttachmentReference{ 1u, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL },
				{},
			} );
		ashes::VkSubpassDependencyArray dependencies
		{
			{
				VK_SUBPASS_EXTERNAL,
				0u,
				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				VK_ACCESS_MEMORY_READ_BIT,
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				VK_DEPENDENCY_BY_REGION_BIT,
			},
			{
				0u,
				VK_SUBPASS_EXTERNAL,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				VK_ACCESS_SHADER_READ_BIT,
				VK_DEPENDENCY_BY_REGION_BIT,
			}
		};
		ashes::RenderPassCreateInfo createInfo
		{
			0u,
			std::move( attaches ),
			std::move( subpasses ),
			std::move( dependencies ),
		};
		m_renderPass = device->createRenderPass( "PickingPass"
			, std::move( createInfo ) );

		ashes::ImageViewCRefArray attachments;
		attachments.emplace_back( m_colourView );
		attachments.emplace_back( m_depthView );
		m_frameBuffer = m_renderPass->createFrameBuffer( "PickingPass"
			, { size.getWidth(), size.getHeight() }
			, std::move( attachments ) );

		return true;
	}

	void PickingPass::doCleanup( RenderDevice const & device )
	{
		m_commandBuffer.reset();
		m_scenes.clear();
		m_stagingBuffer.reset();
		m_frameBuffer.reset();
		m_depthTexture.reset();
		m_colourTexture.reset();
	}

	void PickingPass::doFillUboDescriptor( ashes::DescriptorSetLayout const & layout
		, BillboardListRenderNode & node )
	{
		node.pickingUbo.createSizedBinding( *node.uboDescriptorSet
			, layout.getBinding( PickingUbo::BindingPoint ) );
	}

	void PickingPass::doFillUboDescriptor( ashes::DescriptorSetLayout const & layout
		, SubmeshRenderNode & node )
	{
		node.pickingUbo.createSizedBinding( *node.uboDescriptorSet
			, layout.getBinding( PickingUbo::BindingPoint ) );
	}

	void PickingPass::doFillTextureDescriptor( ashes::DescriptorSetLayout const & layout
		, uint32_t & index
		, BillboardListRenderNode & node
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		node.passNode.fillDescriptor( layout
			, index
			, *node.texDescriptorSet
			, node.pipeline.getFlags().textures );
	}

	void PickingPass::doFillTextureDescriptor( ashes::DescriptorSetLayout const & layout
		, uint32_t & index
		, SubmeshRenderNode & node
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		node.passNode.fillDescriptor( layout
			, index
			, *node.texDescriptorSet
			, node.pipeline.getFlags().textures );
	}

	void PickingPass::doUpdate( RenderQueueArray & CU_UnusedParam( queues ) )
	{
	}

	ShaderPtr PickingPass::doGetVertexShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		VertexWriter writer;
		// Vertex inputs
		auto inPosition = writer.declInput< Vec4 >( "inPosition"
			, RenderPass::VertexInputs::PositionLocation );
		auto inTexture = writer.declInput< Vec3 >( "inTexture"
			, RenderPass::VertexInputs::TextureLocation );
		auto inBoneIds0 = writer.declInput< IVec4 >( "inBoneIds0"
			, RenderPass::VertexInputs::BoneIds0Location
			, checkFlag( flags.programFlags, ProgramFlag::eSkinning ) );
		auto inBoneIds1 = writer.declInput< IVec4 >( "inBoneIds1"
			, RenderPass::VertexInputs::BoneIds1Location
			, checkFlag( flags.programFlags, ProgramFlag::eSkinning ) );
		auto inWeights0 = writer.declInput< Vec4 >( "inWeights0"
			, RenderPass::VertexInputs::Weights0Location
			, checkFlag( flags.programFlags, ProgramFlag::eSkinning ) );
		auto inWeights1 = writer.declInput< Vec4 >( "inWeights1"
			, RenderPass::VertexInputs::Weights1Location
			, checkFlag( flags.programFlags, ProgramFlag::eSkinning ) );
		auto inTransform = writer.declInput< Mat4 >( "inTransform"
			, RenderPass::VertexInputs::TransformLocation
			, checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) );
		auto inMaterial = writer.declInput< Int >( "inMaterial"
			, RenderPass::VertexInputs::MaterialLocation
			, checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) );
		auto inPosition2 = writer.declInput< Vec4 >( "inPosition2"
			, RenderPass::VertexInputs::Position2Location
			, checkFlag( flags.programFlags, ProgramFlag::eMorphing ) );
		auto inTexture2 = writer.declInput< Vec3 >( "inTexture2"
			, RenderPass::VertexInputs::Texture2Location
			, checkFlag( flags.programFlags, ProgramFlag::eMorphing ) );
		auto in = writer.getIn();

		UBO_MATRIX( writer, MatrixUbo::BindingPoint, 0 );
		UBO_MODEL_MATRIX( writer, ModelMatrixUbo::BindingPoint, 0 );
		UBO_MODEL( writer, ModelUbo::BindingPoint, 0 );
		auto skinningData = SkinningUbo::declare( writer, SkinningUbo::BindingPoint, 0, flags.programFlags );
		UBO_MORPHING( writer, MorphingUbo::BindingPoint, 0, flags.programFlags );

		// Outputs
		auto outTexture = writer.declOutput< Vec3 >( "outTexture"
			, RenderPass::VertexOutputs::TextureLocation );
		auto outInstance = writer.declOutput< UInt >( "outInstance"
			, RenderPass::VertexOutputs::InstanceLocation );
		auto outMaterial = writer.declOutput< UInt >( "outMaterial"
			, RenderPass::VertexOutputs::MaterialLocation );
		auto out = writer.getOut();

		writer.implementFunction< sdw::Void >( "main"
			,[&] ()
			{
				auto v4Vertex = writer.declLocale( "v4Vertex"
					, vec4( inPosition.xyz(), 1.0_f ) );
				auto v3Texture = writer.declLocale( "v3Texture"
					, inTexture );
				auto mtxModel = writer.declLocale< Mat4 >( "mtxModel" );

				if ( checkFlag( flags.programFlags, ProgramFlag::eSkinning ) )
				{
					mtxModel = SkinningUbo::computeTransform( skinningData, writer, flags.programFlags );
				}
				else if ( checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) )
				{
					mtxModel = inTransform;
				}
				else
				{
					mtxModel = c3d_curMtxModel;
				}

				if ( checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) )
				{
					outMaterial = writer.cast< UInt >( inMaterial );
				}
				else
				{
					outMaterial = writer.cast< UInt >( c3d_materialIndex );
				}

				if ( checkFlag( flags.programFlags, ProgramFlag::eMorphing ) )
				{
					auto time = writer.declLocale( "time"
						, vec3( 1.0_f - c3d_time ) );
					v4Vertex = vec4( sdw::fma( v4Vertex.xyz(), time, inPosition2.xyz() * c3d_time ), 1.0_f );
					v3Texture = sdw::fma( v3Texture, time, inTexture2 * c3d_time );
				}

				outTexture = v3Texture;
				v4Vertex = mtxModel * v4Vertex;
				v4Vertex = c3d_curView * v4Vertex;
				outInstance = writer.cast< UInt >( in.instanceIndex );
				out.vtx.position = c3d_projection * v4Vertex;
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

		// UBOs
		auto & renderSystem = *getEngine()->getRenderSystem();
		auto materials = shader::createMaterials( writer, flags.passFlags );
		materials->declare( renderSystem.getGpuInformations().hasShaderStorageBuffers() );
		shader::TextureConfigurations textureConfigs{ writer };
		bool hasTextures = !flags.textures.empty();

		if ( hasTextures )
		{
			textureConfigs.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers() );
		}

		UBO_TEXTURES( writer, TexturesUbo::BindingPoint, 0u, hasTextures );
		UBO_PICKING( writer, PickingUbo::BindingPoint, 0u );

		// Fragment Intputs
		auto in = writer.getIn();
		auto vtx_texture = writer.declInput< Vec3 >( "vtx_texture"
			, RenderPass::VertexOutputs::TextureLocation );
		auto vtx_instance = writer.declInput< UInt >( "vtx_instance"
			, RenderPass::VertexOutputs::InstanceLocation );
		auto vtx_material = writer.declInput< UInt >( "vtx_material"
			, RenderPass::VertexOutputs::MaterialLocation );
		auto c3d_maps( writer.declSampledImageArray< FImg2DRgba32 >( "c3d_maps"
			, getMinTextureIndex()
			, 0u
			, std::max( 1u, uint32_t( flags.textures.size() ) )
			, hasTextures ) );

		// Fragment Outputs
		auto pxl_fragColor( writer.declOutput< Vec4 >( "pxl_fragColor", 0 ) );
		shader::Utils utils{ writer };

		writer.implementFunction< sdw::Void >( "main"
			, [&]()
			{
				auto material = materials->getBaseMaterial( vtx_material );
				auto alpha = writer.declLocale( "alpha"
					, material->m_opacity );
				utils.computeOpacityMapContribution( flags.textures
					, textureConfigs
					, c3d_textureConfig
					, c3d_maps
					, vtx_texture
					, alpha );
			utils.applyAlphaFunc( flags.alphaFunc
				, alpha
				, material->m_alphaRef );

			pxl_fragColor = vec4( c3d_drawIndex, c3d_nodeIndex, vtx_instance, 0.0f );
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

	ashes::VkDescriptorSetLayoutBindingArray PickingPass::doCreateUboBindings( PipelineFlags const & flags )const
	{
		return RenderPass::doCreateUboBindings( flags );
	}

	ashes::VkDescriptorSetLayoutBindingArray PickingPass::doCreateTextureBindings( PipelineFlags const & flags )const
	{
		auto index = getMinTextureIndex();
		ashes::VkDescriptorSetLayoutBindingArray textureBindings;

		if ( !flags.textures.empty() )
		{
			textureBindings.push_back( makeDescriptorSetLayoutBinding( index
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT
				, uint32_t( flags.textures.size() ) ) );
			index += uint32_t( flags.textures.size() );
		}

		return textureBindings;
	}

	ashes::PipelineDepthStencilStateCreateInfo PickingPass::doCreateDepthStencilState( PipelineFlags const & flags )const
	{
		return ashes::PipelineDepthStencilStateCreateInfo{ 0u, true, true };
	}

	ashes::PipelineColorBlendStateCreateInfo PickingPass::doCreateBlendState( PipelineFlags const & flags )const
	{
		return RenderPass::createBlendState( BlendMode::eNoBlend, BlendMode::eNoBlend, 1u );
	}

	void PickingPass::doPrepareFrontPipeline( ShaderProgramSPtr program
		, ashes::PipelineVertexInputStateCreateInfoCRefArray const & layouts
		, PipelineFlags const & flags )
	{
	}

	//*********************************************************************************************
}
