#include "GltfImporter/GltfImporterFile.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/ImporterFile.hpp>
#include <Castor3D/Cache/PluginCache.hpp>
#include <Castor3D/Plugin/Plugin.hpp>

namespace
{
	c3d::Plugin::ExtensionArray getExtensions()
	{
		static c3d::Plugin::ExtensionArray extensions;

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
	C3D_Gltf_API void getRequiredVersion( c3d::Version * version );
	C3D_Gltf_API void getType( c3d::PluginType * type );
	C3D_Gltf_API void isDebug( int * value );
	C3D_Gltf_API void getName( char const ** name );
	C3D_Gltf_API void onLoad( c3d::Engine * engine );
	C3D_Gltf_API void onUnload( c3d::Engine * engine );

	C3D_Gltf_API void getRequiredVersion( c3d::Version * version )
	{
		*version = c3d::Version();
	}

	C3D_Gltf_API void isDebug( int * value )
	{
		*value = c3d::system::isDebug() ? 1 : 0;
	}

	C3D_Gltf_API void getType( c3d::PluginType * type )
	{
		*type = c3d::PluginType::eImporter;
	}

	C3D_Gltf_API void getName( char const ** name )
	{
		*name = c3d_gltf::GltfImporterFile::Name.c_str();
	}

	C3D_Gltf_API void onLoad( c3d::Engine * engine )
	{
		auto extensions = getExtensions();

		for ( auto const & [fileExtension, _] : extensions )
		{
			engine->getImporterFileFactory().registerType( c3d::string::lowerCase( fileExtension )
				, cuT( "gltf" )
				, &c3d_gltf::GltfImporterFile::create );
		}
	}

	C3D_Gltf_API void onUnload( c3d::Engine * engine )
	{
		auto extensions = getExtensions();

		for ( auto const & [fileExtension, _] : extensions )
		{
			engine->getImporterFileFactory().unregisterType( c3d::string::lowerCase( fileExtension )
				, cuT( "gltf" ) );
		}
	}
}
