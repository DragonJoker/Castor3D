#include "RendererPlugin.hpp"

#include "Render/RenderSystem.hpp"

#include <Miscellaneous/DynamicLibrary.hpp>

using namespace Castor;

namespace Castor3D
{
	RendererPlugin::RendererPlugin( DynamicLibrarySPtr p_library, Engine * p_engine )
		: Plugin( PluginType::eRenderer, p_library, *p_engine )
	{
		if ( m_pfnOnLoad )
		{
			m_pfnOnLoad( GetEngine(), this );
		}
	}

	RendererPlugin::~RendererPlugin()
	{
		if ( m_pfnOnUnload )
		{
			m_pfnOnUnload( GetEngine() );
		}
	}

	String const & RendererPlugin::GetRendererType()
	{
		return m_type;
	}

	void RendererPlugin::SetRendererType( String const & p_type )
	{
		m_type = p_type;
	}
}
