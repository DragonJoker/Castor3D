#include "Md2Importer.hpp"

#include <Graphics/Colour.hpp>
#include <Graphics/Image.hpp>

#include <Engine.hpp>
#include <Material/Material.hpp>
#include <Material/LegacyPass.hpp>
#include <Mesh/Mesh.hpp>
#include <Scene/Geometry.hpp>
#include <Scene/Scene.hpp>
#include <Scene/SceneNode.hpp>

#include <Event/Frame/FrameListener.hpp>
#include <Event/Frame/InitialiseEvent.hpp>
#include <Cache/CacheView.hpp>
#include <Material/Pass.hpp>
#include <Mesh/Face.hpp>
#include <Mesh/Submesh.hpp>
#include <Mesh/Vertex.hpp>
#include <Mesh/Buffer/Buffer.hpp>
#include <Miscellaneous/Version.hpp>
#include <Plugin/Plugin.hpp>
#include <Render/RenderSystem.hpp>
#include <Texture/TextureLayout.hpp>
#include <Texture/TextureUnit.hpp>

using namespace castor3d;
using namespace castor;

//*************************************************************************************************

namespace C3DMd2
{
	Md2Importer::Md2Importer( Engine & p_engine, String const & p_textureName )
		: Importer( p_engine )
		, m_textureName( p_textureName )
		, m_skins( nullptr )
		, m_texCoords( nullptr )
		, m_triangles( nullptr )
		, m_frames( nullptr )
		, m_pFile( nullptr )
	{
		memset( &m_header, 0, sizeof( Md2Header ) );
	}

	ImporterUPtr Md2Importer::create( Engine & p_engine )
	{
		return std::make_unique< Md2Importer >( p_engine );
	}

	bool Md2Importer::doImportScene( Scene & p_scene )
	{
		auto mesh = p_scene.getMeshCache().add( cuT( "Mesh_MD2" ) );
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

	bool Md2Importer::doImportMesh( Mesh & p_mesh )
	{
		bool result{ false };
		m_pFile = new BinaryFile( m_fileName, File::OpenMode::eRead );
		String materialName = m_fileName.getFileName();
		m_pFile->read( m_header );

		if ( m_header.m_version != 8 )
		{
			Logger::logInfo( cuT( "Invalid file format (Version not 8): " ) + m_fileName );
		}
		else
		{
			auto material = p_mesh.getScene()->getMaterialView().find( materialName );

			if ( !material )
			{
				material = p_mesh.getScene()->getMaterialView().add( materialName, MaterialType::eLegacy );
				material->createPass();
			}

			REQUIRE( material->getType() == MaterialType::eLegacy );
			auto pass = material->getTypedPass< MaterialType::eLegacy >( 0 );
			pass->setShininess( 64.0f );
			doReadMD2Data( *pass );
			doConvertDataStructures( p_mesh );

			for ( auto submesh : p_mesh )
			{
				submesh->setDefaultMaterial( material );
			}

			p_mesh.computeNormals();
			p_mesh.getScene()->getListener().postEvent( makeInitialiseEvent( *material ) );
			result = true;
		}

		doCleanUp();
		return result;
	}

	void Md2Importer::doReadMD2Data( Pass & p_pass )
	{
		uint64_t uiRead;
		Logger::logDebug( StringStream() << cuT( "MD2 File - size: " ) << m_pFile->getLength() );
		Logger::logDebug( StringStream() << cuT( "- Vertices: " ) << m_header.m_numVertices );
		Logger::logDebug( StringStream() << cuT( "- Skins: " ) << m_header.m_numSkins );
		m_skins = new Md2Skin[m_header.m_numSkins];
		m_pFile->seek( m_header.m_offsetSkins );
		uiRead = m_pFile->readArray( m_skins, m_header.m_numSkins );

		if ( m_header.m_numSkins > 0 )
		{
			TextureUnitSPtr pTexture;
			Logger::logDebug( StringStream() << cuT( "- Skins: " ) << ( uiRead / sizeof( Md2Skin ) ) << cuT( "/" ) << m_header.m_numSkins << cuT( " (" ) << uiRead << cuT( " bytes, from " ) << m_header.m_offsetSkins << cuT( ")" ) );

			for ( int i = 0; i < m_header.m_numSkins && !pTexture; i++ )
			{
				Path strValue{ string::stringCast< xchar >( m_skins[i] ) };

				if ( !strValue.empty() )
				{
					pTexture = loadTexture( strValue, p_pass, TextureChannel::eDiffuse );

					if ( pTexture )
					{
						Logger::logDebug( cuT( "- Texture found : " ) + strValue );
					}
					else
					{
						Logger::logDebug( cuT( "- Texture not found : " ) + strValue );
					}
				}
			}
		}

		if ( m_header.m_numTexCoords > 0 )
		{
			m_texCoords = new Md2TexCoord[m_header.m_numTexCoords];
			m_pFile->seek( m_header.m_offsetTexCoords );
			uiRead = m_pFile->readArray( m_texCoords, m_header.m_numTexCoords );
			Logger::logDebug( StringStream() << cuT( "- TexCoords: " ) << ( uiRead / sizeof( Md2TexCoord ) ) << cuT( "/" ) << m_header.m_numTexCoords << cuT( " (" ) << uiRead << cuT( " bytes, from " ) << m_header.m_offsetTexCoords << cuT( ")" ) );
		}

		if ( m_header.m_numTriangles > 0 )
		{
			m_triangles = new Md2Face[m_header.m_numTriangles];
			m_pFile->seek( m_header.m_offsetTriangles );
			uiRead = m_pFile->readArray( m_triangles, m_header.m_numTriangles );
			Logger::logDebug( StringStream() << cuT( "- Triangles: " ) << ( uiRead / sizeof( Md2Face ) ) << cuT( "/" ) << m_header.m_numTriangles << cuT( " (" ) << uiRead << cuT( " bytes, from " ) << m_header.m_offsetTriangles << cuT( ")" ) );
		}

		if ( m_header.m_numFrames > 0 )
		{
			uint8_t buffer[MD2_MAX_FRAMESIZE];
			Logger::logDebug( StringStream() << cuT( "- Frames: " ) << m_header.m_numFrames );
			m_pFile->seek( m_header.m_offsetFrames );
			m_frames = new Md2Frame[m_header.m_numFrames];
			Md2AliasFrame * frame = reinterpret_cast< Md2AliasFrame * >( buffer );
			m_frames[0].m_vertices = new Md2Vertex[m_header.m_numVertices];
			m_pFile->readArray< char >( ( char * )frame, m_header.m_frameSize );
			m_frames[0].m_strName = frame->m_name;
			Md2Vertex * vertices = m_frames[0].m_vertices;

			for ( int j = 0; j < m_header.m_numVertices; j++ )
			{
				vertices[j].m_coords[0] = ( frame->m_aliasVertices[j].m_vertex[0] * frame->m_scale[0] + frame->m_translate[0] ) * 1;
				vertices[j].m_coords[2] = ( frame->m_aliasVertices[j].m_vertex[1] * frame->m_scale[1] + frame->m_translate[1] ) * -1;
				vertices[j].m_coords[1] = ( frame->m_aliasVertices[j].m_vertex[2] * frame->m_scale[2] + frame->m_translate[2] ) * 1;
			}
		}
	}

	void Md2Importer::doConvertDataStructures( Mesh & p_mesh )
	{
		SubmeshSPtr submesh = p_mesh.createSubmesh();
		std::vector< float > arrayNml( m_header.m_numTriangles * 9 );
		std::vector< FaceIndices > arrayFaces( m_header.m_numTriangles );
		BufferElementGroupSPtr pVertex;

		if ( m_header.m_numFrames > 0 )
		{
			//for (int i = 0 ; i < m_header.m_numVertices ; i++)
			//{
			//	pVertex = submesh->addPoint( m_frames[0].m_vertices[i].m_coords );
			//}

			//for (int i = 0 ; i < m_header.m_numTriangles ; i++)
			//{
			//	if (m_triangles[i].m_vertexIndices[0] >= 0 && m_triangles[i].m_vertexIndices[1] >= 0 && m_triangles[i].m_vertexIndices[2] >= 0)
			//	{
			//		submesh->getPoint( m_triangles[i].m_vertexIndices[0] )->setTexCoord( real( m_texCoords[m_triangles[i].m_textureIndices[0]].u ) / real( m_header.m_skinWidth ), real( 1.0 ) - real( m_texCoords[m_triangles[i].m_textureIndices[0]].v ) / real( m_header.m_skinHeight ) );
			//		submesh->getPoint( m_triangles[i].m_vertexIndices[1] )->setTexCoord( real( m_texCoords[m_triangles[i].m_textureIndices[1]].u ) / real( m_header.m_skinWidth ), real( 1.0 ) - real( m_texCoords[m_triangles[i].m_textureIndices[1]].v ) / real( m_header.m_skinHeight ) );
			//		submesh->getPoint( m_triangles[i].m_vertexIndices[2] )->setTexCoord( real( m_texCoords[m_triangles[i].m_textureIndices[2]].u ) / real( m_header.m_skinWidth ), real( 1.0 ) - real( m_texCoords[m_triangles[i].m_textureIndices[2]].v ) / real( m_header.m_skinHeight ) );
			//		submesh->addFace( m_triangles[i].m_vertexIndices[0], m_triangles[i].m_vertexIndices[1], m_triangles[i].m_vertexIndices[2] );
			//	}
			//}

			for ( int i = 0; i < m_header.m_numTriangles; i++ )
			{
				if ( m_triangles[i].m_vertexIndices[0] >= 0 && m_triangles[i].m_vertexIndices[1] >= 0 && m_triangles[i].m_vertexIndices[2] >= 0 )
				{
					Vertex::setTexCoord( submesh->addPoint( m_frames[0].m_vertices[m_triangles[i].m_vertexIndices[0]].m_coords ), real( m_texCoords[m_triangles[i].m_textureIndices[0]].u ) / real( m_header.m_skinWidth ), real( 1.0 ) - real( m_texCoords[m_triangles[i].m_textureIndices[0]].v ) / real( m_header.m_skinHeight ) );
					Vertex::setTexCoord( submesh->addPoint( m_frames[0].m_vertices[m_triangles[i].m_vertexIndices[1]].m_coords ), real( m_texCoords[m_triangles[i].m_textureIndices[1]].u ) / real( m_header.m_skinWidth ), real( 1.0 ) - real( m_texCoords[m_triangles[i].m_textureIndices[1]].v ) / real( m_header.m_skinHeight ) );
					Vertex::setTexCoord( submesh->addPoint( m_frames[0].m_vertices[m_triangles[i].m_vertexIndices[2]].m_coords ), real( m_texCoords[m_triangles[i].m_textureIndices[2]].u ) / real( m_header.m_skinWidth ), real( 1.0 ) - real( m_texCoords[m_triangles[i].m_textureIndices[2]].v ) / real( m_header.m_skinHeight ) );
					submesh->addFace( i * 3 + 0, i * 3 + 1, i * 3 + 2 );
				}
			}

			delete[] m_frames[0].m_vertices;

			//for (int i = 0 ; i < m_header.m_numTriangles ; i++)
			//{
			//	if (m_triangles[i].m_vertexIndices[0] >= 0 && m_triangles[i].m_vertexIndices[1] >= 0 && m_triangles[i].m_vertexIndices[2] >= 0)
			//	{
			//		arrayFaces[i].m_uiVertexIndex[0] = m_triangles[i].m_vertexIndices[0];
			//		arrayFaces[i].m_uiVertexIndex[1] = m_triangles[i].m_vertexIndices[1];
			//		arrayFaces[i].m_uiVertexIndex[2] = m_triangles[i].m_vertexIndices[2];
			//		arrayTex.push_back( float( m_texCoords[m_triangles[i].m_textureIndices[0]].u ) / float( m_header.m_skinWidth ) );
			//		arrayTex.push_back( 1-float( m_texCoords[m_triangles[i].m_textureIndices[0]].v ) / float( m_header.m_skinHeight ) );
			//		arrayTex.push_back( float( m_texCoords[m_triangles[i].m_textureIndices[1]].u ) / float( m_header.m_skinWidth ) );
			//		arrayTex.push_back( 1-float( m_texCoords[m_triangles[i].m_textureIndices[1]].v ) / float( m_header.m_skinHeight ) );
			//		arrayTex.push_back( float( m_texCoords[m_triangles[i].m_textureIndices[2]].u ) / float( m_header.m_skinWidth ) );
			//		arrayTex.push_back( 1-float( m_texCoords[m_triangles[i].m_textureIndices[2]].v ) / float( m_header.m_skinHeight ) );
			//	}
			//}

			//l_submesh->addFaceGroup( &arrayFaces[0], m_header.m_numTriangles, NULL, &arrayTex[0] );
		}
	}

	void Md2Importer::doCleanUp()
	{
		delete m_pFile;
		delete[] m_skins;
		delete m_texCoords;
		delete m_triangles;
		delete m_frames;
		m_pFile = nullptr;
		m_skins = nullptr;
		m_texCoords = nullptr;
		m_triangles = nullptr;
		m_frames = nullptr;
	}
}
