#include "Context.hpp"

#include "Engine.hpp"

#include "FrameBuffer/DepthStencilRenderBuffer.hpp"
#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/RenderBufferAttachment.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "HDR/HdrConfig.hpp"
#include "Miscellaneous/GpuQuery.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderWindow.hpp"
#include "Scene/Skybox.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslSource.hpp>

using namespace castor;

namespace castor3d
{
	Context::Context( RenderSystem & renderSystem )
		: OwnedBy< RenderSystem >{ renderSystem }
		, m_window{ nullptr }
		, m_initialised{ false }
		, m_matrixUbo{ *renderSystem.getEngine() }
		, m_colour{ *this, m_matrixUbo }
		, m_colourCube{ *this, m_matrixUbo }
		, m_colourLayer{ *this, m_matrixUbo }
		, m_colourLayerCube{ *this, m_matrixUbo }
		, m_depth{ *this, m_matrixUbo }
		, m_depthCube{ *this, m_matrixUbo }
		, m_depthLayer{ *this, m_matrixUbo }
		, m_depthLayerCube{ *this, m_matrixUbo }
		, m_variance{ *this, m_matrixUbo }
		, m_varianceCube{ *this, m_matrixUbo }
		, m_cube{ *this, m_matrixUbo }
	{
	}

	Context::~Context()
	{
	}

	bool Context::initialise( RenderWindow * window )
	{
		m_window = window;
		bool result = doInitialise();

		if ( result )
		{
			doSetCurrent();
			m_colour.initialise();
			m_colourCube.initialise();
			m_colourLayer.initialise();
			m_colourLayerCube.initialise();
			m_depth.initialise();
			m_depthCube.initialise();
			m_depthLayer.initialise();
			m_depthLayerCube.initialise();
			m_variance.initialise();
			m_varianceCube.initialise();
			m_cube.initialise();

			doEndCurrent();
		}

		return result;
	}

	void Context::cleanup()
	{
		m_initialised = false;
		doSetCurrent();
		doCleanup();

		m_colour.cleanup();
		m_colourCube.cleanup();
		m_colourLayer.cleanup();
		m_colourLayerCube.cleanup();
		m_depth.cleanup();
		m_depthCube.cleanup();
		m_depthLayer.cleanup();
		m_depthLayerCube.cleanup();
		m_variance.cleanup();
		m_varianceCube.cleanup();
		m_cube.cleanup();

		if ( getRenderSystem()->getMainContext().get() == this )
		{
			getRenderSystem()->cleanupPool();
		}

		doEndCurrent();
		doDestroy();

		m_window = nullptr;
		m_matrixUbo.getUbo().cleanup();
	}

	void Context::setCurrent()
	{
		doSetCurrent();
		getRenderSystem()->setCurrentContext( this );
	}

	void Context::endCurrent()
	{
		getRenderSystem()->setCurrentContext( nullptr );
		doEndCurrent();
	}

	void Context::swapBuffers()
	{
		doSwapBuffers();
	}

	void Context::memoryBarrier( MemoryBarriers const & barriers )
	{
		doMemoryBarrier( barriers );
	}

	void Context::renderTextureCube( Position const & position
		, Size const & size
		, TextureLayout const & texture )
	{
		m_colourCube.render( position
			, size
			, texture );
	}

	void Context::renderTextureCube( Size const & size
		, TextureLayout const & texture
		, uint32_t index )
	{
		m_colourLayerCube.render( size
			, texture
			, index );
	}

	void Context::renderTexture( Position const & position
		, Size const & size
		, TextureLayout const & texture
		, RenderPipeline & pipeline
		, MatrixUbo & matrixUbo )
	{
		m_colour.render( position
			, size
			, texture
			, matrixUbo
			, pipeline );
	}

	void Context::renderTexture( Position const & position
		, Size const & size
		, TextureLayout const & texture )
	{
		m_colour.render( position
			, size
			, texture );
	}

	void Context::renderTextureNearest( Position const & position
		, Size const & size
		, TextureLayout const & texture )
	{
		m_colour.renderNearest( position
			, size
			, texture );
	}

	void Context::renderTexture( Position const & position
		, Size const & size
		, TextureLayout const & texture
		, uint32_t index )
	{
		m_colourLayer.render( position
			, size
			, texture
			, index );
	}

	void Context::renderDepthCube( Position const & position
		, Size const & size
		, TextureLayout const & texture )
	{
		m_depthCube.render( position
			, size
			, texture );
	}

	void Context::renderDepthCube( Position const & position
		, Size const & size
		, TextureLayout const & texture
		, uint32_t index )
	{
		m_depthLayerCube.render( position
			, size
			, texture
			, index );
	}

	void Context::renderDepth( Position const & position
		, Size const & size
		, TextureLayout const & texture )
	{
		m_depth.render( position
			, size
			, texture );
	}

	void Context::renderVariance( Position const & position
		, Size const & size
		, TextureLayout const & texture )
	{
		m_variance.render( position
			, size
			, texture );
	}

	void Context::renderVarianceCube( Position const & position
		, Size const & size
		, TextureLayout const & texture )
	{
		m_varianceCube.render( position
			, size
			, texture );
	}

	void Context::renderDepth( Position const & position
		, Size const & size
		, TextureLayout const & texture
		, uint32_t index )
	{
		m_depthLayer.render( position
			, size
			, texture
			, index );
	}
}
