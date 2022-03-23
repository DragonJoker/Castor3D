#include "Castor3D/Plugin/ImporterPlugin.hpp"

#include <CastorUtils/Miscellaneous/DynamicLibrary.hpp>

namespace castor3d
{
	ImporterPlugin::ImporterPlugin( castor::DynamicLibrarySPtr library, Engine * engine )
		: Plugin( PluginType::eImporter, library, *engine )
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

	void ImporterPlugin::addExtension( Extension const & extension )
	{
		m_extensions.push_back( extension );
	}
}
