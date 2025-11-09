#include "PlyImporter/PlyImporter.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/GeometryCache.hpp>
#include <Castor3D/Cache/MaterialCache.hpp>
#include <Castor3D/Cache/ObjectCache.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Material/Pass/Pass.hpp>
#include <Castor3D/Material/Pass/Component/Base/TwoSidedComponent.hpp>
#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/Face.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/BaseDataComponent.hpp>
#include <Castor3D/Model/Vertex.hpp>
#include <Castor3D/Plugin/Plugin.hpp>
#include <Castor3D/Miscellaneous/Version.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/Viewport.hpp>
#include <Castor3D/Scene/Geometry.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Material/Texture/TextureUnit.hpp>

#include <CastorUtils/Design/ResourceCache.hpp>

namespace c3d_ply
{
	//*********************************************************************************************

	c3d::MbString const PlyImporterFile::Name = "PLY Importer";

	PlyImporterFile::PlyImporterFile( c3d::Engine & engine
		, c3d::Scene * scene
		, c3d::Path const & path
		, c3d::Parameters const & parameters
		, c3d::ProgressBar * progress )
		: c3d::ImporterFile{ engine, scene, path, parameters, progress }
	{
	}

	c3d::ImporterFileUPtr PlyImporterFile::create( c3d::Engine & engine
		, c3d::Scene * scene
		, c3d::Path const & path
		, c3d::Parameters const & parameters
		, c3d::ProgressBar * progress )
	{
		return c3d::makeUniqueDerived< c3d::ImporterFile, PlyImporterFile >( engine, scene, path, parameters, progress );
	}

	c3d::Vector< c3d::String > PlyImporterFile::listMaterials()
	{
		return {};
	}

	c3d::Vector< c3d::ImporterFile::MeshData > PlyImporterFile::listMeshes()
	{
		c3d::Vector< c3d::ImporterFile::MeshData > result;

		if ( getExtension() == "ply" )
		{
			c3d::String name = getName();
			c3d::String meshName = name.substr( 0, name.find_last_of( '.' ) );
			result.emplace_back( getInternalName( meshName ), c3d::String{} );
		}

		return result;
	}

	c3d::Vector< c3d::String > PlyImporterFile::listSkeletons()
	{
		return {};
	}

	c3d::Vector< c3d::ImporterFile::NodeData > PlyImporterFile::listSceneNodes()
	{
		return {};
	}

	c3d::Vector< c3d::ImporterFile::LightData > PlyImporterFile::listLights()
	{
		return {};
	}

	c3d::Vector< PlyImporterFile::LightGroupData > PlyImporterFile::listLightGroups()
	{
		return {};
	}

	c3d::Vector< PlyImporterFile::GeometryData > PlyImporterFile::listGeometries()
	{
		return {};
	}

	c3d::Vector< PlyImporterFile::CameraData > PlyImporterFile::listCameras()
	{
		return {};
	}

	c3d::Vector< c3d::String > PlyImporterFile::listMeshAnimations( c3d::Mesh const & )
	{
		return {};
	}

	c3d::Vector< c3d::String > PlyImporterFile::listSkeletonAnimations( c3d::Skeleton const & )
	{
		return {};
	}

	c3d::Vector< c3d::String > PlyImporterFile::listSceneNodeAnimations( c3d::SceneNode const & )
	{
		return {};
	}

	c3d::Vector< uint32_t > PlyImporterFile::listTextureAnimations( c3d::Material const &, uint32_t )
	{
		return {};
	}

	uint32_t PlyImporterFile::countAllMeshAnimations()const
	{
		return 0;
	}

	uint32_t PlyImporterFile::countAllSkeletonAnimations()const
	{
		return 0;
	}

	uint32_t PlyImporterFile::countAllSceneNodeAnimations()const
	{
		return 0;
	}

	uint32_t PlyImporterFile::countAllTextureAnimations()const
	{
		return 0;
	}

	c3d::MaterialImporterUPtr PlyImporterFile::createMaterialImporter()
	{
		return nullptr;
	}

	c3d::AnimationImporterUPtr PlyImporterFile::createAnimationImporter()
	{
		return nullptr;
	}

	c3d::SkeletonImporterUPtr PlyImporterFile::createSkeletonImporter()
	{
		return nullptr;
	}

	c3d::MeshImporterUPtr PlyImporterFile::createMeshImporter()
	{
		return c3d::makeUniqueDerived< c3d::MeshImporter, PlyMeshImporter >( *getOwner() );
	}

	c3d::SceneNodeImporterUPtr PlyImporterFile::createSceneNodeImporter()
	{
		return nullptr;
	}

	c3d::LightImporterUPtr PlyImporterFile::createLightImporter()
	{
		return nullptr;
	}

	c3d::CameraImporterUPtr PlyImporterFile::createCameraImporter()
	{
		return nullptr;
	}

	//*********************************************************************************************

	PlyMeshImporter::PlyMeshImporter( c3d::Engine & engine )
		: c3d::MeshImporter{ engine, cuT( "Ply" ) }
	{
	}

	bool PlyMeshImporter::doImportMesh( c3d::Mesh & mesh, uint32_t submeshIndex )
	{
		bool result{ false };
		c3d::UInt32Array faces;
		c3d::FloatArray sizes;
		std::ifstream isFile;
		isFile.open( c3d::toUtf8( m_file->getFileName() ).c_str(), std::ios::in );
		std::string strLine;
		std::istringstream ssToken;
		int iNbProperties = 0;

		auto const & materialName = mesh.getName();
		auto & scene = *mesh.getScene();
		auto material = scene.tryFindMaterial( materialName );
		if ( !material )
		{
			auto mat = getOwner()->createMaterial( materialName
				, *getOwner()
				, getOwner()->getDefaultLightingModel() );
			material = scene.addMaterial( materialName, mat, true );
			material->createPass();
		}

		material->getPass( 0 )->createComponent< c3d::TwoSidedComponent >()->setTwoSided( true );

		auto submesh = mesh.createSubmesh();
		submesh->setDefaultMaterial( material );
		auto mapping = submesh->createComponent< c3d::TriFaceMapping >();
		// Parsing the ply identification line
		std::getline( isFile, strLine );

		if ( strLine == "ply" )
		{
			// Parsing the format specification line
			std::getline( isFile, strLine );

			if ( strLine == "format ascii 1.0" )
			{
				int iNbVertex = 0;
				int iNbFaces = 0;

				// Parsing number of vertices
				while ( isFile.good() )
				{
					std::getline( isFile, strLine );

					if ( strLine.find( "element vertex " ) == std::string::npos )
					{
						continue;
					}
					else
					{
						ssToken.str( strLine.substr( std::string( "element vertex " ).length() ) );
						ssToken >> iNbVertex;
						ssToken.clear( std::istringstream::goodbit );
						c3d::log::info << cuT( "Vertices: " ) << iNbVertex << std::endl;
						break;
					}
				}

				// Parsing number of vertex properties
				while ( isFile.good() )
				{
					std::getline( isFile, strLine );

					if ( strLine.find( "property " ) != std::string::npos )
					{
						iNbProperties++;
					}
					else
					{
						isFile.seekg( -isFile.gcount() ); // Unget last line
						c3d::log::info << cuT( "Vertex properties: " ) << iNbProperties << std::endl;
						break;
					}
				}

				// Parsing number of triangles
				while ( isFile.good() )
				{
					std::getline( isFile, strLine );

					if ( strLine.find( "element face " ) == std::string::npos )
					{
						continue;
					}
					else
					{
						ssToken.str( strLine.substr( std::string( "element face " ).size() ) );
						ssToken >> iNbFaces;
						ssToken.clear( std::istringstream::goodbit );
						c3d::log::info << cuT( "Triangles: " ) << iNbFaces << std::endl;
						break;
					}
				}

				// Parsing end of the header
				while ( isFile.good() )
				{
					std::getline( isFile, strLine );

					if ( strLine.find( "end_header" ) == std::string::npos )
						continue;
					else
						break;
				}

				auto & positions = submesh->createComponent< c3d::PositionsComponent >()->getData().getData();
				auto & normals = submesh->createComponent< c3d::NormalsComponent >()->getData().getData();
				positions.resize( size_t( iNbVertex ) );
				normals.resize( size_t( iNbVertex ) );

				if ( iNbProperties >= 8 )
				{
					auto & texcoords = submesh->createComponent< c3d::Texcoords0Component >()->getData().getData();
					auto & tangents = submesh->createComponent< c3d::TangentsComponent >()->getData().getData();
					texcoords.resize( size_t( iNbVertex ) );
					tangents.resize( size_t( iNbVertex ) );
					// Parsing vertices : position + normal + texture
					for ( size_t i = 0u; i < size_t( iNbVertex ); ++i )
					{
						std::getline( isFile, strLine );
						ssToken.str( strLine );
						ssToken >> positions[i][0] >> positions[i][1] >> positions[i][2];
						ssToken >> normals[i][0] >> normals[i][1] >> normals[i][2];
						ssToken >> texcoords[i][0] >> texcoords[i][1];
						ssToken.clear( std::istringstream::goodbit );
					}
				}
				else if ( iNbProperties >= 6 )
				{
					// Parsing vertices : position + normal
					for ( size_t i = 0u; i < size_t( iNbVertex ); ++i )
					{
						std::getline( isFile, strLine );
						ssToken.str( strLine );
						ssToken >> positions[i][0] >> positions[i][1] >> positions[i][2];
						ssToken >> normals[i][0] >> normals[i][1] >> normals[i][2];
						ssToken.clear( std::istringstream::goodbit );
					}
				}
				else
				{
					// Parsing vertices : position
					for ( size_t i = 0u; i < size_t( iNbVertex ); ++i )
					{
						std::getline( isFile, strLine );
						ssToken.str( strLine );
						ssToken >> positions[i][0] >> positions[i][1] >> positions[i][2];
						ssToken.clear( std::istringstream::goodbit );
					}
				}

				// Parsing triangles
				c3d::Vector< c3d::FaceIndices > facesIndices;
				facesIndices.resize( size_t( iNbFaces ) );
				c3d::FaceIndices * face = &facesIndices[0];

				for ( int i = 0; i < iNbFaces; i++ )
				{
					std::getline( isFile, strLine );
					ssToken.str( strLine );
					ssToken >> iNbVertex;

					if ( iNbVertex >= 3 )
					{
						ssToken >> face->m_index[0] >> face->m_index[2] >> face->m_index[1];
						++face;
					}

					ssToken.clear( std::istringstream::goodbit );
				}

				mapping->getData().addFaceGroup( facesIndices.data(), face );
			}

			result = true;
		}

		if ( iNbProperties < 6 )
		{
			mapping->computeNormals( false );
		}
		else if (iNbProperties >= 8 )
		{
			mapping->computeTangents();
		}

		isFile.close();
		return result;
	}

	//*********************************************************************************************
}
