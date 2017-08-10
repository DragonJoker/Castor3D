#include "AseImporter.hpp"

#include <Engine.hpp>
#include <Plugin/ImporterPlugin.hpp>
#include <Miscellaneous/Version.hpp>

using namespace castor3d;
using namespace castor;

#ifndef _WIN32
#	define C3D_Ase_API
#else
#	ifdef AseImporter_EXPORTS
#		define C3D_Ase_API __declspec(dllexport)
#	else
#		define C3D_Ase_API __declspec(dllimport)
#	endif
#endif

castor3d::ImporterPlugin::ExtensionArray getExtensions( castor3d::Engine * p_engine )
{
	ImporterPlugin::Extension extension( cuT( "ASE" ), cuT( "3D Studio Max ASE" ) );
	castor3d::ImporterPlugin::ExtensionArray arrayReturn( 1, extension );
	return arrayReturn;
}

extern "C"
{
	C3D_Ase_API void getRequiredVersion( castor3d::Version * p_version )
	{
		*p_version = castor3d::Version();
	}

	C3D_Ase_API void getType( castor3d::PluginType * p_type )
	{
		*p_type = castor3d::PluginType::eImporter;
	}

	C3D_Ase_API void getName( char const ** p_name )
	{
		static castor::String Name = cuT( "ASE Importer" );
		*p_name = Name.c_str();
	}

	C3D_Ase_API void OnLoad( castor3d::Engine * p_engine, castor3d::Plugin * p_plugin )
	{
		auto plugin = static_cast< castor3d::ImporterPlugin * >( p_plugin );
		auto extensions = getExtensions( p_engine );

		for ( auto const & extension : extensions )
		{
			plugin->addExtension( extension );
			p_engine->getImporterFactory().registerType( castor::string::lowerCase( extension.first ), &Ase::AseImporter::create );
		}
	}

	C3D_Ase_API void OnUnload( castor3d::Engine * p_engine )
	{
		auto extensions = getExtensions( p_engine );

		for ( auto const & extension : extensions )
		{
			p_engine->getImporterFactory().unregisterType( castor::string::lowerCase( extension.first ) );
		}
	}
}
