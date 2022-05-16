#include "Castor3D/Scene/SceneNodeImporter.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/ImporterFile.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"

namespace castor3d
{
	SceneNodeImporter::SceneNodeImporter( Engine & engine )
		: OwnedBy< Engine >{ engine }
	{
	}

	bool SceneNodeImporter::import( SceneNode & node
		, ImporterFile * file
		, Parameters const & parameters )
	{
		m_file = file;
		m_parameters = parameters;
		return doImportSceneNode( node );
	}

	bool SceneNodeImporter::import( SceneNode & node
		, castor::Path const & pathFile
		, Parameters const & parameters )
	{
		auto extension = castor::string::lowerCase( pathFile.getExtension() );
		auto file = node.getEngine()->getImporterFileFactory().create( extension
			, *node.getEngine()
			, pathFile
			, parameters );

		if ( auto importer = file->createSceneNodeImporter() )
		{
			return importer->import( node, file.get(), parameters );
		}

		return false;
	}
}
