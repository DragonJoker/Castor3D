#include "ObjImporter.hpp"

#include <ImporterPlugin.hpp>
#include <Engine.hpp>

using namespace Castor3D;
using namespace Castor;

C3D_Obj_API void GetRequiredVersion( Version & p_version )
{
	p_version = Version();
}

C3D_Obj_API ePLUGIN_TYPE GetType()
{
	return ePLUGIN_TYPE_IMPORTER;
}

C3D_Obj_API String GetName()
{
	return cuT( "OBJ Importer" );
}

C3D_Obj_API ImporterPlugin::ExtensionArray GetExtensions()
{
	ImporterPlugin::ExtensionArray l_arrayReturn;
	l_arrayReturn.push_back( ImporterPlugin::Extension( cuT( "OBJ" ), cuT( "Wavefront Object" ) ) );
	return l_arrayReturn;
}

C3D_Obj_API void Create( Engine * p_pEngine, ImporterPlugin * p_pPlugin )
{
	Logger::Initialise( p_pEngine->GetLoggerInstance() );
	p_pPlugin->AttachImporter( std::make_shared< Obj::ObjImporter >( p_pEngine ) );
}

C3D_Obj_API void Destroy( ImporterPlugin * p_pPlugin )
{
	p_pPlugin->DetachImporter();
	Logger::Cleanup();
}
