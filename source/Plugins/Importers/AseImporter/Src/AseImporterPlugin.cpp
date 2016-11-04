#include "AseImporter.hpp"

#include <Engine.hpp>
#include <Plugin/ImporterPlugin.hpp>
#include <Miscellaneous/Version.hpp>

using namespace Castor3D;
using namespace Castor;

#ifndef _WIN32
#	define C3D_Ase_API
#else
#	ifdef AseImporter_EXPORTS
#		define C3D_Ase_API __declspec(dllexport)
#	else
#		define C3D_Ase_API __declspec(dllimport)
#	endif
#endif

C3D_Ase_API void GetRequiredVersion( Version & p_version )
{
	p_version = Version();
}

C3D_Ase_API PluginType GetType()
{
	return PluginType::eImporter;
}

C3D_Ase_API String GetName()
{
	return cuT( "ASE Importer" );
}

C3D_Ase_API ImporterPlugin::ExtensionArray GetExtensions( Engine * p_engine )
{
	ImporterPlugin::Extension l_extension( cuT( "ASE" ), cuT( "3D Studio Max ASE" ) );
	ImporterPlugin::ExtensionArray l_arrayReturn( 1, l_extension );
	return l_arrayReturn;
}

C3D_Ase_API void OnLoad( Castor3D::Engine * p_engine )
{
	auto l_extensions = GetExtensions( p_engine );

	for ( auto const & l_extension : l_extensions )
	{
		p_engine->GetImporterFactory().Register( Castor::string::lower_case( l_extension.first ), &Ase::AseImporter::Create );
	}
}

C3D_Ase_API void OnUnload( Castor3D::Engine * p_engine )
{
	auto l_extensions = GetExtensions( p_engine );

	for ( auto const & l_extension : l_extensions )
	{
		p_engine->GetImporterFactory().Unregister( Castor::string::lower_case( l_extension.first ) );
	}
}
