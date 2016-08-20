#include "Render/GlPipeline.hpp"

#include "Common/OpenGl.hpp"
#include "Render/GlRenderSystem.hpp"
#include "State/GlRasteriserState.hpp"
#include "State/GlBlendState.hpp"
#include "State/GlMultisampleState.hpp"

#include <Log/Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlPipeline::GlPipeline( OpenGl & p_gl
							, GlRenderSystem & p_renderSystem
							, RasteriserStateUPtr && p_rsState
							, BlendStateUPtr && p_bdState
							, MultisampleStateUPtr && p_msState )
		: Pipeline{ p_renderSystem, std::move( p_rsState ), std::move( p_bdState ), std::move( p_msState ) }
		, Holder{ p_gl }
	{
	}

	GlPipeline::~GlPipeline()
	{
	}

	void GlPipeline::Apply()const
	{
		static_cast< GlRasteriserState & >( *m_rsState ).Apply();
		static_cast< GlBlendState & >( *m_blState ).Apply();
		static_cast< GlMultisampleState & >( *m_msState ).Apply();
	}

	void GlPipeline::ApplyViewport( int p_windowWidth, int p_windowHeight )
	{
		GetOpenGl().Viewport( 0, 0, p_windowWidth, p_windowHeight );
	}
}
