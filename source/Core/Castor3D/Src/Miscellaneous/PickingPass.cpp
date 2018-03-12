#include "PickingPass.hpp"

#include "Cache/MaterialCache.hpp"
#include "Mesh/Submesh.hpp"
#include "Mesh/SubmeshComponent/InstantiationComponent.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderNode/RenderNode_Render.hpp"
#include "Shader/PassBuffer/PassBuffer.hpp"
#include "Shader/Shaders/GlslMaterial.hpp"

#include <RenderPass/FrameBuffer.hpp>
#include <RenderPass/FrameBufferAttachmentArray.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassCreateInfo.hpp>
#include <Shader/ShaderProgram.hpp>

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
				itPipelines.first->apply();
				auto & data = ubo.getData();
				data.drawIndex = uint8_t( type ) + ( ( count & 0x00FFFFFF ) << 8 );
				uint32_t index{ 0u };

				for ( auto itPass : itPipelines.second )
				{
					for ( auto itSubmeshes : itPass.second )
					{
						data.nodeIndex = index++;
						ubo->upload();
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
		inline void doRenderNonInstanced( RenderPass const & pass
			, renderer::UniformBuffer< PickingUboData > & ubo
			, Scene const & scene
			, PickingPass::NodeType type
			, MapType & nodes )
		{
			uint32_t count{ 1u };

			for ( auto itPipelines : nodes )
			{
				pass.updatePipeline( *itPipelines.first );
				itPipelines.first->apply();
				auto & data = ubo.getData();
				data.drawIndex = uint8_t( type ) + ( ( count & 0x00FFFFFF ) << 8 );
				uint32_t index{ 0u };

				for ( auto & renderNode : itPipelines.second )
				{
					data.nodeIndex = index++;
					ubo->upload();

					if ( renderNode.m_data.isInitialised() )
					{
						doUpdateNodeModelMatrix( renderNode );
					}
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

			subnode = std::static_pointer_cast< SubNodeType >( itNode->m_data.shared_from_this() );
			node = std::static_pointer_cast< NodeType >( itNode->m_instance.shared_from_this() );
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

				subnode = itNode->m_data.shared_from_this();
				node = std::static_pointer_cast< Geometry >( itNode->m_instance.shared_from_this() );
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
				auto & nodes = itCam->second.getRenderNodes();
				auto pixel = doFboPick( position, camera, nodes );
				result = doPick( pixel, nodes );
			}
		}

		return result;
	}

	void PickingPass::doRenderNodes( SceneRenderNodes & nodes
		, Camera const & camera )
	{
		m_matrixUbo.update( camera.getView()
			, camera.getViewport().getProjection() );
		doUpdate( nodes.m_scene, nodes.m_instantiatedStaticNodes.m_backCulled );
		doUpdate( nodes.m_scene, nodes.m_staticNodes.m_backCulled );
		doUpdate( nodes.m_scene, nodes.m_skinnedNodes.m_backCulled );
		doUpdate( nodes.m_scene, nodes.m_instantiatedSkinnedNodes.m_backCulled );
		doUpdate( nodes.m_scene, nodes.m_morphingNodes.m_backCulled );
		doUpdate( nodes.m_scene, nodes.m_billboardNodes.m_backCulled );
	}

	Point3f PickingPass::doFboPick( Position const & position
		, Camera const & camera
		, SceneRenderNodes & nodes )
	{
		m_frameBuffer->bind( FrameBufferTarget::eDraw );
		m_frameBuffer->clear( BufferComponent::eColour | BufferComponent::eDepth );
		getEngine()->getMaterialCache().getPassBuffer().bind();
		camera.apply();
		m_pickingUbo.bindTo( 7u );
		doRenderNodes( nodes, camera );
		m_frameBuffer->unbind();

		Position offset
		{
			position.x() - PickingOffset,
			int32_t( camera.getHeight() - position.y() - PickingOffset )
		};
		m_frameBuffer->bind( FrameBufferTarget::eRead );
		m_colourAttach->download( offset
			, *m_buffer );
		m_frameBuffer->unbind();
		auto it = std::static_pointer_cast< PxBuffer< PixelFormat::eRGB32F > >( m_buffer )->begin();
		it += ( PickingOffset * PickingWidth ) + PickingOffset - 1;
		return Point3f{ reinterpret_cast< float const * >( it->constPtr() ) };
	}

	PickingPass::NodeType PickingPass::doPick( Point3f const & pixel
		, SceneRenderNodes & nodes )
	{
		NodeType result{ NodeType::eNone };

		if ( castor::point::lengthSquared( pixel ) )
		{
			result = NodeType( uint32_t( pixel[0] ) & 0xFF );

			switch ( result )
			{
			case NodeType::eStatic:
				doPickFromList( nodes.m_staticNodes.m_backCulled, pixel, m_geometry, m_submesh, m_face );
				break;

			case NodeType::eInstantiatedStatic:
				doPickFromInstantiatedList( nodes.m_instantiatedStaticNodes.m_backCulled, pixel, m_geometry, m_submesh, m_face );
				break;

			case NodeType::eSkinning:
				doPickFromList( nodes.m_skinnedNodes.m_backCulled, pixel, m_geometry, m_submesh, m_face );
				break;

			case NodeType::eInstantiatedSkinning:
				doPickFromInstantiatedList( nodes.m_instantiatedSkinnedNodes.m_backCulled, pixel, m_geometry, m_submesh, m_face );
				break;

			case NodeType::eMorphing:
				doPickFromList( nodes.m_morphingNodes.m_backCulled, pixel, m_geometry, m_submesh, m_face );
				break;

			case NodeType::eBillboard:
				doPickFromList( nodes.m_billboardNodes.m_backCulled, pixel, m_billboard, m_billboard, m_face );
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
		, SubmeshStaticRenderNodesByPipelineMap & nodes )
	{
		doTraverseNodes< true >( *this
			, *m_pickingUbo
			, nodes
			, NodeType::eInstantiatedStatic
			, [&scene, this]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, InstantiationComponent & component
				, StaticRenderNodeArray & renderNodes )
			{
				if ( !renderNodes.empty() && component.hasMatrixBuffer() )
				{
					auto count = doCopyNodesMatrices( renderNodes, component.getData() );
					submesh.drawInstanced( renderNodes[0].m_buffers, count );
				}
			} );
	}

	void PickingPass::doUpdate( Scene const & scene
		, StaticRenderNodesByPipelineMap & nodes )
	{
		doRenderNonInstanced< true >( *this
			, *m_pickingUbo
			, scene
			, NodeType::eStatic
			, nodes );
	}

	void PickingPass::doUpdate( Scene const & scene
		, SkinningRenderNodesByPipelineMap & nodes )
	{
		doRenderNonInstanced< true >( *this
			, *m_pickingUbo
			, scene
			, NodeType::eSkinning
			, nodes );
	}
	
	void PickingPass::doUpdate( Scene const & scene
		, SubmeshSkinningRenderNodesByPipelineMap & nodes )
	{
		doTraverseNodes< true >( *this
			, *m_pickingUbo
			, nodes
			, NodeType::eInstantiatedSkinning
			, [&scene, this]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, InstantiationComponent & component
				, SkinningRenderNodeArray & renderNodes )
			{
				auto & instantiatedBones = submesh.getInstantiatedBones();

				if ( !renderNodes.empty()
					&& component.hasMatrixBuffer()
					&& instantiatedBones.hasInstancedBonesBuffer() )
				{
					auto count = doCopyNodesBones( renderNodes, instantiatedBones.getInstancedBonesBuffer() );
					submesh.drawInstanced( renderNodes[0].m_buffers, count );
				}
			} );
	}

	void PickingPass::doUpdate( Scene const & scene
		, MorphingRenderNodesByPipelineMap & nodes )
	{
		doRenderNonInstanced< true >( *this
			, *m_pickingUbo
			, scene
			, NodeType::eMorphing
			, nodes );
	}

	void PickingPass::doUpdate( Scene const & scene
		, BillboardRenderNodesByPipelineMap & nodes )
	{
		doRenderNonInstanced< true >( *this
			, *m_pickingUbo
			, scene
			, NodeType::eBillboard
			, nodes );
	}

	bool PickingPass::doInitialise( Size const & size )
	{
		auto & renderSystem = *getEngine()->getRenderSystem();
		auto & device = *renderSystem.getCurrentDevice();
		m_pickingUbo = renderer::makeUniformBuffer< PickingUboData >( device
			, 1u
			, 0u
			, renderer::MemoryPropertyFlag::eHostVisible );

		m_colourTexture = createTexture( device
			, size
			, renderer::Format::eR32G32B32A32_SFLOAT
			, renderer::ImageUsageFlag::eColourAttachment | renderer::ImageUsageFlag::eTransferSrc
			, renderer::MemoryPropertyFlag::eHostVisible );
		m_colourView = createView( *m_colourTexture );

		m_depthTexture = createTexture( device
			, size
			, renderer::Format::eD32_SFLOAT
			, renderer::ImageUsageFlag::eDepthStencilAttachment
			, renderer::MemoryPropertyFlag::eDeviceLocal );
		m_depthView = createView( *m_colourTexture );

		m_buffer = PxBufferBase::create( Size{ PickingWidth, PickingWidth }
			, convert( m_colourTexture->getFormat() ) );

		// Create the render pass.
		renderer::RenderPassCreateInfo renderPass;
		renderPass.flags = 0u;

		renderPass.attachments.resize( 2u );
		renderPass.attachments[0].index = 0u;
		renderPass.attachments[0].format = m_colourTexture->getFormat();
		renderPass.attachments[0].loadOp = renderer::AttachmentLoadOp::eClear;
		renderPass.attachments[0].storeOp = renderer::AttachmentStoreOp::eStore;
		renderPass.attachments[0].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
		renderPass.attachments[0].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
		renderPass.attachments[0].samples = renderer::SampleCountFlag::e1;
		renderPass.attachments[0].initialLayout = renderer::ImageLayout::eUndefined;
		renderPass.attachments[0].finalLayout = renderer::ImageLayout::eTransferDstOptimal;

		renderPass.attachments[1].index = 1u;
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
		renderPass.subpasses[0].colorAttachments.push_back( { 1u, renderer::ImageLayout::eDepthStencilAttachmentOptimal } );

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
		m_buffer.reset();
		m_pickingUbo.reset();

		if ( m_frameBuffer )
		{
			m_frameBuffer.reset();
			m_renderPass.reset();
			m_colourView.reset();
			m_colourTexture.reset();
			m_depthView.reset();
			m_depthTexture.reset();
		}
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

	glsl::Shader PickingPass::doGetPixelShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, renderer::CompareOp alphaFunc )const
	{
		using namespace glsl;
		GlslWriter writer = m_renderSystem.createGlslWriter();

		// UBOs
		Ubo uboPicking{ writer, Picking, 7u };
		auto c3d_iDrawIndex( uboPicking.declMember< UInt >( DrawIndex ) );
		auto c3d_iNodeIndex( uboPicking.declMember< UInt >( NodeIndex ) );
		uboPicking.end();
		auto materials = shader::createMaterials( writer
			, passFlags );
		materials->declare();

		// Fragment Intputs
		auto gl_PrimitiveID( writer.declBuiltin< UInt >( cuT( "gl_PrimitiveID" ) ) );
		auto vtx_texture = writer.declInput< Vec3 >( cuT( "vtx_texture" ), 0u );
		auto vtx_material = writer.declInput< Int >( cuT( "vtx_material" ), 1u );
		auto vtx_instance = writer.declInput< Int >( cuT( "vtx_instance" ), 2u );
		auto c3d_mapOpacity( writer.declSampler< Sampler2D >( cuT( "c3d_mapOpacity" )
			, MinTextureIndex
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

	void PickingPass::doPrepareFrontPipeline( renderer::ShaderStageStateArray & program
		, PipelineFlags const & flags )
	{
	}

	void PickingPass::doPrepareBackPipeline( renderer::ShaderStageStateArray & program
		, PipelineFlags const & flags )
	{
		if ( m_backPipelines.find( flags ) == m_backPipelines.end() )
		{
			RasteriserState rsState;
			rsState.setCulledFaces( Culling::eBack );
			DepthStencilState dsState;
			dsState.setDepthTest( true );
			auto & pipeline = *m_backPipelines.emplace( flags
				, getEngine()->getRenderSystem()->createRenderPipeline( std::move( dsState )
					, std::move( rsState )
					, BlendState{}
					, MultisampleState{}
					, program
					, flags ) ).first->second;
			pipeline.addUniformBuffer( m_matrixUbo.getUbo() );
			pipeline.addUniformBuffer( m_modelMatrixUbo.getUbo() );
			pipeline.addUniformBuffer( m_sceneUbo.getUbo() );

			if ( checkFlag( flags.m_programFlags, ProgramFlag::eBillboards ) )
			{
				pipeline.addUniformBuffer( m_billboardUbo.getUbo() );
			}

			if ( checkFlag( flags.m_programFlags, ProgramFlag::eSkinning )
				&& !checkFlag( flags.m_programFlags, ProgramFlag::eInstantiation ) )
			{
				pipeline.addUniformBuffer( m_skinningUbo.getUbo() );
			}

			if ( checkFlag( flags.m_programFlags, ProgramFlag::eMorphing ) )
			{
				pipeline.addUniformBuffer( m_morphingUbo.getUbo() );
			}

			pipeline.addUniformBuffer( m_pickingUbo );
		}
	}
}
