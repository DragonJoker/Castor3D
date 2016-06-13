#include "PlyImporter.hpp"

#if defined( VLD_AVAILABLE )
#	include <vld.h>
#endif

#include <Engine.hpp>

#include <Plugin/ImporterPlugin.hpp>

using namespace Castor3D;
using namespace Castor;

C3D_Ply_API void GetRequiredVersion( Version & p_version )
{
	p_version = Version();
}

C3D_Ply_API ePLUGIN_TYPE GetType()
{
	return ePLUGIN_TYPE_IMPORTER;
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

C3D_Ply_API void Create( Engine * p_pEngine, ImporterPlugin * p_pPlugin )
{
	p_pPlugin->AttachImporter( std::make_shared< C3dPly::PlyImporter >( *p_pEngine ) );
}

C3D_Ply_API void Destroy( ImporterPlugin * p_pPlugin )
{
	p_pPlugin->DetachImporter();
}

C3D_Ply_API void OnLoad( Castor3D::Engine * p_engine )
{
}

C3D_Ply_API void OnUnload( Castor3D::Engine * p_engine )
{
}
