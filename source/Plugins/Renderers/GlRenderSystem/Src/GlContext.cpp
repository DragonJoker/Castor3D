#include "GlContext.hpp"

#if defined( _WIN32 )
#	include "GlMswContext.hpp"
#elif defined( __linux__ )
#	include "GlX11Context.hpp"
#endif

#include "GlPipeline.hpp"
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
		: Context( p_renderSystem )
		, Holder( p_gl )
		, m_glRenderSystem( &p_renderSystem )
		, m_implementation( std::make_unique< GlContextImpl >( GetOpenGl(), this ) )
	{
		m_pipeline = std::make_unique< GlPipeline >( GetOpenGl(), *this );
	}

	GlContext::~GlContext()
	{
		m_pipeline.reset();
		m_implementation.reset();
	}

	GlContextImpl & GlContext::GetImpl()
	{
		return *m_implementation;
	}

	bool GlContext::DoInitialise()
	{
		m_initialised = m_implementation->Initialise( m_window );

		if ( m_initialised )
		{
			if ( m_glRenderSystem->GetOpenGlMajor() < 3 )
			{
				CASTOR_EXCEPTION( cuT( "The supported OpenGL version is insufficient to run Castor3D" ) );
			}
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
		GetImpl().SetCurrent();
	}

	void GlContext::DoEndCurrent()
	{
		GetImpl().EndCurrent();
	}

	void GlContext::DoSwapBuffers()
	{
		GetImpl().SwapBuffers();
	}
}
