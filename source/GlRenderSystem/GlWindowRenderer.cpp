#include "GlRenderSystem/PrecompiledHeader.hpp"

#include "GlRenderSystem/GlWindowRenderer.hpp"
#include "GlRenderSystem/GlContext.hpp"
#include "GlRenderSystem/GlRenderSystem.hpp"
#include "GlRenderSystem/GlShaderProgram.hpp"
#include "GlRenderSystem/OpenGl.hpp"

using namespace Castor3D;

GlWindowRenderer :: GlWindowRenderer()
	:	WindowRenderer()
	,	m_context( NULL)
	,	m_pfnStartRender( NULL)
{
}

GlWindowRenderer :: ~GlWindowRenderer()
{
}

void GlWindowRenderer :: Initialise()
{
	CASTOR_TRACK;
	m_context = new GlContext( m_target);

	if (GlRenderSystem::GetOpenGlMajor() < 3)
	{
		m_pfnStartRender = PStartRenderFunction(	& GlWindowRenderer::_startRenderGl2);
	}
	else
	{
		m_pfnStartRender = PStartRenderFunction(	& GlWindowRenderer::_startRenderGl3);
	}
}

bool GlWindowRenderer :: StartRender()
{
	CASTOR_TRACK;
	bool l_bContinue = m_context->IsInitialised();

	if ( ! l_bContinue)
	{
		l_bContinue = m_context->Initialise();
	}

	if (GlRenderSystem::GetOpenGlMajor() < 3)
	{
		m_pfnStartRender = PStartRenderFunction(	& GlWindowRenderer::_startRenderGl2);
	}
	else
	{
		m_pfnStartRender = PStartRenderFunction(	& GlWindowRenderer::_startRenderGl3);
	}

	if (l_bContinue)
	{
		l_bContinue = (this->*m_pfnStartRender)();
	}

	return l_bContinue;
}

void GlWindowRenderer :: EndRender()
{
	CASTOR_TRACK;
	if (m_context->IsInitialised())
	{
		if (m_target->GetType() == eVIEWPORT_TYPE_3D)
		{
			m_target->GetMainScene()->RenderOverlays();
		}

		m_context->SwapBuffers();
		m_context->EndCurrent();
	}
}

void GlWindowRenderer :: Resize( unsigned int p_width, unsigned int p_height)
{
	CASTOR_TRACK;
	m_target->GetCamera()->Resize( p_width, p_height);
}

bool GlWindowRenderer :: _startRenderGl2()
{
	CASTOR_TRACK;
	m_context->SetCurrent();
	bool l_bReturn;
	l_bReturn  = OpenGl::ClearColor( 0.5f, 0.5f, 0.5f, 1.0f);
	l_bReturn &= OpenGl::Enable( GL_DEPTH_TEST);
	l_bReturn &= OpenGl::Enable( GL_NORMALIZE);

	l_bReturn &= OpenGl::LightModeli( GL_LIGHT_MODEL_LOCAL_VIEWER,GL_TRUE);
	l_bReturn &= OpenGl::LightModeli( GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);

	if (m_target->GetType() == eVIEWPORT_TYPE_3D)
	{
		l_bReturn &= OpenGl::Enable( GL_LIGHTING);
	}
	else
	{
		l_bReturn &= OpenGl::Disable( GL_LIGHTING);
	}

	if ( ! m_target->IsInitialised())
	{
		Logger::LogMessage( cuT( "GlWindowRenderer :: StartRender - Initialisation"));

		CameraPtr l_camera = m_target->GetCamera();

		if (m_target->GetType() == eVIEWPORT_TYPE_3D)
		{
			l_camera->GetParent()->Translate( 0.0f, 0.0f, -5.0f);
		}
		else
		{
			l_bReturn &= OpenGl::Color3f( 1.0f, 1.0f, 1.0f);
		}

		l_camera->Resize(m_target->GetTargetWidth(), m_target->GetTargetHeight());

		if ( ! RenderSystem::IsInitialised())
		{
			RenderSystem::GetSingletonPtr()->Initialise();
			Root::GetSingleton()->GetMaterialManager()->Initialise();
		}

		m_target->SetInitialised();
	}

	l_bReturn &= OpenGl::ShadeModel( GL_SMOOTH);
	l_bReturn &= OpenGl::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	return l_bReturn;
}

bool GlWindowRenderer :: _startRenderGl3()
{
	CASTOR_TRACK;
	m_context->SetCurrent();
	bool l_bReturn;
	l_bReturn  = OpenGl::ClearColor( 0.5f, 0.5f, 0.5f, 0.5f);
	l_bReturn &= OpenGl::Enable( GL_DEPTH_TEST);

	if ( ! m_target->IsInitialised())
	{
		Logger::LogMessage( cuT( "GlWindowRenderer :: StartRender - Initialisation"));

		CameraPtr l_camera = m_target->GetCamera();

		if (m_target->GetType() == eVIEWPORT_TYPE_3D)
		{
			l_camera->GetParent()->Translate( 0.0f, 0.0f, -5.0f);
		}
		else
		{
			l_bReturn &= OpenGl::Color3f( 1.0f, 1.0f, 1.0f);
		}

		l_camera->Resize(m_target->GetTargetWidth(), m_target->GetTargetHeight());

		if ( ! RenderSystem::IsInitialised())
		{
			RenderSystem::GetSingletonPtr()->Initialise();
			Root::GetSingleton()->GetMaterialManager()->Initialise();
		}

		m_target->SetInitialised();
	}

	l_bReturn &= OpenGl::Clear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	return l_bReturn;
}
