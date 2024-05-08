#include "GltfImporter/GltfImporterFile.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/ImporterFile.hpp>
#include <Castor3D/Cache/PluginCache.hpp>
#include <Castor3D/Plugin/ImporterPlugin.hpp>

#include <assimp/version.h>

namespace
{
	static castor3d::ImporterPlugin::ExtensionArray getExtensions( castor3d::Engine * engine )
	{
		static castor3d::ImporterPlugin::ExtensionArray extensions;

		if ( extensions.empty() )
		{
			extensions.emplace_back( cuT( "GLTF" ), cuT( "Text glTF" ) );
			extensions.emplace_back( cuT( "GLB" ), cuT( "Binary glTF" ) );
		}

		return extensions;
	}
}

extern "C"
{
	C3D_Gltf_API void getRequiredVersion( castor3d::Version * version );
	C3D_Gltf_API void getType( castor3d::PluginType * type );
	C3D_Gltf_API void isDebug( int * value );
	C3D_Gltf_API void getName( char const ** name );
	C3D_Gltf_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin );
	C3D_Gltf_API void OnUnload( castor3d::Engine * engine );

	C3D_Gltf_API void getRequiredVersion( castor3d::Version * version )
	{
		*version = castor3d::Version();
	}

	C3D_Gltf_API void isDebug( int * value )
	{
		*value = castor::system::isDebug() ? 1 : 0;
	}

	C3D_Gltf_API void getType( castor3d::PluginType * type )
	{
		*type = castor3d::PluginType::eImporter;
	}

	C3D_Gltf_API void getName( char const ** name )
	{
		*name = c3d_gltf::GltfImporterFile::Name.c_str();
	}

	C3D_Gltf_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin )
	{
		auto importer = static_cast< castor3d::ImporterPlugin * >( plugin );
		auto extensions = getExtensions( engine );

		for ( auto const & extension : extensions )
		{
			importer->addExtension( extension );
			engine->getImporterFileFactory().registerType( castor::string::lowerCase( extension.first )
				, cuT( "gltf" )
				, &c3d_gltf::GltfImporterFile::create );
		}
	}

	C3D_Gltf_API void OnUnload( castor3d::Engine * engine )
	{
		auto extensions = getExtensions( engine );

		for ( auto const & extension : extensions )
		{
			engine->getImporterFileFactory().unregisterType( castor::string::lowerCase( extension.first )
				, cuT( "gltf" ) );
		}
	}
}
