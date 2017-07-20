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

using namespace Castor3D;
using namespace Castor;

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

	ImporterUPtr Md2Importer::Create( Engine & p_engine )
	{
		return std::make_unique< Md2Importer >( p_engine );
	}

	bool Md2Importer::DoImportScene( Scene & p_scene )
	{
		auto mesh = p_scene.GetMeshCache().Add( cuT( "Mesh_MD2" ) );
		bool result = DoImportMesh( *mesh );

		if ( result )
		{
			SceneNodeSPtr node = p_scene.GetSceneNodeCache().Add( mesh->GetName(), p_scene.GetObjectRootNode() );
			GeometrySPtr geometry = p_scene.GetGeometryCache().Add( mesh->GetName(), node, nullptr );
			geometry->SetMesh( mesh );
			m_geometries.insert( { geometry->GetName(), geometry } );
		}

		return result;
	}

	bool Md2Importer::DoImportMesh( Mesh & p_mesh )
	{
		bool result{ false };
		m_pFile = new BinaryFile( m_fileName, File::OpenMode::eRead );
		String materialName = m_fileName.GetFileName();
		m_pFile->Read( m_header );

		if ( m_header.m_version != 8 )
		{
			Logger::LogInfo( cuT( "Invalid file format (Version not 8): " ) + m_fileName );
		}
		else
		{
			auto material = p_mesh.GetScene()->GetMaterialView().Find( materialName );

			if ( !material )
			{
				material = p_mesh.GetScene()->GetMaterialView().Add( materialName, MaterialType::eLegacy );
				material->CreatePass();
			}

			REQUIRE( material->GetType() == MaterialType::eLegacy );
			auto pass = material->GetTypedPass< MaterialType::eLegacy >( 0 );
			pass->SetShininess( 64.0f );
			DoReadMD2Data( *pass );
			DoConvertDataStructures( p_mesh );

			for ( auto submesh : p_mesh )
			{
				submesh->SetDefaultMaterial( material );
			}

			p_mesh.ComputeNormals();
			p_mesh.GetScene()->GetListener().PostEvent( MakeInitialiseEvent( *material ) );
			result = true;
		}

		DoCleanUp();
		return result;
	}

	void Md2Importer::DoReadMD2Data( Pass & p_pass )
	{
		uint64_t uiRead;
		Logger::LogDebug( StringStream() << cuT( "MD2 File - size: " ) << m_pFile->GetLength() );
		Logger::LogDebug( StringStream() << cuT( "- Vertices: " ) << m_header.m_numVertices );
		Logger::LogDebug( StringStream() << cuT( "- Skins: " ) << m_header.m_numSkins );
		m_skins = new Md2Skin[m_header.m_numSkins];
		m_pFile->Seek( m_header.m_offsetSkins );
		uiRead = m_pFile->ReadArray( m_skins, m_header.m_numSkins );

		if ( m_header.m_numSkins > 0 )
		{
			TextureUnitSPtr pTexture;
			Logger::LogDebug( StringStream() << cuT( "- Skins: " ) << ( uiRead / sizeof( Md2Skin ) ) << cuT( "/" ) << m_header.m_numSkins << cuT( " (" ) << uiRead << cuT( " bytes, from " ) << m_header.m_offsetSkins << cuT( ")" ) );

			for ( int i = 0; i < m_header.m_numSkins && !pTexture; i++ )
			{
				Path strValue{ string::string_cast< xchar >( m_skins[i] ) };

				if ( !strValue.empty() )
				{
					pTexture = LoadTexture( strValue, p_pass, TextureChannel::eDiffuse );

					if ( pTexture )
					{
						Logger::LogDebug( cuT( "- Texture found : " ) + strValue );
					}
					else
					{
						Logger::LogDebug( cuT( "- Texture not found : " ) + strValue );
					}
				}
			}
		}

		if ( m_header.m_numTexCoords > 0 )
		{
			m_texCoords = new Md2TexCoord[m_header.m_numTexCoords];
			m_pFile->Seek( m_header.m_offsetTexCoords );
			uiRead = m_pFile->ReadArray( m_texCoords, m_header.m_numTexCoords );
			Logger::LogDebug( StringStream() << cuT( "- TexCoords: " ) << ( uiRead / sizeof( Md2TexCoord ) ) << cuT( "/" ) << m_header.m_numTexCoords << cuT( " (" ) << uiRead << cuT( " bytes, from " ) << m_header.m_offsetTexCoords << cuT( ")" ) );
		}

		if ( m_header.m_numTriangles > 0 )
		{
			m_triangles = new Md2Face[m_header.m_numTriangles];
			m_pFile->Seek( m_header.m_offsetTriangles );
			uiRead = m_pFile->ReadArray( m_triangles, m_header.m_numTriangles );
			Logger::LogDebug( StringStream() << cuT( "- Triangles: " ) << ( uiRead / sizeof( Md2Face ) ) << cuT( "/" ) << m_header.m_numTriangles << cuT( " (" ) << uiRead << cuT( " bytes, from " ) << m_header.m_offsetTriangles << cuT( ")" ) );
		}

		if ( m_header.m_numFrames > 0 )
		{
			uint8_t buffer[MD2_MAX_FRAMESIZE];
			Logger::LogDebug( StringStream() << cuT( "- Frames: " ) << m_header.m_numFrames );
			m_pFile->Seek( m_header.m_offsetFrames );
			m_frames = new Md2Frame[m_header.m_numFrames];
			Md2AliasFrame * frame = reinterpret_cast< Md2AliasFrame * >( buffer );
			m_frames[0].m_vertices = new Md2Vertex[m_header.m_numVertices];
			m_pFile->ReadArray< char >( ( char * )frame, m_header.m_frameSize );
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

	void Md2Importer::DoConvertDataStructures( Mesh & p_mesh )
	{
		SubmeshSPtr submesh = p_mesh.CreateSubmesh();
		std::vector< float > arrayNml( m_header.m_numTriangles * 9 );
		std::vector< FaceIndices > arrayFaces( m_header.m_numTriangles );
		BufferElementGroupSPtr pVertex;

		if ( m_header.m_numFrames > 0 )
		{
			//for (int i = 0 ; i < m_header.m_numVertices ; i++)
			//{
			//	pVertex = submesh->AddPoint( m_frames[0].m_vertices[i].m_coords );
			//}

			//for (int i = 0 ; i < m_header.m_numTriangles ; i++)
			//{
			//	if (m_triangles[i].m_vertexIndices[0] >= 0 && m_triangles[i].m_vertexIndices[1] >= 0 && m_triangles[i].m_vertexIndices[2] >= 0)
			//	{
			//		submesh->GetPoint( m_triangles[i].m_vertexIndices[0] )->SetTexCoord( real( m_texCoords[m_triangles[i].m_textureIndices[0]].u ) / real( m_header.m_skinWidth ), real( 1.0 ) - real( m_texCoords[m_triangles[i].m_textureIndices[0]].v ) / real( m_header.m_skinHeight ) );
			//		submesh->GetPoint( m_triangles[i].m_vertexIndices[1] )->SetTexCoord( real( m_texCoords[m_triangles[i].m_textureIndices[1]].u ) / real( m_header.m_skinWidth ), real( 1.0 ) - real( m_texCoords[m_triangles[i].m_textureIndices[1]].v ) / real( m_header.m_skinHeight ) );
			//		submesh->GetPoint( m_triangles[i].m_vertexIndices[2] )->SetTexCoord( real( m_texCoords[m_triangles[i].m_textureIndices[2]].u ) / real( m_header.m_skinWidth ), real( 1.0 ) - real( m_texCoords[m_triangles[i].m_textureIndices[2]].v ) / real( m_header.m_skinHeight ) );
			//		submesh->AddFace( m_triangles[i].m_vertexIndices[0], m_triangles[i].m_vertexIndices[1], m_triangles[i].m_vertexIndices[2] );
			//	}
			//}

			for ( int i = 0; i < m_header.m_numTriangles; i++ )
			{
				if ( m_triangles[i].m_vertexIndices[0] >= 0 && m_triangles[i].m_vertexIndices[1] >= 0 && m_triangles[i].m_vertexIndices[2] >= 0 )
				{
					Vertex::SetTexCoord( submesh->AddPoint( m_frames[0].m_vertices[m_triangles[i].m_vertexIndices[0]].m_coords ), real( m_texCoords[m_triangles[i].m_textureIndices[0]].u ) / real( m_header.m_skinWidth ), real( 1.0 ) - real( m_texCoords[m_triangles[i].m_textureIndices[0]].v ) / real( m_header.m_skinHeight ) );
					Vertex::SetTexCoord( submesh->AddPoint( m_frames[0].m_vertices[m_triangles[i].m_vertexIndices[1]].m_coords ), real( m_texCoords[m_triangles[i].m_textureIndices[1]].u ) / real( m_header.m_skinWidth ), real( 1.0 ) - real( m_texCoords[m_triangles[i].m_textureIndices[1]].v ) / real( m_header.m_skinHeight ) );
					Vertex::SetTexCoord( submesh->AddPoint( m_frames[0].m_vertices[m_triangles[i].m_vertexIndices[2]].m_coords ), real( m_texCoords[m_triangles[i].m_textureIndices[2]].u ) / real( m_header.m_skinWidth ), real( 1.0 ) - real( m_texCoords[m_triangles[i].m_textureIndices[2]].v ) / real( m_header.m_skinHeight ) );
					submesh->AddFace( i * 3 + 0, i * 3 + 1, i * 3 + 2 );
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

			//l_submesh->AddFaceGroup( &arrayFaces[0], m_header.m_numTriangles, NULL, &arrayTex[0] );
		}
	}

	void Md2Importer::DoCleanUp()
	{
		delete m_pFile;
		delete[] m_skins;
		delete m_texCoords;
		delete m_triangles;
		delete m_frames;
		m_pFile = NULL;
		m_skins = NULL;
		m_texCoords = NULL;
		m_triangles = NULL;
		m_frames = NULL;
	}
}
