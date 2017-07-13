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
			auto l_depthMaps = DepthMapArray{};
			uint32_t l_count{ 1u };

			for ( auto l_itPipelines : p_nodes )
			{
				p_pass.UpdatePipeline( *l_itPipelines.first );
				l_itPipelines.first->Apply();
				auto l_drawIndex = p_ubo.GetUniform< UniformType::eUInt >( DrawIndex );
				auto l_nodeIndex = p_ubo.GetUniform< UniformType::eUInt >( NodeIndex );
				l_drawIndex->SetValue( uint8_t( p_type ) + ( ( l_count & 0x00FFFFFF ) << 8 ) );
				uint32_t l_index{ 0u };

				for ( auto l_itPass : l_itPipelines.second )
				{
					for ( auto l_itSubmeshes : l_itPass.second )
					{
						l_nodeIndex->SetValue( l_index++ );
						p_ubo.Update();
						p_function( *l_itPipelines.first
							, *l_itPass.first
							, *l_itSubmeshes.first
							, l_itSubmeshes.second );
					}
				}

				l_count++;
			}
		}

		template< bool Opaque, typename MapType >
		inline void DoRenderNonInstanced( RenderPass const & p_pass
			, UniformBuffer & p_ubo
			, Scene & p_scene
			, PickingPass::NodeType p_type
			, MapType & p_nodes )
		{
			auto l_depthMaps = DepthMapArray{};
			uint32_t l_count{ 1u };

			for ( auto l_itPipelines : p_nodes )
			{
				p_pass.UpdatePipeline( *l_itPipelines.first );
				l_itPipelines.first->Apply();
				auto l_drawIndex = p_ubo.GetUniform< UniformType::eUInt >( DrawIndex );
				auto l_nodeIndex = p_ubo.GetUniform< UniformType::eUInt >( NodeIndex );
				l_drawIndex->SetValue( uint8_t( p_type ) + ( ( l_count & 0x00FFFFFF ) << 8 ) );
				uint32_t l_index{ 0u };

				for ( auto & l_renderNode : l_itPipelines.second )
				{
					l_nodeIndex->SetValue( l_index++ );
					p_ubo.Update();

					if ( l_renderNode.m_data.IsInitialised() )
					{
						DoRenderNodeNoPass( l_renderNode );
					}
				}

				l_count++;
			}
		}

		template< typename MapType, typename NodeType, typename SubNodeType >
		inline void DoPickFromList( MapType const & p_map
			, Point3f const & p_index
			, std::weak_ptr< NodeType > & p_node
			, std::weak_ptr< SubNodeType > & p_subnode
			, uint32_t & p_face )
		{
			uint32_t l_pipelineIndex{ ( uint32_t( p_index[0] ) >> 8 ) - 1 };
			uint32_t l_nodeIndex{ uint32_t( p_index[1] ) };
			uint32_t l_faceIndex{ uint32_t( p_index[3] ) };

			REQUIRE( p_map.size() > l_pipelineIndex );
			auto l_itPipeline = p_map.begin();

			while ( l_pipelineIndex )
			{
				++l_itPipeline;
				--l_pipelineIndex;
			}

			REQUIRE( l_itPipeline->second.size() > l_nodeIndex );
			auto l_itNode = l_itPipeline->second.begin() + l_nodeIndex;

			p_subnode = std::static_pointer_cast< SubNodeType >( l_itNode->m_data.shared_from_this() );
			p_node = std::static_pointer_cast< NodeType >( l_itNode->m_instance.shared_from_this() );
			p_face = l_faceIndex;
		}

		template<>
		inline void DoPickFromList< SubmeshStaticRenderNodesByPipelineMap, Geometry, Submesh >( SubmeshStaticRenderNodesByPipelineMap const & p_map
			, Point3f const & p_index
			, GeometryWPtr & p_node
			, SubmeshWPtr & p_subnode
			, uint32_t & p_face )
		{
			uint32_t l_pipelineIndex{ ( uint32_t( p_index[0] ) >> 8 ) - 1 };
			uint32_t l_nodeIndex{ uint32_t( p_index[1] ) };
			uint32_t l_instanceIndex{ uint32_t( p_index[2] ) };
			uint32_t l_faceIndex{ uint32_t( p_index[3] ) };

			REQUIRE( p_map.size() > l_pipelineIndex );
			auto l_itPipeline = p_map.begin();

			while ( l_pipelineIndex )
			{
				++l_itPipeline;
				--l_pipelineIndex;
			}

			auto l_itPass = l_itPipeline->second.begin();
			REQUIRE( !l_itPass->second.empty() );
			auto l_itMesh = l_itPass->second.begin();

			while ( l_nodeIndex && l_itPass != l_itPipeline->second.end() )
			{
				while ( l_itMesh != l_itPass->second.end() && l_nodeIndex )
				{
					++l_itMesh;
					--l_nodeIndex;
				}

				if ( l_nodeIndex || l_itMesh == l_itPass->second.end() )
				{
					++l_itPass;

					if ( l_itPass != l_itPipeline->second.end() )
					{
						l_itMesh = l_itPass->second.begin();
					}
				}
			}

			if ( l_itPass != l_itPipeline->second.end()
					&& l_itMesh != l_itPass->second.end() )
			{
				REQUIRE( !l_itMesh->second.empty() );
				auto l_itNode = l_itMesh->second.begin() + l_instanceIndex;

				p_subnode = l_itNode->m_data.shared_from_this();
				p_node = std::static_pointer_cast< Geometry >( l_itNode->m_instance.shared_from_this() );
				p_face = l_faceIndex;
			}
		}

		static uint32_t constexpr PickingWidth = 50u;
		static int constexpr PickingOffset = int( PickingWidth / 2 );
	}

	PickingPass::PickingPass( Engine & p_engine )
		: RenderPass{ cuT( "Picking" ), p_engine, nullptr }
		, m_pickingUbo{ Picking, *p_engine.GetRenderSystem() }
	{
		m_pickingUbo.CreateUniform( UniformType::eUInt, DrawIndex );
		m_pickingUbo.CreateUniform( UniformType::eUInt, NodeIndex );
	}

	PickingPass::~PickingPass()
	{
	}

	void PickingPass::AddScene( Scene & p_scene, Camera & p_camera )
	{
		auto l_itScn = m_scenes.emplace( &p_scene, CameraQueueMap{} ).first;
		auto l_itCam = l_itScn->second.emplace( &p_camera, RenderQueue{ *this, m_opaque, nullptr } ).first;
		l_itCam->second.Initialise( p_scene, p_camera );
	}

	PickingPass::NodeType PickingPass::Pick( Position const & p_position
		, Camera const & p_camera )
	{
		NodeType l_return{ NodeType::eNone };
		m_geometry.reset();
		m_submesh.reset();
		m_face = 0u;

		auto l_itScn = m_scenes.find( p_camera.GetScene() );

		if ( l_itScn != m_scenes.end() )
		{
			auto l_itCam = l_itScn->second.find( &p_camera );

			if ( l_itCam != l_itScn->second.end() )
			{
				l_itCam->second.Update();
				auto & l_nodes = l_itCam->second.GetRenderNodes();
				auto l_pixel = DoFboPick( p_position, p_camera, l_nodes );
				l_return = DoPick( l_pixel, l_nodes );
			}
		}

		return l_return;
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

		Position l_offset
		{
			p_position.x() - PickingOffset,
			int32_t( p_camera.GetHeight() - p_position.y() - PickingOffset )
		};
		m_frameBuffer->Bind( FrameBufferTarget::eRead );
		m_colourAttach->Download( l_offset
			, *m_buffer );
		m_frameBuffer->Unbind();
		auto l_it = std::static_pointer_cast< PxBuffer< PixelFormat::eRGB32F > >( m_buffer )->begin();
		l_it += ( PickingOffset * PickingWidth ) + PickingOffset - 1;
		return Point3f{ reinterpret_cast< float const * >( l_it->const_ptr() ) };
	}

	PickingPass::NodeType PickingPass::DoPick( Point3f const & p_pixel
		, SceneRenderNodes & p_nodes )
	{
		NodeType l_return{ NodeType::eNone };

		if ( Castor::point::length_squared( p_pixel ) )
		{
			l_return = NodeType( uint32_t( p_pixel[0] ) & 0xFF );

			switch ( l_return )
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
				l_return = NodeType::eNone;
				break;
			}
		}

		return l_return;
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
					auto l_count = DoCopyNodesMatrices( p_renderNodes, p_submesh.GetMatrixBuffer() );
					p_submesh.DrawInstanced( p_renderNodes[0].m_buffers, l_count );
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
					auto l_count = DoCopyNodesBones( p_renderNodes, p_submesh.GetInstancedBonesBuffer() );
					p_submesh.DrawInstanced( p_renderNodes[0].m_buffers, l_count );
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
		auto l_size = m_colourTexture->GetDimensions();
		bool l_return = m_colourTexture->Initialise();

		if ( l_return )
		{
			m_frameBuffer = GetEngine()->GetRenderSystem()->CreateFrameBuffer();
			m_frameBuffer->SetClearColour( Colour::from_predef( PredefinedColour::eOpaqueBlack ) );
			m_depthBuffer = m_frameBuffer->CreateDepthStencilRenderBuffer( PixelFormat::eD32F );
			l_return = m_depthBuffer->Create();
		}

		if ( l_return )
		{
			l_return = m_depthBuffer->Initialise( l_size );

			if ( !l_return )
			{
				m_depthBuffer->Destroy();
			}
		}

		if ( l_return )
		{
			m_colourAttach = m_frameBuffer->CreateAttachment( m_colourTexture );
			m_depthAttach = m_frameBuffer->CreateAttachment( m_depthBuffer );
			l_return = m_frameBuffer->Create();
		}

		if ( l_return )
		{
			l_return = m_frameBuffer->Initialise( l_size );

			if ( l_return )
			{
				m_frameBuffer->Bind();
				m_frameBuffer->Attach( AttachmentPoint::eColour, 0, m_colourAttach, m_colourTexture->GetType() );
				m_frameBuffer->Attach( AttachmentPoint::eDepth, m_depthAttach );
				m_frameBuffer->SetDrawBuffer( m_colourAttach );
				l_return = m_frameBuffer->IsComplete();
				m_frameBuffer->Unbind();
			}
			else
			{
				m_frameBuffer->Destroy();
			}
		}

		return l_return;
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

	GLSL::Shader PickingPass::DoGetGeometryShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, SceneFlags const & p_sceneFlags )const
	{
		return GLSL::Shader{};
	}

	GLSL::Shader PickingPass::DoGetLegacyPixelShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, SceneFlags const & p_sceneFlags
		, ComparisonFunc p_alphaFunc )const
	{
		return DoGetPixelShaderSource( p_textureFlags
			, p_programFlags
			, p_sceneFlags
			, p_alphaFunc );
	}

	GLSL::Shader PickingPass::DoGetPbrPixelShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, SceneFlags const & p_sceneFlags
		, ComparisonFunc p_alphaFunc )const
	{
		return DoGetPixelShaderSource( p_textureFlags
			, p_programFlags
			, p_sceneFlags
			, p_alphaFunc );
	}

	GLSL::Shader PickingPass::DoGetPixelShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, SceneFlags const & p_sceneFlags
		, ComparisonFunc p_alphaFunc )const
	{
		using namespace GLSL;
		GlslWriter l_writer = m_renderSystem.CreateGlslWriter();

		// UBOs
		Ubo l_uboPicking{ l_writer, Picking, 7u };
		auto c3d_iDrawIndex( l_uboPicking.DeclMember< UInt >( DrawIndex ) );
		auto c3d_iNodeIndex( l_uboPicking.DeclMember< UInt >( NodeIndex ) );
		l_uboPicking.End();

		// Fragment Intputs
		auto gl_PrimitiveID( l_writer.DeclBuiltin< UInt >( cuT( "gl_PrimitiveID" ) ) );
		auto vtx_instance = l_writer.DeclInput< Int >( cuT( "vtx_instance" ) );

		// Fragment Outputs
		auto pxl_v4FragColor( l_writer.DeclFragData< Vec4 >( cuT( "pxl_v4FragColor" ), 0 ) );

		l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			pxl_v4FragColor = vec4( c3d_iDrawIndex, c3d_iNodeIndex, vtx_instance, gl_PrimitiveID );
		} );

		return l_writer.Finalise();
	}

	void PickingPass::DoUpdateFlags( TextureChannels & p_textureFlags
		, ProgramFlags & p_programFlags
		, SceneFlags & p_sceneFlags )const
	{
		RemFlag( p_programFlags, ProgramFlag::eLighting );
		RemFlag( p_programFlags, ProgramFlag::eAlphaBlending );
		RemFlag( p_textureFlags, TextureChannel::eAll );

		AddFlag( p_programFlags, ProgramFlag::ePicking );
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
			RasteriserState l_rsState;
			l_rsState.SetCulledFaces( Culling::eBack );
			DepthStencilState l_dsState;
			l_dsState.SetDepthTest( true );
			auto & l_pipeline = *m_backPipelines.emplace( p_flags
				, GetEngine()->GetRenderSystem()->CreateRenderPipeline( std::move( l_dsState )
					, std::move( l_rsState )
					, BlendState{}
					, MultisampleState{}
					, p_program
					, p_flags ) ).first->second;
			l_pipeline.AddUniformBuffer( m_matrixUbo.GetUbo() );
			l_pipeline.AddUniformBuffer( m_modelMatrixUbo.GetUbo() );
			l_pipeline.AddUniformBuffer( m_sceneUbo.GetUbo() );

			if ( CheckFlag( p_flags.m_programFlags, ProgramFlag::eBillboards ) )
			{
				l_pipeline.AddUniformBuffer( m_billboardUbo.GetUbo() );
			}

			if ( CheckFlag( p_flags.m_programFlags, ProgramFlag::eSkinning )
				&& !CheckFlag( p_flags.m_programFlags, ProgramFlag::eInstantiation ) )
			{
				l_pipeline.AddUniformBuffer( m_skinningUbo.GetUbo() );
			}

			if ( CheckFlag( p_flags.m_programFlags, ProgramFlag::eMorphing ) )
			{
				l_pipeline.AddUniformBuffer( m_morphingUbo.GetUbo() );
			}

			l_pipeline.AddUniformBuffer( m_pickingUbo );
		}
	}
}
