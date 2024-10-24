#include "Castor3D/Scene/CameraImporter.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/ImporterFile.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Camera.hpp"

CU_ImplementSmartPtr( castor3d, CameraImporter )

namespace castor3d
{
	CameraImporter::CameraImporter( Engine & engine
		, castor::String const & prefix )
		: OwnedBy< Engine >{ engine }
		, m_prefix{ prefix + cuT( " - " ) }
	{
	}

	bool CameraImporter::import( Camera & camera
		, ImporterFile * file
		, Parameters const & parameters )
	{
		m_file = file;
		m_parameters = parameters;
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

	bool CameraImporter::import( Camera & camera
		, castor::Path const & pathFile
		, Parameters const & parameters )
	{
		auto extension = castor::string::lowerCase( pathFile.getExtension() );
		auto file = camera.getEngine()->getImporterFileFactory().create( extension
			, *camera.getEngine()
			, pathFile
			, parameters );

		if ( auto importer = file->createCameraImporter() )
		{
			return importer->import( camera, file.get(), parameters );
		}

		return false;
	}
}
