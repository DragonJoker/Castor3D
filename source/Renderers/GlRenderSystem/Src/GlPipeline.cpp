#include "GlPipeline.hpp"

#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlPipelineImpl::GlPipelineImpl( OpenGl & p_gl, Pipeline & p_pipeline )
		: IPipelineImpl( p_pipeline, true )
		, m_gl( p_gl )
	{
	}

	GlPipelineImpl::~GlPipelineImpl()
	{
	}

	void GlPipelineImpl::ApplyViewport( int p_iWindowWidth, int p_iWindowHeight )
	{
		m_gl.Viewport( 0, 0, p_iWindowWidth, p_iWindowHeight );
	}
}
