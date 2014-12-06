#include "CameraRenderer.hpp"

using namespace Castor;

namespace Castor3D
{
	CameraRenderer::CameraRenderer( RenderSystem * p_pRenderSystem )
		:	Renderer<Camera, CameraRenderer>( p_pRenderSystem )
		,	m_bResize( false )
	{
	}

	CameraRenderer::~CameraRenderer()
	{
	}
}
