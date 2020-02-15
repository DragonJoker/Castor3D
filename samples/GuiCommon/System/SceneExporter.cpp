#include "GuiCommon/System/SceneExporter.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/MeshCache.hpp>
#include <Castor3D/Binary/BinaryMesh.hpp>
#include <Castor3D/Binary/BinarySkeleton.hpp>
#include <Castor3D/Cache/CacheView.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Material/Pass/PhongPass.hpp>
#include <Castor3D/Material/Pass/MetallicRoughnessPbrPass.hpp>
#include <Castor3D/Material/Pass/SpecularGlossinessPbrPass.hpp>
#include <Castor3D/Model/Mesh/Mesh.hpp>
#include <Castor3D/Model/Skeleton/Skeleton.hpp>
#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Model/Vertex.hpp>
#include <Castor3D/Render/RenderWindow.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Material/Texture/Sampler.hpp>
#include <Castor3D/Material/Texture/TextureLayout.hpp>
#include <Castor3D/Material/Texture/TextureUnit.hpp>

#include <CastorUtils/Data/BinaryFile.hpp>

#include <ashespp/Buffer/VertexBuffer.hpp>

#include <iomanip>

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	namespace
	{
		Path getTextureNewPath( Path const & pathSrcFile
			, Path const & pathFolder )
		{
			Path pathReturn( cuT( "Texture" ) );
			pathReturn /= pathSrcFile.getFullFileName();

			if ( !wxDirExists( ( pathFolder / cuT( "Texture" ) ).c_str() ) )
			{
				wxMkDir( string::stringCast< char >( pathFolder / cuT( "Texture" ) ).c_str(), 0777 );
			}

			if ( wxFileExists( pathSrcFile ) )
			{
				Logger::logDebug( cuT( "Copying [" ) + pathSrcFile + cuT( "] to [" ) + pathFolder / pathReturn + cuT( "]" ) );
				wxCopyFile( pathSrcFile, pathFolder / pathReturn );
			}

			return pathReturn;
		}

		Path getMeshNewPath( Path const & path )
		{
			Path pathReturn( path.getPath() / cuT( "Mesh" ) );

			if ( !wxDirExists( pathReturn.c_str() ) )
			{
				wxMkDir( string::stringCast< char >( pathReturn ).c_str(), 0777 );
			}

			return pathReturn;
		}
	}

	//************************************************************************************************

	void ObjSceneExporter::exportScene( Scene const & scene
		, Path const & fileName )
	{
		Path mtlFilePath( fileName );
		string::replace( mtlFilePath, mtlFilePath.getExtension(), cuT( "mtl" ) );
		try
		{
			doExportMaterials( scene, mtlFilePath );
			doExportMeshes( scene, mtlFilePath, fileName );
			wxMessageBox( _( "Export successful" ) );
		}
		catch ( std::exception & exc )
		{
			wxMessageBox( wxString{} << _( "Export failed" ) << wxT( "\n" ) << exc.what() );
		}
	}

	void ObjSceneExporter::doExportMaterials( Scene const & scene
		, Path const & path )const
	{
		auto const & cache = scene.getMaterialView();
		StringStream streamVersion( makeStringStream() );
		streamVersion << Version{};
		StringStream mtl( makeStringStream() );
		mtl << cuT( "################################################################################\n" );
		mtl << cuT( "#" ) << std::setw( 78 ) << std::left << ( cuT( "  MTL File generated by castor::GuiCommon v" ) + streamVersion.str() ) << cuT( "#\n" );
		mtl << cuT( "################################################################################\n\n" );

		for ( auto const & name : cache )
		{
			auto material = cache.find( name );
			mtl << doExportMaterial( path.getPath(), *material ) + cuT( "\n" );
		}

		TextFile fileMtl( path, File::OpenMode::eWrite, File::EncodingMode::eASCII );
		fileMtl.writeText( mtl.str() );
	}

	void ObjSceneExporter::doExportMeshes( castor3d::Scene const & scene
		, castor::Path const & mtlpath
		, castor::Path const & path )const
	{
		StringStream obj( makeStringStream() );
		StringStream streamVersion( makeStringStream() );
		streamVersion << Version{};

		Path pathFileObj = getMeshNewPath( path );
		auto const & cache = scene.getMeshCache();
		cache.lock();

		for ( auto const & it : cache )
		{
			auto mesh = it.second;
			obj << cuT( "################################################################################\n" );
			obj << cuT( "#" ) << std::setw( 78 ) << std::left << ( cuT( "  OBJ File generated by castor::GuiCommon v" ) + streamVersion.str() ) << cuT( "#\n" );
			obj << cuT( "################################################################################\n\n" );
			obj << cuT( "mtllib " ) + mtlpath.getFullFileName() + cuT( "\n\n" );

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

	String ObjSceneExporter::doExportMaterial( Path const & pathMtlFolder
		, Material const & material )const
	{
		StringStream strReturn( makeStringStream() );

		if ( material.getType() == MaterialType::ePhong )
		{
			auto pass = material.getTypedPass< MaterialType::ePhong > ( 0u );

			if ( pass )
			{
				strReturn << cuT( "newmtl " ) << material.getName() << cuT( "\n" );
				auto diffuse = pass->getDiffuse();
				strReturn << cuT( "	Kd " ) << diffuse.red().value() << cuT( " " ) << diffuse.green().value() << cuT( " " ) << diffuse.blue().value() << cuT( "\n" );
				auto specular = pass->getSpecular();
				strReturn << cuT( "	Ks " ) << specular.red().value() << cuT( " " ) << specular.green().value() << cuT( " " ) << specular.blue().value() << cuT( "\n" );
				strReturn << cuT( "	Ns " ) << pass->getShininess() << cuT( "\n" );
				strReturn << cuT( "	d " ) << pass->getOpacity() << cuT( "\n" );

				for ( auto & unit : *pass )
				{
					auto & config = unit->getConfiguration();

					if ( config.colourMask[0] )
					{
						strReturn << doExportTexture( pathMtlFolder, cuT( "map_Kd" ), unit );
					}

					if ( config.opacityMask[0] )
					{
						strReturn << doExportTexture( pathMtlFolder, cuT( "map_d" ), unit );
					}

					if ( config.specularMask[0] )
					{
						strReturn << doExportTexture( pathMtlFolder, cuT( "map_Ks" ), unit );
					}

					if ( config.glossinessMask[0] )
					{
						strReturn << doExportTexture( pathMtlFolder, cuT( "map_Ns" ), unit );
					}

					if ( config.normalMask[0] )
					{
						strReturn << doExportTexture( pathMtlFolder, cuT( "map_Bump" ), unit );
					}
				}
			}
		}
		else if ( material.getType() == MaterialType::eMetallicRoughness )
		{
			auto pass = material.getTypedPass< MaterialType::eMetallicRoughness >( 0u );

			if ( pass )
			{
				strReturn << cuT( "newmtl " ) << material.getName() << cuT( "\n" );
				auto albedo = pass->getAlbedo();
				strReturn << cuT( "	Kd " ) << albedo.red().value() << cuT( " " ) << albedo.green().value() << cuT( " " ) << albedo.blue().value() << cuT( "\n" );
				strReturn << cuT( "	Ks " ) << pass->getMetallic() << cuT( " " ) << pass->getMetallic() << cuT( " " ) << pass->getMetallic() << cuT( " 0.0\n" );
				strReturn << cuT( "	Ns " ) << pass->getRoughness() << cuT( "\n" );
				strReturn << cuT( "	d " ) << pass->getOpacity() << cuT( "\n" );

				for ( auto & unit : *pass )
				{
					auto & config = unit->getConfiguration();

					if ( config.colourMask[0] )
					{
						strReturn << doExportTexture( pathMtlFolder, cuT( "map_Kd" ), unit );
					}

					if ( config.opacityMask[0] )
					{
						strReturn << doExportTexture( pathMtlFolder, cuT( "map_d" ), unit );
					}

					if ( config.specularMask[0] )
					{
						strReturn << doExportTexture( pathMtlFolder, cuT( "map_Ks" ), unit );
					}

					if ( config.glossinessMask[0] )
					{
						strReturn << doExportTexture( pathMtlFolder, cuT( "map_Ns" ), unit );
					}

					if ( config.normalMask[0] )
					{
						strReturn << doExportTexture( pathMtlFolder, cuT( "map_Bump" ), unit );
					}
				}
			}
		}
		else if ( material.getType() == MaterialType::eSpecularGlossiness )
		{
			auto pass = material.getTypedPass< MaterialType::eSpecularGlossiness >( 0u );

			if ( pass )
			{
				strReturn << cuT( "newmtl " ) << material.getName() << cuT( "\n" );
				auto diffuse = pass->getDiffuse();
				strReturn << cuT( "	Kd " ) << diffuse.red().value() << cuT( " " ) << diffuse.green().value() << cuT( " " ) << diffuse.blue().value() << cuT( "\n" );
				auto specular = pass->getSpecular();
				strReturn << cuT( "	Ks " ) << specular.red().value() << cuT( " " ) << specular.green().value() << cuT( " " ) << specular.blue().value() << cuT( "\n" );
				strReturn << cuT( "	Ns " ) << pass->getGlossiness() << cuT( "\n" );
				strReturn << cuT( "	d " ) << pass->getOpacity() << cuT( "\n" );

				for ( auto & unit : *pass )
				{
					auto & config = unit->getConfiguration();

					if ( config.colourMask[0] )
					{
						strReturn << doExportTexture( pathMtlFolder, cuT( "map_Kd" ), unit );
					}

					if ( config.opacityMask[0] )
					{
						strReturn << doExportTexture( pathMtlFolder, cuT( "map_d" ), unit );
					}

					if ( config.specularMask[0] )
					{
						strReturn << doExportTexture( pathMtlFolder, cuT( "map_Ks" ), unit );
					}

					if ( config.glossinessMask[0] )
					{
						strReturn << doExportTexture( pathMtlFolder, cuT( "map_Ns" ), unit );
					}

					if ( config.normalMask[0] )
					{
						strReturn << doExportTexture( pathMtlFolder, cuT( "map_Bump" ), unit );
					}
				}
			}
		}

		return strReturn.str();
	}

	castor::String ObjSceneExporter::doExportTexture( castor::Path const & pathMtlFolder
		, castor::String section
		, TextureUnitSPtr unit )const
	{
		StringStream strReturn( makeStringStream() );

		if ( unit && unit->getTexture() )
		{
			Path path{ unit->getTexture()->getImage().toString() };

			if ( !path.empty() )
			{
				strReturn << cuT( "	" ) << section << cuT( " " ) + getTextureNewPath( path, pathMtlFolder ) << cuT( "\n" );
			}
		}

		return strReturn.str();
	}

	String ObjSceneExporter::doExportMesh( Mesh const & mesh
		, uint32_t & offset
		, uint32_t & count )const
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
				castor::Point3f ptPos;
				castor::Point3f ptNml;
				castor::Point3f ptTex;

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

	void CscnSceneExporter::exportScene( Scene const & scene
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

		Path filePath{ folder / ( fileName.getFileName() + cuT( ".cscn" ) ) };
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

			using LockType = std::unique_lock< MeshCache const >;
			LockType lock{ castor::makeUniqueLock( scene.getMeshCache() ) };

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
