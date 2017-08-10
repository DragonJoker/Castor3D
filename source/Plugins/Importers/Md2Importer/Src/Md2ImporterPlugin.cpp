#include <Engine.hpp>

#include "Md2Importer.hpp"

#include <Engine.hpp>
#include <Plugin/ImporterPlugin.hpp>

using namespace castor3d;
using namespace castor;

#ifndef _WIN32
#	define C3D_Md2_API
#else
#	ifdef Md2Importer_EXPORTS
#		define C3D_Md2_API __declspec(dllexport)
#	else
#		define C3D_Md2_API __declspec(dllimport)
#	endif
#endif

ImporterPlugin::ExtensionArray getExtensions( Engine * p_engine )
{
	ImporterPlugin::Extension extension( cuT( "MD2" ), cuT( "Quake II" ) );
	ImporterPlugin::ExtensionArray arrayReturn( 1, extension );
	return arrayReturn;
}

extern "C"
{
	C3D_Md2_API void getRequiredVersion( castor3d::Version * p_version )
	{
		*p_version = castor3d::Version();
	}

	C3D_Md2_API void getType( castor3d::PluginType * p_type )
	{
		*p_type = castor3d::PluginType::eImporter;
	}

	C3D_Md2_API void getName( char const ** p_name )
	{
		static castor::String Name = cuT( "MD2 Importer" );
		*p_name = Name.c_str();
	}

	C3D_Md2_API void OnLoad( castor3d::Engine * p_engine, castor3d::Plugin * p_plugin )
	{
		auto plugin = static_cast< castor3d::ImporterPlugin * >( p_plugin );
		auto extensions = getExtensions( p_engine );

		for ( auto const & extension : extensions )
		{
			plugin->addExtension( extension );
			p_engine->getImporterFactory().registerType( castor::string::lowerCase( extension.first ), &C3DMd2::Md2Importer::create );
		}
	}

	C3D_Md2_API void OnUnload( castor3d::Engine * p_engine )
	{
		auto extensions = getExtensions( p_engine );

		for ( auto const & extension : extensions )
		{
			p_engine->getImporterFactory().unregisterType( castor::string::lowerCase( extension.first ) );
		}
	}
}
