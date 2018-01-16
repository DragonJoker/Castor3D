#include "PickingPass.hpp"

#include "Cache/MaterialCache.hpp"
#include "FrameBuffer/ColourRenderBuffer.hpp"
#include "FrameBuffer/DepthStencilRenderBuffer.hpp"
#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/RenderBufferAttachment.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Shader/PassBuffer/PassBuffer.hpp"
#include "Shader/Shaders/GlslMaterial.hpp"
#include "Mesh/Submesh.hpp"
#include "Mesh/SubmeshComponent/InstantiationComponent.hpp"
#include "Mesh/Buffer/GeometryBuffers.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderNode/RenderNode_Render.hpp"
#include "Shader/ShaderProgram.hpp"

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
			, UniformBuffer & ubo
			, MapType & nodes
			, PickingPass::NodeType type
			, FuncType function )
		{
			uint32_t count{ 1u };

			for ( auto itPipelines : nodes )
			{
				pass.updatePipeline( *itPipelines.first );
				itPipelines.first->apply();
				auto drawIndex = ubo.getUniform< UniformType::eUInt >( DrawIndex );
				auto nodeIndex = ubo.getUniform< UniformType::eUInt >( NodeIndex );
				drawIndex->setValue( uint8_t( type ) + ( ( count & 0x00FFFFFF ) << 8 ) );
				uint32_t index{ 0u };

				for ( auto itPass : itPipelines.second )
				{
					for ( auto itSubmeshes : itPass.second )
					{
						nodeIndex->setValue( index++ );
						ubo.update();
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
			, UniformBuffer & ubo
			, Scene const & scene
			, PickingPass::NodeType type
			, MapType & nodes )
		{
			uint32_t count{ 1u };

			for ( auto itPipelines : nodes )
			{
				pass.updatePipeline( *itPipelines.first );
				itPipelines.first->apply();
				auto drawIndex = ubo.getUniform< UniformType::eUInt >( DrawIndex );
				auto nodeIndex = ubo.getUniform< UniformType::eUInt >( NodeIndex );
				drawIndex->setValue( uint8_t( type ) + ( ( count & 0x00FFFFFF ) << 8 ) );
				uint32_t index{ 0u };

				for ( auto & renderNode : itPipelines.second )
				{
					nodeIndex->setValue( index++ );
					ubo.update();

					if ( renderNode.m_data.isInitialised() )
					{
						doRenderNodeNoPass( renderNode );
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

		static uint32_t constexpr PickingWidth = 50u;
		static int constexpr PickingOffset = int( PickingWidth / 2 );
	}

	PickingPass::PickingPass( Engine & engine )
		: RenderPass{ cuT( "Picking" ), engine, nullptr }
		, m_pickingUbo{ Picking
			, *engine.getRenderSystem()
			, 7u }
	{
		m_pickingUbo.createUniform( UniformType::eUInt, DrawIndex );
		m_pickingUbo.createUniform( UniformType::eUInt, NodeIndex );
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
		doRender( nodes.m_scene, nodes.m_instantiatedStaticNodes.m_backCulled );
		doRender( nodes.m_scene, nodes.m_staticNodes.m_backCulled );
		doRender( nodes.m_scene, nodes.m_skinnedNodes.m_backCulled );
		doRender( nodes.m_scene, nodes.m_instantiatedSkinnedNodes.m_backCulled );
		doRender( nodes.m_scene, nodes.m_morphingNodes.m_backCulled );
		doRender( nodes.m_scene, nodes.m_billboardNodes.m_backCulled );
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

	void PickingPass::doRender( Scene const & scene
		, SubmeshStaticRenderNodesByPipelineMap & nodes )
	{
		doTraverseNodes< true >( *this
			, m_pickingUbo
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
					auto count = doCopyNodesMatrices( renderNodes, component.getMatrixBuffer() );
					submesh.drawInstanced( renderNodes[0].m_buffers, count );
				}
			} );
	}

	void PickingPass::doRender( Scene const & scene
		, StaticRenderNodesByPipelineMap & nodes )
	{
		doRenderNonInstanced< true >( *this
			, m_pickingUbo
			, scene
			, NodeType::eStatic
			, nodes );
	}

	void PickingPass::doRender( Scene const & scene
		, SkinningRenderNodesByPipelineMap & nodes )
	{
		doRenderNonInstanced< true >( *this
			, m_pickingUbo
			, scene
			, NodeType::eSkinning
			, nodes );
	}
	
	void PickingPass::doRender( Scene const & scene
		, SubmeshSkinningRenderNodesByPipelineMap & nodes )
	{
		doTraverseNodes< true >( *this
			, m_pickingUbo
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

	void PickingPass::doRender( Scene const & scene
		, MorphingRenderNodesByPipelineMap & nodes )
	{
		doRenderNonInstanced< true >( *this
			, m_pickingUbo
			, scene
			, NodeType::eMorphing
			, nodes );
	}

	void PickingPass::doRender( Scene const & scene
		, BillboardRenderNodesByPipelineMap & nodes )
	{
		doRenderNonInstanced< true >( *this
			, m_pickingUbo
			, scene
			, NodeType::eBillboard
			, nodes );
	}

	bool PickingPass::doInitialise( Size const & size )
	{
		m_colourTexture = getEngine()->getRenderSystem()->createTexture( TextureType::eTwoDimensions
			, AccessType::eRead
			, AccessType::eRead | AccessType::eWrite
			, PixelFormat::eRGB32F
			, size );
		m_buffer = PxBufferBase::create( Size{ PickingWidth, PickingWidth }
			, m_colourTexture->getPixelFormat() );
		m_colourTexture->getImage().initialiseSource();
		auto realSize = m_colourTexture->getDimensions();
		bool result = m_colourTexture->initialise();

		if ( result )
		{
			m_frameBuffer = getEngine()->getRenderSystem()->createFrameBuffer();
			m_frameBuffer->setClearColour( RgbaColour::fromPredefined( PredefinedRgbaColour::eOpaqueBlack ) );
			m_depthBuffer = m_frameBuffer->createDepthStencilRenderBuffer( PixelFormat::eD32F );
			result = m_depthBuffer->create();
		}

		if ( result )
		{
			result = m_depthBuffer->initialise( realSize );

			if ( !result )
			{
				m_depthBuffer->destroy();
			}
		}

		if ( result )
		{
			m_colourAttach = m_frameBuffer->createAttachment( m_colourTexture );
			m_depthAttach = m_frameBuffer->createAttachment( m_depthBuffer );
			result = m_frameBuffer->initialise();
		}

		if ( result )
		{
			m_frameBuffer->bind();
			m_frameBuffer->attach( AttachmentPoint::eColour, 0, m_colourAttach, m_colourTexture->getType() );
			m_frameBuffer->attach( AttachmentPoint::eDepth, m_depthAttach );
			m_frameBuffer->setDrawBuffer( m_colourAttach );
			result = m_frameBuffer->isComplete();
			m_frameBuffer->unbind();
		}

		return result;
	}

	void PickingPass::doCleanup()
	{
		m_buffer.reset();
		m_pickingUbo.cleanup();

		if ( m_frameBuffer )
		{
			m_frameBuffer->bind();
			m_frameBuffer->detachAll();
			m_frameBuffer->unbind();
			m_frameBuffer->cleanup();
			m_colourTexture->cleanup();
			m_depthBuffer->cleanup();

			m_depthBuffer->destroy();

			m_depthAttach.reset();
			m_depthBuffer.reset();
			m_colourAttach.reset();
			m_colourTexture.reset();
			m_frameBuffer.reset();
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
		, ComparisonFunc alphaFunc )const
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
		, ComparisonFunc alphaFunc )const
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
		, ComparisonFunc alphaFunc )const
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
		, ComparisonFunc alphaFunc )const
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
		auto vtx_texture = writer.declInput< Vec3 >( cuT( "vtx_texture" ) );
		auto vtx_material = writer.declInput< Int >( cuT( "vtx_material" ) );
		auto vtx_instance = writer.declInput< Int >( cuT( "vtx_instance" ) );
		auto c3d_mapOpacity( writer.declSampler< Sampler2D >( ShaderProgram::MapOpacity
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
			else if ( alphaFunc != ComparisonFunc::eAlways )
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

	void PickingPass::doPrepareFrontPipeline( ShaderProgram & program
		, PipelineFlags const & flags )
	{
	}

	void PickingPass::doPrepareBackPipeline( ShaderProgram & program
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
