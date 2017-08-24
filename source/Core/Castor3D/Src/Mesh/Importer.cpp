#include "Importer.hpp"

#include "Engine.hpp"

#include "Event/Frame/FrameListener.hpp"
#include "Event/Frame/InitialiseEvent.hpp"
#include "Material/Material.hpp"
#include "Material/Pass.hpp"
#include "Mesh/Mesh.hpp"
#include "Mesh/Submesh.hpp"
#include "Scene/Geometry.hpp"
#include "Scene/Scene.hpp"
#include "Texture/TextureImage.hpp"
#include "Texture/TextureLayout.hpp"

using namespace castor;

namespace castor3d
{
	Importer::Importer( Engine & engine )
		: OwnedBy< Engine >( engine )
		, m_fileName()
	{
	}

	bool Importer::importScene( Scene & p_scene, Path const & p_fileName, Parameters const & p_parameters )
	{
		m_fileName = p_fileName;
		m_filePath = m_fileName.getPath();
		m_parameters = p_parameters;
		m_nodes.clear();
		m_geometries.clear();
		bool result = doImportScene( p_scene );

		if ( result )
		{
			for ( auto it : m_geometries )
			{
				auto mesh = it.second->getMesh();
				mesh->computeContainers();
				mesh->computeNormals();

				for ( auto submesh : *mesh )
				{
					p_scene.getListener().postEvent( makeInitialiseEvent( *submesh ) );
				}
			}
		}

		return result;
	}

	bool Importer::importMesh( Mesh & p_mesh, Path const & p_fileName, Parameters const & p_parameters, bool p_initialise )
	{
		m_fileName = p_fileName;
		m_filePath = m_fileName.getPath();
		m_parameters = p_parameters;
		m_nodes.clear();
		m_geometries.clear();
		bool result = true;

		if ( !p_mesh.getSubmeshCount() )
		{
			result = doImportMesh( p_mesh );

			if ( result && p_initialise )
			{
				p_mesh.computeContainers();

				for ( auto submesh : p_mesh )
				{
					p_mesh.getScene()->getListener().postEvent( makeInitialiseEvent( *submesh ) );
				}
			}
		}
		else
		{
			for ( auto submesh : p_mesh )
			{
				submesh->ref( submesh->getDefaultMaterial() );
			}
		}

		return result;
	}

	TextureUnitSPtr Importer::loadTexture( Path const & p_path, Pass & p_pass, TextureChannel p_channel )const
	{
		TextureUnitSPtr unit;
		Path relative;
		Path folder;

		if ( File::fileExists( p_path ) )
		{
			relative = p_path;
		}
		else if ( File::fileExists( m_filePath / p_path ) )
		{
			auto fullPath = m_filePath / p_path;
			folder = fullPath.getPath();
			relative = fullPath.getFileName( true );;
		}
		else
		{
			PathArray files;
			String fileName = p_path.getFileName( true );
			File::listDirectoryFiles( m_filePath, files, true );
			auto it = std::find_if( files.begin(), files.end(), [&fileName]( Path const & p_file )
			{
				return p_file.getFileName( true ) == fileName
					   || p_file.getFileName( true ).find( fileName ) == 0;
			} );

			folder = m_filePath;

			if ( it != files.end() )
			{
				relative = *it;
				relative = Path{ relative.substr( folder.size() + 1 ) };
			}
			else
			{
				relative = Path{ fileName };
			}
		}

		if ( File::fileExists( folder / relative ) )
		{
			try
			{
				TextureUnitSPtr unit = std::make_shared< TextureUnit >( *p_pass.getOwner()->getEngine() );
				unit->setAutoMipmaps( true );
				auto texture = getEngine()->getRenderSystem()->createTexture( TextureType::eTwoDimensions, AccessType::eRead, AccessType::eRead );
				texture->setSource( folder, relative );
				unit->setTexture( texture );
				unit->setChannel( p_channel );
				p_pass.addTextureUnit( unit );
			}
			catch ( std::exception & p_exc )
			{
				unit.reset();
				Logger::logWarning( StringStream() << cuT( "Error encountered while loading texture file " ) << p_path << cuT( ":\n" ) << p_exc.what() );
			}
			catch ( ... )
			{
				unit.reset();
				Logger::logWarning( cuT( "Unknown error encountered while loading texture file " ) + p_path );
			}
		}
		else
		{
			Logger::logWarning( StringStream() << cuT( "Couldn't load texture file " ) << p_path << cuT( ":\nFile does not exist." ) );
		}

		return unit;
	}
}
