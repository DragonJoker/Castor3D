#include "FbxImporter.hpp"

#if defined( VLD_AVAILABLE )
#	include <vld.h>
#endif

#include <Engine.hpp>
#include <Plugin/ImporterPlugin.hpp>

using namespace Castor3D;
using namespace Castor;

#ifndef _WIN32
#	define C3D_Fbx_API
#else
#	ifdef FbxImporter_EXPORTS
#		define C3D_Fbx_API __declspec(dllexport)
#	else
#		define C3D_Fbx_API __declspec(dllimport)
#	endif
#endif

C3D_Fbx_API void GetRequiredVersion( Version & p_version )
{
	p_version = Version();
}

C3D_Fbx_API ePLUGIN_TYPE GetType()
{
	return ePLUGIN_TYPE_IMPORTER;
}

C3D_Fbx_API String GetName()
{
	return cuT( "FBX Importer" );
}

C3D_Fbx_API ImporterPlugin::ExtensionArray GetExtensions( Engine * p_engine )
{
	ImporterPlugin::ExtensionArray l_extensions;
	l_extensions.push_back( ImporterPlugin::Extension( cuT( "FBX" ), cuT( "Autodesk FBX" ) ) );
	return l_extensions;
}

C3D_Fbx_API void OnLoad( Castor3D::Engine * p_engine )
{
	auto l_extensions = GetExtensions( p_engine );

	for ( auto const & l_extension : l_extensions )
	{
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
