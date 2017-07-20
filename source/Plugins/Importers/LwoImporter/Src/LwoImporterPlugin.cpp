#include "LwoImporter.hpp"

#include <Engine.hpp>
#include <Plugin/ImporterPlugin.hpp>
#include <Miscellaneous/Version.hpp>

#ifndef _WIN32
#	define C3D_Lwo_API
#else
#	ifdef LwoImporter_EXPORTS
#		define C3D_Lwo_API __declspec(dllexport)
#	else
#		define C3D_Lwo_API __declspec(dllimport)
#	endif
#endif

Castor3D::ImporterPlugin::ExtensionArray GetExtensions( Castor3D::Engine * p_engine )
{
	Castor3D::ImporterPlugin::Extension extension( cuT( "LWO" ), cuT( "LightWave Model" ) );
	Castor3D::ImporterPlugin::ExtensionArray arrayReturn( 1, extension );
	return arrayReturn;
}

extern "C"
{
	C3D_Lwo_API void GetRequiredVersion( Castor3D::Version * p_version )
	{
		*p_version = Castor3D::Version();
	}

	C3D_Lwo_API void GetType( Castor3D::PluginType * p_type )
	{
		*p_type = Castor3D::PluginType::eImporter;
	}

	C3D_Lwo_API void GetName( char const ** p_name )
	{
		static Castor::String Name = cuT( "LWO Importer" );
		*p_name = Name.c_str();
	}

	C3D_Lwo_API void OnLoad( Castor3D::Engine * p_engine, Castor3D::Plugin * p_plugin )
	{
		auto plugin = static_cast< Castor3D::ImporterPlugin * >( p_plugin );
		auto extensions = GetExtensions( p_engine );

		for ( auto const & extension : extensions )
		{
			plugin->AddExtension( extension );
			p_engine->GetImporterFactory().Register( Castor::string::lower_case( extension.first ), &Lwo::LwoImporter::Create );
		}
	}

	C3D_Lwo_API void OnUnload( Castor3D::Engine * p_engine )
	{
		auto extensions = GetExtensions( p_engine );

		for ( auto const & extension : extensions )
		{
			p_engine->GetImporterFactory().Unregister( Castor::string::lower_case( extension.first ) );
		}
	}
}
