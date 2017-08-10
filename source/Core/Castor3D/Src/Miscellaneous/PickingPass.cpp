#include "PickingPass.hpp"

#include "FrameBuffer/ColourRenderBuffer.hpp"
#include "FrameBuffer/DepthStencilRenderBuffer.hpp"
#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/RenderBufferAttachment.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Mesh/Submesh.hpp"
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
		inline void doTraverseNodes( RenderPass const & p_pass
			, UniformBuffer & p_ubo
			, MapType & p_nodes
			, PickingPass::NodeType p_type
			, FuncType p_function )
		{
			auto depthMaps = DepthMapArray{};
			uint32_t count{ 1u };

			for ( auto itPipelines : p_nodes )
			{
				p_pass.updatePipeline( *itPipelines.first );
				itPipelines.first->apply();
				auto drawIndex = p_ubo.getUniform< UniformType::eUInt >( DrawIndex );
				auto nodeIndex = p_ubo.getUniform< UniformType::eUInt >( NodeIndex );
				drawIndex->setValue( uint8_t( p_type ) + ( ( count & 0x00FFFFFF ) << 8 ) );
				uint32_t index{ 0u };

				for ( auto itPass : itPipelines.second )
				{
					for ( auto itSubmeshes : itPass.second )
					{
						nodeIndex->setValue( index++ );
						p_ubo.update();
						p_function( *itPipelines.first
							, *itPass.first
							, *itSubmeshes.first
							, itSubmeshes.second );
					}
				}

				count++;
			}
		}

		template< bool Opaque, typename MapType >
		inline void doRenderNonInstanced( RenderPass const & p_pass
			, UniformBuffer & p_ubo
			, Scene & p_scene
			, PickingPass::NodeType p_type
			, MapType & p_nodes )
		{
			auto depthMaps = DepthMapArray{};
			uint32_t count{ 1u };

			for ( auto itPipelines : p_nodes )
			{
				p_pass.updatePipeline( *itPipelines.first );
				itPipelines.first->apply();
				auto drawIndex = p_ubo.getUniform< UniformType::eUInt >( DrawIndex );
				auto nodeIndex = p_ubo.getUniform< UniformType::eUInt >( NodeIndex );
				drawIndex->setValue( uint8_t( p_type ) + ( ( count & 0x00FFFFFF ) << 8 ) );
				uint32_t index{ 0u };

				for ( auto & renderNode : itPipelines.second )
				{
					nodeIndex->setValue( index++ );
					p_ubo.update();

					if ( renderNode.m_data.isInitialised() )
					{
						doRenderNodeNoPass( renderNode );
					}
				}

				count++;
			}
		}

		template< typename MapType, typename NodeType, typename SubNodeType >
		inline void doPickFromList( MapType const & p_map
			, Point3f const & p_index
			, std::weak_ptr< NodeType > & p_node
			, std::weak_ptr< SubNodeType > & p_subnode
			, uint32_t & p_face )
		{
			uint32_t pipelineIndex{ ( uint32_t( p_index[0] ) >> 8 ) - 1 };
			uint32_t nodeIndex{ uint32_t( p_index[1] ) };
			uint32_t faceIndex{ uint32_t( p_index[3] ) };

			REQUIRE( p_map.size() > pipelineIndex );
			auto itPipeline = p_map.begin();

			while ( pipelineIndex )
			{
				++itPipeline;
				--pipelineIndex;
			}

			REQUIRE( itPipeline->second.size() > nodeIndex );
			auto itNode = itPipeline->second.begin() + nodeIndex;

			p_subnode = std::static_pointer_cast< SubNodeType >( itNode->m_data.shared_from_this() );
			p_node = std::static_pointer_cast< NodeType >( itNode->m_instance.shared_from_this() );
			p_face = faceIndex;
		}

		template<>
		inline void doPickFromList< SubmeshStaticRenderNodesByPipelineMap, Geometry, Submesh >( SubmeshStaticRenderNodesByPipelineMap const & p_map
			, Point3f const & p_index
			, GeometryWPtr & p_node
			, SubmeshWPtr & p_subnode
			, uint32_t & p_face )
		{
			uint32_t pipelineIndex{ ( uint32_t( p_index[0] ) >> 8 ) - 1 };
			uint32_t nodeIndex{ uint32_t( p_index[1] ) };
			uint32_t instanceIndex{ uint32_t( p_index[2] ) };
			uint32_t faceIndex{ uint32_t( p_index[3] ) };

			REQUIRE( p_map.size() > pipelineIndex );
			auto itPipeline = p_map.begin();

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
				REQUIRE( !itMesh->second.empty() );
				auto itNode = itMesh->second.begin() + instanceIndex;

				p_subnode = itNode->m_data.shared_from_this();
				p_node = std::static_pointer_cast< Geometry >( itNode->m_instance.shared_from_this() );
				p_face = faceIndex;
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

	void PickingPass::addScene( Scene & p_scene, Camera & p_camera )
	{
		auto itScn = m_scenes.emplace( &p_scene, CameraQueueMap{} ).first;
		auto itCam = itScn->second.emplace( &p_camera, RenderQueue{ *this, m_opaque, nullptr } ).first;
		itCam->second.initialise( p_scene, p_camera );
	}

	PickingPass::NodeType PickingPass::pick( Position const & p_position
		, Camera const & p_camera )
	{
		NodeType result{ NodeType::eNone };
		m_geometry.reset();
		m_submesh.reset();
		m_face = 0u;

		auto itScn = m_scenes.find( p_camera.getScene() );

		if ( itScn != m_scenes.end() )
		{
			auto itCam = itScn->second.find( &p_camera );

			if ( itCam != itScn->second.end() )
			{
				itCam->second.update();
				auto & nodes = itCam->second.getRenderNodes();
				auto pixel = doFboPick( p_position, p_camera, nodes );
				result = doPick( pixel, nodes );
			}
		}

		return result;
	}

	void PickingPass::doRenderNodes( SceneRenderNodes & p_nodes
		, Camera const & p_camera )
	{
		m_matrixUbo.update( p_camera.getView()
			, p_camera.getViewport().getProjection() );
		doRender( p_nodes.m_scene, p_nodes.m_instantiatedStaticNodes.m_backCulled );
		doRender( p_nodes.m_scene, p_nodes.m_staticNodes.m_backCulled );
		doRender( p_nodes.m_scene, p_nodes.m_skinnedNodes.m_backCulled );
		doRender( p_nodes.m_scene, p_nodes.m_instantiatedSkinnedNodes.m_backCulled );
		doRender( p_nodes.m_scene, p_nodes.m_morphingNodes.m_backCulled );
		doRender( p_nodes.m_scene, p_nodes.m_billboardNodes.m_backCulled );
	}

	Point3f PickingPass::doFboPick( Position const & p_position
		, Camera const & p_camera
		, SceneRenderNodes & p_nodes )
	{
		m_frameBuffer->bind( FrameBufferTarget::eDraw );
		m_frameBuffer->clear( BufferComponent::eColour | BufferComponent::eDepth );
		p_camera.apply();
		m_pickingUbo.bindTo( 7u );
		doRenderNodes( p_nodes, p_camera );
		m_frameBuffer->unbind();

		Position offset
		{
			p_position.x() - PickingOffset,
			int32_t( p_camera.getHeight() - p_position.y() - PickingOffset )
		};
		m_frameBuffer->bind( FrameBufferTarget::eRead );
		m_colourAttach->download( offset
			, *m_buffer );
		m_frameBuffer->unbind();
		auto it = std::static_pointer_cast< PxBuffer< PixelFormat::eRGB32F > >( m_buffer )->begin();
		it += ( PickingOffset * PickingWidth ) + PickingOffset - 1;
		return Point3f{ reinterpret_cast< float const * >( it->constPtr() ) };
	}

	PickingPass::NodeType PickingPass::doPick( Point3f const & p_pixel
		, SceneRenderNodes & p_nodes )
	{
		NodeType result{ NodeType::eNone };

		if ( castor::point::lengthSquared( p_pixel ) )
		{
			result = NodeType( uint32_t( p_pixel[0] ) & 0xFF );

			switch ( result )
			{
			case NodeType::eInstantiated:
				doPickFromList( p_nodes.m_instantiatedStaticNodes.m_backCulled, p_pixel, m_geometry, m_submesh, m_face );
				break;

			case NodeType::eStatic:
				doPickFromList( p_nodes.m_staticNodes.m_backCulled, p_pixel, m_geometry, m_submesh, m_face );
				break;

			case NodeType::eSkinning:
				doPickFromList( p_nodes.m_skinnedNodes.m_backCulled, p_pixel, m_geometry, m_submesh, m_face );
				break;

			case NodeType::eMorphing:
				doPickFromList( p_nodes.m_morphingNodes.m_backCulled, p_pixel, m_geometry, m_submesh, m_face );
				break;

			case NodeType::eBillboard:
				doPickFromList( p_nodes.m_billboardNodes.m_backCulled, p_pixel, m_billboard, m_billboard, m_face );
				break;

			default:
				FAILURE( "Unsupported index" );
				result = NodeType::eNone;
				break;
			}
		}

		return result;
	}

	void PickingPass::doRender( Scene & p_scene
		, SubmeshStaticRenderNodesByPipelineMap & p_nodes )
	{
		doTraverseNodes< true >( *this
			, m_pickingUbo
			, p_nodes
			, NodeType::eInstantiated
			, [&p_scene, this]( RenderPipeline & p_pipeline
				, Pass & p_pass
				, Submesh & p_submesh
				, StaticRenderNodeArray & p_renderNodes )
			{
				if ( !p_renderNodes.empty() && p_submesh.hasMatrixBuffer() )
				{
					auto count = doCopyNodesMatrices( p_renderNodes, p_submesh.getMatrixBuffer() );
					p_submesh.drawInstanced( p_renderNodes[0].m_buffers, count );
				}
			} );
	}

	void PickingPass::doRender( Scene & p_scene
		, StaticRenderNodesByPipelineMap & p_nodes )
	{
		doRenderNonInstanced< true >( *this
			, m_pickingUbo
			, p_scene
			, NodeType::eStatic
			, p_nodes );
	}

	void PickingPass::doRender( Scene & p_scene
		, SkinningRenderNodesByPipelineMap & p_nodes )
	{
		doRenderNonInstanced< true >( *this
			, m_pickingUbo
			, p_scene
			, NodeType::eSkinning
			, p_nodes );
	}
	
	void PickingPass::doRender( Scene & p_scene
		, SubmeshSkinningRenderNodesByPipelineMap & p_nodes )
	{
		doTraverseNodes< true >( *this
			, m_pickingUbo
			, p_nodes
			, NodeType::eInstantiated
			, [&p_scene, this]( RenderPipeline & p_pipeline
				, Pass & p_pass
				, Submesh & p_submesh
				, SkinningRenderNodeArray & p_renderNodes )
			{
				if ( !p_renderNodes.empty() && p_submesh.hasMatrixBuffer() )
				{
					auto count = doCopyNodesBones( p_renderNodes, p_submesh.getInstancedBonesBuffer() );
					p_submesh.drawInstanced( p_renderNodes[0].m_buffers, count );
				}
			} );
	}

	void PickingPass::doRender( Scene & p_scene
		, MorphingRenderNodesByPipelineMap & p_nodes )
	{
		doRenderNonInstanced< true >( *this
			, m_pickingUbo
			, p_scene
			, NodeType::eMorphing
			, p_nodes );
	}

	void PickingPass::doRender( Scene & p_scene
		, BillboardRenderNodesByPipelineMap & p_nodes )
	{
		doRenderNonInstanced< true >( *this
			, m_pickingUbo
			, p_scene
			, NodeType::eBillboard
			, p_nodes );
	}

	bool PickingPass::doInitialise( Size const & p_size )
	{
		m_colourTexture = getEngine()->getRenderSystem()->createTexture( TextureType::eTwoDimensions
			, AccessType::eRead
			, AccessType::eRead | AccessType::eWrite
			, PixelFormat::eRGB32F
			, p_size );
		m_buffer = PxBufferBase::create( Size{ PickingWidth, PickingWidth }
			, m_colourTexture->getPixelFormat() );
		m_colourTexture->getImage().initialiseSource();
		auto size = m_colourTexture->getDimensions();
		bool result = m_colourTexture->initialise();

		if ( result )
		{
			m_frameBuffer = getEngine()->getRenderSystem()->createFrameBuffer();
			m_frameBuffer->setClearColour( Colour::fromPredefined( PredefinedColour::eOpaqueBlack ) );
			m_depthBuffer = m_frameBuffer->createDepthStencilRenderBuffer( PixelFormat::eD32F );
			result = m_depthBuffer->create();
		}

		if ( result )
		{
			result = m_depthBuffer->initialise( size );

			if ( !result )
			{
				m_depthBuffer->destroy();
			}
		}

		if ( result )
		{
			m_colourAttach = m_frameBuffer->createAttachment( m_colourTexture );
			m_depthAttach = m_frameBuffer->createAttachment( m_depthBuffer );
			result = m_frameBuffer->create();
		}

		if ( result )
		{
			result = m_frameBuffer->initialise( size );

			if ( result )
			{
				m_frameBuffer->bind();
				m_frameBuffer->attach( AttachmentPoint::eColour, 0, m_colourAttach, m_colourTexture->getType() );
				m_frameBuffer->attach( AttachmentPoint::eDepth, m_depthAttach );
				m_frameBuffer->setDrawBuffer( m_colourAttach );
				result = m_frameBuffer->isComplete();
				m_frameBuffer->unbind();
			}
			else
			{
				m_frameBuffer->destroy();
			}
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
			m_frameBuffer->destroy();

			m_depthAttach.reset();
			m_depthBuffer.reset();
			m_colourAttach.reset();
			m_colourTexture.reset();
			m_frameBuffer.reset();
		}
	}

	void PickingPass::doUpdate( RenderQueueArray & CU_PARAM_UNUSED( p_queues ) )
	{
	}

	GLSL::Shader PickingPass::doGetGeometryShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags )const
	{
		return GLSL::Shader{};
	}

	GLSL::Shader PickingPass::doGetLegacyPixelShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc )const
	{
		return doGetPixelShaderSource( textureFlags
			, programFlags
			, sceneFlags
			, alphaFunc );
	}

	GLSL::Shader PickingPass::doGetPbrMRPixelShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc )const
	{
		return doGetPixelShaderSource( textureFlags
			, programFlags
			, sceneFlags
			, alphaFunc );
	}

	GLSL::Shader PickingPass::doGetPbrSGPixelShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc )const
	{
		return doGetPixelShaderSource( textureFlags
			, programFlags
			, sceneFlags
			, alphaFunc );
	}

	GLSL::Shader PickingPass::doGetPixelShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc )const
	{
		using namespace GLSL;
		GlslWriter writer = m_renderSystem.createGlslWriter();

		// UBOs
		Ubo uboPicking{ writer, Picking, 7u };
		auto c3d_iDrawIndex( uboPicking.declMember< UInt >( DrawIndex ) );
		auto c3d_iNodeIndex( uboPicking.declMember< UInt >( NodeIndex ) );
		uboPicking.end();

		// Fragment Intputs
		auto gl_PrimitiveID( writer.declBuiltin< UInt >( cuT( "gl_PrimitiveID" ) ) );
		auto vtx_instance = writer.declInput< Int >( cuT( "vtx_instance" ) );

		// Fragment Outputs
		auto pxl_v4FragColor( writer.declFragData< Vec4 >( cuT( "pxl_v4FragColor" ), 0 ) );

		writer.implementFunction< void >( cuT( "main" ), [&]()
		{
			pxl_v4FragColor = vec4( c3d_iDrawIndex, c3d_iNodeIndex, vtx_instance, gl_PrimitiveID );
		} );

		return writer.finalise();
	}

	void PickingPass::doUpdateFlags( TextureChannels & textureFlags
		, ProgramFlags & programFlags
		, SceneFlags & sceneFlags )const
	{
		remFlag( programFlags, ProgramFlag::eLighting );
		remFlag( programFlags, ProgramFlag::eAlphaBlending );
		remFlag( textureFlags, TextureChannel::eAll );

		addFlag( programFlags, ProgramFlag::ePicking );
	}

	void PickingPass::doUpdatePipeline( RenderPipeline & p_pipeline )const
	{
	}

	void PickingPass::doPrepareFrontPipeline( ShaderProgram & p_program
		, PipelineFlags const & p_flags )
	{
	}

	void PickingPass::doPrepareBackPipeline( ShaderProgram & p_program
		, PipelineFlags const & p_flags )
	{
		if ( m_backPipelines.find( p_flags ) == m_backPipelines.end() )
		{
			RasteriserState rsState;
			rsState.setCulledFaces( Culling::eBack );
			DepthStencilState dsState;
			dsState.setDepthTest( true );
			auto & pipeline = *m_backPipelines.emplace( p_flags
				, getEngine()->getRenderSystem()->createRenderPipeline( std::move( dsState )
					, std::move( rsState )
					, BlendState{}
					, MultisampleState{}
					, p_program
					, p_flags ) ).first->second;
			pipeline.addUniformBuffer( m_matrixUbo.getUbo() );
			pipeline.addUniformBuffer( m_modelMatrixUbo.getUbo() );
			pipeline.addUniformBuffer( m_sceneUbo.getUbo() );

			if ( checkFlag( p_flags.m_programFlags, ProgramFlag::eBillboards ) )
			{
				pipeline.addUniformBuffer( m_billboardUbo.getUbo() );
			}

			if ( checkFlag( p_flags.m_programFlags, ProgramFlag::eSkinning )
				&& !checkFlag( p_flags.m_programFlags, ProgramFlag::eInstantiation ) )
			{
				pipeline.addUniformBuffer( m_skinningUbo.getUbo() );
			}

			if ( checkFlag( p_flags.m_programFlags, ProgramFlag::eMorphing ) )
			{
				pipeline.addUniformBuffer( m_morphingUbo.getUbo() );
			}

			pipeline.addUniformBuffer( m_pickingUbo );
		}
	}
}
