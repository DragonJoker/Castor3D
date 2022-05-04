#include "PlyImporter/PlyImporter.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Model/Mesh/ImporterFactory.hpp>
#include <Castor3D/Plugin/ImporterPlugin.hpp>

#ifndef CU_PlatformWindows
#	define C3D_Ply_API
#else
#	ifdef PlyImporter_EXPORTS
#		define C3D_Ply_API __declspec(dllexport)
#	else
#		define C3D_Ply_API __declspec(dllimport)
#	endif
#endif

namespace
{
	castor3d::ImporterPlugin::ExtensionArray getExtensions( castor3d::Engine * engine )
	{
		castor3d::ImporterPlugin::ExtensionArray arrayReturn;
		arrayReturn.push_back( castor3d::ImporterPlugin::Extension( cuT( "PLY" ), cuT( "Stanford Polygon Library" ) ) );
		return arrayReturn;
	}
}

extern "C"
{
	C3D_Ply_API void getRequiredVersion( castor3d::Version * version );
	C3D_Ply_API void getType( castor3d::PluginType * type );
	C3D_Ply_API void getName( char const ** name );
	C3D_Ply_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin );
	C3D_Ply_API void OnUnload( castor3d::Engine * engine );

	C3D_Ply_API void getRequiredVersion( castor3d::Version * version )
	{
		*version = castor3d::Version();
	}

	C3D_Ply_API void getType( castor3d::PluginType * type )
	{
		*type = castor3d::PluginType::eImporter;
	}

	C3D_Ply_API void getName( char const ** name )
	{
		static castor::String Name = cuT( "PLY Importer" );
		*name = Name.c_str();
	}

	C3D_Ply_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin )
	{
		auto importer = static_cast< castor3d::ImporterPlugin * >( plugin );
		auto extensions = getExtensions( engine );

		for ( auto const & extension : extensions )
		{
			importer->addExtension( extension );
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
