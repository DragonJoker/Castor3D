#include <Engine.hpp>

#include "Md3Importer.hpp"

#include <Plugin/ImporterPlugin.hpp>

using namespace Castor3D;
using namespace Castor;

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

C3D_Md3_API void Create( Engine * p_pEngine, ImporterPlugin * p_pPlugin )
{
	p_pPlugin->AttachImporter( std::make_shared< Md3Importer >( *p_pEngine ) );
}

C3D_Md3_API void Destroy( ImporterPlugin * p_pPlugin )
{
	p_pPlugin->DetachImporter();
}

C3D_Md3_API void OnLoad( Castor3D::Engine * p_engine )
{
}

C3D_Md3_API void OnUnload( Castor3D::Engine * p_engine )
{
}
