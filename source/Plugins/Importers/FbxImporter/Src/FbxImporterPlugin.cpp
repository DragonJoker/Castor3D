#include "FbxImporter.hpp"

#if defined( VLD_AVAILABLE )
#	include <vld.h>
#endif

#include <Engine.hpp>
#include <Plugin/ImporterPlugin.hpp>

using namespace Castor3D;
using namespace Castor;

#ifndef CASTOR_PLATFORM_WINDOWS
#	define C3D_Fbx_API
#else
#	ifdef FbxImporter_EXPORTS
#		define C3D_Fbx_API __declspec(dllexport)
#	else
#		define C3D_Fbx_API __declspec(dllimport)
#	endif
#endif

Castor3D::ImporterPlugin::ExtensionArray GetExtensions( Engine * p_engine )
{
	ImporterPlugin::ExtensionArray l_extensions;
	l_extensions.push_back( ImporterPlugin::Extension( cuT( "FBX2" ), cuT( "Autodesk FBX" ) ) );
	return l_extensions;
}

extern "C"
{
	C3D_Fbx_API void GetRequiredVersion( Castor3D::Version * p_version )
	{
		*p_version = Castor3D::Version();
	}

	C3D_Fbx_API void GetType( Castor3D::PluginType * p_type )
	{
		*p_type = Castor3D::PluginType::eImporter;
	}

	C3D_Fbx_API void GetName( char const ** p_name )
	{
		static Castor::String Name = cuT( "FBX Importer" );
		*p_name = Name.c_str();
	}

	C3D_Fbx_API void OnLoad( Castor3D::Engine * p_engine, Castor3D::Plugin * p_plugin )
	{
		auto l_plugin = static_cast< Castor3D::ImporterPlugin * >( p_plugin );
		auto l_extensions = GetExtensions( p_engine );

		for ( auto const & l_extension : l_extensions )
		{
			l_plugin->AddExtension( l_extension );
			p_engine->GetImporterFactory().Register( Castor::string::lower_case( l_extension.first ), &C3dFbx::FbxSdkImporter::Create );
		}
	}

	C3D_Fbx_API void OnUnload( Castor3D::Engine * p_engine )
	{
		auto l_extensions = GetExtensions( p_engine );

		for ( auto const & l_extension : l_extensions )
		{
			p_engine->GetImporterFactory().Unregister( Castor::string::lower_case( l_extension.first ) );
		}
	}
}
