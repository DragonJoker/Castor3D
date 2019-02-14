#include "SceneExporter.hpp"

#include <Engine.hpp>
#include <Binary/BinaryMesh.hpp>
#include <Binary/BinarySkeleton.hpp>
#include <Cache/CacheView.hpp>
#include <Material/Material.hpp>
#include <Material/LegacyPass.hpp>
#include <Material/MetallicRoughnessPbrPass.hpp>
#include <Material/SpecularGlossinessPbrPass.hpp>
#include <Mesh/Mesh.hpp>
#include <Mesh/Skeleton/Skeleton.hpp>
#include <Mesh/Submesh.hpp>
#include <Mesh/Vertex.hpp>
#include <Render/RenderWindow.hpp>
#include <Scene/Scene.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureLayout.hpp>
#include <Texture/TextureUnit.hpp>

#include <Ashes/Buffer/VertexBuffer.hpp>

#include <Data/BinaryFile.hpp>

#include <iomanip>

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	namespace
	{
		template< typename TObj, typename TKey >
		bool ParseCollection( Engine * engine, Collection< TObj, TKey > & p_collection, BinaryChunk & p_chunk, typename TObj::BinaryParser p_parser )
		{
			bool result = true;
			p_collection.lock();
			auto it = p_collection.begin();

			while ( result && it != p_collection.end() )
			{
				result = p_parser.fill( *it->second, p_chunk );
				++it;
			}

			p_collection.unlock();
			return result;
		}

		template< typename TObj, typename TKey >
		bool ParseManager( Engine * engine, Cache< TKey, TObj > & p_manager, BinaryChunk & p_chunk, typename TObj::BinaryParser p_parser )
		{
			bool result = true;
			auto lock = makeUniqueLock( p_manager );
			auto it = p_manager.begin();

			while ( result && it != p_manager.end() )
			{
				result = p_parser.fill( *it->second, p_chunk );
				++it;
			}

			return result;
		}

		Path getTextureNewPath( Path const & p_pathSrcFile, Path const & p_pathFolder )
		{
			Path pathReturn( cuT( "Texture" ) );
			pathReturn /= p_pathSrcFile.getFullFileName();

			if ( !wxDirExists( ( p_pathFolder / cuT( "Texture" ) ).c_str() ) )
			{
				wxMkDir( string::stringCast< char >( p_pathFolder / cuT( "Texture" ) ).c_str(), 0777 );
			}

			if ( wxFileExists( p_pathSrcFile ) )
			{
				Logger::logDebug( cuT( "Copying [" ) + p_pathSrcFile + cuT( "] to [" ) + p_pathFolder / pathReturn + cuT( "]" ) );
				wxCopyFile( p_pathSrcFile, p_pathFolder / pathReturn );
			}

			return pathReturn;
		}

		Path getMeshNewPath( Path const & p_path )
		{
			Path pathReturn( p_path.getPath() / cuT( "Mesh" ) );

			if ( !wxDirExists( pathReturn.c_str() ) )
			{
				wxMkDir( string::stringCast< char >( pathReturn ).c_str(), 0777 );
			}

			return pathReturn;
		}
	}

	//************************************************************************************************

	void ObjSceneExporter::ExportScene( Scene const & p_scene, Path const & p_fileName )
	{
		Path mtlFilePath( p_fileName );
		string::replace( mtlFilePath, mtlFilePath.getExtension(), cuT( "mtl" ) );
		try
		{
			doExportMaterials( p_scene, mtlFilePath );
			doExportMeshes( p_scene, mtlFilePath, p_fileName );
			wxMessageBox( _( "Export successful" ) );
		}
		catch ( std::exception & exc )
		{
			wxMessageBox( wxString{} << _( "Export failed" ) << wxT( "\n" ) << exc.what() );
		}
	}

	void ObjSceneExporter::doExportMaterials( Scene const & p_scene, Path const & p_path )const
	{
		auto const & cache = p_scene.getMaterialView();
		StringStream streamVersion( makeStringStream() );
		streamVersion << Version{};
		StringStream mtl( makeStringStream() );
		mtl << cuT( "################################################################################\n" );
		mtl << cuT( "#" ) << std::setw( 78 ) << std::left << ( cuT( "  MTL File generated by castor::GuiCommon v" ) + streamVersion.str() ) << cuT( "#\n" );
		mtl << cuT( "################################################################################\n\n" );

		for ( auto const & name : cache )
		{
			auto material = cache.find( name );
			mtl << doExportMaterial( p_path.getPath(), *material ) + cuT( "\n" );
		}

		TextFile fileMtl( p_path, File::OpenMode::eWrite, File::EncodingMode::eASCII );
		fileMtl.writeText( mtl.str() );
	}

	void ObjSceneExporter::doExportMeshes( castor3d::Scene const & p_scene, castor::Path const & p_mtlpath, castor::Path const & p_path )const
	{
		StringStream obj( makeStringStream() );
		StringStream streamVersion( makeStringStream() );
		streamVersion << Version{};

		Path pathFileObj = getMeshNewPath( p_path );
		auto const & cache = p_scene.getMeshCache();
		cache.lock();

		for ( auto const & it : cache )
		{
			auto mesh = it.second;
			obj << cuT( "################################################################################\n" );
			obj << cuT( "#" ) << std::setw( 78 ) << std::left << ( cuT( "  OBJ File generated by castor::GuiCommon v" ) + streamVersion.str() ) << cuT( "#\n" );
			obj << cuT( "################################################################################\n\n" );
			obj << cuT( "mtllib " ) + p_mtlpath.getFullFileName() + cuT( "\n\n" );

			uint32_t offset = 1;
			uint32_t count = 0;
			obj << doExportMesh( *mesh, offset, count ) << cuT( "\n" );
			TextFile fileObj( pathFileObj / ( it.first + cuT( ".obj" ) )
				, File::OpenMode::eWrite
				, File::EncodingMode::eASCII );
			fileObj.writeText( obj.str() );
		}

		cache.unlock();
	}

	String ObjSceneExporter::doExportMaterial( Path const & p_pathMtlFolder, Material const & p_material )const
	{
		StringStream strReturn( makeStringStream() );

		if ( p_material.getType() == MaterialType::eLegacy )
		{
			auto pass = p_material.getTypedPass< MaterialType::eLegacy > ( 0u );

			if ( pass )
			{
				strReturn << cuT( "newmtl " ) << p_material.getName() << cuT( "\n" );
				auto diffuse = pass->getDiffuse();
				strReturn << cuT( "	Kd " ) << diffuse.red().value() << cuT( " " ) << diffuse.green().value() << cuT( " " ) << diffuse.blue().value() << cuT( "\n" );
				auto specular = pass->getSpecular();
				strReturn << cuT( "	Ks " ) << specular.red().value() << cuT( " " ) << specular.green().value() << cuT( " " ) << specular.blue().value() << cuT( "\n" );
				strReturn << cuT( "	Ns " ) << pass->getShininess() << cuT( "\n" );
				strReturn << cuT( "	d " ) << pass->getOpacity() << cuT( "\n" );

				strReturn << doExportTexture( p_pathMtlFolder, cuT( "map_Kd" ), pass->getTextureUnit( TextureChannel::eDiffuse ) );
				strReturn << doExportTexture( p_pathMtlFolder, cuT( "map_Bump" ), pass->getTextureUnit( TextureChannel::eNormal ) );
				strReturn << doExportTexture( p_pathMtlFolder, cuT( "map_d" ), pass->getTextureUnit( TextureChannel::eOpacity ) );
				strReturn << doExportTexture( p_pathMtlFolder, cuT( "map_Ks" ), pass->getTextureUnit( TextureChannel::eSpecular ) );
				strReturn << doExportTexture( p_pathMtlFolder, cuT( "map_Ns" ), pass->getTextureUnit( TextureChannel::eGloss ) );
			}
		}
		else if ( p_material.getType() == MaterialType::ePbrMetallicRoughness )
		{
			auto pass = p_material.getTypedPass< MaterialType::ePbrMetallicRoughness >( 0u );

			if ( pass )
			{
				strReturn << cuT( "newmtl " ) << p_material.getName() << cuT( "\n" );
				auto albedo = pass->getAlbedo();
				strReturn << cuT( "	Kd " ) << albedo.red().value() << cuT( " " ) << albedo.green().value() << cuT( " " ) << albedo.blue().value() << cuT( "\n" );
				strReturn << cuT( "	Ks " ) << pass->getMetallic() << cuT( " " ) << pass->getMetallic() << cuT( " " ) << pass->getMetallic() << cuT( " 0.0\n" );
				strReturn << cuT( "	Ns " ) << pass->getRoughness() << cuT( "\n" );
				strReturn << cuT( "	d " ) << pass->getOpacity() << cuT( "\n" );

				strReturn << doExportTexture( p_pathMtlFolder, cuT( "map_Kd" ), pass->getTextureUnit( TextureChannel::eAlbedo ) );
				strReturn << doExportTexture( p_pathMtlFolder, cuT( "map_Bump" ), pass->getTextureUnit( TextureChannel::eNormal ) );
				strReturn << doExportTexture( p_pathMtlFolder, cuT( "map_d" ), pass->getTextureUnit( TextureChannel::eOpacity ) );
				strReturn << doExportTexture( p_pathMtlFolder, cuT( "map_Ks" ), pass->getTextureUnit( TextureChannel::eMetallic ) );
				strReturn << doExportTexture( p_pathMtlFolder, cuT( "map_Ns" ), pass->getTextureUnit( TextureChannel::eRoughness ) );
			}
		}
		else if ( p_material.getType() == MaterialType::ePbrSpecularGlossiness )
		{
			auto pass = p_material.getTypedPass< MaterialType::ePbrSpecularGlossiness >( 0u );

			if ( pass )
			{
				strReturn << cuT( "newmtl " ) << p_material.getName() << cuT( "\n" );
				auto diffuse = pass->getDiffuse();
				strReturn << cuT( "	Kd " ) << diffuse.red().value() << cuT( " " ) << diffuse.green().value() << cuT( " " ) << diffuse.blue().value() << cuT( "\n" );
				auto specular = pass->getSpecular();
				strReturn << cuT( "	Ks " ) << specular.red().value() << cuT( " " ) << specular.green().value() << cuT( " " ) << specular.blue().value() << cuT( "\n" );
				strReturn << cuT( "	Ns " ) << pass->getGlossiness() << cuT( "\n" );
				strReturn << cuT( "	d " ) << pass->getOpacity() << cuT( "\n" );

				strReturn << doExportTexture( p_pathMtlFolder, cuT( "map_Kd" ), pass->getTextureUnit( TextureChannel::eDiffuse ) );
				strReturn << doExportTexture( p_pathMtlFolder, cuT( "map_Bump" ), pass->getTextureUnit( TextureChannel::eNormal ) );
				strReturn << doExportTexture( p_pathMtlFolder, cuT( "map_d" ), pass->getTextureUnit( TextureChannel::eOpacity ) );
				strReturn << doExportTexture( p_pathMtlFolder, cuT( "map_Ks" ), pass->getTextureUnit( TextureChannel::eSpecular ) );
				strReturn << doExportTexture( p_pathMtlFolder, cuT( "map_Ns" ), pass->getTextureUnit( TextureChannel::eGloss ) );
			}
		}

		return strReturn.str();
	}

	castor::String ObjSceneExporter::doExportTexture( castor::Path const & p_pathMtlFolder, castor::String p_section, TextureUnitSPtr p_unit )const
	{
		StringStream strReturn( makeStringStream() );

		if ( p_unit && p_unit->getTexture() )
		{
			Path path{ p_unit->getTexture()->getImage().toString() };

			if ( !path.empty() )
			{
				strReturn << cuT( "	" ) << p_section << cuT( " " ) + getTextureNewPath( path, p_pathMtlFolder ) << cuT( "\n" );
			}
		}

		return strReturn.str();
	}

	String ObjSceneExporter::doExportMesh( Mesh const & mesh, uint32_t & offset, uint32_t & count )const
	{
		StringStream result( makeStringStream() );

		for ( auto const & submesh : mesh )
		{
			if ( submesh->hasComponent( TriFaceMapping::Name ) )
			{
				StringStream strV( makeStringStream() );
				StringStream strVT( makeStringStream() );
				StringStream strVN( makeStringStream() );
				StringStream strF( makeStringStream() );
				ashes::Buffer< uint32_t > & idxBuffer = submesh->getIndexBuffer();
				auto stride = uint32_t( sizeof( InterleavedVertex ) );
				Point3r ptPos;
				Point3r ptNml;
				Point3r ptTex;

				for ( auto & point : submesh->getPoints() )
				{
					strV << cuT( "v " ) << point.pos[0] << " " << point.pos[1] << " " << point.pos[2] << cuT( "\n" );
					strVN << cuT( "vn " ) << point.nml[0] << " " << point.nml[1] << " " << point.nml[2] << cuT( "\n" );
					strVT << cuT( "vt " ) << point.tex[0] << " " << point.tex[1] << cuT( "\n" );
				}

				strF << cuT( "usemtl " ) << submesh->getDefaultMaterial()->getName() << cuT( "\ns off\n" );
				auto & indexMapping = *submesh->getComponent< TriFaceMapping >();

				for ( uint32_t j = 0; j < indexMapping.getCount(); j++ )
				{
					auto & face = indexMapping[j * 3];
					uint32_t v0 = offset + face[0];
					uint32_t v1 = offset + face[1];
					uint32_t v2 = offset + face[2];
					strF << cuT( "f " ) << v0 << cuT( "/" ) << v0 << cuT( "/" ) << v0 << cuT( " " ) << v1 << cuT( "/" ) << v1 << cuT( "/" ) << v1 << cuT( " " ) << v2 << cuT( "/" ) << v2 << cuT( "/" ) << v2 << cuT( "\n" );
				}

				result << cuT( "g mesh" ) << count << cuT( "\n" ) << strV.str() << cuT( "\n" ) << strVN.str() << cuT( "\n" ) << strVT.str() << cuT( "\n" ) << strF.str() << cuT( "\n" );
				offset += uint32_t( submesh->getPoints().size() );
				count++;
			}
		}

		return result.str();
	}

	//************************************************************************************************

	namespace
	{
		template< typename ObjType, typename ViewType >
		bool writeView( ViewType const & view
			, String const & elemsName
			, TextFile & file )
		{
			bool result = true;

			if ( !view.isEmpty() )
			{
				result = file.writeText( cuT( "\n// " ) + elemsName + cuT( "\n" ) ) > 0;

				if ( result )
				{
					Logger::logInfo( cuT( "Scene::write - " ) + elemsName );

					for ( auto const & name : view )
					{
						auto elem = view.find( name );
						result &= typename ObjType::TextWriter{ String{} }( *elem, file );
					}
				}
			}

			return result;
		}
	}

	void CscnSceneExporter::ExportScene( Scene const & scene
		, Path const & fileName )
	{
		Path folder = fileName.getPath() / fileName.getFileName();

		if ( !File::directoryExists( folder ) )
		{
			File::directoryCreate( folder );
		}

		if ( !File::directoryExists( folder / cuT( "Materials" ) ) )
		{
			File::directoryCreate( folder / cuT( "Materials" ) );
		}

		Path filePath = folder / ( fileName.getFileName() + cuT( ".cscn" ) );
		filePath = Path{ filePath };
		bool result = false;
		{
			TextFile mtlFile( filePath, File::OpenMode::eWrite, File::EncodingMode::eASCII );
			result = writeView< Sampler >( scene.getSamplerView()
				, cuT( "Samplers" )
				, mtlFile );

			if ( result )
			{
				result = writeView< Material >( scene.getMaterialView()
					, cuT( "Materials" )
					, mtlFile );
			}
		}

		if ( result )
		{
			File::copyFile( filePath, folder / cuT( "Materials" ) );
			File::deleteFile( filePath );
			TextFile scnFile( Path{ filePath }, File::OpenMode::eWrite, File::EncodingMode::eASCII );
			result = Scene::TextWriter( String(), cuT( "Materials" ) / filePath.getFileName( true ) )( scene, scnFile );
		}

		Path subfolder{ cuT( "Meshes" ) };

		if ( result )
		{
			if ( !File::directoryExists( folder / subfolder ) )
			{
				File::directoryCreate( folder / subfolder );
			}

			auto lock = makeUniqueLock( scene.getMeshCache() );

			for ( auto const & it : scene.getMeshCache() )
			{
				auto mesh = it.second;

				if ( result && it.second->isSerialisable() )
				{
					Path base{ folder / subfolder };
					Path path{ base / it.first + cuT( ".cmsh" ) };
					{
						BinaryFile file{ path, File::OpenMode::eWrite };
						result = BinaryWriter< Mesh >{}.write( *mesh, file );
					}

					auto skeleton = mesh->getSkeleton();

					if ( result && skeleton )
					{
						BinaryFile file{ base / ( it.first + cuT( ".cskl" ) ), File::OpenMode::eWrite };
						result = BinaryWriter< Skeleton >{}.write( *skeleton, file );
					}
				}
			}
		}

		if ( result )
		{
			wxMessageBox( _( "Export successful" ) );
		}
		else
		{
			wxMessageBox( _( "Export failed" ) );
		}
	}
}
