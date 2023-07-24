#include "GltfImporter/GltfSceneNodeImporter.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/SceneNode.hpp>

namespace c3d_gltf
{
	GltfSceneNodeImporter::GltfSceneNodeImporter( castor3d::Engine & engine, uint32_t instanceIndex )
		: castor3d::SceneNodeImporter{ engine }
		, m_instanceIndex{ instanceIndex }
	{
	}

	bool GltfSceneNodeImporter::doImportSceneNode( castor3d::SceneNode & node )
	{
		auto & file = static_cast< GltfImporterFile const & >( *m_file );
		auto name = node.getName();
		auto it = std::find_if( file.getNodes().begin()
			, file.getNodes().end()
			, [&name]( castor3d::ImporterFile::NodeData const & lookup )
			{
				return name == lookup.name;
			} );

		if ( it == file.getNodes().end() )
		{
			return false;
		}

		castor3d::log::info << cuT( "  SceneNode found: [" ) << name << cuT( "]" ) << std::endl;
		node.setPosition( it->transform.translate );
		node.setScale( it->transform.scale );
		node.setOrientation( it->transform.rotate );

		return true;
	}
}
