#include "SMaxImporter.hpp"

#include <Engine.hpp>

#include <Plugin/ImporterPlugin.hpp>

using namespace Castor3D;
using namespace Castor;

C3D_SMax_API void GetRequiredVersion( Version & p_version )
{
	p_version = Version();
}

C3D_SMax_API ePLUGIN_TYPE GetType()
{
	return ePLUGIN_TYPE_IMPORTER;
}

C3D_SMax_API String GetName()
{
	return cuT( "3DS Importer" );
}

C3D_SMax_API ImporterPlugin::ExtensionArray GetExtensions( Engine * p_engine )
{
	ImporterPlugin::Extension l_extension( cuT( "3DS" ), cuT( "3D Studio Max 3DS" ) );
	ImporterPlugin::ExtensionArray l_arrayReturn( 1, l_extension );
	return l_arrayReturn;
}

C3D_SMax_API void OnLoad( Castor3D::Engine * p_engine )
{
	auto l_extensions = GetExtensions( p_engine );

	for ( auto const & l_extension : l_extensions )
	{
		p_engine->GetImporterFactory().Register( Castor::string::lower_case( l_extension.first ), &C3dSMax::SMaxImporter::Create );
	}
}

C3D_SMax_API void OnUnload( Castor3D::Engine * p_engine )
{
	auto l_extensions = GetExtensions( p_engine );

	for ( auto const & l_extension : l_extensions )
	{
		p_engine->GetImporterFactory().Unregister( Castor::string::lower_case( l_extension.first ) );
	}
}
