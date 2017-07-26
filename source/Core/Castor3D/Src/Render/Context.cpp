#include "Context.hpp"

#include "Engine.hpp"

#include "FrameBuffer/DepthStencilRenderBuffer.hpp"
#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/RenderBufferAttachment.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Miscellaneous/GpuQuery.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderWindow.hpp"
#include "Scene/Skybox.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslSource.hpp>

using namespace Castor;

namespace Castor3D
{
	Context::Context( RenderSystem & renderSystem )
		: OwnedBy< RenderSystem >{ renderSystem }
		, m_window{ nullptr }
		, m_initialised{ false }
		, m_matrixUbo{ *renderSystem.GetEngine() }
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
	}

	Context::~Context()
	{
	}

	bool Context::Initialise( RenderWindow * window )
	{
		m_window = window;
		bool result = DoInitialise();

		if ( result )
		{
			DoSetCurrent();
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

		return result;
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

		DoEndCurrent();
		DoDestroy();

		m_window = nullptr;
		m_matrixUbo.GetUbo().Cleanup();
	}

	void Context::SetCurrent()
	{
		DoSetCurrent();
		GetRenderSystem()->SetCurrentContext( this );
	}

	void Context::EndCurrent()
	{
		GetRenderSystem()->SetCurrentContext( nullptr );
		DoEndCurrent();
	}

	void Context::SwapBuffers()
	{
		DoSwapBuffers();
	}

	void Context::Barrier( MemoryBarriers const & barriers )
	{
		DoBarrier( barriers );
	}

	void Context::RenderTextureCube( Position const & position
		, Size const & size
		, TextureLayout const & texture )
	{
		m_colourCube.Render( position
			, size
			, texture );
	}

	void Context::RenderTextureCube( Size const & size
		, TextureLayout const & texture
		, uint32_t index )
	{
		m_colourLayerCube.Render( size
			, texture
			, index );
	}

	void Context::RenderTexture( Position const & position
		, Size const & size
		, TextureLayout const & texture
		, RenderPipeline & pipeline
		, MatrixUbo & matrixUbo )
	{
		m_colour.Render( position
			, size
			, texture
			, matrixUbo
			, pipeline );
	}

	void Context::RenderTexture( Position const & position
		, Size const & size
		, TextureLayout const & texture )
	{
		m_colour.Render( position
			, size
			, texture );
	}

	void Context::RenderTexture( Position const & position
		, Size const & size
		, TextureLayout const & texture
		, uint32_t index )
	{
		m_colourLayer.Render( position
			, size
			, texture
			, index );
	}

	void Context::RenderDepthCube( Position const & position
		, Size const & size
		, TextureLayout const & texture )
	{
		m_depthCube.Render( position
			, size
			, texture );
	}

	void Context::RenderDepthCube( Position const & position
		, Size const & size
		, TextureLayout const & texture
		, uint32_t index )
	{
		m_depthLayerCube.Render( position
			, size
			, texture
			, index );
	}

	void Context::RenderDepth( Position const & position
		, Size const & size
		, TextureLayout const & texture )
	{
		m_depth.Render( position
			, size
			, texture );
	}

	void Context::RenderDepth( Position const & position
		, Size const & size
		, TextureLayout const & texture
		, uint32_t index )
	{
		m_depthLayer.Render( position
			, size
			, texture
			, index );
	}
}
