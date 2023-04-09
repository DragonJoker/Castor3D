#include "Castor3D/Scene/Light/LightImporter.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/ImporterFile.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Light/Light.hpp"

CU_ImplementSmartPtr( castor3d, LightImporter )

namespace castor3d
{
	LightImporter::LightImporter( Engine & engine )
		: OwnedBy< Engine >{ engine }
	{
	}

	bool LightImporter::import( Light & light
		, ImporterFile * file
		, Parameters const & parameters )
	{
		m_file = file;
		m_parameters = parameters;
		return doImportLight( light );
	}

	bool LightImporter::import( Light & light
		, castor::Path const & path
		, Parameters const & parameters )
	{
		auto & engine = *light.getOwner()->getEngine();
		auto extension = castor::string::lowerCase( path.getExtension() );

		if ( !engine.getImporterFileFactory().isTypeRegistered( extension ) )
		{
			log::error << cuT( "Importer for [" ) << extension << cuT( "] files is not registered, make sure you've got the matching plug-in installed." );
			return false;
		}

		auto file = engine.getImporterFileFactory().create( extension
			, engine
			, path
			, parameters );

		if ( auto importer = file->createLightImporter() )
		{
			return importer->import( light, file.get(), parameters );
		}

		return false;
	}
}
