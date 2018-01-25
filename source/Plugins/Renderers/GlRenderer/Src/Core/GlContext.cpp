#include "Core/GlContext.hpp"

#if RENDERLIB_WIN32
#	include "GlMswContext.hpp"
#elif RENDERLIB_XLIB
#	include "GlX11Context.hpp"
#endif

namespace gl_renderer
{
	Context::Context( renderer::ConnectionPtr && connection )
		: m_connection{ std::move( connection ) }
	{
	}

	ContextPtr Context::create( renderer::ConnectionPtr && connection )
	{
#if defined( _WIN32 )
		return std::make_unique< MswContext >( std::move( connection ) );
#elif defined( __linux__ )
		return std::make_unique< X11Context >( std::move( connection ) );
#endif
	}
}
