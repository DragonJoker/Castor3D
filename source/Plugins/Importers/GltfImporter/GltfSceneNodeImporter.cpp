#include "GltfImporter/GltfSceneNodeImporter.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/SceneNode.hpp>

namespace c3d_gltf
{
	GltfSceneNodeImporter::GltfSceneNodeImporter( castor3d::Engine & engine )
		: castor3d::SceneNodeImporter{ engine, cuT( "Gltf" ) }
	{
	}

	bool GltfSceneNodeImporter::doImportSceneNode( castor3d::SceneNode & node )
	{
		auto & file = static_cast< GltfImporterFile const & >( *m_file );
		auto name = node.getName();
		auto it = file.getNodes().find( name );

		if ( it == file.getNodes().end() )
		{
			return false;
		}

		node.setPosition( it->second->translate );
		node.setScale( it->second->scale );
		node.setOrientation( it->second->rotate );

		return true;
	}
}
