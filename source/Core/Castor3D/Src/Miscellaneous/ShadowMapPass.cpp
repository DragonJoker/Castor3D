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
#include "Scene/Light/SpotLight.hpp"
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
							  , FuncType p_function )
		{
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
			}
		}

		template< typename MapType >
		void DoRenderNonInstanced( Scene & p_scene
								   , Camera const & p_camera
								   , MapType & p_nodes )
		{
			for ( auto l_itPipelines : p_nodes )
			{
				l_itPipelines.first->SetProjectionMatrix( p_camera.GetViewport().GetProjection() );
				l_itPipelines.first->SetViewMatrix( p_camera.GetView() );
				l_itPipelines.first->Apply();

				for ( auto & l_renderNode : l_itPipelines.second )
				{
					if ( l_renderNode.m_data.IsInitialised() )
					{
						l_renderNode.Render();
					}
				}
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
				case LightType::Directional:
					l_viewport.SetOrtho( -512.0_r, 511.0_r, -512.0_r, 511.0_r, 1.0_r, 1000.0_r );
					break;

				case LightType::Spot:
					l_viewport.SetPerspective( m_light.GetSpotLight()->GetCutOff(), real( p_size.width() ) / p_size.height(), 1.0_r, 1000.0_r );
					break;
				}

				m_cameraNode = std::make_shared< SceneNode >( cuT( "ShadowMap_" ) + m_light.GetName(), m_scene );
				m_camera = std::make_shared< Camera >( cuT( "ShadowMap_" ) + m_light.GetName()
													   , m_scene
													   , m_cameraNode
													   , l_viewport );
				m_camera->Resize( p_size );
			}
		}

		return l_return;
	}

	void ShadowMapPass::Cleanup()
	{
		if ( m_frameBuffer )
		{
			m_camera->Detach();
			m_camera.reset();
			m_cameraNode.reset();

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
		m_cameraNode->SetPosition( m_light.GetParent()->GetDerivedPosition() );
		m_cameraNode->SetOrientation( m_light.GetParent()->GetDerivedOrientation() );
		auto l_size = m_depthTexture->GetImage().GetDimensions();

		switch ( m_light.GetLightType() )
		{
		case LightType::Directional:
			break;

		case LightType::Spot:
			m_camera->GetViewport().SetPerspective( m_light.GetSpotLight()->GetCutOff(), real( l_size.width() ) / l_size.height(), 1.0_r, 1000.0_r );
			break;
		}

		m_camera->Update();
		m_renderQueue.Prepare( *m_camera, m_scene );
	}

	void ShadowMapPass::Render()
	{
		if ( m_camera && m_frameBuffer->Bind( FrameBufferMode::Automatic, FrameBufferTarget::Draw ) )
		{
			m_frameBuffer->Clear();
			auto & l_nodes = m_renderQueue.GetRenderNodes( *m_camera, m_scene );
			m_camera->Apply();
			DoRenderOpaqueNodes( l_nodes, *m_camera );
			DoRenderTransparentNodes( l_nodes, *m_camera );
			m_frameBuffer->Unbind();

#if 0

			if ( m_depthTexture->Bind( 0 ) )
			{
				auto l_data = m_depthTexture->Lock( 0, AccessType::Read );

				if ( l_data )
				{
					auto l_dimensions = m_depthTexture->GetImage().GetDimensions();
					Image l_image{ cuT( "tmp" ), l_dimensions, m_depthTexture->GetImage().GetPixelFormat(), l_data, m_depthTexture->GetImage().GetPixelFormat() };
					Image::BinaryWriter()( l_image, Engine::GetEngineDirectory() / cuT( "\\ColourBuffer_ShadowMap.hdr" ) );
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
		DoRenderInstancedSubmeshesInstanced( p_nodes.m_scene, p_camera, p_nodes.m_instancedGeometries.m_opaqueRenderNodesBack );
		DoRenderStaticSubmeshesNonInstanced( p_nodes.m_scene, p_camera, p_nodes.m_staticGeometries.m_opaqueRenderNodesBack );
		DoRenderAnimatedSubmeshesNonInstanced( p_nodes.m_scene, p_camera, p_nodes.m_animatedGeometries.m_opaqueRenderNodesBack );
		DoRenderBillboards( p_nodes.m_scene, p_camera, p_nodes.m_billboards.m_opaqueRenderNodesBack );
	}

	void ShadowMapPass::DoRenderTransparentNodes( SceneRenderNodes & p_nodes, Camera const & p_camera )
	{
		DoRenderInstancedSubmeshesInstanced( p_nodes.m_scene, p_camera, p_nodes.m_instancedGeometries.m_transparentRenderNodesBack );
		DoRenderStaticSubmeshesNonInstanced( p_nodes.m_scene, p_camera, p_nodes.m_staticGeometries.m_transparentRenderNodesBack );
		DoRenderAnimatedSubmeshesNonInstanced( p_nodes.m_scene, p_camera, p_nodes.m_animatedGeometries.m_transparentRenderNodesBack );
		DoRenderBillboards( p_nodes.m_scene, p_camera, p_nodes.m_billboards.m_transparentRenderNodesBack );
	}

	void ShadowMapPass::DoRenderInstancedSubmeshesInstanced( Scene & p_scene, Camera const & p_camera, SubmeshStaticRenderNodesByPipelineMap & p_nodes )
	{
		DoTraverseNodes( p_camera, p_nodes, [this]( Pipeline & p_pipeline, Pass & p_pass, Submesh & p_submesh, StaticGeometryRenderNodeArray & p_renderNodes )
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

				p_renderNodes[0].BindPass( MASK_MTXMODE_MODEL );
				p_submesh.DrawInstanced( p_renderNodes[0].m_buffers, l_count );
				p_renderNodes[0].UnbindPass();
			}
		} );
	}

	void ShadowMapPass::DoRenderStaticSubmeshesNonInstanced( Scene & p_scene, Camera const & p_camera, StaticGeometryRenderNodesByPipelineMap & p_nodes )
	{
		DoRenderNonInstanced( p_scene, p_camera, p_nodes );
	}

	void ShadowMapPass::DoRenderAnimatedSubmeshesNonInstanced( Scene & p_scene, Camera const & p_camera, AnimatedGeometryRenderNodesByPipelineMap & p_nodes )
	{
		DoRenderNonInstanced( p_scene, p_camera, p_nodes );
	}

	void ShadowMapPass::DoRenderBillboards( Scene & p_scene, Camera const & p_camera, BillboardRenderNodesByPipelineMap & p_nodes )
	{
		DoRenderNonInstanced( p_scene, p_camera, p_nodes );
	}

	String ShadowMapPass::DoGetOpaquePixelShaderSource( uint16_t p_textureFlags, uint8_t p_programFlags, uint8_t p_sceneFlags )const
	{
		using namespace GLSL;
		GlslWriter l_writer = m_renderSystem.CreateGlslWriter();

		// Fragment Intputs
		auto gl_FragCoord( l_writer.GetBuiltin< Vec4 >( cuT( "gl_FragCoord" ) ) );

		// Fragment Outputs
		auto pxl_fFragDepth ( l_writer.GetFragData< Float >( cuT( "pxl_fFragDepth" ), 0 ) );

		l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			//pxl_fFragDepth = gl_FragCoord.z();
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

	Pipeline & ShadowMapPass::DoPrepareOpaqueFrontPipeline( ShaderProgram & p_program, PipelineFlags const & p_flags )
	{
		auto l_it = m_frontOpaquePipelines.find( p_flags );

		if ( l_it == m_frontOpaquePipelines.end() )
		{
			RasteriserState l_rsState;
			l_rsState.SetCulledFaces( Culling::None );
			l_it = m_frontOpaquePipelines.emplace( p_flags, GetEngine()->GetRenderSystem()->CreatePipeline( DepthStencilState{}, std::move( l_rsState ), BlendState{}, MultisampleState{}, p_program, p_flags ) ).first;
		}

		return *l_it->second;
	}

	Pipeline & ShadowMapPass::DoPrepareOpaqueBackPipeline( ShaderProgram & p_program, PipelineFlags const & p_flags )
	{
		auto l_it = m_backOpaquePipelines.find( p_flags );

		if ( l_it == m_backOpaquePipelines.end() )
		{
			RasteriserState l_rsState;
			l_rsState.SetCulledFaces( Culling::None );
			l_it = m_backOpaquePipelines.emplace( p_flags, GetEngine()->GetRenderSystem()->CreatePipeline( DepthStencilState{}, std::move( l_rsState ), BlendState{}, MultisampleState{}, p_program, p_flags ) ).first;
		}

		return *l_it->second;
	}

	Pipeline & ShadowMapPass::DoPrepareTransparentFrontPipeline( ShaderProgram & p_program, PipelineFlags const & p_flags )
	{
		auto l_it = m_frontTransparentPipelines.find( p_flags );

		if ( l_it == m_frontTransparentPipelines.end() )
		{
			RasteriserState l_rsState;
			l_rsState.SetCulledFaces( Culling::None );
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
			l_rsState.SetCulledFaces( Culling::None );
			l_it = m_backTransparentPipelines.emplace( p_flags, GetEngine()->GetRenderSystem()->CreatePipeline( DepthStencilState{}, std::move( l_rsState ), BlendState{}, MultisampleState{}, p_program, p_flags ) ).first;
		}

		return *l_it->second;
	}

	void ShadowMapPass::DoCompleteProgramFlags( uint8_t & p_programFlags )const
	{
		AddFlag( p_programFlags, ProgramFlag::ShadowMap );
	}
}
