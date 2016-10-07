#include "ShadowMapPass.hpp"

#include "Engine.hpp"
#include "ShaderCache.hpp"

#include "FrameBuffer/ColourRenderBuffer.hpp"
#include "FrameBuffer/DepthStencilRenderBuffer.hpp"
#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/RenderBufferAttachment.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Mesh/Submesh.hpp"
#include "Mesh/Buffer/BufferElementGroup.hpp"
#include "Mesh/Buffer/GeometryBuffers.hpp"
#include "Mesh/Buffer/VertexBuffer.hpp"
#include "Render/Pipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Render/RenderTarget.hpp"
#include "Scene/BillboardList.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Geometry.hpp"
#include "Scene/Light/Light.hpp"
#include "Scene/Light/DirectionalLight.hpp"
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
		static String const ShadowMap = cuT( "ShadowMap" );

		template< typename MapType, typename FuncType >
		void DoTraverseNodes( Camera const & p_camera
							  , MapType & p_nodes
							  , uint8_t p_index
							  , FuncType p_function )
		{
			uint32_t l_count{ 1u };

			for ( auto l_itPipelines : p_nodes )
			{
				l_itPipelines.first->SetProjectionMatrix( p_camera.GetViewport().GetProjection() );
				l_itPipelines.first->SetViewMatrix( p_camera.GetView() );
				l_itPipelines.first->Apply();

				for ( auto l_itPass : l_itPipelines.second )
				{
					for ( auto l_itSubmeshes : l_itPass.second )
					{
						p_function( *l_itPipelines.first, *l_itPass.first, *l_itSubmeshes.first, l_itSubmeshes.second );
					}
				}

				l_count++;
			}
		}

		template< typename MapType >
		void DoRenderNonInstanced( Scene & p_scene
								   , Camera const & p_camera
								   , uint8_t p_index
								   , MapType & p_nodes )
		{
			uint32_t l_count{ 1u };

			for ( auto l_itPipelines : p_nodes )
			{
				l_itPipelines.first->SetProjectionMatrix( p_camera.GetViewport().GetProjection() );
				l_itPipelines.first->SetViewMatrix( p_camera.GetView() );
				l_itPipelines.first->Apply();

				for ( auto & l_renderNode : l_itPipelines.second )
				{
					if ( l_renderNode.m_data.IsInitialised() )
					{
						l_renderNode.Render( p_scene, p_camera );
					}
				}

				l_count++;
			}
		}
	}

	ShadowMapPass::ShadowMapPass( Engine & p_engine, Scene & p_scene, Light const & p_light )
		: RenderPass{ ShadowMap, p_engine }
		, m_light{ p_light }
		, m_scene{ p_scene }
	{
		m_renderQueue.AddScene( m_scene );
	}

	ShadowMapPass::~ShadowMapPass()
	{
	}

	bool ShadowMapPass::Initialise( Size const & p_size )
	{
		bool l_return = true;

		if ( !m_frameBuffer )
		{
			m_frameBuffer = GetEngine()->GetRenderSystem()->CreateFrameBuffer();
			m_frameBuffer->SetClearColour( Colour::from_predef( Colour::Predefined::OpaqueBlack ) );
			m_depthTexture = GetEngine()->GetRenderSystem()->CreateTexture( TextureType::TwoDimensions, AccessType::Read, AccessType::ReadWrite );
			m_depthTexture->GetImage().SetSource( p_size, PixelFormat::D32F );
			auto l_size = m_depthTexture->GetImage().GetDimensions();

			l_return = m_depthTexture->Create();

			if ( l_return )
			{
				l_return = m_depthTexture->Initialise();

				if ( !l_return )
				{
					m_depthTexture->Destroy();
				}
			}

			if ( l_return )
			{
				m_depthAttach = m_frameBuffer->CreateAttachment( m_depthTexture );
				l_return = m_frameBuffer->Create();
			}

			if ( l_return )
			{
				l_return = m_frameBuffer->Initialise( l_size );

				if ( l_return && m_frameBuffer->Bind( FrameBufferMode::Config ) )
				{
					m_frameBuffer->Attach( AttachmentPoint::Depth, 0, m_depthAttach, m_depthTexture->GetType() );
					l_return = m_frameBuffer->IsComplete();
					m_frameBuffer->Unbind();
				}
				else
				{
					m_frameBuffer->Destroy();
				}
			}

			if ( l_return )
			{
				Viewport l_viewport{ *GetEngine() };

				switch ( m_light.GetLightType() )
				{
				case eLIGHT_TYPE_DIRECTIONAL:
					l_viewport.SetOrtho( -512.0_r, 511.0_r, -512.0_r, 511.0_r, 1.0_r, 1000.0_r );
					break;
				}

				m_camera = std::make_unique< Camera >( cuT( "ShadowMap_" ) + m_light.GetName(), m_scene, nullptr, l_viewport );
				m_camera->Resize( p_size );
			}
		}

		return l_return;
	}

	void ShadowMapPass::Cleanup()
	{
		if ( m_frameBuffer )
		{
			m_camera.reset();

			m_frameBuffer->Bind( FrameBufferMode::Config );
			m_frameBuffer->DetachAll();
			m_frameBuffer->Unbind();
			m_frameBuffer->Cleanup();
			m_depthTexture->Cleanup();

			m_depthTexture->Destroy();
			m_frameBuffer->Destroy();

			m_depthAttach.reset();
			m_depthTexture.reset();
			m_frameBuffer.reset();
		}

		for ( auto l_buffer : m_geometryBuffers )
		{
			l_buffer->Cleanup();
		}

		m_geometryBuffers.clear();
	}
	
	void ShadowMapPass::Update()
	{
		m_renderQueue.Prepare( *m_camera, m_scene );
		Point3r l_position;

		switch ( m_light.GetLightType() )
		{
		case eLIGHT_TYPE_DIRECTIONAL:
			l_position = -m_light.GetDirectionalLight()->GetDirection();
			break;
		}

		matrix::look_at( m_view, l_position, Point3r{ 0, 0, 0 }, Point3r{ 0, 1, 0 } );
		m_camera->GetViewport().Update();
		m_camera->SetView( m_view );
	}

	void ShadowMapPass::Render()
	{
		if ( m_frameBuffer->Bind( FrameBufferMode::Automatic, FrameBufferTarget::Draw ) )
		{
			m_frameBuffer->Clear();
			auto & l_nodes = m_renderQueue.GetRenderNodes( *m_camera, m_scene );
			DoRenderOpaqueNodes( l_nodes, *m_camera );
			DoRenderTransparentNodes( l_nodes, *m_camera );
			m_frameBuffer->Unbind();

#if DEBUG_BUFFERS

			if ( m_depthTexture->Bind( 0 ) )
			{
				auto l_data = m_depthTexture->Lock( 0, AccessType::Read );

				if ( l_data )
				{
					auto l_dimensions = m_depthTexture->GetImage().GetDimensions();
					Image l_image{ cuT( "tmp" ), l_dimensions, m_depthTexture->GetImage().GetPixelFormat(), l_data, m_depthTexture->GetImage().GetPixelFormat() };
					Image::BinaryWriter()( l_image, Engine::GetEngineDirectory() / cuT( "\\ColourBuffer_Picking.hdr" ) );
				}

				m_depthTexture->Unbind( 0 );
			}

#endif
		}
	}

	AnimatedGeometryRenderNode ShadowMapPass::CreateAnimatedNode( Pass & p_pass
															   , Pipeline & p_pipeline
															   , Submesh & p_submesh
															   , Geometry & p_primitive
															   , AnimatedSkeletonSPtr p_skeleton
															   , AnimatedMeshSPtr p_mesh )
	{
		auto l_animationBuffer = p_pipeline.GetProgram().FindFrameVariableBuffer( ShaderProgram::BufferAnimation );
		auto l_buffer = p_submesh.GetGeometryBuffers( p_pipeline.GetProgram() );
		m_geometryBuffers.insert( l_buffer );

		return AnimatedGeometryRenderNode
		{
			DoCreateSceneRenderNode( *p_primitive.GetScene(), p_pipeline ),
			DoCreatePassRenderNode( p_pass, p_pipeline ),
			*l_buffer,
			*p_primitive.GetParent(),
			p_submesh,
			p_primitive,
			p_skeleton.get(),
			p_mesh.get(),
			*l_animationBuffer
		};
	}

	StaticGeometryRenderNode ShadowMapPass::CreateStaticNode( Pass & p_pass
														   , Pipeline & p_pipeline
														   , Submesh & p_submesh
														   , Geometry & p_primitive )
	{
		auto l_buffer = p_submesh.GetGeometryBuffers( p_pipeline.GetProgram() );
		m_geometryBuffers.insert( l_buffer );

		return StaticGeometryRenderNode
		{
			DoCreateSceneRenderNode( *p_primitive.GetScene(), p_pipeline ),
			DoCreatePassRenderNode( p_pass, p_pipeline ),
			*l_buffer,
			*p_primitive.GetParent(),
			p_submesh,
			p_primitive,
		};
	}

	BillboardRenderNode ShadowMapPass::CreateBillboardNode( Pass & p_pass
														 , Pipeline & p_pipeline
														 , BillboardList & p_billboard )
	{
		auto l_billboardBuffer = p_pipeline.GetProgram().FindFrameVariableBuffer( ShaderProgram::BufferBillboards );
		Point2iFrameVariableSPtr l_pt2i;
		auto l_buffer = p_billboard.GetGeometryBuffers( p_pipeline.GetProgram() );
		m_geometryBuffers.insert( l_buffer );

		return BillboardRenderNode
		{
			DoCreateSceneRenderNode( *p_billboard.GetScene(), p_pipeline ),
			DoCreatePassRenderNode( p_pass, p_pipeline ),
			*l_buffer,
			*p_billboard.GetParent(),
			p_billboard,
			*l_billboardBuffer,
			*l_billboardBuffer->GetVariable( ShaderProgram::Dimensions, l_pt2i )
		};
	}

	void ShadowMapPass::DoRenderOpaqueNodes( SceneRenderNodes & p_nodes, Camera const & p_camera )
	{
		DoRenderInstancedSubmeshesInstanced( p_nodes.m_scene, p_camera, 0u, p_nodes.m_instancedGeometries.m_opaqueRenderNodes );
		DoRenderStaticSubmeshesNonInstanced( p_nodes.m_scene, p_camera, 2u, p_nodes.m_staticGeometries.m_opaqueRenderNodes );
		DoRenderAnimatedSubmeshesNonInstanced( p_nodes.m_scene, p_camera, 4u, p_nodes.m_animatedGeometries.m_opaqueRenderNodes );
		DoRenderBillboards( p_nodes.m_scene, p_camera, 6u, p_nodes.m_billboards.m_opaqueRenderNodes );
	}

	void ShadowMapPass::DoRenderTransparentNodes( SceneRenderNodes & p_nodes, Camera const & p_camera )
	{
		DoRenderInstancedSubmeshesInstanced( p_nodes.m_scene, p_camera, 1u, p_nodes.m_instancedGeometries.m_transparentRenderNodesBack );
		DoRenderStaticSubmeshesNonInstanced( p_nodes.m_scene, p_camera, 3u, p_nodes.m_staticGeometries.m_transparentRenderNodesBack );
		DoRenderAnimatedSubmeshesNonInstanced( p_nodes.m_scene, p_camera, 5u, p_nodes.m_animatedGeometries.m_transparentRenderNodesBack );
		DoRenderBillboards( p_nodes.m_scene, p_camera, 7u, p_nodes.m_billboards.m_transparentRenderNodesBack );
	}

	void ShadowMapPass::DoRenderInstancedSubmeshesInstanced( Scene & p_scene, Camera const & p_camera, uint8_t p_index, SubmeshStaticRenderNodesByPipelineMap & p_nodes )
	{
		DoTraverseNodes( p_camera, p_nodes, p_index, [&p_scene, &p_camera, this]( Pipeline & p_pipeline, Pass & p_pass, Submesh & p_submesh, StaticGeometryRenderNodeArray & p_renderNodes )
		{
			if ( !p_renderNodes.empty() && p_submesh.HasMatrixBuffer() )
			{
				uint32_t l_count = uint32_t( p_renderNodes.size() );
				uint8_t * l_buffer = p_submesh.GetMatrixBuffer().data();
				const uint32_t l_stride = 16 * sizeof( real );

				for ( auto const & l_renderNode : p_renderNodes )
				{
					std::memcpy( l_buffer, l_renderNode.m_sceneNode.GetDerivedTransformationMatrix().const_ptr(), l_stride );
					l_buffer += l_stride;
				}

				p_renderNodes[0].BindPass( p_scene, p_camera, MASK_MTXMODE_MODEL );
				p_submesh.DrawInstanced( p_renderNodes[0].m_buffers, l_count );
				p_renderNodes[0].UnbindPass( p_scene );
			}
		} );
	}

	void ShadowMapPass::DoRenderStaticSubmeshesNonInstanced( Scene & p_scene, Camera const & p_camera, uint8_t p_index, StaticGeometryRenderNodesByPipelineMap & p_nodes )
	{
		DoRenderNonInstanced( p_scene, p_camera, p_index, p_nodes );
	}

	void ShadowMapPass::DoRenderAnimatedSubmeshesNonInstanced( Scene & p_scene, Camera const & p_camera, uint8_t p_index, AnimatedGeometryRenderNodesByPipelineMap & p_nodes )
	{
		DoRenderNonInstanced( p_scene, p_camera, p_index, p_nodes );
	}

	void ShadowMapPass::DoRenderBillboards( Scene & p_scene, Camera const & p_camera, uint8_t p_index, BillboardRenderNodesByPipelineMap & p_nodes )
	{
		DoRenderNonInstanced( p_scene, p_camera, p_index, p_nodes );
	}

	String ShadowMapPass::DoGetOpaquePixelShaderSource( uint16_t p_textureFlags, uint8_t p_programFlags, uint8_t p_sceneFlags )const
	{
		using namespace GLSL;
		GlslWriter l_writer = m_renderSystem.CreateGlslWriter();

		// UBOs
		UBO_SCENE( l_writer );

		// Fragment Intputs
		auto gl_FragCoord( l_writer.GetBuiltin< Vec4 >( cuT( "gl_FragCoord" ) ) );

		// Fragment Outputs
		auto pxl_fFragDepth ( l_writer.GetFragData< Float >( cuT( "pxl_fFragDepth" ), 0 ) );

		l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			pxl_fFragDepth = gl_FragCoord.z();
		} );

		return l_writer.Finalise();
	}

	String ShadowMapPass::DoGetTransparentPixelShaderSource( uint16_t p_textureFlags, uint8_t p_programFlags, uint8_t p_sceneFlags )const
	{
		return DoGetOpaquePixelShaderSource( p_textureFlags, p_programFlags, p_sceneFlags );
	}

	void ShadowMapPass::DoUpdateOpaquePipeline( Camera const & p_camera, Pipeline & p_pipeline, TextureLayoutArray const & p_depthMaps )const
	{
		auto & l_sceneUbo = p_pipeline.GetSceneUbo();
		p_camera.FillShader( l_sceneUbo );
	}

	void ShadowMapPass::DoUpdateTransparentPipeline( Camera const & p_camera, Pipeline & p_pipeline, TextureLayoutArray const & p_depthMaps )const
	{
		auto & l_sceneUbo = p_pipeline.GetSceneUbo();
		p_camera.FillShader( l_sceneUbo );
	}

	Pipeline & ShadowMapPass::DoPrepareOpaquePipeline( ShaderProgram & p_program, PipelineFlags const & p_flags )
	{
		auto l_it = m_opaquePipelines.find( p_flags );

		if ( l_it == m_opaquePipelines.end() )
		{
			RasteriserState l_rsState;
			l_rsState.SetCulledFaces( Culling::Back );
			l_it = m_opaquePipelines.emplace( p_flags, GetEngine()->GetRenderSystem()->CreatePipeline( DepthStencilState{}, std::move( l_rsState ), BlendState{}, MultisampleState{}, p_program, p_flags ) ).first;
		}

		return *l_it->second;
	}

	Pipeline & ShadowMapPass::DoPrepareTransparentFrontPipeline( ShaderProgram & p_program, PipelineFlags const & p_flags )
	{
		auto l_it = m_frontTransparentPipelines.find( p_flags );

		if ( l_it == m_frontTransparentPipelines.end() )
		{
			RasteriserState l_rsState;
			l_rsState.SetCulledFaces( Culling::Back );
			l_it = m_frontTransparentPipelines.emplace( p_flags, GetEngine()->GetRenderSystem()->CreatePipeline( DepthStencilState{}, std::move( l_rsState ), BlendState{}, MultisampleState{}, p_program, p_flags ) ).first;
		}

		return *l_it->second;
	}

	Pipeline & ShadowMapPass::DoPrepareTransparentBackPipeline( ShaderProgram & p_program, PipelineFlags const & p_flags )
	{
		auto l_it = m_backTransparentPipelines.find( p_flags );

		if ( l_it == m_backTransparentPipelines.end() )
		{
			RasteriserState l_rsState;
			l_rsState.SetCulledFaces( Culling::Back );
			l_it = m_backTransparentPipelines.emplace( p_flags, GetEngine()->GetRenderSystem()->CreatePipeline( DepthStencilState{}, std::move( l_rsState ), BlendState{}, MultisampleState{}, p_program, p_flags ) ).first;
		}

		return *l_it->second;
	}

	void ShadowMapPass::DoCompleteProgramFlags( uint8_t & p_programFlags )const
	{
	}
}
