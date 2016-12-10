#include "PickingPass.hpp"

#include "Engine.hpp"
#include "Cache/ShaderCache.hpp"

#include "FrameBuffer/ColourRenderBuffer.hpp"
#include "FrameBuffer/DepthStencilRenderBuffer.hpp"
#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/RenderBufferAttachment.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Mesh/Submesh.hpp"
#include "Mesh/Buffer/BufferElementGroup.hpp"
#include "Mesh/Buffer/GeometryBuffers.hpp"
#include "Mesh/Buffer/VertexBuffer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Render/RenderTarget.hpp"
#include "Scene/BillboardList.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Geometry.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Shader/FrameVariableBuffer.hpp"
#include "State/DepthStencilState.hpp"
#include "State/MultisampleState.hpp"
#include "State/RasteriserState.hpp"
#include "Texture/TextureLayout.hpp"

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
			, Camera const & p_camera
			, MapType & p_nodes
			, PickingPass::NodeType p_type
			, FuncType p_function )
		{
			auto l_depthMaps = DepthMapArray{};
			uint32_t l_count{ 1u };

			for ( auto l_itPipelines : p_nodes )
			{
				p_pass.UpdatePipeline( p_camera, *l_itPipelines.first, l_depthMaps );
				l_itPipelines.first->Apply();
				FrameVariableBufferSPtr l_ubo = l_itPipelines.first->GetProgram().FindFrameVariableBuffer( Picking );
				OneUIntFrameVariableSPtr l_drawIndex;
				OneUIntFrameVariableSPtr l_nodeIndex;
				l_ubo->GetVariable( DrawIndex, l_drawIndex );
				l_ubo->GetVariable( NodeIndex, l_nodeIndex );
				l_drawIndex->SetValue( uint8_t( p_type ) + ( ( l_count & 0x00FFFFFF ) << 8 ) );
				uint32_t l_index{ 0u };

				for ( auto l_itPass : l_itPipelines.second )
				{
					for ( auto l_itSubmeshes : l_itPass.second )
					{
						l_nodeIndex->SetValue( l_index++ );
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
			, Scene & p_scene
			, Camera const & p_camera
			, PickingPass::NodeType p_type
			, MapType & p_nodes )
		{
			auto l_depthMaps = DepthMapArray{};
			uint32_t l_count{ 1u };

			for ( auto l_itPipelines : p_nodes )
			{
				p_pass.UpdatePipeline( p_camera, *l_itPipelines.first, l_depthMaps );
				l_itPipelines.first->Apply();
				FrameVariableBufferSPtr l_ubo = l_itPipelines.first->GetProgram().FindFrameVariableBuffer( Picking );
				OneUIntFrameVariableSPtr l_drawIndex;
				OneUIntFrameVariableSPtr l_nodeIndex;
				l_ubo->GetVariable( DrawIndex, l_drawIndex );
				l_ubo->GetVariable( NodeIndex, l_nodeIndex );
				l_drawIndex->SetValue( uint8_t( p_type ) + ( ( l_count & 0x00FFFFFF ) << 8 ) );
				uint32_t l_index{ 0u };

				for ( auto & l_renderNode : l_itPipelines.second )
				{
					l_nodeIndex->SetValue( l_index++ );

					if ( l_renderNode.m_data.IsInitialised() )
					{
						l_renderNode.Render( l_depthMaps );
					}
				}

				l_count++;
			}
		}

		inline void DoUpdateProgram( ShaderProgram & p_program )
		{
			if ( !p_program.FindFrameVariableBuffer( Picking ) )
			{
				auto & l_picking = p_program.CreateFrameVariableBuffer( Picking, ShaderTypeFlag::ePixel );
				l_picking.CreateVariable< OneUIntFrameVariable >( DrawIndex );
				l_picking.CreateVariable< OneUIntFrameVariable >( NodeIndex );

				if ( p_program.GetRenderSystem()->GetCurrentContext() )
				{
					p_program.Bind( false );
					l_picking.Initialise();
					p_program.Unbind();
				}
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
	}

	PickingPass::PickingPass( Engine & p_engine )
		: RenderPass{ cuT( "Picking" ), p_engine, true }
	{
	}

	PickingPass::~PickingPass()
	{
	}

	bool PickingPass::Initialise( Size const & p_size )
	{
		m_colourTexture = GetEngine()->GetRenderSystem()->CreateTexture( TextureType::eTwoDimensions, AccessType::eRead, AccessType::eRead | AccessType::eWrite, PixelFormat::eRGB32F, p_size );
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
				m_frameBuffer->Bind( FrameBufferMode::eConfig );
				m_frameBuffer->Attach( AttachmentPoint::eColour, 0, m_colourAttach, m_colourTexture->GetType() );
				m_frameBuffer->Attach( AttachmentPoint::eDepth, m_depthAttach );
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

	void PickingPass::Cleanup()
	{
		if ( m_frameBuffer )
		{
			m_frameBuffer->Bind( FrameBufferMode::eConfig );
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

		for ( auto l_buffer : m_geometryBuffers )
		{
			l_buffer->Cleanup();
		}

		m_geometryBuffers.clear();
	}

	void PickingPass::AddScene( Scene & p_scene, Camera & p_camera )
	{
		auto l_itScn = m_scenes.emplace( &p_scene, CameraQueueMap{} ).first;
		auto l_itCam = l_itScn->second.emplace( &p_camera, RenderQueue{ *this, m_opaque } ).first;
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

	AnimatedGeometryRenderNode PickingPass::CreateAnimatedNode( Pass & p_pass
		, RenderPipeline & p_pipeline
		, Submesh & p_submesh
		, Geometry & p_primitive
		, AnimatedSkeletonSPtr p_skeleton
		, AnimatedMeshSPtr p_mesh )
	{
		auto l_modelBuffer = p_pipeline.GetProgram().FindFrameVariableBuffer( ShaderProgram::BufferModel );
		OneIntFrameVariableSPtr l_receiver;
		auto l_animationBuffer = p_pipeline.GetProgram().FindFrameVariableBuffer( ShaderProgram::BufferAnimation );
		auto l_buffer = p_submesh.GetGeometryBuffers( p_pipeline.GetProgram() );
		m_geometryBuffers.insert( l_buffer );

		return AnimatedGeometryRenderNode
		{
			DoCreateSceneRenderNode( *p_primitive.GetScene(), p_pipeline ),
			DoCreatePassRenderNode( p_pass, p_pipeline ),
			*l_buffer,
			*p_primitive.GetParent(),
			*l_modelBuffer->GetVariable( ShaderProgram::ShadowReceiver, l_receiver ),
			p_submesh,
			p_primitive,
			p_skeleton.get(),
			p_mesh.get(),
			*l_animationBuffer
		};
	}

	StaticGeometryRenderNode PickingPass::CreateStaticNode( Pass & p_pass
		, RenderPipeline & p_pipeline
		, Submesh & p_submesh
		, Geometry & p_primitive )
	{
		auto l_modelBuffer = p_pipeline.GetProgram().FindFrameVariableBuffer( ShaderProgram::BufferModel );
		OneIntFrameVariableSPtr l_receiver;
		auto l_buffer = p_submesh.GetGeometryBuffers( p_pipeline.GetProgram() );
		m_geometryBuffers.insert( l_buffer );

		return StaticGeometryRenderNode
		{
			DoCreateSceneRenderNode( *p_primitive.GetScene(), p_pipeline ),
			DoCreatePassRenderNode( p_pass, p_pipeline ),
			*l_buffer,
			*p_primitive.GetParent(),
			*l_modelBuffer->GetVariable( ShaderProgram::ShadowReceiver, l_receiver ),
			p_submesh,
			p_primitive,
		};
	}

	BillboardRenderNode PickingPass::CreateBillboardNode( Pass & p_pass
		, RenderPipeline & p_pipeline
		, BillboardBase & p_billboard )
	{
		auto l_modelBuffer = p_pipeline.GetProgram().FindFrameVariableBuffer( ShaderProgram::BufferModel );
		OneIntFrameVariableSPtr l_receiver;
		auto l_billboardBuffer = p_pipeline.GetProgram().FindFrameVariableBuffer( ShaderProgram::BufferBillboards );
		Point2iFrameVariableSPtr l_pt2i;
		auto l_buffer = p_billboard.GetGeometryBuffers( p_pipeline.GetProgram() );
		m_geometryBuffers.insert( l_buffer );

		return BillboardRenderNode
		{
			DoCreateSceneRenderNode( p_billboard.GetParentScene(), p_pipeline ),
			DoCreatePassRenderNode( p_pass, p_pipeline ),
			*l_buffer,
			*p_billboard.GetNode(),
			*l_modelBuffer->GetVariable( ShaderProgram::ShadowReceiver, l_receiver ),
			p_billboard,
			*l_billboardBuffer,
			*l_billboardBuffer->GetVariable( ShaderProgram::Dimensions, l_pt2i ),
			*l_billboardBuffer->GetVariable( ShaderProgram::WindowSize, l_pt2i )
		};
	}

	void PickingPass::DoRenderNodes( SceneRenderNodes & p_nodes
		, Camera const & p_camera )
	{
		DoRenderInstancedSubmeshes( p_nodes.m_scene, p_camera, p_nodes.m_instancedGeometries.m_backCulled );
		DoRenderStaticSubmeshes( p_nodes.m_scene, p_camera, p_nodes.m_staticGeometries.m_backCulled );
		DoRenderAnimatedSubmeshes( p_nodes.m_scene, p_camera, p_nodes.m_animatedGeometries.m_backCulled );
		DoRenderBillboards( p_nodes.m_scene, p_camera, p_nodes.m_billboards.m_backCulled );
	}

	Point3f PickingPass::DoFboPick( Position const & p_position
		, Camera const & p_camera
		, SceneRenderNodes & p_nodes )
	{
		m_frameBuffer->Bind( FrameBufferMode::eAutomatic, FrameBufferTarget::eDraw );
		m_frameBuffer->Clear();
		p_camera.Apply();
		DoRenderNodes( p_nodes, p_camera );
		m_frameBuffer->Unbind();

		m_colourTexture->Bind( 0 );
		Point3f l_pixel;
		auto l_data = m_colourTexture->Lock( AccessType::eRead, 0u );

		if ( l_data )
		{
			auto l_dimensions = m_colourTexture->GetDimensions();
			auto l_format = m_colourTexture->GetPixelFormat();
			Image l_image{ cuT( "tmp" ), l_dimensions, l_format, l_data, l_format };
			l_image.GetPixel( p_position.x(), l_dimensions.height() - 1 - p_position.y(), reinterpret_cast< uint8_t * >( l_pixel.ptr() ), l_format );
			m_colourTexture->Unlock( false, 0u );
		}

		m_colourTexture->Unbind( 0 );
		return l_pixel;
	}

	PickingPass::NodeType PickingPass::DoPick( Point3f const & p_pixel
		, SceneRenderNodes & p_nodes )
	{
		NodeType l_return{ NodeType::eNone };

		if ( Castor::point::distance_squared( p_pixel ) )
		{
			l_return = NodeType( uint32_t( p_pixel[0] ) & 0xFF );

			switch ( l_return )
			{
			case NodeType::eInstantiated:
				DoPickFromList( p_nodes.m_instancedGeometries.m_backCulled, p_pixel, m_geometry, m_submesh, m_face );
				break;

			case NodeType::eStatic:
				DoPickFromList( p_nodes.m_staticGeometries.m_backCulled, p_pixel, m_geometry, m_submesh, m_face );
				break;

			case NodeType::eAnimated:
				DoPickFromList( p_nodes.m_animatedGeometries.m_backCulled, p_pixel, m_geometry, m_submesh, m_face );
				break;

			case NodeType::eBillboard:
				DoPickFromList( p_nodes.m_billboards.m_backCulled, p_pixel, m_billboard, m_billboard, m_face );
				break;

			default:
				FAILURE( "Unsupported index" );
				l_return = NodeType::eNone;
				break;
			}
		}

		return l_return;
	}

	void PickingPass::DoRenderInstancedSubmeshes( Scene & p_scene
		, Camera const & p_camera
		, SubmeshStaticRenderNodesByPipelineMap & p_nodes )
	{
		DoTraverseNodes< true >( *this
			, p_camera
			, p_nodes
			, NodeType::eInstantiated
			, [&p_scene, &p_camera, this]( RenderPipeline & p_pipeline
				, Pass & p_pass
				, Submesh & p_submesh
				, StaticGeometryRenderNodeArray & p_renderNodes )
			{
				if ( !p_renderNodes.empty() && p_submesh.HasMatrixBuffer() )
				{
					auto l_count = DoCopyNodesMatrices( p_renderNodes, p_submesh.GetMatrixBuffer() );
					auto l_depthMaps = DepthMapArray{};
					p_renderNodes[0].BindPass( l_depthMaps, MASK_MTXMODE_MODEL );
					p_submesh.DrawInstanced( p_renderNodes[0].m_buffers, l_count );
					p_renderNodes[0].UnbindPass( l_depthMaps );
				}
			} );
	}

	void PickingPass::DoRenderStaticSubmeshes( Scene & p_scene
		, Camera const & p_camera
		, StaticGeometryRenderNodesByPipelineMap & p_nodes )
	{
		DoRenderNonInstanced< true >( *this
			, p_scene
			, p_camera
			, NodeType::eStatic
			, p_nodes );
	}

	void PickingPass::DoRenderAnimatedSubmeshes( Scene & p_scene
		, Camera const & p_camera
		, AnimatedGeometryRenderNodesByPipelineMap & p_nodes )
	{
		DoRenderNonInstanced< true >( *this
			, p_scene
			, p_camera
			, NodeType::eAnimated
			, p_nodes );
	}

	void PickingPass::DoRenderBillboards( Scene & p_scene
		, Camera const & p_camera
		, BillboardRenderNodesByPipelineMap & p_nodes )
	{
		DoRenderNonInstanced< true >( *this
			, p_scene
			, p_camera
			, NodeType::eBillboard
			, p_nodes );
	}

	String PickingPass::DoGetGeometryShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, uint8_t p_sceneFlags )const
	{
		return String{};
	}

	String PickingPass::DoGetPixelShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, uint8_t p_sceneFlags )const
	{
		using namespace GLSL;
		GlslWriter l_writer = m_renderSystem.CreateGlslWriter();

		// UBOs
		UBO_MATRIX( l_writer );
		UBO_SCENE( l_writer );
		UBO_PASS( l_writer );
		UBO_MODEL( l_writer );

		Ubo l_uboPicking{ l_writer, Picking };
		auto c3d_iDrawIndex( l_uboPicking.GetUniform< UInt >( DrawIndex ) );
		auto c3d_iNodeIndex( l_uboPicking.GetUniform< UInt >( NodeIndex ) );
		l_uboPicking.End();

		// Fragment Intputs
		auto gl_PrimitiveID( l_writer.GetBuiltin< UInt >( cuT( "gl_PrimitiveID" ) ) );
		auto vtx_instance = l_writer.GetInput< Int >( cuT( "vtx_instance" ) );

		// Fragment Outputs
		auto pxl_v4FragColor( l_writer.GetFragData< Vec4 >( cuT( "pxl_v4FragColor" ), 0 ) );

		l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			pxl_v4FragColor = vec4( c3d_iDrawIndex, c3d_iNodeIndex, vtx_instance, gl_PrimitiveID );
		} );

		return l_writer.Finalise();
	}

	void PickingPass::DoUpdateFlags( TextureChannels & p_textureFlags
		, ProgramFlags & p_programFlags )const
	{
		RemFlag( p_programFlags, ProgramFlag::eLighting );
		RemFlag( p_programFlags, ProgramFlag::eAlphaBlending );
		RemFlag( p_textureFlags, TextureChannel::eAll );

		AddFlag( p_programFlags, ProgramFlag::ePicking );
	}

	void PickingPass::DoUpdatePipeline( RenderPipeline & p_pipeline
		, DepthMapArray & p_depthMaps )const
	{
	}

	void PickingPass::DoPrepareFrontPipeline( ShaderProgram & p_program
		, PipelineFlags const & p_flags )
	{
	}

	void PickingPass::DoPrepareBackPipeline( ShaderProgram & p_program
		, PipelineFlags const & p_flags )
	{
		auto l_it = m_backPipelines.find( p_flags );

		if ( l_it == m_backPipelines.end() )
		{
			DoUpdateProgram( p_program );
			RasteriserState l_rsState;
			l_rsState.SetCulledFaces( Culling::eBack );
			l_it = m_backPipelines.emplace( p_flags
				, GetEngine()->GetRenderSystem()->CreateRenderPipeline( DepthStencilState{}
					, std::move( l_rsState )
					, BlendState{}
					, MultisampleState{}
					, p_program
					, p_flags ) ).first;
		}
	}
}
