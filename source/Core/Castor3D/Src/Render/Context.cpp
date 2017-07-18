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
	Context::Context( RenderSystem & p_renderSystem )
		: OwnedBy< RenderSystem >{ p_renderSystem }
		, m_window{ nullptr }
		, m_initialised{ false }
		, m_bMultiSampling{ false }
		, m_matrixUbo{ *p_renderSystem.GetEngine() }
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

	bool Context::Initialise( RenderWindow * p_window )
	{
		m_window = p_window;
		m_timerQuery[0] = GetRenderSystem()->CreateQuery( QueryType::eTimeElapsed );
		m_timerQuery[1] = GetRenderSystem()->CreateQuery( QueryType::eTimeElapsed );
		m_bMultiSampling = p_window->IsMultisampling();
		bool l_result = DoInitialise();

		if ( l_result )
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

		return l_result;
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
		m_matrixUbo.GetUbo().Cleanup();
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

	void Context::Barrier( MemoryBarriers const & p_barriers )
	{
		DoBarrier( p_barriers );
	}

	void Context::RenderTextureCube( Position const & p_position
		, Size const & p_size
		, TextureLayout const & p_texture )
	{
		m_colourCube.Render( p_position
			, p_size
			, p_texture );
	}

	void Context::RenderTextureCube( Size const & p_size
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
		, MatrixUbo & p_matrixUbo )
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

	void Context::RenderDepthCube( Position const & p_position
		, Size const & p_size
		, TextureLayout const & p_texture )
	{
		m_depthCube.Render( p_position
			, p_size
			, p_texture );
	}

	void Context::RenderDepthCube( Position const & p_position
		, Size const & p_size
		, TextureLayout const & p_texture
		, uint32_t p_index )
	{
		m_depthLayerCube.Render( p_position
			, p_size
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
