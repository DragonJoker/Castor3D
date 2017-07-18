#include "ObjImporter.hpp"

#include <Engine.hpp>
#include <Plugin/ImporterPlugin.hpp>

#include <Engine.hpp>

using namespace Castor3D;
using namespace Castor;

#ifndef CASTOR_PLATFORM_WINDOWS
#	define C3D_Obj_API
#else
#	ifdef ObjImporter_EXPORTS
#		define C3D_Obj_API __declspec(dllexport)
#	else
#		define C3D_Obj_API __declspec(dllimport)
#	endif
#endif

Castor3D::ImporterPlugin::ExtensionArray GetExtensions( Engine * p_engine )
{
	ImporterPlugin::ExtensionArray arrayReturn;
	arrayReturn.push_back( ImporterPlugin::Extension( cuT( "OBJ" ), cuT( "Wavefront Object" ) ) );
	return arrayReturn;
}

extern "C"
{
	C3D_Obj_API void GetRequiredVersion( Castor3D::Version * p_version )
	{
		*p_version = Castor3D::Version();
	}

	C3D_Obj_API void GetType( Castor3D::PluginType * p_type )
	{
		*p_type = Castor3D::PluginType::eImporter;
	}

	C3D_Obj_API void GetName( char const ** p_name )
	{
		static Castor::String Name = cuT( "OBJ Importer" );
		*p_name = Name.c_str();
	}

	C3D_Obj_API void OnLoad( Castor3D::Engine * p_engine, Castor3D::Plugin * p_plugin )
	{
		auto plugin = static_cast< Castor3D::ImporterPlugin * >( p_plugin );
		auto extensions = GetExtensions( p_engine );

		for ( auto const & extension : extensions )
		{
			plugin->AddExtension( extension );
			p_engine->GetImporterFactory().Register( Castor::string::lower_case( extension.first ), &Obj::ObjImporter::Create );
		}
	}

	C3D_Obj_API void OnUnload( Castor3D::Engine * p_engine )
	{
		auto extensions = GetExtensions( p_engine );

		for ( auto const & extension : extensions )
		{
			p_engine->GetImporterFactory().Unregister( Castor::string::lower_case( extension.first ) );
		}
	}
}
