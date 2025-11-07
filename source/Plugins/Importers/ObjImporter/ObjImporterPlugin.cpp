#include "ObjImporter/ObjImporterFile.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/ImporterFile.hpp>
#include <Castor3D/Cache/PluginCache.hpp>
#include <Castor3D/Plugin/Plugin.hpp>

namespace
{
	c3d::Vector< c3d::Pair< c3d::String, c3d::Plugin::Extension > > getExtensions()
	{
		c3d::Vector< c3d::Pair< c3d::String, c3d::Plugin::Extension > > extensions;
		using Extension = c3d::Plugin::Extension;

		if ( extensions.empty() )
		{
			extensions.emplace_back( cuT( "obj" ), Extension{ cuT( "OBJ" ), cuT( "Wavefront Object" ) } );
		}

		return extensions;
	}
}

extern "C"
{
	C3D_Obj_API void getRequiredVersion( c3d::Version * version );
	C3D_Obj_API void getType( c3d::PluginType * type );
	C3D_Obj_API void isDebug( int * value );
	C3D_Obj_API void getName( char const ** name );
	C3D_Obj_API void onLoad( c3d::Engine * engine );
	C3D_Obj_API void onUnload( c3d::Engine * engine );

	C3D_Obj_API void getRequiredVersion( c3d::Version * version )
	{
		*version = c3d::Version();
	}

	C3D_Obj_API void isDebug( int * value )
	{
		*value = c3d::system::isDebug() ? 1 : 0;
	}

	C3D_Obj_API void getType( c3d::PluginType * type )
	{
		*type = c3d::PluginType::eImporter;
	}

	C3D_Obj_API void getName( char const ** name )
	{
		*name = c3d_obj::ObjImporterFile::Name.c_str();
	}

	C3D_Obj_API void onLoad( c3d::Engine * engine )
	{
		auto extensions = getExtensions();

		for ( auto const & [fileExtension, extension] : extensions )
		{
			engine->getImporterFileFactory().registerType( c3d::string::lowerCase( extension.first )
				, fileExtension
				, &c3d_obj::ObjImporterFile::create );
		}
	}

	C3D_Obj_API void onUnload( c3d::Engine * engine )
	{
		auto extensions = getExtensions();

		for ( auto const & [fileExtension, extension] : extensions )
		{
			engine->getImporterFileFactory().unregisterType( c3d::string::lowerCase( extension.first )
				, fileExtension );
		}
	}
}
