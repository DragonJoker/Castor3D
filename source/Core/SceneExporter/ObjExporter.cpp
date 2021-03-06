#include "SceneExporter/ObjExporter.hpp"

#include <Castor3D/Binary/BinaryMesh.hpp>
#include <Castor3D/Binary/BinarySkeleton.hpp>
#include <Castor3D/Cache/CacheView.hpp>
#include <Castor3D/Cache/MaterialCache.hpp>
#include <Castor3D/Cache/MeshCache.hpp>
#include <Castor3D/Cache/PluginCache.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Material/Pass/Pass.hpp>
#include <Castor3D/Material/Pass/PhongPass.hpp>
#include <Castor3D/Material/Pass/MetallicRoughnessPbrPass.hpp>
#include <Castor3D/Material/Pass/SpecularGlossinessPbrPass.hpp>
#include <Castor3D/Material/Texture/TextureUnit.hpp>
#include <Castor3D/Material/Texture/TextureLayout.hpp>
#include <Castor3D/Material/Texture/TextureView.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Model/Mesh/Importer.hpp>
#include <Castor3D/Model/Mesh/ImporterFactory.hpp>
#include <Castor3D/Model/Mesh/Mesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Model/Skeleton/Skeleton.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Text/TextMaterial.hpp>
#include <Castor3D/Text/TextScene.hpp>

#include <CastorUtils/Data/File.hpp>

namespace castor3d::exporter
{
	namespace
	{
		namespace
		{
			castor::Path getTextureNewPath( castor::Path const & pathSrcFile
				, castor::Path const & pathFolder )
			{
				castor::Path pathReturn( cuT( "Texture" ) );
				pathReturn /= pathSrcFile.getFullFileName();

				if ( !castor::File::directoryExists( pathFolder / cuT( "Texture" ) ) )
				{
					castor::File::directoryCreate( pathFolder / cuT( "Texture" ) );
				}

				if ( castor::File::fileExists( pathSrcFile ) )
				{
					castor::Logger::logDebug( cuT( "Copying [" ) + pathSrcFile + cuT( "] to [" ) + pathFolder / pathReturn + cuT( "]" ) );
					castor::File::copyFileName( pathSrcFile, pathFolder / pathReturn );
				}

				return pathReturn;
			}

			castor::Path getMeshNewPath( castor::Path const & path )
			{
				castor::Path pathReturn( path.getPath() / cuT( "Mesh" ) );

				if ( !castor::File::directoryExists( pathReturn ) )
				{
					castor::File::directoryCreate( pathReturn );
				}

				return pathReturn;
			}
		}
	}

	ObjSceneExporter::ObjSceneExporter( ExportOptions options )
		: SceneExporter{ std::move( options ) }
	{
	}

	bool ObjSceneExporter::exportMesh( castor3d::Scene const & scene
		, castor3d::Mesh const & mesh
		, castor::Path const & outputFolder
		, castor::String const & outputName )
	{
		return false;
	}

	bool ObjSceneExporter::exportScene( castor3d::Scene const & scene
		, castor::Path const & fileName )
	{
		castor::Path mtlFilePath( fileName );
		castor::string::replace( mtlFilePath, mtlFilePath.getExtension(), cuT( "mtl" ) );
		bool result = false;

		try
		{
			doExportMaterials( scene, mtlFilePath );
			doExportMeshes( scene, mtlFilePath, fileName );
			result = true;
		}
		catch ( std::exception & exc )
		{
			castor3d::log::error << cuT( "Export failed\n" ) << exc.what() << std::endl;
		}

		return result;
	}

	void ObjSceneExporter::doExportMaterials( Scene const & scene
		, castor::Path const & path )const
	{
		auto const & cache = scene.getMaterialView();
		castor::StringStream streamVersion( castor::makeStringStream() );
		streamVersion << Version{};
		castor::StringStream mtl( castor::makeStringStream() );
		mtl << cuT( "################################################################################\n" );
		mtl << cuT( "#" ) << std::setw( 78 ) << std::left << ( cuT( "  MTL File generated by castor::GuiCommon v" ) + streamVersion.str() ) << cuT( "#\n" );
		mtl << cuT( "################################################################################\n\n" );

		for ( auto const & name : cache )
		{
			auto material = cache.find( name );
			mtl << doExportMaterial( path.getPath(), *material ) + cuT( "\n" );
		}

		castor::TextFile fileMtl( path
			, castor::File::OpenMode::eWrite
			, castor::File::EncodingMode::eASCII );
		fileMtl.writeText( mtl.str() );
	}

	void ObjSceneExporter::doExportMeshes( castor3d::Scene const & scene
		, castor::Path const & mtlpath
		, castor::Path const & path )const
	{
		castor::StringStream obj( castor::makeStringStream() );
		castor::StringStream streamVersion( castor::makeStringStream() );
		streamVersion << Version{};

		castor::Path pathFileObj = getMeshNewPath( path );
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
			castor::TextFile fileObj( pathFileObj / ( it.first + cuT( ".obj" ) )
				, castor::File::OpenMode::eWrite
				, castor::File::EncodingMode::eASCII );
			fileObj.writeText( obj.str() );
		}

		cache.unlock();
	}

	castor::String ObjSceneExporter::doExportMaterial( castor::Path const & pathMtlFolder
		, Material const & material )const
	{
		castor::StringStream strReturn( castor::makeStringStream() );

		if ( material.getType() == MaterialType::ePhong )
		{
			auto pass = material.getTypedPass< MaterialType::ePhong >( 0u );

			if ( pass )
			{
				strReturn << cuT( "newmtl " ) << material.getName() << cuT( "\n" );
				auto diffuse = pass->getDiffuse();
				strReturn << cuT( "	Kd " ) << diffuse.red().value() << cuT( " " ) << diffuse.green().value() << cuT( " " ) << diffuse.blue().value() << cuT( "\n" );
				auto specular = pass->getSpecular();
				strReturn << cuT( "	Ks " ) << specular.red().value() << cuT( " " ) << specular.green().value() << cuT( " " ) << specular.blue().value() << cuT( "\n" );
				strReturn << cuT( "	Ns " ) << pass->getShininess().value() << cuT( "\n" );
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
		castor::StringStream strReturn( castor::makeStringStream() );

		if ( unit && unit->getTexture() )
		{
			castor::Path path{ unit->getTexture()->getDefaultView().toString() };

			if ( !path.empty() )
			{
				strReturn << cuT( "	" ) << section << cuT( " " ) + getTextureNewPath( path, pathMtlFolder ) << cuT( "\n" );
			}
		}

		return strReturn.str();
	}

	castor::String ObjSceneExporter::doExportMesh( Mesh const & mesh
		, uint32_t & offset
		, uint32_t & count )const
	{
		castor::StringStream result( castor::makeStringStream() );

		for ( auto const & submesh : mesh )
		{
			if ( submesh->hasComponent( TriFaceMapping::Name ) )
			{
				castor::StringStream strV( castor::makeStringStream() );
				castor::StringStream strVT( castor::makeStringStream() );
				castor::StringStream strVN( castor::makeStringStream() );
				castor::StringStream strF( castor::makeStringStream() );
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
}
