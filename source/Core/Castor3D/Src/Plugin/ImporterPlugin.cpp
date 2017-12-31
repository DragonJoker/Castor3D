#include "ImporterPlugin.hpp"

#include <Miscellaneous/DynamicLibrary.hpp>

using namespace castor;

namespace castor3d
{
	ImporterPlugin::ImporterPlugin( DynamicLibrarySPtr p_library, Engine * engine )
		: Plugin( PluginType::eImporter, p_library, *engine )
	{
		load();
	}

	ImporterPlugin::~ImporterPlugin()
	{
		unload();
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
