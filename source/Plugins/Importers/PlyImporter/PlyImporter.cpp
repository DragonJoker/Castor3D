#include "PlyImporter/PlyImporter.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Event/Frame/InitialiseEvent.hpp>
#include <Castor3D/Cache/CacheView.hpp>
#include <Castor3D/Cache/CameraCache.hpp>
#include <Castor3D/Cache/GeometryCache.hpp>
#include <Castor3D/Cache/MaterialCache.hpp>
#include <Castor3D/Cache/MeshCache.hpp>
#include <Castor3D/Cache/SceneNodeCache.hpp>
#include <Castor3D/Cache/SceneCache.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Material/Pass/Pass.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/Face.hpp>
#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Model/Vertex.hpp>
#include <Castor3D/Plugin/Plugin.hpp>
#include <Castor3D/Miscellaneous/Version.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/Viewport.hpp>
#include <Castor3D/Scene/Geometry.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Material/Texture/TextureUnit.hpp>

using namespace castor3d;
using namespace castor;

namespace C3dPly
{
	PlyImporter::PlyImporter( Engine & engine )
		: MeshImporter( engine )
	{
	}

	MeshImporterUPtr PlyImporter::create( Engine & engine )
	{
		return std::make_unique< PlyImporter >( engine );
	}

	bool PlyImporter::doImportMesh( Mesh & p_mesh )
	{
		bool result{ false };
		UInt32Array faces;
		FloatArray sizes;
		String name = m_fileName.getFileName();
		String meshName = name.substr( 0, name.find_last_of( '.' ) );
		String materialName = meshName;
		std::ifstream isFile;
		isFile.open( string::stringCast< char >( m_fileName ).c_str(), std::ios::in );
		std::string strLine;
		std::istringstream ssToken;
		String::size_type stIndex;
		int iNbProperties = 0;
		SubmeshSPtr submesh = p_mesh.createSubmesh();
		MaterialSPtr pMaterial = p_mesh.getScene()->getMaterialView().find( materialName );

		if ( !pMaterial )
		{
			pMaterial = p_mesh.getScene()->getMaterialView().add( materialName, p_mesh.getScene()->getPassesType() );
			pMaterial->createPass();
		}

		pMaterial->getPass( 0 )->setTwoSided( true );
		submesh->setDefaultMaterial( pMaterial );
		auto mapping = std::make_shared< TriFaceMapping >( *submesh );
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
						log::info << cuT( "Vertices: " ) << iNbVertex << std::endl;
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
						log::info << cuT( "Vertex properties: " ) << iNbProperties << std::endl;
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
						log::info << cuT( "Triangles: " ) << iNbFaces << std::endl;
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
						ssToken >> vertex.pos[0] >> vertex.pos[1] >> vertex.pos[2];
						ssToken >> vertex.nml[0] >> vertex.nml[1] >> vertex.nml[2];
						ssToken >> vertex.tex[0] >> vertex.tex[1];
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
						ssToken >> vertex.pos[0] >> vertex.pos[1] >> vertex.pos[2];
						ssToken >> vertex.nml[0] >> vertex.nml[1] >> vertex.nml[2];
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
						ssToken >> vertex.pos[0] >> vertex.pos[1] >> vertex.pos[2];
						ssToken.clear( std::istringstream::goodbit );
					}
				}

				submesh->addPoints( vertices );
				// Parsing triangles
				FaceSPtr pFace;
				std::vector< FaceIndices > faces( iNbFaces );
				FaceIndices * face = &faces[0];

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

				mapping->addFaceGroup( faces.data(), face );
			}

			result = true;
		}

		submesh->computeContainers();

		if ( iNbProperties < 6 )
		{
			mapping->computeNormals( false );
		}
		else
		{
			mapping->computeTangentsFromNormals();
		}

		submesh->setIndexMapping( mapping );
		isFile.close();
		return result;
	}
}
