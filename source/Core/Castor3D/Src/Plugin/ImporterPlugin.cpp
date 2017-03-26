#include "ImporterPlugin.hpp"

#include <Miscellaneous/DynamicLibrary.hpp>

using namespace Castor;

namespace Castor3D
{
	ImporterPlugin::ImporterPlugin( DynamicLibrarySPtr p_library, Engine * p_engine )
		: Plugin( PluginType::eImporter, p_library, *p_engine )
	{
		if ( m_pfnOnLoad )
		{
			m_pfnOnLoad( GetEngine(), this );
		}
	}

	ImporterPlugin::~ImporterPlugin()
	{
		if ( m_pfnOnUnload )
		{
			m_pfnOnUnload( GetEngine() );
		}
	}

	ImporterPlugin::ExtensionArray const & ImporterPlugin::GetExtensions()
	{
		return m_extensions;
	}

	void ImporterPlugin::AddExtension( Extension const & p_extension )
	{
		m_extensions.push_back( p_extension );
	}
}
