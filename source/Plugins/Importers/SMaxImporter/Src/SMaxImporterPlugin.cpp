#include "SMaxImporter.hpp"

#include <Engine.hpp>
#include <ImporterPlugin.hpp>

using namespace SMax;
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

C3D_SMax_API ImporterPlugin::ExtensionArray GetExtensions()
{
	ImporterPlugin::Extension l_extension( cuT( "3DS" ), cuT( "3D Studio Max 3DS" ) );
	ImporterPlugin::ExtensionArray l_arrayReturn( 1, l_extension );
	return l_arrayReturn;
}

C3D_SMax_API void Create( Engine * p_pEngine, ImporterPlugin * p_pPlugin )
{
	p_pPlugin->AttachImporter( std::make_shared< SMaxImporter >( *p_pEngine ) );
}

C3D_SMax_API void Destroy( ImporterPlugin * p_pPlugin )
{
	p_pPlugin->DetachImporter();
}

C3D_SMax_API void OnLoad( Castor3D::Engine * p_engine )
{
}

C3D_SMax_API void OnUnload( Castor3D::Engine * p_engine )
{
}
