#include "LwoImporter.hpp"

#include <Engine.hpp>
#include <Plugin/ImporterPlugin.hpp>
#include <Miscellaneous/Version.hpp>

C3D_Lwo_API void GetRequiredVersion( Castor3D::Version & p_version )
{
	p_version = Castor3D::Version();
}

C3D_Lwo_API Castor3D::ePLUGIN_TYPE GetType()
{
	return Castor3D::ePLUGIN_TYPE_IMPORTER;
}

C3D_Lwo_API Castor::String GetName()
{
	return cuT( "LWO Importer" );
}

C3D_Lwo_API Castor3D::ImporterPlugin::ExtensionArray GetExtensions( Castor3D::Engine * p_engine )
{
	Castor3D::ImporterPlugin::Extension l_extension( cuT( "LWO" ), cuT( "LightWave Model" ) );
	Castor3D::ImporterPlugin::ExtensionArray l_arrayReturn( 1, l_extension );
	return l_arrayReturn;
}

C3D_Lwo_API void OnLoad( Castor3D::Engine * p_engine )
{
	auto l_extensions = GetExtensions( p_engine );

	for ( auto const & l_extension : l_extensions )
	{
		p_engine->GetImporterFactory().Register( Castor::string::lower_case( l_extension.first ), &Lwo::LwoImporter::Create );
	}
}

C3D_Lwo_API void OnUnload( Castor3D::Engine * p_engine )
{
	auto l_extensions = GetExtensions( p_engine );

	for ( auto const & l_extension : l_extensions )
	{
		p_engine->GetImporterFactory().Unregister( Castor::string::lower_case( l_extension.first ) );
	}
}
