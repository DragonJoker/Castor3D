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
	ImporterPlugin::Extension l_extension( cuT( "MD2" ), cuT( "Quake II" ) );
	ImporterPlugin::ExtensionArray l_arrayReturn( 1, l_extension );
	return l_arrayReturn;
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
		auto l_plugin = static_cast< Castor3D::ImporterPlugin * >( p_plugin );
		auto l_extensions = GetExtensions( p_engine );

		for ( auto const & l_extension : l_extensions )
		{
			l_plugin->AddExtension( l_extension );
			p_engine->GetImporterFactory().Register( Castor::string::lower_case( l_extension.first ), &C3DMd2::Md2Importer::Create );
		}
	}

	C3D_Md2_API void OnUnload( Castor3D::Engine * p_engine )
	{
		auto l_extensions = GetExtensions( p_engine );

		for ( auto const & l_extension : l_extensions )
		{
			p_engine->GetImporterFactory().Unregister( Castor::string::lower_case( l_extension.first ) );
		}
	}
}
