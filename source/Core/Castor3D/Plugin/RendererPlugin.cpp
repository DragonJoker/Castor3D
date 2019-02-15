#include "Castor3D/Plugin/RendererPlugin.hpp"

#include "Castor3D/Render/RenderSystem.hpp"

#include <CastorUtils/Miscellaneous/DynamicLibrary.hpp>

using namespace castor;

namespace castor3d
{
	RendererPlugin::RendererPlugin( DynamicLibrarySPtr p_library, Engine * engine )
		: Plugin( PluginType::eRenderer, p_library, *engine )
	{
		load();
	}

	RendererPlugin::~RendererPlugin()
	{
		unload();
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
