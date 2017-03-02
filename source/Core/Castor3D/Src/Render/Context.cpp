#include "Context.hpp"

#include "Engine.hpp"
#include "Cache/ShaderCache.hpp"

#include "RenderPipeline.hpp"
#include "RenderWindow.hpp"
#include "RenderSystem.hpp"
#include "Viewport.hpp"

#include "Event/Frame/InitialiseEvent.hpp"
#include "Mesh/Vertex.hpp"
#include "Mesh/Buffer/Buffer.hpp"
#include "Miscellaneous/GpuQuery.hpp"
#include "Scene/Skybox.hpp"
#include "Shader/UniformBuffer.hpp"
#include "Shader/ShaderProgram.hpp"
#include "State/DepthStencilState.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslSource.hpp>

using namespace Castor;

namespace Castor3D
{
	Context::Context( RenderSystem & p_renderSystem )
		: OwnedBy< RenderSystem >{ p_renderSystem }
		, m_window{ nullptr }
		, m_initialised{ false }
		, m_bMultiSampling{ false }
		, m_matrixUbo{ ShaderProgram::BufferMatrix, p_renderSystem }
		, m_colour{ *this, m_matrixUbo }
		, m_colourLayer{ *this, m_matrixUbo }
		, m_depth{ *this, m_matrixUbo }
		, m_depthLayer{ *this, m_matrixUbo }
		, m_cube{ *this, m_matrixUbo }
	{
		UniformBuffer::FillMatrixBuffer( m_matrixUbo );
	}

	Context::~Context()
	{
	}

	bool Context::Initialise( RenderWindow * p_window )
	{
		m_window = p_window;
		m_timerQuery[0] = GetRenderSystem()->CreateQuery( QueryType::eTimeElapsed );
		m_timerQuery[1] = GetRenderSystem()->CreateQuery( QueryType::eTimeElapsed );
		m_bMultiSampling = p_window->IsMultisampling();
		bool l_return = DoInitialise();

		if ( l_return )
		{
			DoSetCurrent();
			m_timerQuery[0]->Initialise();
			m_timerQuery[1]->Initialise();
			m_timerQuery[1 - m_queryIndex]->Begin();
			m_timerQuery[1 - m_queryIndex]->End();

			m_colour.Initialise();
			m_colourLayer.Initialise();
			m_depth.Initialise();
			m_depthLayer.Initialise();
			m_cube.Initialise();

			DoEndCurrent();
		}

		return l_return;
	}

	void Context::Cleanup()
	{
		m_initialised = false;
		DoSetCurrent();
		DoCleanup();

		m_colour.Cleanup();
		m_colourLayer.Cleanup();
		m_depth.Cleanup();
		m_depthLayer.Cleanup();
		m_cube.Cleanup();

		m_timerQuery[0]->Cleanup();
		m_timerQuery[1]->Cleanup();
		DoEndCurrent();
		DoDestroy();

		m_bMultiSampling = false;
		m_timerQuery[0].reset();
		m_timerQuery[1].reset();
		m_window = nullptr;
		m_matrixUbo.Cleanup();
	}

	void Context::SetCurrent()
	{
		DoSetCurrent();
		GetRenderSystem()->SetCurrentContext( this );
		m_timerQuery[m_queryIndex]->Begin();
	}

	void Context::EndCurrent()
	{
		m_timerQuery[m_queryIndex]->End();
		m_queryIndex = 1 - m_queryIndex;
		uint64_t l_time = 0;
		m_timerQuery[m_queryIndex]->GetInfos( QueryInfo::eResult, l_time );
		GetRenderSystem()->IncGpuTime( std::chrono::nanoseconds( l_time ) );
		GetRenderSystem()->SetCurrentContext( nullptr );
		DoEndCurrent();
	}

	void Context::SwapBuffers()
	{
		DoSwapBuffers();
	}

	void Context::PrepareSkybox( TextureLayout const & p_texture
		, Size const & p_size
		, Skybox & p_skybox )
	{
		// Create the cube texture.
		auto l_texture = GetRenderSystem()->CreateTexture( TextureType::eCube
			, AccessType::eNone
			, AccessType::eRead | AccessType::eWrite
			, PixelFormat::eRGB32F
			, p_size );
		l_texture->GetImage( uint32_t( CubeMapFace::ePositiveX ) ).InitialiseSource();
		l_texture->GetImage( uint32_t( CubeMapFace::eNegativeX ) ).InitialiseSource();
		l_texture->GetImage( uint32_t( CubeMapFace::ePositiveY ) ).InitialiseSource();
		l_texture->GetImage( uint32_t( CubeMapFace::eNegativeY ) ).InitialiseSource();
		l_texture->GetImage( uint32_t( CubeMapFace::ePositiveZ ) ).InitialiseSource();
		l_texture->GetImage( uint32_t( CubeMapFace::eNegativeZ ) ).InitialiseSource();
		l_texture->Initialise();

		// Create the one shot FBO and attaches
		auto l_fbo = GetRenderSystem()->CreateFrameBuffer();
		std::array< FrameBufferAttachmentSPtr, 6 > l_attachs
		{
			{
				l_fbo->CreateAttachment( l_texture, CubeMapFace::ePositiveX ),
				l_fbo->CreateAttachment( l_texture, CubeMapFace::eNegativeX ),
				l_fbo->CreateAttachment( l_texture, CubeMapFace::ePositiveY ),
				l_fbo->CreateAttachment( l_texture, CubeMapFace::eNegativeY ),
				l_fbo->CreateAttachment( l_texture, CubeMapFace::ePositiveZ ),
				l_fbo->CreateAttachment( l_texture, CubeMapFace::eNegativeZ ),
			}
		};
		// Create The depth RBO.
		auto l_depthRbo = l_fbo->CreateDepthStencilRenderBuffer( PixelFormat::eD24 );
		l_depthRbo->Create();
		l_depthRbo->Initialise( p_size );
		auto l_depthAttach = l_fbo->CreateAttachment( l_depthRbo );

		// Fill the FBO
		l_fbo->Create();
		l_fbo->Initialise( p_size );
		l_fbo->Bind( FrameBufferMode::eConfig );
		l_fbo->Attach( AttachmentPoint::eDepth, l_depthAttach );
		REQUIRE( l_fbo->IsComplete() );
		l_fbo->Unbind();
		
		// Render the equirectangular texture to the cube faces.
		m_cube.Render( p_size, p_texture, l_texture, l_fbo, l_attachs );

		// Cleanup the one shot FBO and attaches
		l_fbo->Bind();
		l_fbo->DetachAll();
		l_fbo->Unbind();

		l_depthRbo->Cleanup();
		l_depthRbo->Destroy();

		for ( auto & l_attach : l_attachs )
		{
			l_attach.reset();
		}

		l_depthAttach.reset();
		l_fbo->Cleanup();
		l_fbo->Destroy();

		// Set the cube texture to the skybox.
		p_skybox.SetTexture( l_texture );
	}

	void Context::RenderTextureCube( Castor::Size const & p_size
		, TextureLayout const & p_texture )
	{
		int l_w = p_size.width();
		int l_h = p_size.height();
		DoRenderTextureFace( Position{ l_w * 0, l_h * 1 }
			, p_size
			, p_texture
			, CubeMapFace::eNegativeX
			, *m_rtotPipeline.m_texture.m_pipeline
			, m_matrixUbo
			, *m_rtotPipeline.m_texture.m_geometryBuffers );
		DoRenderTextureFace( Position{ l_w * 1, l_h * 1 }
			, p_size
			, p_texture
			, CubeMapFace::eNegativeY
			, *m_rtotPipeline.m_texture.m_pipeline
			, m_matrixUbo
			, *m_rtotPipeline.m_texture.m_geometryBuffers );
		DoRenderTextureFace( Position{ l_w * 2, l_h * 1 }
			, p_size
			, p_texture
			, CubeMapFace::ePositiveX
			, *m_rtotPipeline.m_texture.m_pipeline
			, m_matrixUbo
			, *m_rtotPipeline.m_texture.m_geometryBuffers );
		DoRenderTextureFace( Position{ l_w * 3, l_h * 1 }
			, p_size
			, p_texture
			, CubeMapFace::ePositiveY
			, *m_rtotPipeline.m_texture.m_pipeline
			, m_matrixUbo
			, *m_rtotPipeline.m_texture.m_geometryBuffers );
		DoRenderTextureFace( Position{ l_w * 1, l_h * 0 }
			, p_size
			, p_texture
			, CubeMapFace::eNegativeZ
			, *m_rtotPipeline.m_texture.m_pipeline
			, m_matrixUbo
			, *m_rtotPipeline.m_texture.m_geometryBuffers );
		DoRenderTextureFace( Position{ l_w * 1, l_h * 2 }
			, p_size
			, p_texture
			, CubeMapFace::ePositiveZ
			, *m_rtotPipeline.m_texture.m_pipeline
			, m_matrixUbo
			, *m_rtotPipeline.m_texture.m_geometryBuffers );
	}

	void Context::RenderTextureCube( Castor::Size const & p_size
		, TextureLayout const & p_texture
		, uint32_t p_index )
	{
		int l_w = p_size.width();
		int l_h = p_size.height();
		DoRenderTextureFace( Position{ l_w * 0, l_h * 1 }
			, p_size
			, p_texture
			, CubeMapFace::eNegativeX
			, *m_rtotPipeline.m_textureArray.m_pipeline
			, m_matrixUbo
			, *m_rtotPipeline.m_textureArray.m_geometryBuffers
			, p_index );
		DoRenderTextureFace( Position{ l_w * 1, l_h * 1 }
			, p_size
			, p_texture
			, CubeMapFace::eNegativeY
			, *m_rtotPipeline.m_textureArray.m_pipeline
			, m_matrixUbo
			, *m_rtotPipeline.m_textureArray.m_geometryBuffers
			, p_index );
		DoRenderTextureFace( Position{ l_w * 2, l_h * 1 }
			, p_size
			, p_texture
			, CubeMapFace::ePositiveX
			, *m_rtotPipeline.m_textureArray.m_pipeline
			, m_matrixUbo
			, *m_rtotPipeline.m_textureArray.m_geometryBuffers
			, p_index );
		DoRenderTextureFace( Position{ l_w * 3, l_h * 1 }
			, p_size
			, p_texture
			, CubeMapFace::ePositiveY
			, *m_rtotPipeline.m_textureArray.m_pipeline
			, m_matrixUbo
			, *m_rtotPipeline.m_textureArray.m_geometryBuffers
			, p_index );
		DoRenderTextureFace( Position{ l_w * 1, l_h * 0 }
			, p_size
			, p_texture
			, CubeMapFace::eNegativeZ
			, *m_rtotPipeline.m_textureArray.m_pipeline
			, m_matrixUbo
			, *m_rtotPipeline.m_textureArray.m_geometryBuffers
			, p_index );
		DoRenderTextureFace( Position{ l_w * 1, l_h * 2 }
			, p_size
			, p_texture
			, CubeMapFace::ePositiveZ
			, *m_rtotPipeline.m_textureArray.m_pipeline
			, m_matrixUbo
			, *m_rtotPipeline.m_textureArray.m_geometryBuffers
			, p_index );
	}

	void Context::RenderTexture( Position const & p_position
		, Size const & p_size
		, TextureLayout const & p_texture
		, RenderPipeline & p_pipeline
		, UniformBuffer & p_matrixUbo )
	{
		m_colour.Render( p_size, p_texture, p_matrixUbo, p_pipeline );
	}

	void Context::RenderTexture( Position const & p_position
		, Size const & p_size
		, TextureLayout const & p_texture )
	{
		m_colour.Render( p_size, p_texture );
	}

	void Context::RenderTexture( Position const & p_position
		, Size const & p_size
		, TextureLayout const & p_texture
		, uint32_t p_index )
	{
		m_colourLayer.Render( p_size, p_texture, p_index );
	}

	void Context::RenderDepthCube( Castor::Size const & p_size
		, TextureLayout const & p_texture )
	{
		int l_w = p_size.width();
		int l_h = p_size.height();
		DoRenderTextureFace( Position{ l_w * 0, l_h * 1 }
			, p_size
			, p_texture
			, CubeMapFace::eNegativeX
			, *m_rtotPipeline.m_depth.m_pipeline
			, m_matrixUbo
			, *m_rtotPipeline.m_depth.m_geometryBuffers );
		DoRenderTextureFace( Position{ l_w * 1, l_h * 1 }
			, p_size
			, p_texture
			, CubeMapFace::eNegativeY
			, *m_rtotPipeline.m_depth.m_pipeline
			, m_matrixUbo
			, *m_rtotPipeline.m_depth.m_geometryBuffers );
		DoRenderTextureFace( Position{ l_w * 2, l_h * 1 }
			, p_size
			, p_texture
			, CubeMapFace::ePositiveX
			, *m_rtotPipeline.m_depth.m_pipeline
			, m_matrixUbo
			, *m_rtotPipeline.m_depth.m_geometryBuffers );
		DoRenderTextureFace( Position{ l_w * 3, l_h * 1 }
			, p_size
			, p_texture
			, CubeMapFace::ePositiveY
			, *m_rtotPipeline.m_depth.m_pipeline
			, m_matrixUbo
			, *m_rtotPipeline.m_depth.m_geometryBuffers );
		DoRenderTextureFace( Position{ l_w * 1, l_h * 0 }
			, p_size
			, p_texture
			, CubeMapFace::eNegativeZ
			, *m_rtotPipeline.m_depth.m_pipeline
			, m_matrixUbo
			, *m_rtotPipeline.m_depth.m_geometryBuffers );
		DoRenderTextureFace( Position{ l_w * 1, l_h * 2 }
			, p_size
			, p_texture
			, CubeMapFace::ePositiveZ
			, *m_rtotPipeline.m_depth.m_pipeline
			, m_matrixUbo
			, *m_rtotPipeline.m_depth.m_geometryBuffers );
	}

	void Context::RenderDepthCube( Castor::Size const & p_size
		, TextureLayout const & p_texture
		, uint32_t p_index )
	{
		int l_w = p_size.width();
		int l_h = p_size.height();
		DoRenderTextureFace( Position{ l_w * 0, l_h * 1 }
			, p_size
			, p_texture
			, CubeMapFace::eNegativeX
			, *m_rtotPipeline.m_depthArray.m_pipeline
			, m_matrixUbo
			, *m_rtotPipeline.m_depthArray.m_geometryBuffers
			, p_index );
		DoRenderTextureFace( Position{ l_w * 1, l_h * 1 }
			, p_size
			, p_texture
			, CubeMapFace::eNegativeY
			, *m_rtotPipeline.m_depthArray.m_pipeline
			, m_matrixUbo
			, *m_rtotPipeline.m_depthArray.m_geometryBuffers
			, p_index );
		DoRenderTextureFace( Position{ l_w * 2, l_h * 1 }
			, p_size
			, p_texture
			, CubeMapFace::ePositiveX
			, *m_rtotPipeline.m_depthArray.m_pipeline
			, m_matrixUbo
			, *m_rtotPipeline.m_depthArray.m_geometryBuffers
			, p_index );
		DoRenderTextureFace( Position{ l_w * 3, l_h * 1 }
			, p_size
			, p_texture
			, CubeMapFace::ePositiveY
			, *m_rtotPipeline.m_depthArray.m_pipeline
			, m_matrixUbo
			, *m_rtotPipeline.m_depthArray.m_geometryBuffers
			, p_index );
		DoRenderTextureFace( Position{ l_w * 1, l_h * 0 }
			, p_size
			, p_texture
			, CubeMapFace::eNegativeZ
			, *m_rtotPipeline.m_depthArray.m_pipeline
			, m_matrixUbo
			, *m_rtotPipeline.m_depthArray.m_geometryBuffers
			, p_index );
		DoRenderTextureFace( Position{ l_w * 1, l_h * 2 }
			, p_size
			, p_texture
			, CubeMapFace::ePositiveZ
			, *m_rtotPipeline.m_depthArray.m_pipeline
			, m_matrixUbo
			, *m_rtotPipeline.m_depthArray.m_geometryBuffers
			, p_index );
	}

	void Context::RenderDepth( Position const & p_position
		, Size const & p_size
		, TextureLayout const & p_texture )
	{
		m_depth.Render( p_size, p_texture );
	}

	void Context::RenderDepth( Position const & p_position
		, Size const & p_size
		, TextureLayout const & p_texture
		, uint32_t p_index )
	{
		m_depthLayer.Render( p_size, p_texture, p_index );
	}

	void Context::DoRenderTextureFace( Position const & p_position
		, Size const & p_size
		, TextureLayout const & p_texture
		, CubeMapFace p_face
		, RenderPipeline & p_pipeline
		, UniformBuffer & p_matrixUbo
		, GeometryBuffers const & p_geometryBuffers )
	{
		REQUIRE( p_texture.GetType() == TextureType::eCube );
		m_rtotPipeline.m_viewport.SetPosition( p_position );
		m_rtotPipeline.m_viewport.Resize( p_size );
		m_rtotPipeline.m_viewport.Update();
		m_rtotPipeline.m_viewport.Apply();
		p_pipeline.SetProjectionMatrix( m_rtotPipeline.m_viewport.GetProjection() );

		auto l_variable = p_pipeline.GetProgram().FindUniform< UniformType::eFloat >( cuT( "c3d_fIndex" ), ShaderType::ePixel );

		if ( l_variable )
		{
			l_variable->SetValue( float( p_face ) / float( CubeMapFace::eCount ) );
		}

		p_pipeline.ApplyProjection( p_matrixUbo );
		p_matrixUbo.Update();
		p_pipeline.Apply();

		p_texture.Bind( 0u );
		m_rtotPipeline.m_sampler->Bind( 0u );
		p_geometryBuffers.Draw( uint32_t( m_rtotPipeline.m_arrayVertex.size() ), 0 );
		m_rtotPipeline.m_sampler->Unbind( 0u );
		p_texture.Unbind( 0u );
	}

	void Context::DoRenderTextureFace( Position const & p_position
		, Size const & p_size
		, TextureLayout const & p_texture
		, CubeMapFace p_face
		, RenderPipeline & p_pipeline
		, UniformBuffer & p_matrixUbo
		, GeometryBuffers const & p_geometryBuffers
		, uint32_t p_index )
	{
		REQUIRE( p_texture.GetType() == TextureType::eCube );
		m_rtotPipeline.m_viewport.SetPosition( p_position );
		m_rtotPipeline.m_viewport.Resize( p_size );
		m_rtotPipeline.m_viewport.Update();
		m_rtotPipeline.m_viewport.Apply();
		p_pipeline.SetProjectionMatrix( m_rtotPipeline.m_viewport.GetProjection() );

		auto l_variable = p_pipeline.GetProgram().FindUniform< UniformType::eFloat >( cuT( "c3d_fIndex" ), ShaderType::ePixel );

		if ( l_variable )
		{
			l_variable->SetValue( float( p_face ) / float( CubeMapFace::eCount ) );
		}

		p_pipeline.ApplyProjection( p_matrixUbo );
		p_matrixUbo.Update();
		p_pipeline.Apply();

		p_texture.Bind( 0u );
		m_rtotPipeline.m_sampler->Bind( 0u );
		p_geometryBuffers.Draw( uint32_t( m_rtotPipeline.m_arrayVertex.size() ), 0 );
		m_rtotPipeline.m_sampler->Unbind( 0u );
		p_texture.Unbind( 0u );
	}

}
