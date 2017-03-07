#include "Context.hpp"

#include "Engine.hpp"
#include "Cache/ShaderCache.hpp"

#include "RenderPipeline.hpp"
#include "RenderWindow.hpp"
#include "RenderSystem.hpp"
#include "Viewport.hpp"

#include "Event/Frame/InitialiseEvent.hpp"
#include "FrameBuffer/DepthStencilRenderBuffer.hpp"
#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/RenderBufferAttachment.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
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
		, m_colourCube{ *this, m_matrixUbo }
		, m_colourLayer{ *this, m_matrixUbo }
		, m_colourLayerCube{ *this, m_matrixUbo }
		, m_depth{ *this, m_matrixUbo }
		, m_depthCube{ *this, m_matrixUbo }
		, m_depthLayer{ *this, m_matrixUbo }
		, m_depthLayerCube{ *this, m_matrixUbo }
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
			m_colourCube.Initialise();
			m_colourLayer.Initialise();
			m_colourLayerCube.Initialise();
			m_depth.Initialise();
			m_depthCube.Initialise();
			m_depthLayer.Initialise();
			m_depthLayerCube.Initialise();
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
		m_colourCube.Cleanup();
		m_colourLayer.Cleanup();
		m_colourLayerCube.Cleanup();
		m_depth.Cleanup();
		m_depthCube.Cleanup();
		m_depthLayer.Cleanup();
		m_depthLayerCube.Cleanup();
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
		m_colourCube.Render( p_size, p_texture );
	}

	void Context::RenderTextureCube( Castor::Size const & p_size
		, TextureLayout const & p_texture
		, uint32_t p_index )
	{
		m_colourLayerCube.Render( p_size
			, p_texture
			, p_index );
	}

	void Context::RenderTexture( Position const & p_position
		, Size const & p_size
		, TextureLayout const & p_texture
		, RenderPipeline & p_pipeline
		, UniformBuffer & p_matrixUbo )
	{
		m_colour.Render( p_position
			, p_size
			, p_texture
			, p_matrixUbo
			, p_pipeline );
	}

	void Context::RenderTexture( Position const & p_position
		, Size const & p_size
		, TextureLayout const & p_texture )
	{
		m_colour.Render( p_position
			, p_size
			, p_texture );
	}

	void Context::RenderTexture( Position const & p_position
		, Size const & p_size
		, TextureLayout const & p_texture
		, uint32_t p_index )
	{
		m_colourLayer.Render( p_position
			, p_size
			, p_texture
			, p_index );
	}

	void Context::RenderDepthCube( Castor::Size const & p_size
		, TextureLayout const & p_texture )
	{
		m_depthCube.Render( p_size, p_texture );
	}

	void Context::RenderDepthCube( Castor::Size const & p_size
		, TextureLayout const & p_texture
		, uint32_t p_index )
	{
		m_depthLayerCube.Render( p_size
			, p_texture
			, p_index );
	}

	void Context::RenderDepth( Position const & p_position
		, Size const & p_size
		, TextureLayout const & p_texture )
	{
		m_depth.Render( p_position
			, p_size
			, p_texture );
	}

	void Context::RenderDepth( Position const & p_position
		, Size const & p_size
		, TextureLayout const & p_texture
		, uint32_t p_index )
	{
		m_depthLayer.Render( p_position
			, p_size
			, p_texture
			, p_index );
	}
}
