#include "LwoImporter.hpp"

#include <ImporterPlugin.hpp>

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

C3D_Lwo_API Castor3D::ImporterPlugin::ExtensionArray GetExtensions()
{
	Castor3D::ImporterPlugin::Extension l_extension( cuT( "LWO" ), cuT( "LightWave Model" ) );
	Castor3D::ImporterPlugin::ExtensionArray l_arrayReturn( 1, l_extension );
	return l_arrayReturn;
}

C3D_Lwo_API void Create( Castor3D::Engine * p_pEngine, Castor3D::ImporterPlugin * p_pPlugin )
{
	p_pPlugin->AttachImporter( std::make_shared< Lwo::LwoImporter >( p_pEngine ) );
}

C3D_Lwo_API void Destroy( Castor3D::ImporterPlugin * p_pPlugin )
{
	p_pPlugin->DetachImporter();
}

C3D_Lwo_API void OnLoad( Castor3D::Engine * p_engine )
{
}

C3D_Lwo_API void OnUnload( Castor3D::Engine * p_engine )
{
}
