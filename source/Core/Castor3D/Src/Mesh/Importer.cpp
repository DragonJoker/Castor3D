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

using namespace Castor;

namespace Castor3D
{
	Importer::Importer( Engine & engine )
		: OwnedBy< Engine >( engine )
		, m_fileName()
	{
	}

	bool Importer::ImportScene( Scene & p_scene, Path const & p_fileName, Parameters const & p_parameters )
	{
		m_fileName = p_fileName;
		m_filePath = m_fileName.GetPath();
		m_parameters = p_parameters;
		m_nodes.clear();
		m_geometries.clear();
		bool result = DoImportScene( p_scene );

		if ( result )
		{
			for ( auto it : m_geometries )
			{
				auto mesh = it.second->GetMesh();
				mesh->ComputeContainers();
				mesh->ComputeNormals();

				for ( auto submesh : *mesh )
				{
					p_scene.GetListener().PostEvent( MakeInitialiseEvent( *submesh ) );
				}
			}
		}

		return result;
	}

	bool Importer::ImportMesh( Mesh & p_mesh, Path const & p_fileName, Parameters const & p_parameters, bool p_initialise )
	{
		m_fileName = p_fileName;
		m_filePath = m_fileName.GetPath();
		m_parameters = p_parameters;
		m_nodes.clear();
		m_geometries.clear();
		bool result = true;

		if ( !p_mesh.GetSubmeshCount() )
		{
			result = DoImportMesh( p_mesh );

			if ( result && p_initialise )
			{
				p_mesh.ComputeContainers();

				for ( auto submesh : p_mesh )
				{
					p_mesh.GetScene()->GetListener().PostEvent( MakeInitialiseEvent( *submesh ) );
				}
			}
		}
		else
		{
			for ( auto submesh : p_mesh )
			{
				submesh->Ref( submesh->GetDefaultMaterial() );
			}
		}

		return result;
	}

	TextureUnitSPtr Importer::LoadTexture( Path const & p_path, Pass & p_pass, TextureChannel p_channel )const
	{
		TextureUnitSPtr unit;
		Path relative;
		Path folder;

		if ( File::FileExists( p_path ) )
		{
			relative = p_path;
		}
		else
		{
			PathArray files;
			String fileName = p_path.GetFileName( true );
			File::ListDirectoryFiles( m_filePath, files, true );
			auto it = std::find_if( files.begin(), files.end(), [&fileName]( Path const & p_file )
			{
				return p_file.GetFileName( true ) == fileName
					   || p_file.GetFileName( true ).find( fileName ) == 0;
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

		if ( File::FileExists( folder / relative ) )
		{
			try
			{
				TextureUnitSPtr unit = std::make_shared< TextureUnit >( *p_pass.GetOwner()->GetEngine() );
				unit->SetAutoMipmaps( true );
				auto texture = GetEngine()->GetRenderSystem()->CreateTexture( TextureType::eTwoDimensions, AccessType::eRead, AccessType::eRead );
				texture->SetSource( folder, relative );
				unit->SetTexture( texture );
				unit->SetChannel( p_channel );
				p_pass.AddTextureUnit( unit );
			}
			catch ( std::exception & p_exc )
			{
				unit.reset();
				Logger::LogWarning( StringStream() << cuT( "Error encountered while loading texture file " ) << p_path << cuT( ":\n" ) << p_exc.what() );
			}
			catch ( ... )
			{
				unit.reset();
				Logger::LogWarning( cuT( "Unknown error encountered while loading texture file " ) + p_path );
			}
		}
		else
		{
			Logger::LogWarning( StringStream() << cuT( "Couldn't load texture file " ) << p_path << cuT( ":\nFile does not exist." ) );
		}

		return unit;
	}
}
