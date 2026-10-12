#include "Castor3D/Scene/SceneNodeImporter.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/ImporterFile.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"

CU_ImplementSmartPtr( c3d, SceneNodeImporter )

namespace c3d
{
	SceneNodeImporter::SceneNodeImporter( Engine & engine
		, String const & prefix )
		: OwnedBy< Engine >{ engine }
		, m_prefix{ prefix + cuT( " - " ) }
	{
	}

	SceneNodeUPtr SceneNodeImporter::importData( String const & name
		, SceneNodeCreateInfo const & createInfo
		, ImporterFile * file
		, ImportConfig const & config )
	{
		if ( !m_file )
		{
			m_file = file;
		}

		auto result = doCreateSceneNode( name, createInfo );

		if ( !result
			|| !importData( *result, file, config ) )
		{
			return nullptr;
		}

		return result;
	}

	bool SceneNodeImporter::importData( SceneNode & node
		, ImporterFile * file
		, ImportConfig const & config )
	{
		m_file = file;
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
		, Path const & pathFile
		, ImportConfig const & config )
	{
		auto extension = string::lowerCase( pathFile.getExtension() );
		auto file = node.getEngine()->getImporterFileFactory().create( extension
			, *node.getEngine()
			, pathFile
			, config );

		if ( auto importer = file->createSceneNodeImporter() )
		{
			return importer->importData( node, file.get(), config );
		}

		return false;
	}

	SceneNodeUPtr SceneNodeImporter::doCreateSceneNode( String const & name
		, SceneNodeCreateInfo const & createInfo )
	{
		return createInfo.scene->createSceneNode( name, createInfo );
	}
}
