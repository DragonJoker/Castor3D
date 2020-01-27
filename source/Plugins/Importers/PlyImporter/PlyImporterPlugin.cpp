#include "PlyImporter/PlyImporter.hpp"

#if defined( VLD_AVAILABLE )
#	include <vld.h>
#endif

#include <Castor3D/Engine.hpp>
#include <Castor3D/Plugin/ImporterPlugin.hpp>

using namespace castor3d;
using namespace castor;

#ifndef CU_PlatformWindows
#	define C3D_Ply_API
#else
#	ifdef castor3dPlyImporter_EXPORTS
#		define C3D_Ply_API __declspec(dllexport)
#	else
#		define C3D_Ply_API __declspec(dllimport)
#	endif
#endif

castor3d::ImporterPlugin::ExtensionArray getExtensions( Engine * engine )
{
	ImporterPlugin::ExtensionArray arrayReturn;
	arrayReturn.push_back( ImporterPlugin::Extension( cuT( "PLY" ), cuT( "Stanford Polygon Library" ) ) );
	return arrayReturn;
}

extern "C"
{
	C3D_Ply_API void getRequiredVersion( castor3d::Version * p_version )
	{
		*p_version = castor3d::Version();
	}

	C3D_Ply_API void getType( castor3d::PluginType * p_type )
	{
		*p_type = castor3d::PluginType::eImporter;
	}

	C3D_Ply_API void getName( char const ** p_name )
	{
		static castor::String Name = cuT( "PLY Importer" );
		*p_name = Name.c_str();
	}

	C3D_Ply_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * p_plugin )
	{
		auto plugin = static_cast< castor3d::ImporterPlugin * >( p_plugin );
		auto extensions = getExtensions( engine );

		for ( auto const & extension : extensions )
		{
			plugin->addExtension( extension );
			engine->getImporterFactory().registerType( castor::string::lowerCase( extension.first ), &C3dPly::PlyImporter::create );
		}
	}

	C3D_Ply_API void OnUnload( castor3d::Engine * engine )
	{
		auto extensions = getExtensions( engine );

		for ( auto const & extension : extensions )
		{
			engine->getImporterFactory().unregisterType( castor::string::lowerCase( extension.first ) );
		}
	}
}
