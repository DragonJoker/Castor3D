#include "Castor3D/Plugin/ImporterPlugin.hpp"

#include <CastorUtils/Miscellaneous/DynamicLibrary.hpp>

CU_ImplementSmartPtr( castor3d, ImporterPlugin )

namespace castor3d
{
	ImporterPlugin::ImporterPlugin( castor::DynamicLibraryUPtr library, Engine * engine )
		: Plugin( PluginType::eImporter, castor::move( library ), *engine )
	{
		load();
	}

	ImporterPlugin::~ImporterPlugin()noexcept
	{
		unload();
	}

	ImporterPlugin::ExtensionArray const & ImporterPlugin::getExtensions()const
	{
		return m_extensions;
	}

	void ImporterPlugin::addExtension( Extension const & extension )
	{
		m_extensions.push_back( extension );
	}
}
