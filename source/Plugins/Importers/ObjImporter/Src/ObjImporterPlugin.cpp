#include "ObjImporter.hpp"

#include <Engine.hpp>
#include <Plugin/ImporterPlugin.hpp>

#include <Engine.hpp>

using namespace Castor3D;
using namespace Castor;

C3D_Obj_API void GetRequiredVersion( Version & p_version )
{
	p_version = Version();
}

C3D_Obj_API PluginType GetType()
{
	return PluginType::eImporter;
}

C3D_Obj_API String GetName()
{
	return cuT( "OBJ Importer" );
}

C3D_Obj_API ImporterPlugin::ExtensionArray GetExtensions( Engine * p_engine )
{
	ImporterPlugin::ExtensionArray l_arrayReturn;
	l_arrayReturn.push_back( ImporterPlugin::Extension( cuT( "OBJ" ), cuT( "Wavefront Object" ) ) );
	return l_arrayReturn;
}

C3D_Obj_API void OnLoad( Castor3D::Engine * p_engine )
{
	auto l_extensions = GetExtensions( p_engine );

	for ( auto const & l_extension : l_extensions )
	{
		p_engine->GetImporterFactory().Register( Castor::string::lower_case( l_extension.first ), &Obj::ObjImporter::Create );
	}
}

C3D_Obj_API void OnUnload( Castor3D::Engine * p_engine )
{
	auto l_extensions = GetExtensions( p_engine );

	for ( auto const & l_extension : l_extensions )
	{
		p_engine->GetImporterFactory().Unregister( Castor::string::lower_case( l_extension.first ) );
	}
}
