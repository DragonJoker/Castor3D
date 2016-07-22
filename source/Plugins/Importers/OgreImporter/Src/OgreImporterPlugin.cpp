#include "OgreImporter.hpp"

#if defined( VLD_AVAILABLE )
#	include <vld.h>
#endif

#include <PluginCache.hpp>

#include <Plugin/ImporterPlugin.hpp>

#ifndef _WIN32
#	define C3D_Ogre_API
#else
#	ifdef OgreImporter_EXPORTS
#		define C3D_Ogre_API __declspec(dllexport)
#	else
#		define C3D_Ogre_API __declspec(dllimport)
#	endif
#endif

C3D_Ogre_API void GetRequiredVersion( Castor3D::Version & p_version )
{
	p_version = Castor3D::Version{ CASTOR_VERSION_MAJOR, CASTOR_VERSION_MINOR, CASTOR_VERSION_BUILD };
}

C3D_Ogre_API Castor3D::ePLUGIN_TYPE GetType()
{
	return Castor3D::ePLUGIN_TYPE_IMPORTER;
}

C3D_Ogre_API Castor::String GetName()
{
	return cuT( "Ogre3D Importer" );
}

C3D_Ogre_API Castor3D::ImporterPlugin::ExtensionArray GetExtensions( Castor3D::Engine * p_engine )
{
	Castor3D::ImporterPlugin::ExtensionArray l_extensions;
	l_extensions.emplace_back( cuT( "MESH" ), cuT( "Ogre 3D Mesh" ) );
	return l_extensions;
}

C3D_Ogre_API void OnLoad( Castor3D::Engine * p_engine )
{
	auto l_extensions = GetExtensions( p_engine );

	for ( auto const & l_extension : l_extensions )
	{
		p_engine->GetImporterFactory().Register( Castor::string::lower_case( l_extension.first ), &C3dOgre::OgreImporter::Create );
	}
}

C3D_Ogre_API void OnUnload( Castor3D::Engine * p_engine )
{
	auto l_extensions = GetExtensions( p_engine );

	for ( auto const & l_extension : l_extensions )
	{
		p_engine->GetImporterFactory().Unregister( Castor::string::lower_case( l_extension.first ) );
	}
}

