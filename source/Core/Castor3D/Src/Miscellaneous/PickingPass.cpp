#include "PickingPass.hpp"

#include "Cache/MaterialCache.hpp"
#include "Mesh/Submesh.hpp"
#include "Mesh/SubmeshComponent/InstantiationComponent.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderNode/RenderNode_Render.hpp"
#include "Shader/PassBuffer/PassBuffer.hpp"
#include "Shader/Shaders/GlslMaterial.hpp"
#include "Shader/Program.hpp"

#include <Command/CommandBuffer.hpp>
#include <RenderPass/FrameBuffer.hpp>
#include <RenderPass/FrameBufferAttachment.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassCreateInfo.hpp>
#include <Sync/BufferMemoryBarrier.hpp>

#include <GlslSource.hpp>

#include <Graphics/Image.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		static String const Picking = cuT( "Picking" );
		static String const DrawIndex = cuT( "c3d_iDrawIndex" );
		static String const NodeIndex = cuT( "c3d_iNodeIndex" );

		template< bool Opaque, typename MapType, typename FuncType >
		inline void doTraverseNodes( RenderPass const & pass
			, renderer::UniformBuffer< PickingUboData > & ubo
			, MapType & nodes
			, PickingPass::NodeType type
			, FuncType function )
		{
			uint32_t count{ 1u };

			for ( auto itPipelines : nodes )
			{
				pass.updatePipeline( *itPipelines.first );
				auto & data = ubo.getData();
				data.drawIndex = uint8_t( type ) + ( ( count & 0x00FFFFFF ) << 8 );
				uint32_t index{ 0u };

				for ( auto itPass : itPipelines.second )
				{
					for ( auto itSubmeshes : itPass.second )
					{
						data.nodeIndex = index++;
						ubo.upload();
						function( *itPipelines.first
							, *itPass.first
							, *itSubmeshes.first
							, itSubmeshes.first->getInstantiation()
							, itSubmeshes.second );
					}
				}

				count++;
			}
		}

		template< bool Opaque, typename MapType >
		inline void doUpdateNonInstanced( RenderPass const & pass
			, renderer::UniformBuffer< PickingUboData > & ubo
			, Scene const & scene
			, PickingPass::NodeType type
			, MapType & nodes )
		{
			uint32_t count{ 1u };

			for ( auto itPipelines : nodes )
			{
				pass.updatePipeline( *itPipelines.first );
				auto & data = ubo.getData();
				data.drawIndex = uint8_t( type ) + ( ( count & 0x00FFFFFF ) << 8 );
				uint32_t index{ 0u };

				for ( auto & renderNode : itPipelines.second )
				{
					data.nodeIndex = index++;
					ubo.upload();
				}

				count++;
			}
		}

		template< typename MapType, typename NodeType, typename SubNodeType >
		inline void doPickFromList( MapType const & map
			, Point3f const & index
			, std::weak_ptr< NodeType > & node
			, std::weak_ptr< SubNodeType > & subnode
			, uint32_t & face )
		{
			uint32_t pipelineIndex{ ( uint32_t( index[0] ) >> 8 ) - 1 };
			uint32_t nodeIndex{ uint32_t( index[1] ) };
			uint32_t faceIndex{ uint32_t( index[3] ) };

			REQUIRE( map.size() > pipelineIndex );
			auto itPipeline = map.begin();

			while ( pipelineIndex )
			{
				++itPipeline;
				--pipelineIndex;
			}

			REQUIRE( itPipeline->second.size() > nodeIndex );
			auto itNode = itPipeline->second.begin() + nodeIndex;

			subnode = std::static_pointer_cast< SubNodeType >( ( *itNode )->data.shared_from_this() );
			node = std::static_pointer_cast< NodeType >( ( *itNode )->instance.shared_from_this() );
			face = faceIndex;
		}

		template< typename MapType, typename NodeType, typename SubNodeType >
		inline void doPickFromInstantiatedList( MapType const & map
			, Point3f const & index
			, std::weak_ptr< NodeType > & node
			, std::weak_ptr< SubNodeType > & subnode
			, uint32_t & face )
		{
			uint32_t pipelineIndex{ ( uint32_t( index[0] ) >> 8 ) - 1 };
			uint32_t nodeIndex{ uint32_t( index[1] ) };

			REQUIRE( map.size() > pipelineIndex );
			auto itPipeline = map.begin();

			while ( pipelineIndex )
			{
				++itPipeline;
				--pipelineIndex;
			}

			auto itPass = itPipeline->second.begin();
			REQUIRE( !itPass->second.empty() );
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
				REQUIRE( !itMesh->second.empty() );
				auto itNode = itMesh->second.begin() + instanceIndex;

				subnode = ( *itNode )->data.shared_from_this();
				node = std::static_pointer_cast< Geometry >( ( *itNode )->instance.shared_from_this() );
				face = faceIndex;
			}
		}

		renderer::TexturePtr createTexture( renderer::Device const & device
			, Size const & size
			, renderer::Format format
			, renderer::ImageUsageFlags usage
			, renderer::MemoryPropertyFlag memory )
		{
			renderer::ImageCreateInfo image{};
			image.arrayLayers = 1u;
			image.mipLevels = 1u;
			image.extent.width = size.getWidth();
			image.extent.height = size.getHeight();
			image.extent.depth = 1u;
			image.flags = 0u;
			image.format = format;
			image.imageType = renderer::TextureType::e2D;
			image.initialLayout = renderer::ImageLayout::eUndefined;
			image.samples = renderer::SampleCountFlag::e1;
			image.sharingMode = renderer::SharingMode::eExclusive;
			image.tiling = renderer::ImageTiling::eOptimal;
			image.usage = usage;
			return device.createTexture( image, memory );
		}

		renderer::TextureViewPtr createView( renderer::Texture const & texture )
		{
			renderer::ImageViewCreateInfo view;
			view.format = texture.getFormat();
			view.viewType = renderer::TextureViewType::e2D;
			view.components.r = renderer::ComponentSwizzle::eIdentity;
			view.components.g = renderer::ComponentSwizzle::eIdentity;
			view.components.b = renderer::ComponentSwizzle::eIdentity;
			view.components.a = renderer::ComponentSwizzle::eIdentity;
			view.subresourceRange.aspectMask = renderer::getAspectMask( view.format );
			view.subresourceRange.baseArrayLayer = 0u;
			view.subresourceRange.layerCount = 1u;
			view.subresourceRange.baseMipLevel = 0u;
			view.subresourceRange.levelCount = 1u;
			return texture.createView( view );
		}

		static uint32_t constexpr PickingWidth = 50u;
		static int constexpr PickingOffset = int( PickingWidth / 2 );
	}

	PickingPass::PickingPass( Engine & engine )
		: RenderPass{ cuT( "Picking" ), engine, nullptr }
	{
	}

	PickingPass::~PickingPass()
	{
	}

	void PickingPass::addScene( Scene & scene, Camera & camera )
	{
		auto itScn = m_scenes.emplace( &scene, CameraQueueMap{} ).first;
		auto itCam = itScn->second.emplace( &camera, RenderQueue{ *this, m_opaque, nullptr } ).first;
		itCam->second.initialise( scene, camera );
	}

	PickingPass::NodeType PickingPass::pick( Position const & position
		, Camera const & camera )
	{
		NodeType result{ NodeType::eNone };
		m_geometry.reset();
		m_submesh.reset();
		m_face = 0u;

		auto itScn = m_scenes.find( camera.getScene() );

		if ( itScn != m_scenes.end() )
		{
			auto itCam = itScn->second.find( &camera );

			if ( itCam != itScn->second.end() )
			{
				itCam->second.update();
				doUpdateNodes( itCam->second.getCulledRenderNodes() );
				auto pixel = doFboPick( position, camera, itCam->second.getCommandBuffer() );
				result = doPick( pixel, itCam->second.getCulledRenderNodes() );
			}
		}

		return result;
	}

	void PickingPass::doUpdateNodes( SceneCulledRenderNodes & nodes )
	{
		m_matrixUbo.update( nodes.camera.getView()
			, nodes.camera.getViewport().getProjection() );
		doUpdate( nodes.scene, nodes.camera, nodes.instancedStaticNodes.backCulled );
		doUpdate( nodes.scene, nodes.camera, nodes.staticNodes.backCulled );
		doUpdate( nodes.scene, nodes.camera, nodes.skinnedNodes.backCulled );
		doUpdate( nodes.scene, nodes.camera, nodes.instancedSkinnedNodes.backCulled );
		doUpdate( nodes.scene, nodes.camera, nodes.morphingNodes.backCulled );
		doUpdate( nodes.scene, nodes.camera, nodes.billboardNodes.backCulled );
	}

	Point3f PickingPass::doFboPick( Position const & position
		, Camera const & camera
		, renderer::CommandBuffer const & commandBuffer )
	{
		static renderer::ClearValueArray clearValues
		{
			renderer::ClearColorValue{ 0.0, 0.0, 0.0, 1.0 },
			renderer::DepthStencilClearValue{ 0.0f, 1 }
		};

		if ( m_commandBuffer->begin() )
		{
			m_commandBuffer->beginRenderPass( *m_renderPass
				, *m_frameBuffer
				, clearValues
				, renderer::SubpassContents::eSecondaryCommandBuffers );
			m_commandBuffer->executeCommands( { commandBuffer } );
			m_commandBuffer->endRenderPass();

			//m_copyRegion.imageOffset.x = int32_t( position.x() - PickingOffset );
			//m_copyRegion.imageOffset.y = int32_t( camera.getHeight() - position.y() - PickingOffset );

			//m_commandBuffer->memoryBarrier( renderer::PipelineStageFlag::eTransfer
			//	, renderer::PipelineStageFlag::eTransfer
			//	, m_stagingBuffer->getBuffer().makeTransferDestination() );
			//m_commandBuffer->copyToBuffer( m_copyRegion
			//	, *m_colourTexture
			//	, m_stagingBuffer->getBuffer() );
			//m_commandBuffer->memoryBarrier( renderer::PipelineStageFlag::eTransfer
			//	, renderer::PipelineStageFlag::eTransfer
			//	, m_stagingBuffer->getBuffer().makeMemoryTransitionBarrier( renderer::AccessFlag::eMemoryRead ) );
			m_commandBuffer->end();
		}

		auto & renderSystem = *getEngine()->getRenderSystem();
		auto & device = *renderSystem.getCurrentDevice();
		renderer::FencePtr fence = device.createFence();
		device.getGraphicsQueue().submit( *m_commandBuffer, fence.get() );
		fence->wait( renderer::FenceTimeout );

		//if ( auto * data = m_stagingBuffer->lock( 0u, m_stagingBuffer->getCount(), renderer::MemoryMapFlag::eRead ) )
		//{
		//	std::memcpy( m_buffer->ptr(), data, m_stagingBuffer->getCount() );
		//	m_stagingBuffer->unlock();
		//}

		auto it = std::static_pointer_cast< PxBuffer< PixelFormat::eRGBA32F > >( m_buffer )->begin();
		it += ( PickingOffset * PickingWidth ) + PickingOffset - 1;
		return Point3f{ reinterpret_cast< float const * >( it->constPtr() ) };
	}

	PickingPass::NodeType PickingPass::doPick( Point3f const & pixel
		, SceneCulledRenderNodes & nodes )
	{
		NodeType result{ NodeType::eNone };

		if ( castor::point::lengthSquared( pixel ) )
		{
			result = NodeType( uint32_t( pixel[0] ) & 0xFF );

			switch ( result )
			{
			case NodeType::eStatic:
				doPickFromList( nodes.staticNodes.backCulled, pixel, m_geometry, m_submesh, m_face );
				break;

			case NodeType::eInstantiatedStatic:
				doPickFromInstantiatedList( nodes.instancedStaticNodes.backCulled, pixel, m_geometry, m_submesh, m_face );
				break;

			case NodeType::eSkinning:
				doPickFromList( nodes.skinnedNodes.backCulled, pixel, m_geometry, m_submesh, m_face );
				break;

			case NodeType::eInstantiatedSkinning:
				doPickFromInstantiatedList( nodes.instancedSkinnedNodes.backCulled, pixel, m_geometry, m_submesh, m_face );
				break;

			case NodeType::eMorphing:
				doPickFromList( nodes.morphingNodes.backCulled, pixel, m_geometry, m_submesh, m_face );
				break;

			case NodeType::eBillboard:
				doPickFromList( nodes.billboardNodes.backCulled, pixel, m_billboard, m_billboard, m_face );
				break;

			default:
				FAILURE( "Unsupported index" );
				result = NodeType::eNone;
				break;
			}
		}

		return result;
	}

	void PickingPass::doUpdate( Scene const & scene
		, Camera const & camera
		, SubmeshStaticRenderNodesPtrByPipelineMap & nodes )
	{
		doTraverseNodes< true >( *this
			, *m_pickingUbo
			, nodes
			, NodeType::eInstantiatedStatic
			, [&scene, &camera, this]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, InstantiationComponent & component
				, StaticRenderNodePtrArray & renderNodes )
			{
				if ( !renderNodes.empty() && component.hasMatrixBuffer() )
				{
					doCopyNodesMatrices( renderNodes, camera, component.getData() );
				}
			} );
	}

	void PickingPass::doUpdate( Scene const & scene
		, Camera const & camera
		, StaticRenderNodesPtrByPipelineMap & nodes )
	{
		doUpdateNonInstanced< true >( *this
			, *m_pickingUbo
			, scene
			, NodeType::eStatic
			, nodes );
	}

	void PickingPass::doUpdate( Scene const & scene
		, Camera const & camera
		, SkinningRenderNodesPtrByPipelineMap & nodes )
	{
		doUpdateNonInstanced< true >( *this
			, *m_pickingUbo
			, scene
			, NodeType::eSkinning
			, nodes );
	}
	
	void PickingPass::doUpdate( Scene const & scene
		, Camera const & camera
		, SubmeshSkinningRenderNodesPtrByPipelineMap & nodes )
	{
		doTraverseNodes< true >( *this
			, *m_pickingUbo
			, nodes
			, NodeType::eInstantiatedSkinning
			, [&scene, &camera, this]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, InstantiationComponent & component
				, SkinningRenderNodePtrArray & renderNodes )
			{
				auto & instantiatedBones = submesh.getInstantiatedBones();

				if ( !renderNodes.empty()
					&& component.hasMatrixBuffer()
					&& instantiatedBones.hasInstancedBonesBuffer() )
				{
					doCopyNodesBones( renderNodes, camera, instantiatedBones.getInstancedBonesBuffer() );
				}
			} );
	}

	void PickingPass::doUpdate( Scene const & scene
		, Camera const & camera
		, MorphingRenderNodesPtrByPipelineMap & nodes )
	{
		doUpdateNonInstanced< true >( *this
			, *m_pickingUbo
			, scene
			, NodeType::eMorphing
			, nodes );
	}

	void PickingPass::doUpdate( Scene const & scene
		, Camera const & camera
		, BillboardRenderNodesPtrByPipelineMap & nodes )
	{
		doUpdateNonInstanced< true >( *this
			, *m_pickingUbo
			, scene
			, NodeType::eBillboard
			, nodes );
	}

	bool PickingPass::doInitialise( Size const & size )
	{
		auto & renderSystem = *getEngine()->getRenderSystem();
		auto & device = *renderSystem.getCurrentDevice();
		m_commandBuffer = device.getGraphicsCommandPool().createCommandBuffer();
		m_pickingUbo = renderer::makeUniformBuffer< PickingUboData >( device
			, 1u
			, 0u
			, renderer::MemoryPropertyFlag::eHostVisible );

		m_colourTexture = createTexture( device
			, size
			, renderer::Format::eR32G32B32A32_SFLOAT
			, renderer::ImageUsageFlag::eColourAttachment
				| renderer::ImageUsageFlag::eTransferSrc
				| renderer::ImageUsageFlag::eSampled
			, renderer::MemoryPropertyFlag::eDeviceLocal );
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
			, renderer::Format::eD32_SFLOAT
			, renderer::ImageUsageFlag::eDepthStencilAttachment
			, renderer::MemoryPropertyFlag::eDeviceLocal );
		m_depthView = createView( *m_depthTexture );

		m_stagingBuffer = renderer::makeBuffer< Point4f >( device
			, PickingWidth * PickingWidth
			, renderer::BufferTarget::eTransferDst
			, renderer::MemoryPropertyFlag::eHostVisible );

		m_buffer = PxBufferBase::create( Size{ PickingWidth, PickingWidth }
			, convert( m_colourTexture->getFormat() ) );

		// Create the render pass.
		renderer::RenderPassCreateInfo renderPass;
		renderPass.flags = 0u;

		renderPass.attachments.resize( 2u );
		renderPass.attachments[0].format = m_colourTexture->getFormat();
		renderPass.attachments[0].loadOp = renderer::AttachmentLoadOp::eClear;
		renderPass.attachments[0].storeOp = renderer::AttachmentStoreOp::eStore;
		renderPass.attachments[0].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
		renderPass.attachments[0].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
		renderPass.attachments[0].samples = renderer::SampleCountFlag::e1;
		renderPass.attachments[0].initialLayout = renderer::ImageLayout::eUndefined;
		renderPass.attachments[0].finalLayout = renderer::ImageLayout::eTransferSrcOptimal;

		renderPass.attachments[1].format = m_depthTexture->getFormat();
		renderPass.attachments[1].loadOp = renderer::AttachmentLoadOp::eClear;
		renderPass.attachments[1].storeOp = renderer::AttachmentStoreOp::eStore;
		renderPass.attachments[1].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
		renderPass.attachments[1].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
		renderPass.attachments[1].samples = renderer::SampleCountFlag::e1;
		renderPass.attachments[1].initialLayout = renderer::ImageLayout::eUndefined;
		renderPass.attachments[1].finalLayout = renderer::ImageLayout::eDepthStencilAttachmentOptimal;

		renderPass.subpasses.resize( 1u );
		renderPass.subpasses[0].flags = 0u;
		renderPass.subpasses[0].pipelineBindPoint = renderer::PipelineBindPoint::eGraphics;
		renderPass.subpasses[0].colorAttachments.push_back( { 0u, renderer::ImageLayout::eColourAttachmentOptimal } );
		renderPass.subpasses[0].depthStencilAttachment = { 1u, renderer::ImageLayout::eDepthStencilAttachmentOptimal };

		renderPass.dependencies.resize( 2u );
		renderPass.dependencies[0].srcSubpass = renderer::ExternalSubpass;
		renderPass.dependencies[0].dstSubpass = 0u;
		renderPass.dependencies[0].srcStageMask = renderer::PipelineStageFlag::eTopOfPipe;
		renderPass.dependencies[0].dstStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[0].srcAccessMask = renderer::AccessFlag::eMemoryRead;
		renderPass.dependencies[0].dstAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[0].dependencyFlags = renderer::DependencyFlag::eByRegion;

		renderPass.dependencies[1].srcSubpass = 0u;
		renderPass.dependencies[1].dstSubpass = renderer::ExternalSubpass;
		renderPass.dependencies[1].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[1].dstStageMask = renderer::PipelineStageFlag::eFragmentShader;
		renderPass.dependencies[1].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[1].dstAccessMask = renderer::AccessFlag::eShaderRead;
		renderPass.dependencies[1].dependencyFlags = renderer::DependencyFlag::eByRegion;

		m_renderPass = device.createRenderPass( renderPass );

		renderer::FrameBufferAttachmentArray attachments;
		attachments.emplace_back( *( m_renderPass->getAttachments().begin() + 0u ), *m_colourView );
		attachments.emplace_back( *( m_renderPass->getAttachments().begin() + 1u ), *m_depthView );
		m_frameBuffer = m_renderPass->createFrameBuffer( { size.getWidth(), size.getHeight() }
			, std::move( attachments ) );

		return true;
	}

	void PickingPass::doCleanup()
	{
		m_commandBuffer.reset();
		m_buffer.reset();
		m_pickingUbo.reset();
		m_scenes.clear();
		m_stagingBuffer.reset();
		m_frameBuffer.reset();
		m_colourView.reset();
		m_depthView.reset();
		m_depthTexture.reset();
		m_colourTexture.reset();
	}

	void PickingPass::doFillUboDescriptor( renderer::DescriptorSetLayout const & layout
		, uint32_t & index
		, BillboardListRenderNode & node )
	{
		node.uboDescriptorSet->createBinding( layout.getBinding( UboBindingPoint )
			, *m_pickingUbo );
	}

	void PickingPass::doFillUboDescriptor( renderer::DescriptorSetLayout const & layout
		, uint32_t & index
		, SubmeshRenderNode & node )
	{
		node.uboDescriptorSet->createBinding( layout.getBinding( UboBindingPoint )
			, *m_pickingUbo );
	}

	void PickingPass::doFillTextureDescriptor( renderer::DescriptorSetLayout const & layout
		, uint32_t & index
		, BillboardListRenderNode & node )
	{
		node.passNode.fillDescriptor( layout
			, index
			, *node.texDescriptorSet
			, true );
	}

	void PickingPass::doFillTextureDescriptor( renderer::DescriptorSetLayout const & layout
		, uint32_t & index
		, SubmeshRenderNode & node )
	{
		node.passNode.fillDescriptor( layout
			, index
			, *node.texDescriptorSet
			, true );
	}

	void PickingPass::doUpdate( RenderQueueArray & CU_PARAM_UNUSED( queues ) )
	{
	}

	glsl::Shader PickingPass::doGetGeometryShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags )const
	{
		return glsl::Shader{};
	}

	glsl::Shader PickingPass::doGetLegacyPixelShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, renderer::CompareOp alphaFunc )const
	{
		return doGetPixelShaderSource( passFlags
			, textureFlags
			, programFlags
			, sceneFlags
			, alphaFunc );
	}

	glsl::Shader PickingPass::doGetPbrMRPixelShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, renderer::CompareOp alphaFunc )const
	{
		return doGetPixelShaderSource( passFlags
			, textureFlags
			, programFlags
			, sceneFlags
			, alphaFunc );
	}

	glsl::Shader PickingPass::doGetPbrSGPixelShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, renderer::CompareOp alphaFunc )const
	{
		return doGetPixelShaderSource( passFlags
			, textureFlags
			, programFlags
			, sceneFlags
			, alphaFunc );
	}

	glsl::Shader PickingPass::doGetVertexShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, bool invertNormals )const
	{
		using namespace glsl;
		auto writer = getEngine()->getRenderSystem()->createGlslWriter();
		// Vertex inputs
		auto position = writer.declAttribute< Vec4 >( cuT( "position" )
			, RenderPass::VertexInputs::PositionLocation );
		auto texture = writer.declAttribute< Vec3 >( cuT( "texcoord" )
			, RenderPass::VertexInputs::TextureLocation );
		auto bone_ids0 = writer.declAttribute< IVec4 >( cuT( "bone_ids0" )
			, RenderPass::VertexInputs::BoneIds0Location
			, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto bone_ids1 = writer.declAttribute< IVec4 >( cuT( "bone_ids1" )
			, RenderPass::VertexInputs::BoneIds1Location
			, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto weights0 = writer.declAttribute< Vec4 >( cuT( "weights0" )
			, RenderPass::VertexInputs::Weights0Location
			, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto weights1 = writer.declAttribute< Vec4 >( cuT( "weights1" )
			, RenderPass::VertexInputs::Weights1Location
			, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto transform = writer.declAttribute< Mat4 >( cuT( "transform" )
			, RenderPass::VertexInputs::TransformLocation
			, checkFlag( programFlags, ProgramFlag::eInstantiation ) );
		auto material = writer.declAttribute< Int >( cuT( "material" )
			, RenderPass::VertexInputs::MaterialLocation
			, checkFlag( programFlags, ProgramFlag::eInstantiation ) );
		auto position2 = writer.declAttribute< Vec4 >( cuT( "position2" )
			, RenderPass::VertexInputs::Position2Location
			, checkFlag( programFlags, ProgramFlag::eMorphing ) );
		auto texture2 = writer.declAttribute< Vec3 >( cuT( "texture2" )
			, RenderPass::VertexInputs::Texture2Location
			, checkFlag( programFlags, ProgramFlag::eMorphing ) );
		auto gl_InstanceID( writer.declBuiltin< Int >( writer.getInstanceID() ) );

		UBO_MATRIX( writer, MatrixUbo::BindingPoint, 0 );
		UBO_MODEL_MATRIX( writer, ModelMatrixUbo::BindingPoint, 0 );
		UBO_MODEL( writer, ModelUbo::BindingPoint, 0 );
		SkinningUbo::declare( writer, SkinningUbo::BindingPoint, 0, programFlags );
		UBO_MORPHING( writer, MorphingUbo::BindingPoint, 0, programFlags );

		// Outputs
		auto vtx_texture = writer.declOutput< Vec3 >( cuT( "vtx_texture" )
			, RenderPass::VertexOutputs::TextureLocation );
		auto vtx_instance = writer.declOutput< Int >( cuT( "vtx_instance" )
			, RenderPass::VertexOutputs::InstanceLocation );
		auto vtx_material = writer.declOutput< Int >( cuT( "vtx_material" )
			, RenderPass::VertexOutputs::MaterialLocation );
		auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

		std::function< void() > main = [&]()
		{
			auto v4Vertex = writer.declLocale( cuT( "v4Vertex" )
				, vec4( position.xyz(), 1.0 ) );
			auto v3Texture = writer.declLocale( cuT( "v3Texture" )
				, texture );
			auto mtxModel = writer.declLocale< Mat4 >( cuT( "mtxModel" ) );

			if ( checkFlag( programFlags, ProgramFlag::eSkinning ) )
			{
				mtxModel = SkinningUbo::computeTransform( writer, programFlags );
			}
			else if ( checkFlag( programFlags, ProgramFlag::eInstantiation ) )
			{
				mtxModel = transform;
			}
			else
			{
				mtxModel = c3d_mtxModel;
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
				v4Vertex = vec4( glsl::fma( v4Vertex.xyz(), time, position2.xyz() * c3d_time ), 1.0 );
				v3Texture = glsl::fma( v3Texture, time, texture2 * c3d_time );
			}

			vtx_texture = v3Texture;
			v4Vertex = mtxModel * v4Vertex;
			v4Vertex = c3d_curView * v4Vertex;
			vtx_instance = gl_InstanceID;
			gl_Position = c3d_projection * v4Vertex;
		};

		writer.implementFunction< void >( cuT( "main" ), main );
		return writer.finalise();
	}

	glsl::Shader PickingPass::doGetPixelShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, renderer::CompareOp alphaFunc )const
	{
		using namespace glsl;
		GlslWriter writer = m_renderSystem.createGlslWriter();

		// UBOs
		auto materials = shader::createMaterials( writer
			, passFlags );
		materials->declare();
		Ubo uboPicking{ writer, Picking, 7u };
		auto c3d_iDrawIndex( uboPicking.declMember< UInt >( DrawIndex ) );
		auto c3d_iNodeIndex( uboPicking.declMember< UInt >( NodeIndex ) );
		uboPicking.end();

		// Fragment Intputs
		auto gl_PrimitiveID( writer.declBuiltin< UInt >( cuT( "gl_PrimitiveID" ) ) );
		auto vtx_texture = writer.declInput< Vec3 >( cuT( "vtx_texture" )
			, RenderPass::VertexOutputs::TextureLocation );
		auto vtx_instance = writer.declInput< Int >( cuT( "vtx_instance" )
			, RenderPass::VertexOutputs::InstanceLocation );
		auto vtx_material = writer.declInput< Int >( cuT( "vtx_material" )
			, RenderPass::VertexOutputs::MaterialLocation );
		auto c3d_mapOpacity( writer.declSampler< Sampler2D >( cuT( "c3d_mapOpacity" )
			, MinBufferIndex
			, 0u
			, checkFlag( textureFlags, TextureChannel::eOpacity ) ) );

		// Fragment Outputs
		auto pxl_fragColor( writer.declFragData< Vec4 >( cuT( "pxl_fragColor" ), 0 ) );

		writer.implementFunction< void >( cuT( "main" ), [&]()
		{
			auto material = materials->getBaseMaterial( vtx_material );
			auto alpha = writer.declLocale( cuT( "alpha" )
				, material->m_opacity() );

			if ( checkFlag( textureFlags, TextureChannel::eOpacity ) )
			{
				alpha *= texture( c3d_mapOpacity, vtx_texture.xy() ).r();
				shader::applyAlphaFunc( writer
					, alphaFunc
					, alpha
					, material->m_alphaRef() );
			}
			else if ( alphaFunc != renderer::CompareOp::eAlways )
			{
				shader::applyAlphaFunc( writer
					, alphaFunc
					, alpha
					, material->m_alphaRef() );
			}

			pxl_fragColor = vec4( c3d_iDrawIndex, c3d_iNodeIndex, vtx_instance, gl_PrimitiveID );
			pxl_fragColor = vec4( 1.0_f, 1.0, 1.0, 1.0 );
		} );

		return writer.finalise();
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

	renderer::DescriptorSetLayoutBindingArray PickingPass::doCreateUboBindings( PipelineFlags const & flags )const
	{
		renderer::DescriptorSetLayoutBindingArray result = RenderPass::doCreateUboBindings( flags );
		result.emplace_back( UboBindingPoint, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eFragment );
		return result;
	}

	renderer::DescriptorSetLayoutBindingArray PickingPass::doCreateTextureBindings( PipelineFlags const & flags )const
	{
		auto index = MinBufferIndex;
		renderer::DescriptorSetLayoutBindingArray textureBindings;

		if ( checkFlag( flags.m_textureFlags, TextureChannel::eOpacity ) )
		{
			textureBindings.emplace_back( index++, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment );
		}

		return textureBindings;
	}

	void PickingPass::doPrepareFrontPipeline( ShaderProgramSPtr program
		, renderer::VertexLayoutCRefArray const & layouts
		, PipelineFlags const & flags )
	{
	}

	void PickingPass::doPrepareBackPipeline( ShaderProgramSPtr program
		, renderer::VertexLayoutCRefArray const & layouts
		, PipelineFlags const & flags )
	{
		if ( m_backPipelines.find( flags ) == m_backPipelines.end() )
		{
			renderer::RasterisationState rsState;
			rsState.cullMode = renderer::CullModeFlag::eBack;
			renderer::DepthStencilState dsState{ 0u, true, false };
			auto bsState = renderer::ColourBlendState::createDefault();
			auto & pipeline = *m_backPipelines.emplace( flags
				, std::make_unique< RenderPipeline >( *getEngine()->getRenderSystem()
					, std::move( dsState )
					, std::move( rsState )
					, std::move( bsState )
					, renderer::MultisampleState{}
					, program
					, flags ) ).first->second;
			pipeline.setVertexLayouts( layouts );
			pipeline.setViewport( { m_colourTexture->getDimensions().width, m_colourTexture->getDimensions().height, 0, 0 } );
			pipeline.setScissor( { 0, 0, m_colourTexture->getDimensions().width, m_colourTexture->getDimensions().height } );

			auto initialise = [this, &pipeline, flags]()
			{
				auto uboBindings = doCreateUboBindings( flags );
				auto textureBindings = doCreateTextureBindings( flags );
				auto uboLayout = getEngine()->getRenderSystem()->getCurrentDevice()->createDescriptorSetLayout( std::move( uboBindings ) );
				auto texLayout = getEngine()->getRenderSystem()->getCurrentDevice()->createDescriptorSetLayout( std::move( textureBindings ) );
				std::vector< renderer::DescriptorSetLayoutPtr > layouts;
				layouts.emplace_back( std::move( uboLayout ) );
				layouts.emplace_back( std::move( texLayout ) );
				pipeline.setDescriptorSetLayouts( std::move( layouts ) );
				pipeline.initialise( getRenderPass(), renderer::PrimitiveTopology::eTriangleList );
			};

			if ( getEngine()->getRenderSystem()->hasCurrentDevice() )
			{
				initialise();
			}
			else
			{
				getEngine()->postEvent( makeFunctorEvent( EventType::ePreRender, initialise ) );
			}
		}
	}
}
