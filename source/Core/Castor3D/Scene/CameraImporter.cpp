#include "Castor3D/Scene/CameraImporter.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/ImporterFile.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Camera.hpp"

CU_ImplementSmartPtr( castor3d, CameraImporter )

namespace castor3d
{
	CameraImporter::CameraImporter( Engine & engine )
		: OwnedBy< Engine >{ engine }
	{
	}

	bool CameraImporter::import( Camera & node
		, ImporterFile * file
		, Parameters const & parameters )
	{
		m_file = file;
		m_parameters = parameters;
		return doImportCamera( node );
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
