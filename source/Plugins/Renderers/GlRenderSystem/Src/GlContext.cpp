#include "GlContext.hpp"

#if defined( _WIN32 )
#	include "GlMswContext.hpp"
#elif defined( __linux__ )
#	include "GlX11Context.hpp"
#endif

#include "GlRenderSystem.hpp"

#include <RenderWindow.hpp>
#include <ShaderProgram.hpp>
#include <GeometryBuffers.hpp>
#include <IndexBuffer.hpp>
#include <VertexBuffer.hpp>

using namespace Castor;
using namespace Castor3D;

namespace GlRender
{
	GlContext::GlContext( GlRenderSystem & p_renderSystem, OpenGl & p_gl )
		: Context( p_renderSystem, false )
		, m_glRenderSystem( &p_renderSystem )
		, Holder( p_gl )
	{
		m_implementation = new GlContextImpl( GetOpenGl(), this );
	}

	GlContext::~GlContext()
	{
		delete m_implementation;
	}

	GlContextImpl * GlContext::GetImpl()
	{
		return m_implementation;
	}

	void GlContext::UpdateFullScreen( bool p_value )
	{
	}

	bool GlContext::DoInitialise()
	{
		m_initialised = m_implementation->Initialise( m_window );

		if ( m_initialised )
		{
		}

		return m_initialised;
	}

	void GlContext::DoCleanup()
	{
	}

	void GlContext::DoDestroy()
	{
		m_implementation->Cleanup();
	}

	void GlContext::DoSetCurrent()
	{
		GetImpl()->SetCurrent();
	}

	void GlContext::DoEndCurrent()
	{
		GetImpl()->EndCurrent();
	}

	void GlContext::DoSwapBuffers()
	{
		GetImpl()->SwapBuffers();
	}

	void GlContext::DoSetAlphaFunc( eALPHA_FUNC p_func, uint8_t p_value )
	{
		GetOpenGl().AlphaFunc( GetOpenGl().Get( p_func ), p_value / 255.0f );
	}

	void GlContext::DoCullFace( eFACE p_eCullFace )
	{
		if ( p_eCullFace == eFACE_NONE )
		{
			GetOpenGl().Disable( eGL_TWEAK_CULL_FACE );
		}
		else
		{
			GetOpenGl().Enable( eGL_TWEAK_CULL_FACE );
			GetOpenGl().CullFace( GetOpenGl().Get( p_eCullFace ) );
		}
	}
}
