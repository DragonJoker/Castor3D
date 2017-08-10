#include "RendererPlugin.hpp"

#include "Render/RenderSystem.hpp"

#include <Miscellaneous/DynamicLibrary.hpp>

using namespace castor;

namespace castor3d
{
	RendererPlugin::RendererPlugin( DynamicLibrarySPtr p_library, Engine * engine )
		: Plugin( PluginType::eRenderer, p_library, *engine )
	{
		if ( m_pfnOnLoad )
		{
			m_pfnOnLoad( getEngine(), this );
		}
	}

	RendererPlugin::~RendererPlugin()
	{
		if ( m_pfnOnUnload )
		{
			m_pfnOnUnload( getEngine() );
		}
	}

	String const & RendererPlugin::getRendererType()
	{
		return m_type;
	}

	void RendererPlugin::setRendererType( String const & p_type )
	{
		m_type = p_type;
	}
}
