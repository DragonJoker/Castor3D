#include "PlyImporter.hpp"

#include <ImporterPlugin.hpp>

using namespace Castor3D;
using namespace Castor;
using namespace C3dPly;

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

C3D_Ply_API ImporterPlugin::ExtensionArray GetExtensions()
{
	ImporterPlugin::ExtensionArray l_arrayReturn;
	l_arrayReturn.push_back( ImporterPlugin::Extension( cuT( "PLY" ), cuT( "Stanford Polygon Library" ) ) );
	return l_arrayReturn;
}

C3D_Ply_API void Create( Engine * p_pEngine, ImporterPlugin * p_pPlugin )
{
	Logger::Initialise( p_pEngine->GetLoggerInstance() );
	ImporterSPtr l_pImporter = std::make_shared< PlyImporter >( p_pEngine );
	p_pPlugin->AttachImporter( l_pImporter );
}

C3D_Ply_API void Destroy( ImporterPlugin * p_pPlugin )
{
	p_pPlugin->DetachImporter();
	Logger::Cleanup();
}
