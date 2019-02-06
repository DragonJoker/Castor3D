#include "PickingPass.hpp"

#include "Cache/MaterialCache.hpp"
#include "Material/Material.hpp"
#include "Material/Pass.hpp"
#include "Mesh/Submesh.hpp"
#include "Mesh/SubmeshComponent/InstantiationComponent.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderNode/RenderNode_Render.hpp"
#include "Shader/PassBuffer/PassBuffer.hpp"
#include "Shader/Program.hpp"
#include "Shader/Shaders/GlslMaterial.hpp"
#include "Shader/Ubos/PickingUbo.hpp"

#include <Command/CommandBuffer.hpp>
#include <RenderPass/FrameBuffer.hpp>
#include <RenderPass/FrameBufferAttachment.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassCreateInfo.hpp>
#include <Sync/BufferMemoryBarrier.hpp>

#include <ShaderWriter/Source.hpp>

#include <Graphics/Image.hpp>

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		static String const Picking = cuT( "Picking" );
		static String const DrawIndex = cuT( "c3d_iDrawIndex" );
		static String const NodeIndex = cuT( "c3d_iNodeIndex" );

		template< bool Opaque, typename MapType, typename FuncType >
		inline void doTraverseNodes( RenderPass const & pass
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
							auto & data = itSubmeshes.second[0]->pickingUbo.getData();
							data.drawIndex = drawIndex;
							data.nodeIndex = index++;

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
		inline void doUpdateNonInstanced( RenderPass const & pass
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
					auto & data = renderNode->pickingUbo.getData();
					data.drawIndex = drawIndex;
					data.nodeIndex = index++;
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
			uint32_t nodeIndex{ uint32_t( index[1] ) };
			uint32_t faceIndex{ uint32_t( index[3] ) };

			CU_Require( map.size() > pipelineIndex );
			auto itPipeline = map.begin();

			while ( pipelineIndex )
			{
				++itPipeline;
				--pipelineIndex;
			}

			CU_Require( itPipeline->second.size() > nodeIndex );
			auto itNode = itPipeline->second.begin() + nodeIndex;

			subnode = std::static_pointer_cast< SubNodeType >( ( *itNode )->data.shared_from_this() );
			node = std::static_pointer_cast< NodeType >( ( *itNode )->instance.shared_from_this() );
			face = faceIndex;
		}

		template< typename MapType, typename NodeType, typename SubNodeType >
		inline void doPickFromInstantiatedList( MapType const & map
			, Point4f const & index
			, std::weak_ptr< NodeType > & node
			, std::weak_ptr< SubNodeType > & subnode
			, uint32_t & face )
		{
			uint32_t pipelineIndex{ ( uint32_t( index[0] ) >> 8 ) - 1 };
			uint32_t nodeIndex{ uint32_t( index[1] ) };

			CU_Require( map.size() > pipelineIndex );
			auto itPipeline = map.begin();

			while ( pipelineIndex )
			{
				++itPipeline;
				--pipelineIndex;
			}

			auto itPass = itPipeline->second.begin();
			CU_Require( !itPass->second.empty() );
			auto itMesh = itPass->second.begin();

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
				uint32_t instanceIndex{ uint32_t( index[2] ) };
				uint32_t faceIndex{ uint32_t( index[3] ) };
				CU_Require( !itMesh->second.empty() );
				auto itNode = itMesh->second.begin() + instanceIndex;

				subnode = ( *itNode )->data.shared_from_this();
				node = std::static_pointer_cast< Geometry >( ( *itNode )->instance.shared_from_this() );
				face = faceIndex;
			}
		}

		ashes::TexturePtr createTexture( ashes::Device const & device
			, Size const & size
			, ashes::Format format
			, ashes::ImageUsageFlags usage
			, ashes::MemoryPropertyFlag memory )
		{
			ashes::ImageCreateInfo image{};
			image.arrayLayers = 1u;
			image.mipLevels = 1u;
			image.extent.width = size.getWidth();
			image.extent.height = size.getHeight();
			image.extent.depth = 1u;
			image.flags = 0u;
			image.format = format;
			image.imageType = ashes::TextureType::e2D;
			image.initialLayout = ashes::ImageLayout::eUndefined;
			image.samples = ashes::SampleCountFlag::e1;
			image.sharingMode = ashes::SharingMode::eExclusive;
			image.tiling = ashes::ImageTiling::eOptimal;
			image.usage = usage;
			return device.createTexture( image, memory );
		}

		ashes::TextureViewPtr createView( ashes::Texture const & texture )
		{
			ashes::ImageViewCreateInfo view;
			view.format = texture.getFormat();
			view.viewType = ashes::TextureViewType::e2D;
			view.components.r = ashes::ComponentSwizzle::eIdentity;
			view.components.g = ashes::ComponentSwizzle::eIdentity;
			view.components.b = ashes::ComponentSwizzle::eIdentity;
			view.components.a = ashes::ComponentSwizzle::eIdentity;
			view.subresourceRange.aspectMask = ashes::getAspectMask( view.format );
			view.subresourceRange.baseArrayLayer = 0u;
			view.subresourceRange.layerCount = 1u;
			view.subresourceRange.baseMipLevel = 0u;
			view.subresourceRange.levelCount = 1u;
			return texture.createView( view );
		}

		static uint32_t constexpr PickingWidth = 50u;
		static int constexpr PickingOffset = int( PickingWidth / 2 );
	}

	//*********************************************************************************************

	uint32_t const PickingPass::UboBindingPoint = 7u;

	PickingPass::PickingPass( Engine & engine
		, MatrixUbo const & matrixUbo
		, SceneCuller & culler )
		: RenderPass{ cuT( "Picking" )
			, cuT( "Picking" )
			, engine
			, matrixUbo
			, culler
			, nullptr }
	{
	}

	PickingPass::~PickingPass()
	{
	}

	void PickingPass::addScene( Scene & scene, Camera & camera )
	{
	}

	PickNodeType PickingPass::pick( Position const & position
		, Camera const & camera )
	{
		PickNodeType result{ PickNodeType::eNone };
		m_geometry.reset();
		m_submesh.reset();
		m_face = 0u;
		auto & myCamera = getCuller().getCamera();
		ShadowMapLightTypeArray shadowMaps;
		m_renderQueue.update( shadowMaps );
		doUpdateNodes( m_renderQueue.getCulledRenderNodes() );
		auto pixel = doFboPick( position, myCamera, m_renderQueue.getCommandBuffer() );
		result = doPick( pixel, m_renderQueue.getCulledRenderNodes() );
		return result;
	}

	void PickingPass::doUpdateNodes( SceneCulledRenderNodes & nodes )
	{
		auto & myCamera = getCuller().getCamera();
		auto & myScene = getCuller().getScene();
		m_matrixUbo.update( myCamera.getView()
			, myCamera.getProjection() );
		doUpdate( nodes.instancedStaticNodes.backCulled );
		doUpdate( nodes.staticNodes.backCulled );
		doUpdate( nodes.skinnedNodes.backCulled );
		doUpdate( nodes.instancedSkinnedNodes.backCulled );
		doUpdate( nodes.morphingNodes.backCulled );
		doUpdate( nodes.billboardNodes.backCulled );
		myScene.getGeometryCache().uploadPickingUbos();
	}

	Point4f PickingPass::doFboPick( Position const & position
		, Camera const & camera
		, ashes::CommandBuffer const & commandBuffer )
	{
		static ashes::ClearValueArray clearValues
		{
			ashes::ClearColorValue{ 0.0, 0.0, 0.0, 1.0 },
			ashes::DepthStencilClearValue{ 1.0f, 0 }
		};

		m_commandBuffer->begin();
		m_commandBuffer->beginRenderPass( *m_renderPass
			, *m_frameBuffer
			, clearValues
			, ashes::SubpassContents::eSecondaryCommandBuffers );
		m_commandBuffer->executeCommands( { commandBuffer } );
		m_commandBuffer->endRenderPass();

		m_copyRegion.imageOffset.x = int32_t( position.x() - PickingOffset );
		m_copyRegion.imageOffset.y = int32_t( camera.getHeight() - position.y() - PickingOffset );

#if !C3D_DebugPicking
		m_commandBuffer->memoryBarrier( ashes::PipelineStageFlag::eTransfer
			, ashes::PipelineStageFlag::eTransfer
			, m_stagingBuffer->getBuffer().makeTransferDestination() );
		m_commandBuffer->copyToBuffer( m_copyRegion
			, *m_colourTexture
			, m_stagingBuffer->getBuffer() );
		m_commandBuffer->memoryBarrier( ashes::PipelineStageFlag::eTransfer
			, ashes::PipelineStageFlag::eTransfer
			, m_stagingBuffer->getBuffer().makeMemoryTransitionBarrier( ashes::AccessFlag::eMemoryRead ) );
#endif

		m_commandBuffer->end();

		auto & device = getCurrentDevice( *this );
		ashes::FencePtr fence = device.createFence();
		device.getGraphicsQueue().submit( *m_commandBuffer, fence.get() );
		fence->wait( ashes::FenceTimeout );
		device.waitIdle();

#if !C3D_DebugPicking
		if ( auto * data = m_stagingBuffer->lock( 0u, m_stagingBuffer->getCount(), ashes::MemoryMapFlag::eRead ) )
		{
			std::copy( data, data + m_stagingBuffer->getCount(), m_buffer.begin() );
			m_stagingBuffer->unlock();
		}
#endif

		auto it = m_buffer.begin();
		it += ( PickingOffset * PickingWidth ) + PickingOffset - 1;
		return *it;
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

	bool PickingPass::doInitialise( Size const & size )
	{
		auto & device = getCurrentDevice( *this );
		m_commandBuffer = device.getGraphicsCommandPool().createCommandBuffer();

		m_colourTexture = createTexture( device
			, size
			, ashes::Format::eR32G32B32A32_SFLOAT
			, ashes::ImageUsageFlag::eColourAttachment
				| ashes::ImageUsageFlag::eTransferSrc
				| ashes::ImageUsageFlag::eSampled
			, ashes::MemoryPropertyFlag::eDeviceLocal );
		m_colourView = createView( *m_colourTexture );

		m_copyRegion.bufferImageHeight = 0u;
		m_copyRegion.bufferOffset = 0u;
		m_copyRegion.bufferRowLength = 0u;
		m_copyRegion.imageExtent.width = PickingWidth;
		m_copyRegion.imageExtent.height = PickingWidth;
		m_copyRegion.imageExtent.depth = 1u;
		m_copyRegion.imageOffset.z = 0u;
		m_copyRegion.imageSubresource.aspectMask = m_colourView->getSubResourceRange().aspectMask;
		m_copyRegion.imageSubresource.baseArrayLayer = 0u;
		m_copyRegion.imageSubresource.layerCount = 1u;
		m_copyRegion.imageSubresource.mipLevel = 0u;

		m_depthTexture = createTexture( device
			, size
			, ashes::Format::eD32_SFLOAT
			, ashes::ImageUsageFlag::eDepthStencilAttachment
			, ashes::MemoryPropertyFlag::eDeviceLocal );
		m_depthView = createView( *m_depthTexture );

		m_stagingBuffer = ashes::makeBuffer< Point4f >( device
			, PickingWidth * PickingWidth
			, ashes::BufferTarget::eTransferDst
			, ashes::MemoryPropertyFlag::eHostVisible );

		m_buffer.resize( PickingWidth * PickingWidth );

		// Create the render pass.
		ashes::RenderPassCreateInfo renderPass;
		renderPass.flags = 0u;

		renderPass.attachments.resize( 2u );
		renderPass.attachments[0].format = m_colourTexture->getFormat();
		renderPass.attachments[0].loadOp = ashes::AttachmentLoadOp::eClear;
		renderPass.attachments[0].storeOp = ashes::AttachmentStoreOp::eStore;
		renderPass.attachments[0].stencilLoadOp = ashes::AttachmentLoadOp::eDontCare;
		renderPass.attachments[0].stencilStoreOp = ashes::AttachmentStoreOp::eDontCare;
		renderPass.attachments[0].samples = ashes::SampleCountFlag::e1;
		renderPass.attachments[0].initialLayout = ashes::ImageLayout::eUndefined;
		renderPass.attachments[0].finalLayout = ashes::ImageLayout::eTransferSrcOptimal;

		renderPass.attachments[1].format = m_depthTexture->getFormat();
		renderPass.attachments[1].loadOp = ashes::AttachmentLoadOp::eClear;
		renderPass.attachments[1].storeOp = ashes::AttachmentStoreOp::eStore;
		renderPass.attachments[1].stencilLoadOp = ashes::AttachmentLoadOp::eDontCare;
		renderPass.attachments[1].stencilStoreOp = ashes::AttachmentStoreOp::eDontCare;
		renderPass.attachments[1].samples = ashes::SampleCountFlag::e1;
		renderPass.attachments[1].initialLayout = ashes::ImageLayout::eUndefined;
		renderPass.attachments[1].finalLayout = ashes::ImageLayout::eDepthStencilAttachmentOptimal;

		renderPass.subpasses.resize( 1u );
		renderPass.subpasses[0].flags = 0u;
		renderPass.subpasses[0].pipelineBindPoint = ashes::PipelineBindPoint::eGraphics;
		renderPass.subpasses[0].colorAttachments.push_back( { 0u, ashes::ImageLayout::eColourAttachmentOptimal } );
		renderPass.subpasses[0].depthStencilAttachment = { 1u, ashes::ImageLayout::eDepthStencilAttachmentOptimal };

		renderPass.dependencies.resize( 2u );
		renderPass.dependencies[0].srcSubpass = ashes::ExternalSubpass;
		renderPass.dependencies[0].dstSubpass = 0u;
		renderPass.dependencies[0].srcStageMask = ashes::PipelineStageFlag::eTopOfPipe;
		renderPass.dependencies[0].dstStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[0].srcAccessMask = ashes::AccessFlag::eMemoryRead;
		renderPass.dependencies[0].dstAccessMask = ashes::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[0].dependencyFlags = ashes::DependencyFlag::eByRegion;

		renderPass.dependencies[1].srcSubpass = 0u;
		renderPass.dependencies[1].dstSubpass = ashes::ExternalSubpass;
		renderPass.dependencies[1].srcStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[1].dstStageMask = ashes::PipelineStageFlag::eFragmentShader;
		renderPass.dependencies[1].srcAccessMask = ashes::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[1].dstAccessMask = ashes::AccessFlag::eShaderRead;
		renderPass.dependencies[1].dependencyFlags = ashes::DependencyFlag::eByRegion;

		m_renderPass = device.createRenderPass( renderPass );

		ashes::FrameBufferAttachmentArray attachments;
		attachments.emplace_back( *( m_renderPass->getAttachments().begin() + 0u ), *m_colourView );
		attachments.emplace_back( *( m_renderPass->getAttachments().begin() + 1u ), *m_depthView );
		m_frameBuffer = m_renderPass->createFrameBuffer( { size.getWidth(), size.getHeight() }
			, std::move( attachments ) );

		return true;
	}

	void PickingPass::doCleanup()
	{
		m_commandBuffer.reset();
		m_scenes.clear();
		m_stagingBuffer.reset();
		m_frameBuffer.reset();
		m_colourView.reset();
		m_depthView.reset();
		m_depthTexture.reset();
		m_colourTexture.reset();
	}

	void PickingPass::doFillUboDescriptor( ashes::DescriptorSetLayout const & layout
		, BillboardListRenderNode & node )
	{
		node.uboDescriptorSet->createBinding( layout.getBinding( PickingUbo::BindingPoint )
			, node.pickingUbo.buffer->getBuffer()
			, node.pickingUbo.offset
			, 1u );
	}

	void PickingPass::doFillUboDescriptor( ashes::DescriptorSetLayout const & layout
		, SubmeshRenderNode & node )
	{
		node.uboDescriptorSet->createBinding( layout.getBinding( PickingUbo::BindingPoint )
			, node.pickingUbo.buffer->getBuffer()
			, node.pickingUbo.offset
			, 1u );
	}

	void PickingPass::doFillTextureDescriptor( ashes::DescriptorSetLayout const & layout
		, uint32_t & index
		, BillboardListRenderNode & node
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		node.passNode.fillDescriptor( layout
			, index
			, *node.texDescriptorSet
			, true );
	}

	void PickingPass::doFillTextureDescriptor( ashes::DescriptorSetLayout const & layout
		, uint32_t & index
		, SubmeshRenderNode & node
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		node.passNode.fillDescriptor( layout
			, index
			, *node.texDescriptorSet
			, true );
	}

	void PickingPass::doUpdate( RenderQueueArray & CU_UnusedParam( queues ) )
	{
	}

	ShaderPtr PickingPass::doGetGeometryShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags )const
	{
		return ShaderPtr{};
	}

	ShaderPtr PickingPass::doGetLegacyPixelShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ashes::CompareOp alphaFunc )const
	{
		return doGetPixelShaderSource( passFlags
			, textureFlags
			, programFlags
			, sceneFlags
			, alphaFunc );
	}

	ShaderPtr PickingPass::doGetPbrMRPixelShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ashes::CompareOp alphaFunc )const
	{
		return doGetPixelShaderSource( passFlags
			, textureFlags
			, programFlags
			, sceneFlags
			, alphaFunc );
	}

	ShaderPtr PickingPass::doGetPbrSGPixelShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ashes::CompareOp alphaFunc )const
	{
		return doGetPixelShaderSource( passFlags
			, textureFlags
			, programFlags
			, sceneFlags
			, alphaFunc );
	}

	ShaderPtr PickingPass::doGetVertexShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, bool invertNormals )const
	{
		using namespace sdw;
		VertexWriter writer;
		// Vertex inputs
		auto position = writer.declInput< Vec4 >( cuT( "position" )
			, RenderPass::VertexInputs::PositionLocation );
		auto uv = writer.declInput< Vec3 >( cuT( "uv" )
			, RenderPass::VertexInputs::TextureLocation );
		auto bone_ids0 = writer.declInput< IVec4 >( cuT( "bone_ids0" )
			, RenderPass::VertexInputs::BoneIds0Location
			, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto bone_ids1 = writer.declInput< IVec4 >( cuT( "bone_ids1" )
			, RenderPass::VertexInputs::BoneIds1Location
			, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto weights0 = writer.declInput< Vec4 >( cuT( "weights0" )
			, RenderPass::VertexInputs::Weights0Location
			, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto weights1 = writer.declInput< Vec4 >( cuT( "weights1" )
			, RenderPass::VertexInputs::Weights1Location
			, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto transform = writer.declInput< Mat4 >( cuT( "transform" )
			, RenderPass::VertexInputs::TransformLocation
			, checkFlag( programFlags, ProgramFlag::eInstantiation ) );
		auto material = writer.declInput< Int >( cuT( "material" )
			, RenderPass::VertexInputs::MaterialLocation
			, checkFlag( programFlags, ProgramFlag::eInstantiation ) );
		auto position2 = writer.declInput< Vec4 >( cuT( "position2" )
			, RenderPass::VertexInputs::Position2Location
			, checkFlag( programFlags, ProgramFlag::eMorphing ) );
		auto texture2 = writer.declInput< Vec3 >( cuT( "texture2" )
			, RenderPass::VertexInputs::Texture2Location
			, checkFlag( programFlags, ProgramFlag::eMorphing ) );
		auto in = writer.getIn();

		UBO_MATRIX( writer, MatrixUbo::BindingPoint, 0 );
		UBO_MODEL_MATRIX( writer, ModelMatrixUbo::BindingPoint, 0 );
		UBO_MODEL( writer, ModelUbo::BindingPoint, 0 );
		auto skinningData = SkinningUbo::declare( writer, SkinningUbo::BindingPoint, 0, programFlags );
		UBO_MORPHING( writer, MorphingUbo::BindingPoint, 0, programFlags );

		// Outputs
		auto vtx_texture = writer.declOutput< Vec3 >( cuT( "vtx_texture" )
			, RenderPass::VertexOutputs::TextureLocation );
		auto vtx_instance = writer.declOutput< UInt >( cuT( "vtx_instance" )
			, RenderPass::VertexOutputs::InstanceLocation );
		auto vtx_material = writer.declOutput< Int >( cuT( "vtx_material" )
			, RenderPass::VertexOutputs::MaterialLocation );
		auto out = writer.getOut();

		std::function< void() > main = [&]()
		{
			auto v4Vertex = writer.declLocale( cuT( "v4Vertex" )
				, vec4( position.xyz(), 1.0 ) );
			auto v3Texture = writer.declLocale( cuT( "v3Texture" )
				, uv );
			auto mtxModel = writer.declLocale< Mat4 >( cuT( "mtxModel" ) );

			if ( checkFlag( programFlags, ProgramFlag::eSkinning ) )
			{
				mtxModel = SkinningUbo::computeTransform( skinningData, writer, programFlags );
			}
			else if ( checkFlag( programFlags, ProgramFlag::eInstantiation ) )
			{
				mtxModel = transform;
			}
			else
			{
				mtxModel = c3d_curMtxModel;
			}

			if ( checkFlag( programFlags, ProgramFlag::eInstantiation ) )
			{
				vtx_material = material;
			}
			else
			{
				vtx_material = c3d_materialIndex;
			}

			if ( checkFlag( programFlags, ProgramFlag::eMorphing ) )
			{
				auto time = writer.declLocale( cuT( "time" )
					, vec3( 1.0_f - c3d_time ) );
				v4Vertex = vec4( sdw::fma( v4Vertex.xyz(), time, position2.xyz() * c3d_time ), 1.0 );
				v3Texture = sdw::fma( v3Texture, time, texture2 * c3d_time );
			}

			vtx_texture = v3Texture;
			v4Vertex = mtxModel * v4Vertex;
			v4Vertex = c3d_curView * v4Vertex;
			vtx_instance = in.gl_InstanceID;
			out.gl_out.gl_Position = c3d_projection * v4Vertex;
		};

		writer.implementFunction< sdw::Void >( cuT( "main" ), main );
		return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr PickingPass::doGetPixelShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ashes::CompareOp alphaFunc )const
	{
		using namespace sdw;
		FragmentWriter writer;

		// UBOs
		auto materials = shader::createMaterials( writer
			, passFlags );
		materials->declare( getEngine()->getRenderSystem()->getGpuInformations().hasShaderStorageBuffers() );
		UBO_PICKING( writer, PickingUbo::BindingPoint, 0u );

		// Fragment Intputs
		auto in = writer.getIn();
		auto vtx_texture = writer.declInput< Vec3 >( cuT( "vtx_texture" )
			, RenderPass::VertexOutputs::TextureLocation );
		auto vtx_instance = writer.declInput< UInt >( cuT( "vtx_instance" )
			, RenderPass::VertexOutputs::InstanceLocation );
		auto vtx_material = writer.declInput< Int >( cuT( "vtx_material" )
			, RenderPass::VertexOutputs::MaterialLocation );
		auto c3d_mapOpacity( writer.declSampledImage< FImg2DR32 >( cuT( "c3d_mapOpacity" )
			, MinBufferIndex
			, 0u
			, checkFlag( textureFlags, TextureChannel::eOpacity ) ) );

		// Fragment Outputs
		auto pxl_fragColor( writer.declOutput< Vec4 >( cuT( "pxl_fragColor" ), 0 ) );

		writer.implementFunction< sdw::Void >( cuT( "main" ), [&]()
		{
			auto material = materials->getBaseMaterial( vtx_material );
			auto alpha = writer.declLocale( cuT( "alpha" )
				, material->m_opacity );

			if ( checkFlag( textureFlags, TextureChannel::eOpacity ) )
			{
				alpha *= texture( c3d_mapOpacity, vtx_texture.xy() );
				shader::applyAlphaFunc( writer
					, alphaFunc
					, alpha
					, material->m_alphaRef );
			}
			else if ( alphaFunc != ashes::CompareOp::eAlways )
			{
				shader::applyAlphaFunc( writer
					, alphaFunc
					, alpha
					, material->m_alphaRef );
			}

			pxl_fragColor = vec4( c3d_drawIndex, c3d_nodeIndex, vtx_instance, in.gl_PrimitiveID );
#if C3D_DebugPicking
			pxl_fragColor /= 255.0_f;
#endif
		} );

		return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
	}

	void PickingPass::doUpdateFlags( PassFlags & passFlags
		, TextureChannels & textureFlags
		, ProgramFlags & programFlags
		, SceneFlags & sceneFlags )const
	{
		remFlag( programFlags, ProgramFlag::eLighting );
		remFlag( passFlags, PassFlag::eAlphaBlending );
		remFlag( textureFlags, TextureChannel::eAll );

		addFlag( programFlags, ProgramFlag::ePicking );
	}

	void PickingPass::doUpdatePipeline( RenderPipeline & pipeline )const
	{
	}

	ashes::DescriptorSetLayoutBindingArray PickingPass::doCreateUboBindings( PipelineFlags const & flags )const
	{
		ashes::DescriptorSetLayoutBindingArray result = RenderPass::doCreateUboBindings( flags );
		result.emplace_back( UboBindingPoint, ashes::DescriptorType::eUniformBuffer, ashes::ShaderStageFlag::eFragment );

		if ( checkFlag( flags.programFlags, ProgramFlag::eBillboards ) )
		{
			result.emplace_back( BillboardUbo::BindingPoint, ashes::DescriptorType::eUniformBuffer, ashes::ShaderStageFlag::eVertex );
		}

		return result;
	}

	ashes::DescriptorSetLayoutBindingArray PickingPass::doCreateTextureBindings( PipelineFlags const & flags )const
	{
		auto index = MinBufferIndex;
		ashes::DescriptorSetLayoutBindingArray textureBindings;

		if ( checkFlag( flags.textureFlags, TextureChannel::eOpacity ) )
		{
			textureBindings.emplace_back( index++, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment );
		}

		return textureBindings;
	}

	ashes::DepthStencilState PickingPass::doCreateDepthStencilState( PipelineFlags const & flags )const
	{
		return ashes::DepthStencilState{ 0u, true, true };
	}

	ashes::ColourBlendState PickingPass::doCreateBlendState( PipelineFlags const & flags )const
	{
		return RenderPass::createBlendState( BlendMode::eNoBlend, BlendMode::eNoBlend, 1u );
	}

	void PickingPass::doPrepareFrontPipeline( ShaderProgramSPtr program
		, ashes::VertexLayoutCRefArray const & layouts
		, PipelineFlags const & flags )
	{
	}

	//*********************************************************************************************
}
