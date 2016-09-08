#include "PickingPass.hpp"

#include "Engine.hpp"
#include "ShaderCache.hpp"

#include "FrameBuffer/ColourRenderBuffer.hpp"
#include "FrameBuffer/DepthStencilRenderBuffer.hpp"
#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/RenderBufferAttachment.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Mesh/Buffer/BufferElementGroup.hpp"
#include "Mesh/Buffer/GeometryBuffers.hpp"
#include "Mesh/Buffer/VertexBuffer.hpp"
#include "Render/Pipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Render/RenderTarget.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Geometry.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Shader/FrameVariableBuffer.hpp"
#include "State/DepthStencilState.hpp"
#include "State/MultisampleState.hpp"
#include "State/RasteriserState.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslSource.hpp>

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		template< typename MapType >
		void DoRenderNonInstanced( Scene & p_scene
								   , Camera const & p_camera
								   , Pipeline & p_pipeline
								   , MapType & p_nodes
								   , bool p_register )
		{
			for ( auto l_itPrograms : p_nodes )
			{
				for ( auto & l_renderNode : l_itPrograms.second )
				{
					l_renderNode.Render( p_scene, p_camera, p_pipeline );

					if ( p_register )
					{
						m_renderedObjects.push_back( l_renderNode );
					}
				}
			}
		}
	}

	PickingPass::PickingPass( Engine & p_engine )
		: RenderPass{ cuT( "Picking" ), p_engine }
		, m_renderQueue{ *this }
	{
		RasteriserState l_rsState;
		l_rsState.SetCulledFaces( Culling::Back );
		m_pipeline = GetEngine()->GetRenderSystem()->CreatePipeline( DepthStencilState{}
																	 , std::move( l_rsState )
																	 , BlendState{}
																	 , MultisampleState{} );
	}

	PickingPass::~PickingPass()
	{
	}

	bool PickingPass::Initialise( Size const & p_size )
	{
		m_colourTexture = GetEngine()->GetRenderSystem()->CreateTexture( TextureType::TwoDimensions, AccessType::Read, AccessType::ReadWrite );
		m_colourTexture->GetImage().SetSource( p_size, PixelFormat::A8R8G8B8 );
		auto l_size = m_colourTexture->GetImage().GetDimensions();

		bool l_return = m_colourTexture->Create();

		if ( l_return )
		{
			l_return = m_colourTexture->Initialise();

			if ( !l_return )
			{
				m_colourTexture->Destroy();
			}
		}

		if ( l_return )
		{
			m_frameBuffer = GetEngine()->GetRenderSystem()->CreateFrameBuffer();
			m_frameBuffer->SetClearColour( Colour::from_predef( Colour::Predefined::OpaqueBlack ) );
			m_depthBuffer = m_frameBuffer->CreateDepthStencilRenderBuffer( PixelFormat::D32F );
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

			if ( l_return && m_frameBuffer->Bind( FrameBufferMode::Config ) )
			{
				m_frameBuffer->Attach( AttachmentPoint::Colour, 0, m_colourAttach, m_colourTexture->GetType() );
				m_frameBuffer->Attach( AttachmentPoint::Depth, m_depthAttach );
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
			m_frameBuffer->Bind( FrameBufferMode::Config );
			m_frameBuffer->DetachAll();
			m_frameBuffer->Unbind();
			m_frameBuffer->Cleanup();
			m_colourTexture->Cleanup();
			m_depthBuffer->Cleanup();

			m_colourTexture->Destroy();
			m_depthBuffer->Destroy();
			m_frameBuffer->Destroy();

			m_depthAttach.reset();
			m_depthBuffer.reset();
			m_colourAttach.reset();
			m_colourTexture.reset();
			m_frameBuffer.reset();
		}
	}

	GeometrySPtr PickingPass::Pick( Castor::Position const & p_position, Camera const & p_camera )
	{
		if ( m_frameBuffer->Bind( FrameBufferMode::Automatic, FrameBufferTarget::Draw ) )
		{
			m_frameBuffer->Clear();
			auto & l_nodes = m_renderQueue.GetRenderNodes( p_camera, *m_scenes.begin()->first );

			if ( !l_nodes.m_staticGeometries.m_opaqueRenderNodes.empty()
				 || !l_nodes.m_instancedGeometries.m_opaqueRenderNodes.empty()
				 || !l_nodes.m_animatedGeometries.m_opaqueRenderNodes.empty()
				 || !l_nodes.m_billboards.m_opaqueRenderNodes.empty() )
			{
				m_pipeline->SetProjectionMatrix( p_camera.GetViewport().GetProjection() );
				m_pipeline->SetViewMatrix( p_camera.GetView() );
				m_pipeline->Apply();

				DoRenderOpaqueNodes( l_nodes, p_camera );
				DoRenderTransparentNodes( l_nodes, p_camera );
			}

			m_frameBuffer->Unbind();

#if 1// DEBUG_BUFFERS

			if ( m_colourTexture->Bind( 0 ) )
			{
				auto l_data = m_colourTexture->Lock( 0, AccessType::Read );

				if ( l_data )
				{
					Image::BinaryWriter()( Image( cuT( "tmp" )
												  , m_colourTexture->GetImage().GetDimensions()
												  , PixelFormat::A8R8G8B8
												  , l_data
												  , m_colourTexture->GetImage().GetPixelFormat() )
										   , Engine::GetEngineDirectory() / cuT( "\\ColourBuffer_Picking.png" ) );
				}

				m_colourTexture->Unbind( 0 );
			}

#endif
		}

		return nullptr;
	}

	void PickingPass::DoRenderOpaqueNodes( SceneRenderNodes & p_nodes, Camera const & p_camera )
	{
		if ( !p_nodes.m_staticGeometries.m_opaqueRenderNodes.empty() )
		{
			DoRenderStaticSubmeshesNonInstanced( p_nodes.m_scene, p_camera, *m_pipeline, p_nodes.m_staticGeometries.m_opaqueRenderNodes );
		}

		if ( !p_nodes.m_instancedGeometries.m_opaqueRenderNodes.empty() )
		{
			if ( m_renderSystem.GetGpuInformations().HasInstancing() )
			{
				DoRenderInstancedSubmeshesInstanced( p_nodes.m_scene, p_camera, *m_pipeline, p_nodes.m_instancedGeometries.m_opaqueRenderNodes );
			}
			else
			{
				DoRenderInstancedSubmeshesNonInstanced( p_nodes.m_scene, p_camera, *m_pipeline, p_nodes.m_instancedGeometries.m_opaqueRenderNodes );
			}
		}

		if ( !p_nodes.m_animatedGeometries.m_opaqueRenderNodes.empty() )
		{
			DoRenderAnimatedSubmeshesNonInstanced( p_nodes.m_scene, p_camera, *m_pipeline, p_nodes.m_animatedGeometries.m_opaqueRenderNodes );
		}

		if ( !p_nodes.m_billboards.m_opaqueRenderNodes.empty() )
		{
			DoRenderBillboards( p_nodes.m_scene, p_camera, *m_pipeline, p_nodes.m_billboards.m_opaqueRenderNodes );
		}
	}

	void PickingPass::DoRenderTransparentNodes( SceneRenderNodes & p_nodes, Camera const & p_camera )
	{
		for ( size_t i = 0; i < p_nodes.m_instancedGeometries.m_transparentRenderNodes.size(); ++i )
		{
			DoRenderInstancedSubmeshesInstanced( p_nodes.m_scene, p_camera, *m_pipeline, p_nodes.m_instancedGeometries.m_transparentRenderNodes[i], true );
			DoRenderStaticSubmeshesNonInstanced( p_nodes.m_scene, p_camera, *m_pipeline, p_nodes.m_staticGeometries.m_transparentRenderNodes[i], true );
			DoRenderAnimatedSubmeshesNonInstanced( p_nodes.m_scene, p_camera, *m_pipeline, p_nodes.m_animatedGeometries.m_transparentRenderNodes[i], true );
			DoRenderBillboards( p_nodes.m_scene, p_camera, *m_pipeline, p_nodes.m_billboards.m_transparentRenderNodes[i], true );
		}
	}

	String PickingPass::DoGetOpaquePixelShaderSource( uint32_t p_textureFlags, uint32_t p_programFlags )const
	{
		using namespace GLSL;
		GlslWriter l_writer = m_renderSystem.CreateGlslWriter();

		// UBOs
		UBO_MATRIX( l_writer );
		UBO_SCENE( l_writer );
		UBO_PASS( l_writer );

		// Fragment Intputs
		auto vtx_texture( l_writer.GetInput< Vec3 >( cuT( "vtx_texture" ) ) );
		auto c3d_iDrawIndex( l_writer.GetUniform< UInt >( cuT( "c3d_iDrawIndex" ) ) );
		auto c3d_iObjectIndex( l_writer.GetUniform< UInt >( cuT( "c3d_iObjectIndex" ) ) );
		auto gl_PrimitiveID( l_writer.GetBuiltin< UInt >( cuT( "gl_PrimitiveID" ) ) );

		// Fragment Outputs
		auto pxl_v4FragColor( l_writer.GetFragData< Vec4 >( cuT( "pxl_v4FragColor" ), 0 ) );

		l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			pxl_v4FragColor = vec4( c3d_iDrawIndex, c3d_iObjectIndex, gl_PrimitiveID, 1.0 );
		} );

		return l_writer.Finalise();
	}

	String PickingPass::DoGetTransparentPixelShaderSource( uint32_t p_textureFlags, uint32_t p_programFlags )const
	{
		return DoGetOpaquePixelShaderSource( p_textureFlags, p_programFlags );
	}
}
