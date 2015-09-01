#include <Engine.hpp>

#include "Md2Importer.hpp"

#include <ImporterPlugin.hpp>

using namespace Castor3D;
using namespace Castor;

C3D_Md2_API void GetRequiredVersion( Version & p_version )
{
	p_version = Version();
}

C3D_Md2_API ePLUGIN_TYPE GetType()
{
	return ePLUGIN_TYPE_IMPORTER;
}

C3D_Md2_API String GetName()
{
	return cuT( "MD2 Importer" );
}

C3D_Md2_API ImporterPlugin::ExtensionArray GetExtensions()
{
	ImporterPlugin::Extension l_extension( cuT( "MD2" ), cuT( "Quake II" ) );
	ImporterPlugin::ExtensionArray l_arrayReturn( 1, l_extension );
	return l_arrayReturn;
}

C3D_Md2_API void Create( Engine * p_pEngine, ImporterPlugin * p_pPlugin )
{
	p_pPlugin->AttachImporter( std::make_shared< Md2Importer >( p_pEngine ) );
}

C3D_Md2_API void Destroy( ImporterPlugin * p_pPlugin )
{
	p_pPlugin->DetachImporter();
}

C3D_Md2_API void OnLoad( Castor3D::Engine * p_engine )
{
}

C3D_Md2_API void OnUnload( Castor3D::Engine * p_engine )
{
}
