#include "GlWindowRenderer.hpp"
#include "GlContext.hpp"
#include "GlRenderSystem.hpp"
#include "GlShaderProgram.hpp"
#include "OpenGl.hpp"
#include "GlFrameBuffer.hpp"

#include <Viewport.hpp>
#include <Logger.hpp>

using namespace GlRender;
using namespace Castor3D;
using namespace Castor;

GlWindowRenderer::GlWindowRenderer( OpenGl & p_gl, GlRenderSystem * p_pRenderSystem )
	:	WindowRenderer( p_pRenderSystem	)
	,	m_gl( p_gl	)
{
}

GlWindowRenderer::~GlWindowRenderer()
{
}

bool GlWindowRenderer::SetCurrent()
{
	bool l_bReturn = m_target->GetContext()->IsInitialised();

	if ( l_bReturn )
	{
		m_target->GetContext()->SetCurrent();
	}

	return l_bReturn;
}

void GlWindowRenderer::EndCurrent()
{
	if ( m_target->GetContext()->IsInitialised() )
	{
		m_target->GetContext()->EndCurrent();
	}
}

bool GlWindowRenderer::BeginScene()
{
	SetCurrent();
	return m_pfnBeginScene();
}

bool GlWindowRenderer::EndScene()
{
	return m_pfnEndScene();
}

void GlWindowRenderer::EndRender()
{
	if ( m_target->GetContext()->IsInitialised() )
	{
		m_target->GetContext()->SwapBuffers();
	}

	EndCurrent();
}

bool GlWindowRenderer::DoInitialise()
{
	Logger::LogInfo( cuT( "GlWindowRenderer::Initialise" ) );

	if ( m_gl.GetVersion() < 30 )
	{
		if ( m_target->GetViewportType() != eVIEWPORT_TYPE_3D )
		{
//			m_gl.Color3f( 1.0f, 1.0f, 1.0f );
		}

		m_pfnBeginScene = PBeginSceneFunction( [&]()
		{
			bool l_bReturn = true;
			l_bReturn &= m_gl.ShadeModel(	eGL_SHADE_MODEL_SMOOTH );
			l_bReturn &= m_gl.LightModel(	eGL_LIGHT_MODEL_PARAM_LOCAL_VIEWER,		eGL_TRUE	);
			l_bReturn &= m_gl.LightModel(	eGL_LIGHT_MODEL_PARAM_COLOR_CONTROL,	eGL_LIGHT_MODEL_PARAM_SEPARATE_SPECULAR_COLOR	);
			l_bReturn &= m_gl.Enable( eGL_TWEAK_NORMALIZE );

			if ( m_target->GetViewportType() == eVIEWPORT_TYPE_3D )
			{
				l_bReturn &= m_gl.Enable( eGL_TWEAK_LIGHTING );
			}
			else
			{
				l_bReturn &= m_gl.Disable( eGL_TWEAK_LIGHTING );
			}

			return l_bReturn;
		} );
	}
	else
	{
		m_pfnBeginScene = PBeginSceneFunction( [&]()
		{
			return true;
		} );
	}

	m_pfnEndScene = PEndSceneFunction( [&]()
	{
		return m_gl.HasFbo();
	} );
	return true;
}

void GlWindowRenderer::DoCleanup()
{
}