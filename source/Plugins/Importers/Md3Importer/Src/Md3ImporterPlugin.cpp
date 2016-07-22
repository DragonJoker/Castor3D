#include <Engine.hpp>

#include "Md3Importer.hpp"

#include <Plugin/ImporterPlugin.hpp>

using namespace Castor3D;
using namespace Castor;

#ifndef _WIN32
#	define C3D_Md3_API
#else
#	ifdef Md3Importer_EXPORTS
#		define C3D_Md3_API __declspec(dllexport)
#	else
#		define C3D_Md3_API __declspec(dllimport)
#	endif
#endif

C3D_Md3_API void GetRequiredVersion( Version & p_version )
{
	p_version = Version();
}

C3D_Md3_API ePLUGIN_TYPE GetType()
{
	return ePLUGIN_TYPE_IMPORTER;
}

C3D_Md3_API String GetName()
{
	return cuT( "MD3 Importer" );
}

C3D_Md3_API ImporterPlugin::ExtensionArray GetExtensions( Engine * p_engine )
{
	ImporterPlugin::Extension l_extension( cuT( "MD3" ), cuT( "Quake III" ) );
	ImporterPlugin::ExtensionArray l_arrayReturn( 1, l_extension );
	return l_arrayReturn;
}

C3D_Md3_API void OnLoad( Castor3D::Engine * p_engine )
{
	auto l_extensions = GetExtensions( p_engine );

	for ( auto const & l_extension : l_extensions )
	{
		p_engine->GetImporterFactory().Register( Castor::string::lower_case( l_extension.first ), &C3DMd3::Md3Importer::Create );
	}
}

C3D_Md3_API void OnUnload( Castor3D::Engine * p_engine )
{
	auto l_extensions = GetExtensions( p_engine );

	for ( auto const & l_extension : l_extensions )
	{
		p_engine->GetImporterFactory().Unregister( Castor::string::lower_case( l_extension.first ) );
	}
}
