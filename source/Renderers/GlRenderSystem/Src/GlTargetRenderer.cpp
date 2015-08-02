#include "GlTargetRenderer.hpp"
#include "GlRenderSystem.hpp"
#include "GlShaderProgram.hpp"
#include "OpenGl.hpp"
#include "GlFrameBuffer.hpp"
#include "GlRenderBufferAttachment.hpp"
#include "GlTextureAttachment.hpp"

#include <Scene.hpp>
#include <Logger.hpp>

using namespace GlRender;
using namespace Castor3D;
using namespace Castor;

GlTargetRenderer::GlTargetRenderer( OpenGl & p_gl, GlRenderSystem * p_pRenderSystem )
	:	TargetRenderer( p_pRenderSystem	)
	,	m_pfnBeginScene( NULL	)
	,	m_pfnEndScene( NULL	)
	,	m_bCleaned( false	)
	,	m_gl( p_gl	)
{
}

GlTargetRenderer::~GlTargetRenderer()
{
}

bool GlTargetRenderer::Initialise()
{
	if ( !m_target->IsInitialised() )
	{
		Logger::LogInfo( cuT( "GlTargetRenderer::Initialise" ) );
	}

	if ( static_cast< GlRenderSystem * >( m_pRenderSystem )->GetOpenGlMajor() < 3 )
	{
		if ( m_gl.HasFbo() )
		{
			m_pfnBeginScene		= &GlTargetRenderer::DoBeginSceneFbo;
			m_pfnEndScene		= &GlTargetRenderer::DoEndScene;
		}
		else
		{
			m_pfnBeginScene		= &GlTargetRenderer::DoBeginSceneGl2;
			m_pfnEndScene		= &GlTargetRenderer::DoEndScene;
		}
	}
	else
	{
		if ( m_gl.HasFbo() )
		{
			m_pfnBeginScene		= &GlTargetRenderer::DoBeginSceneFbo;
			m_pfnEndScene		= &GlTargetRenderer::DoEndScene;
		}
		else
		{
			m_pfnBeginScene		= &GlTargetRenderer::DoBeginSceneGl3;
			m_pfnEndScene		= &GlTargetRenderer::DoEndScene;
		}
	}

	return true;
}

void GlTargetRenderer::Cleanup()
{
	m_bCleaned = true;
}

void GlTargetRenderer::BeginScene()
{
	( this->*m_pfnBeginScene )();
}

void GlTargetRenderer::EndScene()
{
	( this->*m_pfnEndScene )();
}

bool GlTargetRenderer::DoBeginSceneGl2()
{
	bool l_bReturn = true;

	if ( m_target->GetScene() )
	{
		m_gl.ClearColor( m_target->GetScene()->GetBackgroundColour() );
	}
	else
	{
		m_gl.ClearColor( 0.5f, 0.5f, 0.5f, 1.0f );
	}

	return l_bReturn;
}

bool GlTargetRenderer::DoBeginSceneGl3()
{
	return DoBeginSceneGl2();
}

bool GlTargetRenderer::DoBeginSceneFbo()
{
	if ( !m_bCleaned )
	{
		DoUpdateSize();
	}

	return DoBeginSceneGl3();
}

void GlTargetRenderer::DoInitialise()
{
}

void GlTargetRenderer::DoUpdateSize()
{
}

RenderBufferAttachmentSPtr GlTargetRenderer::CreateAttachment( RenderBufferSPtr p_pRenderBuffer )const
{
	return std::make_shared< GlRenderBufferAttachment >( m_gl, p_pRenderBuffer );
}

TextureAttachmentSPtr GlTargetRenderer::CreateAttachment( DynamicTextureSPtr p_pTexture )const
{
	return std::make_shared< GlTextureAttachment >( m_gl, p_pTexture );
}

FrameBufferSPtr GlTargetRenderer::CreateFrameBuffer()const
{
	return std::make_shared< GlFrameBuffer >( m_gl, m_pRenderSystem->GetEngine() );
}
