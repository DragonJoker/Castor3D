#include <Engine.hpp>

#include "Md2Importer.hpp"

#include <Engine.hpp>
#include <Plugin/ImporterPlugin.hpp>

using namespace Castor3D;
using namespace Castor;

#ifndef _WIN32
#	define C3D_Md2_API
#else
#	ifdef Md2Importer_EXPORTS
#		define C3D_Md2_API __declspec(dllexport)
#	else
#		define C3D_Md2_API __declspec(dllimport)
#	endif
#endif

ImporterPlugin::ExtensionArray GetExtensions( Engine * p_engine )
{
	ImporterPlugin::Extension extension( cuT( "MD2" ), cuT( "Quake II" ) );
	ImporterPlugin::ExtensionArray arrayReturn( 1, extension );
	return arrayReturn;
}

extern "C"
{
	C3D_Md2_API void GetRequiredVersion( Castor3D::Version * p_version )
	{
		*p_version = Castor3D::Version();
	}

	C3D_Md2_API void GetType( Castor3D::PluginType * p_type )
	{
		*p_type = Castor3D::PluginType::eImporter;
	}

	C3D_Md2_API void GetName( char const ** p_name )
	{
		static Castor::String Name = cuT( "MD2 Importer" );
		*p_name = Name.c_str();
	}

	C3D_Md2_API void OnLoad( Castor3D::Engine * p_engine, Castor3D::Plugin * p_plugin )
	{
		auto plugin = static_cast< Castor3D::ImporterPlugin * >( p_plugin );
		auto extensions = GetExtensions( p_engine );

		for ( auto const & extension : extensions )
		{
			plugin->AddExtension( extension );
			p_engine->GetImporterFactory().Register( Castor::string::lower_case( extension.first ), &C3DMd2::Md2Importer::Create );
		}
	}

	C3D_Md2_API void OnUnload( Castor3D::Engine * p_engine )
	{
		auto extensions = GetExtensions( p_engine );

		for ( auto const & extension : extensions )
		{
			p_engine->GetImporterFactory().Unregister( Castor::string::lower_case( extension.first ) );
		}
	}
}
