#include "Render/GlPipeline.hpp"

#include "Common/OpenGl.hpp"

#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlPipeline::GlPipeline( OpenGl & p_gl, Context & p_context )
		: Pipeline( p_context )
		, Holder( p_gl )
	{
	}

	GlPipeline::~GlPipeline()
	{
	}

	void GlPipeline::ApplyViewport( int p_windowWidth, int p_windowHeight )
	{
		GetOpenGl().Viewport( 0, 0, p_windowWidth, p_windowHeight );
	}
}
