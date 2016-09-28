#include "PlyImporter.hpp"

#if defined( VLD_AVAILABLE )
#	include <vld.h>
#endif

#include <Engine.hpp>
#include <Plugin/ImporterPlugin.hpp>

using namespace Castor3D;
using namespace Castor;

#ifndef _WIN32
#	define C3D_Ply_API
#else
#	ifdef PlyImporter_EXPORTS
#		define C3D_Ply_API __declspec(dllexport)
#	else
#		define C3D_Ply_API __declspec(dllimport)
#	endif
#endif

C3D_Ply_API void GetRequiredVersion( Version & p_version )
{
	p_version = Version();
}

C3D_Ply_API PluginType GetType()
{
	return PluginType::Importer;
}

C3D_Ply_API String GetName()
{
	return cuT( "PLY Importer" );
}

C3D_Ply_API ImporterPlugin::ExtensionArray GetExtensions( Engine * p_engine )
{
	ImporterPlugin::ExtensionArray l_arrayReturn;
	l_arrayReturn.push_back( ImporterPlugin::Extension( cuT( "PLY" ), cuT( "Stanford Polygon Library" ) ) );
	return l_arrayReturn;
}

C3D_Ply_API void OnLoad( Castor3D::Engine * p_engine )
{
	auto l_extensions = GetExtensions( p_engine );

	for ( auto const & l_extension : l_extensions )
	{
		p_engine->GetImporterFactory().Register( Castor::string::lower_case( l_extension.first ), &C3dPly::PlyImporter::Create );
	}
}

C3D_Ply_API void OnUnload( Castor3D::Engine * p_engine )
{
	auto l_extensions = GetExtensions( p_engine );

	for ( auto const & l_extension : l_extensions )
	{
		p_engine->GetImporterFactory().Unregister( Castor::string::lower_case( l_extension.first ) );
	}
}
