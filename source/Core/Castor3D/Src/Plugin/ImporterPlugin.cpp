#include "ImporterPlugin.hpp"

#include <Miscellaneous/DynamicLibrary.hpp>

using namespace castor;

namespace castor3d
{
	ImporterPlugin::ImporterPlugin( DynamicLibrarySPtr p_library, Engine * engine )
		: Plugin( PluginType::eImporter, p_library, *engine )
	{
		if ( m_pfnOnLoad )
		{
			m_pfnOnLoad( getEngine(), this );
		}
	}

	ImporterPlugin::~ImporterPlugin()
	{
		if ( m_pfnOnUnload )
		{
			m_pfnOnUnload( getEngine() );
		}
	}

	ImporterPlugin::ExtensionArray const & ImporterPlugin::getExtensions()
	{
		return m_extensions;
	}

	void ImporterPlugin::addExtension( Extension const & p_extension )
	{
		m_extensions.push_back( p_extension );
	}
}
