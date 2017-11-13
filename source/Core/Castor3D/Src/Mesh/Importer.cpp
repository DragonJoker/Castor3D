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

	bool Importer::importScene( Scene & scene
		, Path const & fileName
		, Parameters const & parameters )
	{
		bool splitSubmeshes = false;
		m_parameters.get( cuT( "split_mesh" ), splitSubmeshes );
		m_fileName = fileName;
		m_filePath = m_fileName.getPath();
		m_parameters = parameters;
		m_nodes.clear();
		m_geometries.clear();
		bool result = doImportScene( scene );

		if ( result )
		{
			for ( auto it : m_geometries )
			{
				auto mesh = it.second->getMesh();
				mesh->computeContainers();
				mesh->computeNormals();

				for ( auto submesh : *mesh )
				{
					scene.getListener().postEvent( makeInitialiseEvent( *submesh ) );
				}
			}
		}

		return result;
	}

	bool Importer::importMesh( Mesh & mesh
		, Path const & fileName
		, Parameters const & parameters
		, bool initialise )
	{
		bool splitSubmeshes = false;
		m_parameters.get( cuT( "split_mesh" ), splitSubmeshes );
		m_fileName = fileName;
		m_filePath = m_fileName.getPath();
		m_parameters = parameters;
		m_nodes.clear();
		m_geometries.clear();
		bool result = true;

		if ( !mesh.getSubmeshCount() )
		{
			result = doImportMesh( mesh );

			if ( result && initialise )
			{
				mesh.computeContainers();

				for ( auto submesh : mesh )
				{
					mesh.getScene()->getListener().postEvent( makeInitialiseEvent( *submesh ) );
				}
			}
		}
		else
		{
			for ( auto submesh : mesh )
			{
				submesh->setMaterial( nullptr, submesh->getDefaultMaterial(), false );
			}
		}

		return result;
	}

	TextureUnitSPtr Importer::loadTexture( Path const & path
		, Pass & pass
		, TextureChannel channel )const
	{
		TextureUnitSPtr unit;
		Path relative;
		Path folder;

		if ( File::fileExists( path ) )
		{
			relative = path;
		}
		else if ( File::fileExists( m_filePath / path ) )
		{
			auto fullPath = m_filePath / path;
			folder = fullPath.getPath();
			relative = fullPath.getFileName( true );;
		}
		else
		{
			PathArray files;
			String fileName = path.getFileName( true );
			File::listDirectoryFiles( m_filePath, files, true );
			auto it = std::find_if( files.begin()
				, files.end()
				, [&fileName]( Path const & p_file )
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
				TextureUnitSPtr unit = std::make_shared< TextureUnit >( *pass.getOwner()->getEngine() );
				unit->setAutoMipmaps( true );
				auto texture = getEngine()->getRenderSystem()->createTexture( TextureType::eTwoDimensions
					, AccessType::eRead
					, AccessType::eRead );
				texture->setSource( folder, relative );
				unit->setTexture( texture );
				unit->setChannel( channel );
				pass.addTextureUnit( unit );
			}
			catch ( std::exception & p_exc )
			{
				unit.reset();
				Logger::logWarning( StringStream() << cuT( "Error encountered while loading texture file " ) << path << cuT( ":\n" ) << p_exc.what() );
			}
			catch ( ... )
			{
				unit.reset();
				Logger::logWarning( cuT( "Unknown error encountered while loading texture file " ) + path );
			}
		}
		else
		{
			Logger::logWarning( StringStream() << cuT( "Couldn't load texture file " ) << path << cuT( ":\nFile does not exist." ) );
		}

		return unit;
	}
}
