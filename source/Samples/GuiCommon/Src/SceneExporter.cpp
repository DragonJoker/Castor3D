#include "SceneExporter.hpp"

#include <Engine.hpp>
#include <Cache/CacheView.hpp>
#include <Material/Material.hpp>
#include <Material/Pass.hpp>
#include <Mesh/Mesh.hpp>
#include <Mesh/Submesh.hpp>
#include <Mesh/Vertex.hpp>
#include <Mesh/Buffer/GeometryBuffers.hpp>
#include <Mesh/Buffer/IndexBuffer.hpp>
#include <Mesh/Buffer/VertexBuffer.hpp>
#include <Render/RenderWindow.hpp>
#include <Scene/Scene.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureLayout.hpp>
#include <Texture/TextureUnit.hpp>

#include <Data/BinaryFile.hpp>

#include <iomanip>

using namespace Castor3D;
using namespace Castor;

namespace GuiCommon
{
	namespace
	{
		template< typename TObj, typename TKey >
		bool ParseCollection( Engine * p_engine, Collection< TObj, TKey > & p_collection, BinaryChunk & p_chunk, typename TObj::BinaryParser p_parser )
		{
			bool l_result = true;
			p_collection.lock();
			auto l_it = p_collection.begin();

			while ( l_result && l_it != p_collection.end() )
			{
				l_result = p_parser.Fill( *l_it->second, p_chunk );
				++l_it;
			}

			p_collection.unlock();
			return l_result;
		}

		template< typename TObj, typename TKey >
		bool ParseManager( Engine * p_engine, Cache< TKey, TObj > & p_manager, BinaryChunk & p_chunk, typename TObj::BinaryParser p_parser )
		{
			bool l_result = true;
			auto l_lock = make_unique_lock( p_manager );
			auto l_it = p_manager.begin();

			while ( l_result && l_it != p_manager.end() )
			{
				l_result = p_parser.Fill( *l_it->second, p_chunk );
				++l_it;
			}

			return l_result;
		}

		Path GetTextureNewPath( Path const & p_pathSrcFile, Path const & p_pathFolder )
		{
			Path l_pathReturn( cuT( "Texture" ) );
			l_pathReturn /= p_pathSrcFile.GetFullFileName();

			if ( !wxDirExists( ( p_pathFolder / cuT( "Texture" ) ).c_str() ) )
			{
				wxMkDir( string::string_cast< char >( p_pathFolder / cuT( "Texture" ) ).c_str(), 0777 );
			}

			if ( wxFileExists( p_pathSrcFile ) )
			{
				Logger::LogDebug( cuT( "Copying [" ) + p_pathSrcFile + cuT( "] to [" ) + p_pathFolder / l_pathReturn + cuT( "]" ) );
				wxCopyFile( p_pathSrcFile, p_pathFolder / l_pathReturn );
			}

			return l_pathReturn;
		}
	}

	//************************************************************************************************

	void ObjSceneExporter::ExportScene( Scene const & p_scene, Path const & p_fileName )
	{
		Path l_mtlFilePath( p_fileName );
		string::replace( l_mtlFilePath, l_mtlFilePath.GetExtension(), cuT( "mtl" ) );
		DoExportMaterials( p_scene, l_mtlFilePath );
		DoExportMeshes( p_scene, l_mtlFilePath, p_fileName );
	}

	void ObjSceneExporter::DoExportMaterials( Scene const & p_scene, Path const & p_path )const
	{
		auto const & l_cache = p_scene.GetMaterialView();
		Version l_version;
		StringStream l_streamVersion;
		l_streamVersion << l_version.m_iMajor << cuT( "." ) << l_version.m_iMinor << cuT( "." ) << l_version.m_iBuild;
		StringStream l_mtl;
		l_mtl << cuT( "################################################################################\n" );
		l_mtl << cuT( "#" ) << std::setw( 78 ) << std::left << ( cuT( "  MTL File generated by Castor::GuiCommon v" ) + l_streamVersion.str() ) << cuT( "#\n" );
		l_mtl << cuT( "################################################################################\n\n" );

		for ( auto const & l_name : l_cache )
		{
			auto l_material = l_cache.Find( l_name );
			l_mtl << DoExportMaterial( p_path.GetPath(), *l_material ) + cuT( "\n" );
		}

		TextFile l_fileMtl( p_path, File::eOPEN_MODE_WRITE, File::eENCODING_MODE_ASCII );
		l_fileMtl.WriteText( l_mtl.str() );
	}

	void ObjSceneExporter::DoExportMeshes( Castor3D::Scene const & p_scene, Castor::Path const & p_mtlpath, Castor::Path const & p_path )const
	{
		StringStream l_obj;
		Version l_version;
		StringStream l_streamVersion;
		l_streamVersion << l_version.m_iMajor << cuT( "." ) << l_version.m_iMinor << cuT( "." ) << l_version.m_iBuild;

		Path l_pathFileObj( p_path );
		string::replace( l_pathFileObj, l_pathFileObj.GetExtension(), cuT( "obj" ) );
		l_obj << cuT( "################################################################################\n" );
		l_obj << cuT( "#" ) << std::setw( 78 ) << std::left << ( cuT( "  OBJ File generated by Castor::GuiCommon v" ) + l_streamVersion.str() ) << cuT( "#\n" );
		l_obj << cuT( "################################################################################\n\n" );
		l_obj << cuT( "mtllib " ) + p_mtlpath.GetFullFileName() + cuT( "\n\n" );

		uint32_t l_offset = 1;
		uint32_t l_count = 0;
		auto const & l_cache = p_scene.GetMeshCache();

		for ( auto const & l_it : l_cache )
		{
			auto l_mesh = l_it.second;
			l_obj << DoExportMesh( *l_mesh, l_offset, l_count ) << cuT( "\n" );
		}

		TextFile l_fileObj( l_pathFileObj, File::eOPEN_MODE_WRITE, File::eENCODING_MODE_ASCII );
		l_fileObj.WriteText( l_obj.str() );
	}

	String ObjSceneExporter::DoExportMaterial( Path const & p_pathMtlFolder, Material const & p_material )const
	{
		StringStream l_strReturn;
		auto l_pass = p_material.GetPass( 0 );

		if ( l_pass )
		{
			l_strReturn << cuT( "newmtl " ) << p_material.GetName() << cuT( "\n" );
			auto l_ambient = l_pass->GetAmbient();
			l_strReturn << cuT( "	Ka " ) << l_ambient.red().value() << cuT( " " ) << l_ambient.green().value() << cuT( " " ) << l_ambient.blue().value() << cuT( "\n" );
			auto l_diffuse = l_pass->GetDiffuse();
			l_strReturn << cuT( "	Kd " ) << l_diffuse.red().value() << cuT( " " ) << l_diffuse.green().value() << cuT( " " ) << l_diffuse.blue().value() << cuT( "\n" );
			auto l_specular = l_pass->GetSpecular();
			l_strReturn << cuT( "	Ks " ) << l_specular.red().value() << cuT( " " ) << l_specular.green().value() << cuT( " " ) << l_specular.blue().value() << cuT( "\n" );
			l_strReturn << cuT( "	Ns " ) << l_pass->GetShininess() << cuT( "\n" );
			l_strReturn << cuT( "	d " ) << l_pass->GetAlpha() << cuT( "\n" );

			l_strReturn << DoExportTexture( p_pathMtlFolder, cuT( "map_Ka" ), l_pass->GetTextureUnit( TextureChannel::Ambient ) );
			l_strReturn << DoExportTexture( p_pathMtlFolder, cuT( "map_Kd" ), l_pass->GetTextureUnit( TextureChannel::Diffuse ) );
			l_strReturn << DoExportTexture( p_pathMtlFolder, cuT( "map_Bump" ), l_pass->GetTextureUnit( TextureChannel::Normal ) );
			l_strReturn << DoExportTexture( p_pathMtlFolder, cuT( "map_d" ), l_pass->GetTextureUnit( TextureChannel::Opacity ) );
			l_strReturn << DoExportTexture( p_pathMtlFolder, cuT( "map_Ks" ), l_pass->GetTextureUnit( TextureChannel::Specular ) );
			l_strReturn << DoExportTexture( p_pathMtlFolder, cuT( "map_Ns" ), l_pass->GetTextureUnit( TextureChannel::Gloss ) );
		}

		return l_strReturn.str();
	}

	Castor::String ObjSceneExporter::DoExportTexture( Castor::Path const & p_pathMtlFolder, Castor::String p_section, TextureUnitSPtr p_unit )const
	{
		StringStream l_strReturn;

		if ( p_unit )
		{
			Path l_path{ p_unit->GetTexture()->GetImage().ToString() };

			if ( !l_path.empty() )
			{
				l_strReturn << cuT( "	" ) << p_section << cuT( " " ) + GetTextureNewPath( l_path, p_pathMtlFolder ) << cuT( "\n" );
			}
		}

		return l_strReturn.str();
	}

	String ObjSceneExporter::DoExportMesh( Mesh const & p_mesh, uint32_t & p_offset, uint32_t & p_count )const
	{
		StringStream l_strReturn;

		for ( auto const & l_submesh : p_mesh )
		{
			StringStream l_strV;
			StringStream l_strVT;
			StringStream l_strVN;
			StringStream l_strF;
			VertexBuffer & l_vtxBuffer = l_submesh->GetVertexBuffer();
			IndexBuffer & l_idxBuffer = l_submesh->GetIndexBuffer();
			uint32_t l_stride = l_vtxBuffer.GetDeclaration().GetStride();
			uint32_t l_uiNbPoints = l_vtxBuffer.GetSize() / l_stride;
			uint32_t l_uiNbFaces = l_idxBuffer.GetSize() / 3;
			uint8_t * l_pVtx = l_vtxBuffer.data();
			uint32_t * l_pIdx = l_idxBuffer.data();
			Point3r l_ptPos;
			Point3r l_ptNml;
			Point3r l_ptTex;

			for ( uint32_t j = 0; j < l_uiNbPoints; j++ )
			{
				real * l_vertex = reinterpret_cast< real * >( &l_pVtx[j * l_stride] );
				Vertex::GetPosition( l_vertex, l_ptPos );
				Vertex::GetNormal( l_vertex, l_ptNml );
				Vertex::GetTexCoord( l_vertex, l_ptTex );
				l_strV  << cuT( "v " ) << l_ptPos[0] << " " << l_ptPos[1] << " " << l_ptPos[2] << cuT( "\n" );
				l_strVN << cuT( "vn " ) << l_ptNml[0] << " " << l_ptNml[1] << " " << l_ptNml[2] << cuT( "\n" );
				l_strVT << cuT( "vt " ) << l_ptTex[0] << " " << l_ptTex[1] << cuT( "\n" );
			}

			l_strF << cuT( "usemtl " ) << l_submesh->GetDefaultMaterial()->GetName() << cuT( "\ns off\n" );

			for ( uint32_t j = 0; j < l_uiNbFaces; j++ )
			{
				uint32_t * l_pFace = &l_pIdx[j * 3];
				uint32_t l_v0 = p_offset + l_pFace[0];
				uint32_t l_v1 = p_offset + l_pFace[1];
				uint32_t l_v2 = p_offset + l_pFace[2];
				l_strF << cuT( "f " ) << l_v0 << cuT( "/" ) << l_v0 << cuT( "/" ) << l_v0 << cuT( " " ) << l_v1 << cuT( "/" ) << l_v1 << cuT( "/" ) << l_v1 << cuT( " " ) << l_v2 << cuT( "/" ) << l_v2 << cuT( "/" ) << l_v2 << cuT( "\n" );
			}

			l_strReturn << cuT( "g mesh" ) << p_count << cuT( "\n" ) << l_strV.str() << cuT( "\n" ) << l_strVN.str() << cuT( "\n" ) << l_strVT.str() << cuT( "\n" ) << l_strF.str() << cuT( "\n" );
			p_offset += l_uiNbPoints;
			p_count++;
		}

		return l_strReturn.str();
	}

	//************************************************************************************************

	void CscnSceneExporter::ExportScene( Scene const & p_scene, Path const & p_fileName )
	{
		bool l_result = true;
		Path l_folder = p_fileName.GetPath() / p_fileName.GetFileName();

		if ( !File::DirectoryExists( l_folder ) )
		{
			File::DirectoryCreate( l_folder );
		}

		Path l_filePath = l_folder / p_fileName.GetFileName();

		if ( l_result )
		{
			TextFile l_scnFile( Path{ l_filePath + cuT( ".cscn" ) }, File::eOPEN_MODE_WRITE, File::eENCODING_MODE_ASCII );
			l_result = Scene::TextWriter( String() )( p_scene, l_scnFile );
		}

		Path l_subfolder{ cuT( "Meshes" ) };

		if ( l_result )
		{
			if ( !File::DirectoryExists( l_folder / l_subfolder ) )
			{
				File::DirectoryCreate( l_folder / l_subfolder );
			}

			auto l_lock = make_unique_lock( p_scene.GetMeshCache() );

			for ( auto const & l_it : p_scene.GetMeshCache() )
			{
				auto l_mesh = l_it.second;
				Path l_path{ l_folder / l_subfolder / l_it.first + cuT( ".cmsh" ) };
				BinaryFile l_file{ l_path, File::eOPEN_MODE_WRITE };
				l_result &= BinaryWriter< Mesh >{}.Write( *l_mesh, l_file );
			}
		}

		if ( l_result )
		{
			wxMessageBox( _( "Export successful" ) );
		}
		else
		{
			wxMessageBox( _( "Export failed" ) );
		}
	}
}
