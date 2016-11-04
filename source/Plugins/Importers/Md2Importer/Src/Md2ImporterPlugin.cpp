#include <Engine.hpp>

#include "Md2Importer.hpp"

#include <Engine.hpp>
#include <Plugin/ImporterPlugin.hpp>

using namespace Castor3D;
using namespace Castor;

#ifndef _WIN32
#	define C3D_Md2_API
#else
#	ifdef Md2Importer_EXPORTS
#		define C3D_Md2_API __declspec(dllexport)
#	else
#		define C3D_Md2_API __declspec(dllimport)
#	endif
#endif

C3D_Md2_API void GetRequiredVersion( Version & p_version )
{
	p_version = Version();
}

C3D_Md2_API PluginType GetType()
{
	return PluginType::eImporter;
}

C3D_Md2_API String GetName()
{
	return cuT( "MD2 Importer" );
}

C3D_Md2_API ImporterPlugin::ExtensionArray GetExtensions( Engine * p_engine )
{
	ImporterPlugin::Extension l_extension( cuT( "MD2" ), cuT( "Quake II" ) );
	ImporterPlugin::ExtensionArray l_arrayReturn( 1, l_extension );
	return l_arrayReturn;
}

C3D_Md2_API void OnLoad( Castor3D::Engine * p_engine )
{
	auto l_extensions = GetExtensions( p_engine );

	for ( auto const & l_extension : l_extensions )
	{
		p_engine->GetImporterFactory().Register( Castor::string::lower_case( l_extension.first ), &C3DMd2::Md2Importer::Create );
	}
}

C3D_Md2_API void OnUnload( Castor3D::Engine * p_engine )
{
	auto l_extensions = GetExtensions( p_engine );

	for ( auto const & l_extension : l_extensions )
	{
		p_engine->GetImporterFactory().Unregister( Castor::string::lower_case( l_extension.first ) );
	}
}
