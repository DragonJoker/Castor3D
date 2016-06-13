#include "Importer.hpp"

#include "Engine.hpp"

#include "Event/Frame/InitialiseEvent.hpp"
#include "Material/Pass.hpp"
#include "Mesh/Mesh.hpp"
#include "Mesh/Submesh.hpp"
#include "Render/RenderSystem.hpp"
#include "Texture/TextureImage.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureUnit.hpp"

using namespace Castor;

namespace Castor3D
{
	Importer::Importer( Engine & p_engine )
		: OwnedBy< Engine >( p_engine )
		, m_fileName()
	{
	}

	SceneSPtr Importer::ImportScene( Path const & p_fileName, Parameters const & p_parameters )
	{
		m_fileName = p_fileName;
		m_filePath = m_fileName.GetPath();
		m_parameters = p_parameters;
		m_nodes.clear();
		m_geometries.clear();
		return DoImportScene();
	}

	MeshSPtr Importer::ImportMesh( Scene & p_scene, Path const & p_fileName, Parameters const & p_parameters )
	{
		m_fileName = p_fileName;
		m_filePath = m_fileName.GetPath();
		m_parameters = p_parameters;
		m_nodes.clear();
		m_geometries.clear();
		MeshSPtr l_mesh = DoImportMesh( p_scene );

		if ( !l_mesh )
		{
			CASTOR_EXCEPTION( cuT( "The import failed." ) );
		}

		for ( auto l_submesh : *l_mesh )
		{
			GetEngine()->PostEvent( MakeInitialiseEvent( *l_submesh ) );
		}

		return l_mesh;
	}

	TextureUnitSPtr Importer::LoadTexture( Path const & p_path, Pass & p_pass, TextureChannel p_channel )
	{
		TextureUnitSPtr l_unit;
		Path l_relative;
		Path l_folder;

		if ( File::FileExists( p_path ) )
		{
			l_relative = p_path;
		}
		else
		{
			PathArray l_files;
			String l_fileName = p_path.GetFileName( true );
			File::ListDirectoryFiles( m_filePath, l_files, true );
			auto l_it = std::find_if( l_files.begin(), l_files.end(), [&l_fileName]( Path const & p_file )
			{
				return p_file.GetFileName( true ) == l_fileName;
			} );

			l_folder = m_filePath;

			if ( l_it != l_files.end() )
			{
				l_relative = *l_it;
				l_relative = Path{ l_relative.substr( l_folder.size() + 1 ) };
			}
			else
			{
				l_relative = Path{ l_fileName };
			}
		}

		if ( File::FileExists( l_folder / l_relative ) )
		{
			try
			{
				TextureUnitSPtr l_unit = std::make_shared< TextureUnit >( *p_pass.GetEngine() );
				l_unit->SetAutoMipmaps( true );
				auto l_texture = GetEngine()->GetRenderSystem()->CreateTexture( TextureType::TwoDimensions, eACCESS_TYPE_READ, eACCESS_TYPE_READ );
				l_texture->GetImage().SetSource( l_folder, l_relative );
				l_unit->SetTexture( l_texture );
				l_unit->SetChannel( p_channel );
				p_pass.AddTextureUnit( l_unit );
			}
			catch ( std::exception & p_exc )
			{
				l_unit.reset();
				Logger::LogWarning( StringStream() << cuT( "Error encountered while loading texture file " ) << p_path << cuT( ":\n" ) << p_exc.what() );
			}
			catch ( ... )
			{
				l_unit.reset();
				Logger::LogWarning( cuT( "Unknown error encountered while loading texture file " ) + p_path );
			}
		}
		else
		{
			Logger::LogWarning( StringStream() << cuT( "Couldn't load texture file " ) << p_path << cuT( ":\nFile does not exist." ) );
		}

		return l_unit;
	}
}
