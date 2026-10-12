#include "Castor3D/Scene/CameraImporter.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/ImporterFile.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Camera.hpp"

CU_ImplementSmartPtr( c3d, CameraImporter )

namespace c3d
{
	CameraImporter::CameraImporter( Engine & engine
		, String const & prefix )
		: OwnedBy< Engine >{ engine }
		, m_prefix{ prefix + cuT( " - " ) }
	{
	}

	CameraRes CameraImporter::importData( String const & name
		, CameraCreateInfo const & createInfo
		, ImporterFile * file
		, ImportConfig const & config )
	{
		if ( !m_file )
		{
			m_file = file;
		}

		auto result = doCreateCamera( name, createInfo );

		if ( !result
			|| !importData( *result, file, config ) )
		{
			return nullptr;
		}

		return result;
	}

	bool CameraImporter::importData( Camera & camera
		, ImporterFile * file
		, ImportConfig const & config )
	{
		m_file = file;
		log::info << getPrefix() << cuT( "Loading Camera [" ) << camera.getName() << cuT( "]" ) << std::endl;
		bool result = doImportCamera( camera );

		if ( result )
		{
			log::info << getPrefix() << cuT( "Loaded Camera [" ) << camera.getName() << cuT( "]" ) << std::endl;
		}
		else
		{
			log::info << getPrefix() << cuT( "Couldn't load Camera [" ) << camera.getName() << cuT( "]" ) << std::endl;
		}

		return result;
	}

	bool CameraImporter::importData( Camera & camera
		, Path const & pathFile
		, ImportConfig const & config )
	{
		auto extension = string::lowerCase( pathFile.getExtension() );
		auto file = camera.getEngine()->getImporterFileFactory().create( extension
			, *camera.getEngine()
			, pathFile
			, config );

		if ( auto importer = file->createCameraImporter() )
		{
			return importer->importData( camera, file.get(), config );
		}

		return false;
	}

	CameraRes CameraImporter::doCreateCamera( String const & name
		, CameraCreateInfo const & createInfo )
	{
		return createInfo.scene->createCamera( name, createInfo );
	}
}
