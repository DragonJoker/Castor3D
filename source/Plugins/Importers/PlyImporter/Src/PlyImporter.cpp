#include "PlyImporter.hpp"

#include <Engine.hpp>

#include <Event/Frame/InitialiseEvent.hpp>
#include <Cache/CacheView.hpp>
#include <Cache/CameraCache.hpp>
#include <Cache/GeometryCache.hpp>
#include <Cache/MaterialCache.hpp>
#include <Cache/MeshCache.hpp>
#include <Cache/SceneNodeCache.hpp>
#include <Cache/SceneCache.hpp>
#include <Material/Material.hpp>
#include <Material/Pass.hpp>
#include <Mesh/Face.hpp>
#include <Mesh/Submesh.hpp>
#include <Mesh/Vertex.hpp>
#include <Plugin/Plugin.hpp>
#include <Miscellaneous/Version.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/Viewport.hpp>
#include <Scene/Geometry.hpp>
#include <Scene/Scene.hpp>
#include <Texture/TextureUnit.hpp>

using namespace castor3d;
using namespace castor;

namespace C3dPly
{
	PlyImporter::PlyImporter( Engine & engine )
		: Importer( engine )
	{
	}

	ImporterUPtr PlyImporter::create( Engine & engine )
	{
		return std::make_unique< PlyImporter >( engine );
	}

	bool PlyImporter::doImportScene( Scene & p_scene )
	{
		auto mesh = p_scene.getMeshCache().add( cuT( "Mesh_PLY" ) );
		bool result = doImportMesh( *mesh );

		if ( result )
		{
			SceneNodeSPtr node = p_scene.getSceneNodeCache().add( mesh->getName(), p_scene.getObjectRootNode() );
			GeometrySPtr geometry = p_scene.getGeometryCache().add( mesh->getName(), node, nullptr );
			geometry->setMesh( mesh );
			m_geometries.insert( { geometry->getName(), geometry } );
		}

		return result;
	}

	bool PlyImporter::doImportMesh( Mesh & p_mesh )
	{
		bool result{ false };
		UIntArray faces;
		RealArray sizes;
		String name = m_fileName.getFileName();
		String meshName = name.substr( 0, name.find_last_of( '.' ) );
		String materialName = meshName;
		std::ifstream isFile;
		isFile.open( string::stringCast< char >( m_fileName ).c_str(), std::ios::in );
		std::string strLine;
		std::istringstream ssToken;
		String::size_type stIndex;
		int iNbProperties = 0;
		VertexSPtr pVertex;
		Coords3r ptNml;
		Coords2r ptTex;
		SubmeshSPtr submesh = p_mesh.createSubmesh();
		MaterialSPtr pMaterial = p_mesh.getScene()->getMaterialView().find( materialName );

		if ( !pMaterial )
		{
			pMaterial = p_mesh.getScene()->getMaterialView().add( materialName, MaterialType::eLegacy );
			pMaterial->createPass();
		}

		pMaterial->getPass( 0 )->setTwoSided( true );
		submesh->setDefaultMaterial( pMaterial );
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

					if ( ( stIndex = strLine.find( "element vertex " ) ) == std::string::npos )
					{
						continue;
					}
					else
					{
						ssToken.str( strLine.substr( std::string( "element vertex " ).length() ) );
						ssToken >> iNbVertex;
						ssToken.clear( std::istringstream::goodbit );
						Logger::logInfo( StringStream() << cuT( "Vertices: " ) << iNbVertex );
						break;
					}
				}

				// Parsing number of vertex properties
				while ( isFile.good() )
				{
					std::getline( isFile, strLine );

					if ( ( stIndex = strLine.find( "property " ) ) != std::string::npos )
					{
						iNbProperties++;
					}
					else
					{
						isFile.seekg( -isFile.gcount() ); // Unget last line
						Logger::logDebug( StringStream() << cuT( "Vertex properties: " ) << iNbProperties );
						break;
					}
				}

				// Parsing number of triangles
				while ( isFile.good() )
				{
					std::getline( isFile, strLine );

					if ( ( stIndex = strLine.find( "element face " ) ) == std::string::npos )
					{
						continue;
					}
					else
					{
						ssToken.str( strLine.substr( std::string( "element face " ).size() ) );
						ssToken >> iNbFaces;
						ssToken.clear( std::istringstream::goodbit );
						Logger::logInfo( StringStream() << cuT( "Triangles: " ) << iNbFaces );
						break;
					}
				}

				// Parsing end of the header
				while ( isFile.good() )
				{
					std::getline( isFile, strLine );

					if ( ( stIndex = strLine.find( "end_header" ) ) == std::string::npos )
					{
						continue;
					}
					else
					{
						break;
					}
				}

				std::vector< InterleavedVertex > vertices{ size_t( iNbVertex ) };

				if ( iNbProperties >= 8 )
				{
					// Parsing vertices : position + normal + texture
					for ( auto & vertex : vertices )
					{
						std::getline( isFile, strLine );
						ssToken.str( strLine );
						ssToken >> vertex.m_pos[0] >> vertex.m_pos[1] >> vertex.m_pos[2];
						ssToken >> vertex.m_nml[0] >> vertex.m_nml[1] >> vertex.m_nml[2];
						ssToken >> vertex.m_tex[0] >> vertex.m_tex[1];
						ssToken.clear( std::istringstream::goodbit );
					}
				}
				else if ( iNbProperties >= 6 )
				{
					// Parsing vertices : position + normal
					for ( auto & vertex : vertices )
					{
						std::getline( isFile, strLine );
						ssToken.str( strLine );
						ssToken >> vertex.m_pos[0] >> vertex.m_pos[1] >> vertex.m_pos[2];
						ssToken >> vertex.m_nml[0] >> vertex.m_nml[1] >> vertex.m_nml[2];
						ssToken.clear( std::istringstream::goodbit );
					}
				}
				else
				{
					// Parsing vertices : position
					for ( auto & vertex : vertices )
					{
						std::getline( isFile, strLine );
						ssToken.str( strLine );
						ssToken >> vertex.m_pos[0] >> vertex.m_pos[1] >> vertex.m_pos[2];
						ssToken.clear( std::istringstream::goodbit );
					}
				}

				submesh->addPoints( vertices );
				// Parsing triangles
				FaceSPtr pFace;
				std::vector< FaceIndices > faces( iNbFaces );
				FaceIndices * pFaces = &faces[0];

				for ( int i = 0; i < iNbFaces; i++ )
				{
					std::getline( isFile, strLine );
					ssToken.str( strLine );
					ssToken >> iNbVertex;

					if ( iNbVertex >= 3 )
					{
						ssToken >> pFaces->m_index[0] >> pFaces->m_index[1] >> pFaces->m_index[2];
						pFaces++;
					}

					ssToken.clear( std::istringstream::goodbit );
				}

				submesh->addFaceGroup( faces );
			}

			result = true;
		}

		submesh->computeContainers();

		if ( iNbProperties < 6 )
		{
			submesh->computeNormals( false );
		}
		else
		{
			submesh->computeTangentsFromNormals();
		}

		isFile.close();
		return result;
	}
}
