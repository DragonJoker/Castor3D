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

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		static String const Picking = cuT( "Picking" );
		static String const DrawIndex = cuT( "c3d_iDrawIndex" );
		static String const NodeIndex = cuT( "c3d_iNodeIndex" );

		template< bool Opaque, typename MapType, typename FuncType >
		inline void DoTraverseNodes( RenderPass const & p_pass
			, UniformBuffer & p_ubo
			, MapType & p_nodes
			, PickingPass::NodeType p_type
			, FuncType p_function )
		{
			auto depthMaps = DepthMapArray{};
			uint32_t count{ 1u };

			for ( auto itPipelines : p_nodes )
			{
				p_pass.UpdatePipeline( *itPipelines.first );
				itPipelines.first->Apply();
				auto drawIndex = p_ubo.GetUniform< UniformType::eUInt >( DrawIndex );
				auto nodeIndex = p_ubo.GetUniform< UniformType::eUInt >( NodeIndex );
				drawIndex->SetValue( uint8_t( p_type ) + ( ( count & 0x00FFFFFF ) << 8 ) );
				uint32_t index{ 0u };

				for ( auto itPass : itPipelines.second )
				{
					for ( auto itSubmeshes : itPass.second )
					{
						nodeIndex->SetValue( index++ );
						p_ubo.Update();
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
		inline void DoRenderNonInstanced( RenderPass const & p_pass
			, UniformBuffer & p_ubo
			, Scene & p_scene
			, PickingPass::NodeType p_type
			, MapType & p_nodes )
		{
			auto depthMaps = DepthMapArray{};
			uint32_t count{ 1u };

			for ( auto itPipelines : p_nodes )
			{
				p_pass.UpdatePipeline( *itPipelines.first );
				itPipelines.first->Apply();
				auto drawIndex = p_ubo.GetUniform< UniformType::eUInt >( DrawIndex );
				auto nodeIndex = p_ubo.GetUniform< UniformType::eUInt >( NodeIndex );
				drawIndex->SetValue( uint8_t( p_type ) + ( ( count & 0x00FFFFFF ) << 8 ) );
				uint32_t index{ 0u };

				for ( auto & renderNode : itPipelines.second )
				{
					nodeIndex->SetValue( index++ );
					p_ubo.Update();

					if ( renderNode.m_data.IsInitialised() )
					{
						DoRenderNodeNoPass( renderNode );
					}
				}

				count++;
			}
		}

		template< typename MapType, typename NodeType, typename SubNodeType >
		inline void DoPickFromList( MapType const & p_map
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
		inline void DoPickFromList< SubmeshStaticRenderNodesByPipelineMap, Geometry, Submesh >( SubmeshStaticRenderNodesByPipelineMap const & p_map
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
		, m_pickingUbo{ Picking, *engine.GetRenderSystem() }
	{
		m_pickingUbo.CreateUniform( UniformType::eUInt, DrawIndex );
		m_pickingUbo.CreateUniform( UniformType::eUInt, NodeIndex );
	}

	PickingPass::~PickingPass()
	{
	}

	void PickingPass::AddScene( Scene & p_scene, Camera & p_camera )
	{
		auto itScn = m_scenes.emplace( &p_scene, CameraQueueMap{} ).first;
		auto itCam = itScn->second.emplace( &p_camera, RenderQueue{ *this, m_opaque, nullptr } ).first;
		itCam->second.Initialise( p_scene, p_camera );
	}

	PickingPass::NodeType PickingPass::Pick( Position const & p_position
		, Camera const & p_camera )
	{
		NodeType result{ NodeType::eNone };
		m_geometry.reset();
		m_submesh.reset();
		m_face = 0u;

		auto itScn = m_scenes.find( p_camera.GetScene() );

		if ( itScn != m_scenes.end() )
		{
			auto itCam = itScn->second.find( &p_camera );

			if ( itCam != itScn->second.end() )
			{
				itCam->second.Update();
				auto & nodes = itCam->second.GetRenderNodes();
				auto pixel = DoFboPick( p_position, p_camera, nodes );
				result = DoPick( pixel, nodes );
			}
		}

		return result;
	}

	void PickingPass::DoRenderNodes( SceneRenderNodes & p_nodes
		, Camera const & p_camera )
	{
		m_matrixUbo.Update( p_camera.GetView()
			, p_camera.GetViewport().GetProjection() );
		DoRender( p_nodes.m_scene, p_nodes.m_instantiatedStaticNodes.m_backCulled );
		DoRender( p_nodes.m_scene, p_nodes.m_staticNodes.m_backCulled );
		DoRender( p_nodes.m_scene, p_nodes.m_skinnedNodes.m_backCulled );
		DoRender( p_nodes.m_scene, p_nodes.m_instantiatedSkinnedNodes.m_backCulled );
		DoRender( p_nodes.m_scene, p_nodes.m_morphingNodes.m_backCulled );
		DoRender( p_nodes.m_scene, p_nodes.m_billboardNodes.m_backCulled );
	}

	Point3f PickingPass::DoFboPick( Position const & p_position
		, Camera const & p_camera
		, SceneRenderNodes & p_nodes )
	{
		m_frameBuffer->Bind( FrameBufferTarget::eDraw );
		m_frameBuffer->Clear( BufferComponent::eColour | BufferComponent::eDepth );
		p_camera.Apply();
		m_pickingUbo.BindTo( 7u );
		DoRenderNodes( p_nodes, p_camera );
		m_frameBuffer->Unbind();

		Position offset
		{
			p_position.x() - PickingOffset,
			int32_t( p_camera.GetHeight() - p_position.y() - PickingOffset )
		};
		m_frameBuffer->Bind( FrameBufferTarget::eRead );
		m_colourAttach->Download( offset
			, *m_buffer );
		m_frameBuffer->Unbind();
		auto it = std::static_pointer_cast< PxBuffer< PixelFormat::eRGB32F > >( m_buffer )->begin();
		it += ( PickingOffset * PickingWidth ) + PickingOffset - 1;
		return Point3f{ reinterpret_cast< float const * >( it->const_ptr() ) };
	}

	PickingPass::NodeType PickingPass::DoPick( Point3f const & p_pixel
		, SceneRenderNodes & p_nodes )
	{
		NodeType result{ NodeType::eNone };

		if ( Castor::point::length_squared( p_pixel ) )
		{
			result = NodeType( uint32_t( p_pixel[0] ) & 0xFF );

			switch ( result )
			{
			case NodeType::eInstantiated:
				DoPickFromList( p_nodes.m_instantiatedStaticNodes.m_backCulled, p_pixel, m_geometry, m_submesh, m_face );
				break;

			case NodeType::eStatic:
				DoPickFromList( p_nodes.m_staticNodes.m_backCulled, p_pixel, m_geometry, m_submesh, m_face );
				break;

			case NodeType::eSkinning:
				DoPickFromList( p_nodes.m_skinnedNodes.m_backCulled, p_pixel, m_geometry, m_submesh, m_face );
				break;

			case NodeType::eMorphing:
				DoPickFromList( p_nodes.m_morphingNodes.m_backCulled, p_pixel, m_geometry, m_submesh, m_face );
				break;

			case NodeType::eBillboard:
				DoPickFromList( p_nodes.m_billboardNodes.m_backCulled, p_pixel, m_billboard, m_billboard, m_face );
				break;

			default:
				FAILURE( "Unsupported index" );
				result = NodeType::eNone;
				break;
			}
		}

		return result;
	}

	void PickingPass::DoRender( Scene & p_scene
		, SubmeshStaticRenderNodesByPipelineMap & p_nodes )
	{
		DoTraverseNodes< true >( *this
			, m_pickingUbo
			, p_nodes
			, NodeType::eInstantiated
			, [&p_scene, this]( RenderPipeline & p_pipeline
				, Pass & p_pass
				, Submesh & p_submesh
				, StaticRenderNodeArray & p_renderNodes )
			{
				if ( !p_renderNodes.empty() && p_submesh.HasMatrixBuffer() )
				{
					auto count = DoCopyNodesMatrices( p_renderNodes, p_submesh.GetMatrixBuffer() );
					p_submesh.DrawInstanced( p_renderNodes[0].m_buffers, count );
				}
			} );
	}

	void PickingPass::DoRender( Scene & p_scene
		, StaticRenderNodesByPipelineMap & p_nodes )
	{
		DoRenderNonInstanced< true >( *this
			, m_pickingUbo
			, p_scene
			, NodeType::eStatic
			, p_nodes );
	}

	void PickingPass::DoRender( Scene & p_scene
		, SkinningRenderNodesByPipelineMap & p_nodes )
	{
		DoRenderNonInstanced< true >( *this
			, m_pickingUbo
			, p_scene
			, NodeType::eSkinning
			, p_nodes );
	}
	
	void PickingPass::DoRender( Scene & p_scene
		, SubmeshSkinningRenderNodesByPipelineMap & p_nodes )
	{
		DoTraverseNodes< true >( *this
			, m_pickingUbo
			, p_nodes
			, NodeType::eInstantiated
			, [&p_scene, this]( RenderPipeline & p_pipeline
				, Pass & p_pass
				, Submesh & p_submesh
				, SkinningRenderNodeArray & p_renderNodes )
			{
				if ( !p_renderNodes.empty() && p_submesh.HasMatrixBuffer() )
				{
					auto count = DoCopyNodesBones( p_renderNodes, p_submesh.GetInstancedBonesBuffer() );
					p_submesh.DrawInstanced( p_renderNodes[0].m_buffers, count );
				}
			} );
	}

	void PickingPass::DoRender( Scene & p_scene
		, MorphingRenderNodesByPipelineMap & p_nodes )
	{
		DoRenderNonInstanced< true >( *this
			, m_pickingUbo
			, p_scene
			, NodeType::eMorphing
			, p_nodes );
	}

	void PickingPass::DoRender( Scene & p_scene
		, BillboardRenderNodesByPipelineMap & p_nodes )
	{
		DoRenderNonInstanced< true >( *this
			, m_pickingUbo
			, p_scene
			, NodeType::eBillboard
			, p_nodes );
	}

	bool PickingPass::DoInitialise( Size const & p_size )
	{
		m_colourTexture = GetEngine()->GetRenderSystem()->CreateTexture( TextureType::eTwoDimensions
			, AccessType::eRead
			, AccessType::eRead | AccessType::eWrite
			, PixelFormat::eRGB32F
			, p_size );
		m_buffer = PxBufferBase::create( Size{ PickingWidth, PickingWidth }
			, m_colourTexture->GetPixelFormat() );
		m_colourTexture->GetImage().InitialiseSource();
		auto size = m_colourTexture->GetDimensions();
		bool result = m_colourTexture->Initialise();

		if ( result )
		{
			m_frameBuffer = GetEngine()->GetRenderSystem()->CreateFrameBuffer();
			m_frameBuffer->SetClearColour( Colour::from_predef( PredefinedColour::eOpaqueBlack ) );
			m_depthBuffer = m_frameBuffer->CreateDepthStencilRenderBuffer( PixelFormat::eD32F );
			result = m_depthBuffer->Create();
		}

		if ( result )
		{
			result = m_depthBuffer->Initialise( size );

			if ( !result )
			{
				m_depthBuffer->Destroy();
			}
		}

		if ( result )
		{
			m_colourAttach = m_frameBuffer->CreateAttachment( m_colourTexture );
			m_depthAttach = m_frameBuffer->CreateAttachment( m_depthBuffer );
			result = m_frameBuffer->Create();
		}

		if ( result )
		{
			result = m_frameBuffer->Initialise( size );

			if ( result )
			{
				m_frameBuffer->Bind();
				m_frameBuffer->Attach( AttachmentPoint::eColour, 0, m_colourAttach, m_colourTexture->GetType() );
				m_frameBuffer->Attach( AttachmentPoint::eDepth, m_depthAttach );
				m_frameBuffer->SetDrawBuffer( m_colourAttach );
				result = m_frameBuffer->IsComplete();
				m_frameBuffer->Unbind();
			}
			else
			{
				m_frameBuffer->Destroy();
			}
		}

		return result;
	}

	void PickingPass::DoCleanup()
	{
		m_buffer.reset();
		m_pickingUbo.Cleanup();

		if ( m_frameBuffer )
		{
			m_frameBuffer->Bind();
			m_frameBuffer->DetachAll();
			m_frameBuffer->Unbind();
			m_frameBuffer->Cleanup();
			m_colourTexture->Cleanup();
			m_depthBuffer->Cleanup();

			m_depthBuffer->Destroy();
			m_frameBuffer->Destroy();

			m_depthAttach.reset();
			m_depthBuffer.reset();
			m_colourAttach.reset();
			m_colourTexture.reset();
			m_frameBuffer.reset();
		}
	}

	void PickingPass::DoUpdate( RenderQueueArray & CU_PARAM_UNUSED( p_queues ) )
	{
	}

	GLSL::Shader PickingPass::DoGetGeometryShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags )const
	{
		return GLSL::Shader{};
	}

	GLSL::Shader PickingPass::DoGetLegacyPixelShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc )const
	{
		return DoGetPixelShaderSource( textureFlags
			, programFlags
			, sceneFlags
			, alphaFunc );
	}

	GLSL::Shader PickingPass::DoGetPbrMRPixelShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc )const
	{
		return DoGetPixelShaderSource( textureFlags
			, programFlags
			, sceneFlags
			, alphaFunc );
	}

	GLSL::Shader PickingPass::DoGetPbrSGPixelShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc )const
	{
		return DoGetPixelShaderSource( textureFlags
			, programFlags
			, sceneFlags
			, alphaFunc );
	}

	GLSL::Shader PickingPass::DoGetPixelShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc )const
	{
		using namespace GLSL;
		GlslWriter writer = m_renderSystem.CreateGlslWriter();

		// UBOs
		Ubo uboPicking{ writer, Picking, 7u };
		auto c3d_iDrawIndex( uboPicking.DeclMember< UInt >( DrawIndex ) );
		auto c3d_iNodeIndex( uboPicking.DeclMember< UInt >( NodeIndex ) );
		uboPicking.End();

		// Fragment Intputs
		auto gl_PrimitiveID( writer.DeclBuiltin< UInt >( cuT( "gl_PrimitiveID" ) ) );
		auto vtx_instance = writer.DeclInput< Int >( cuT( "vtx_instance" ) );

		// Fragment Outputs
		auto pxl_v4FragColor( writer.DeclFragData< Vec4 >( cuT( "pxl_v4FragColor" ), 0 ) );

		writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			pxl_v4FragColor = vec4( c3d_iDrawIndex, c3d_iNodeIndex, vtx_instance, gl_PrimitiveID );
		} );

		return writer.Finalise();
	}

	void PickingPass::DoUpdateFlags( TextureChannels & textureFlags
		, ProgramFlags & programFlags
		, SceneFlags & sceneFlags )const
	{
		RemFlag( programFlags, ProgramFlag::eLighting );
		RemFlag( programFlags, ProgramFlag::eAlphaBlending );
		RemFlag( textureFlags, TextureChannel::eAll );

		AddFlag( programFlags, ProgramFlag::ePicking );
	}

	void PickingPass::DoUpdatePipeline( RenderPipeline & p_pipeline )const
	{
	}

	void PickingPass::DoPrepareFrontPipeline( ShaderProgram & p_program
		, PipelineFlags const & p_flags )
	{
	}

	void PickingPass::DoPrepareBackPipeline( ShaderProgram & p_program
		, PipelineFlags const & p_flags )
	{
		if ( m_backPipelines.find( p_flags ) == m_backPipelines.end() )
		{
			RasteriserState rsState;
			rsState.SetCulledFaces( Culling::eBack );
			DepthStencilState dsState;
			dsState.SetDepthTest( true );
			auto & pipeline = *m_backPipelines.emplace( p_flags
				, GetEngine()->GetRenderSystem()->CreateRenderPipeline( std::move( dsState )
					, std::move( rsState )
					, BlendState{}
					, MultisampleState{}
					, p_program
					, p_flags ) ).first->second;
			pipeline.AddUniformBuffer( m_matrixUbo.GetUbo() );
			pipeline.AddUniformBuffer( m_modelMatrixUbo.GetUbo() );
			pipeline.AddUniformBuffer( m_sceneUbo.GetUbo() );

			if ( CheckFlag( p_flags.m_programFlags, ProgramFlag::eBillboards ) )
			{
				pipeline.AddUniformBuffer( m_billboardUbo.GetUbo() );
			}

			if ( CheckFlag( p_flags.m_programFlags, ProgramFlag::eSkinning )
				&& !CheckFlag( p_flags.m_programFlags, ProgramFlag::eInstantiation ) )
			{
				pipeline.AddUniformBuffer( m_skinningUbo.GetUbo() );
			}

			if ( CheckFlag( p_flags.m_programFlags, ProgramFlag::eMorphing ) )
			{
				pipeline.AddUniformBuffer( m_morphingUbo.GetUbo() );
			}

			pipeline.AddUniformBuffer( m_pickingUbo );
		}
	}
}
