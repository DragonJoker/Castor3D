#include "Castor3D/Scene/SceneNodeImporter.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/ImporterFile.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"

CU_ImplementSmartPtr( castor3d, SceneNodeImporter )

namespace castor3d
{
	SceneNodeImporter::SceneNodeImporter( Engine & engine
		, castor::String const & prefix )
		: OwnedBy< Engine >{ engine }
		, m_prefix{ prefix + cuT( " - " ) }
	{
	}

	SceneNodeUPtr SceneNodeImporter::importData( castor::String const & name
		, SceneNodeCreateInfo const & createInfo
		, ImporterFile * file
		, Parameters const & parameters )
	{
		if ( !m_file )
		{
			m_file = file;
		}

		auto result = doCreateSceneNode( name, createInfo );

		if ( !result
			|| !importData( *result, file, parameters ) )
		{
			return nullptr;
		}

		return result;
	}

	bool SceneNodeImporter::importData( SceneNode & node
		, ImporterFile * file
		, Parameters const & parameters )
	{
		m_file = file;
		m_parameters = parameters;
		log::info << getPrefix() << cuT( "Loading SceneNode [" ) << node.getName() << cuT( "]" ) << std::endl;
		bool result = doImportSceneNode( node );

		if ( result )
		{
			log::info << getPrefix() << cuT( "Loaded SceneNode [" ) << node.getName() << cuT( "]" ) << std::endl;
		}
		else
		{
			log::info << getPrefix() << cuT( "Couldn't load SceneNode [" ) << node.getName() << cuT( "]" ) << std::endl;
		}

		return result;
	}

	bool SceneNodeImporter::importData( SceneNode & node
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
			return importer->importData( node, file.get(), parameters );
		}

		return false;
	}

	SceneNodeUPtr SceneNodeImporter::doCreateSceneNode( castor::String const & name
		, SceneNodeCreateInfo const & createInfo )
	{
		return createInfo.scene->createSceneNode( name, createInfo );
	}
}
