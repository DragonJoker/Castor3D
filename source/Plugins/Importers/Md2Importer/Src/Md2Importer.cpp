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
		auto l_mesh = p_scene.GetMeshCache().Add( cuT( "Mesh_MD2" ) );
		bool l_return = DoImportMesh( *l_mesh );

		if ( l_return )
		{
			SceneNodeSPtr l_node = p_scene.GetSceneNodeCache().Add( l_mesh->GetName(), p_scene.GetObjectRootNode() );
			GeometrySPtr l_geometry = p_scene.GetGeometryCache().Add( l_mesh->GetName(), l_node, nullptr );
			l_geometry->SetMesh( l_mesh );
			m_geometries.insert( { l_geometry->GetName(), l_geometry } );
		}

		return l_return;
	}

	bool Md2Importer::DoImportMesh( Mesh & p_mesh )
	{
		bool l_return{ false };
		m_pFile = new BinaryFile( m_fileName, File::OpenMode::eRead );
		String l_materialName = m_fileName.GetFileName();
		m_pFile->Read( m_header );

		if ( m_header.m_version != 8 )
		{
			Logger::LogInfo( cuT( "Invalid file format (Version not 8): " ) + m_fileName );
		}
		else
		{
			auto l_material = p_mesh.GetScene()->GetMaterialView().Find( l_materialName );

			if ( !l_material )
			{
				l_material = p_mesh.GetScene()->GetMaterialView().Add( l_materialName, MaterialType::eLegacy );
				l_material->CreatePass();
			}

			REQUIRE( l_material->GetType() == MaterialType::eLegacy );
			auto l_pass = l_material->GetTypedPass< MaterialType::eLegacy >( 0 );
			l_pass->SetAmbient( Castor::Colour::from_components( 0.0f, 0.0f, 0.0f, 1.0f ) );
			l_pass->SetEmissive( Castor::HdrColour::from_components( 0.5f, 0.5f, 0.5f, 1.0f ) );
			l_pass->SetShininess( 64.0f );
			DoReadMD2Data( *l_pass );
			DoConvertDataStructures( p_mesh );

			for ( auto l_submesh : p_mesh )
			{
				l_submesh->SetDefaultMaterial( l_material );
			}

			p_mesh.ComputeNormals();
			p_mesh.GetScene().GetListener().PostEvent( MakeInitialiseEvent( *l_material ) );
			l_return = true;
		}

		DoCleanUp();
		return l_return;
	}

	void Md2Importer::DoReadMD2Data( Pass & p_pass )
	{
		uint64_t l_uiRead;
		Logger::LogDebug( StringStream() << cuT( "MD2 File - size: " ) << m_pFile->GetLength() );
		Logger::LogDebug( StringStream() << cuT( "- Vertices: " ) << m_header.m_numVertices );
		Logger::LogDebug( StringStream() << cuT( "- Skins: " ) << m_header.m_numSkins );
		m_skins = new Md2Skin[m_header.m_numSkins];
		m_pFile->Seek( m_header.m_offsetSkins );
		l_uiRead = m_pFile->ReadArray( m_skins, m_header.m_numSkins );

		if ( m_header.m_numSkins > 0 )
		{
			TextureUnitSPtr l_pTexture;
			Logger::LogDebug( StringStream() << cuT( "- Skins: " ) << ( l_uiRead / sizeof( Md2Skin ) ) << cuT( "/" ) << m_header.m_numSkins << cuT( " (" ) << l_uiRead << cuT( " bytes, from " ) << m_header.m_offsetSkins << cuT( ")" ) );

			for ( int i = 0; i < m_header.m_numSkins && !l_pTexture; i++ )
			{
				Path l_strValue{ string::string_cast< xchar >( m_skins[i] ) };

				if ( !l_strValue.empty() )
				{
					l_pTexture = LoadTexture( l_strValue, p_pass, TextureChannel::eDiffuse );

					if ( l_pTexture )
					{
						Logger::LogDebug( cuT( "- Texture found : " ) + l_strValue );
					}
					else
					{
						Logger::LogDebug( cuT( "- Texture not found : " ) + l_strValue );
					}
				}
			}
		}

		if ( m_header.m_numTexCoords > 0 )
		{
			m_texCoords = new Md2TexCoord[m_header.m_numTexCoords];
			m_pFile->Seek( m_header.m_offsetTexCoords );
			l_uiRead = m_pFile->ReadArray( m_texCoords, m_header.m_numTexCoords );
			Logger::LogDebug( StringStream() << cuT( "- TexCoords: " ) << ( l_uiRead / sizeof( Md2TexCoord ) ) << cuT( "/" ) << m_header.m_numTexCoords << cuT( " (" ) << l_uiRead << cuT( " bytes, from " ) << m_header.m_offsetTexCoords << cuT( ")" ) );
		}

		if ( m_header.m_numTriangles > 0 )
		{
			m_triangles = new Md2Face[m_header.m_numTriangles];
			m_pFile->Seek( m_header.m_offsetTriangles );
			l_uiRead = m_pFile->ReadArray( m_triangles, m_header.m_numTriangles );
			Logger::LogDebug( StringStream() << cuT( "- Triangles: " ) << ( l_uiRead / sizeof( Md2Face ) ) << cuT( "/" ) << m_header.m_numTriangles << cuT( " (" ) << l_uiRead << cuT( " bytes, from " ) << m_header.m_offsetTriangles << cuT( ")" ) );
		}

		if ( m_header.m_numFrames > 0 )
		{
			uint8_t l_buffer[MD2_MAX_FRAMESIZE];
			Logger::LogDebug( StringStream() << cuT( "- Frames: " ) << m_header.m_numFrames );
			m_pFile->Seek( m_header.m_offsetFrames );
			m_frames = new Md2Frame[m_header.m_numFrames];
			Md2AliasFrame * l_frame = reinterpret_cast< Md2AliasFrame * >( l_buffer );
			m_frames[0].m_vertices = new Md2Vertex[m_header.m_numVertices];
			m_pFile->ReadArray< char >( ( char * )l_frame, m_header.m_frameSize );
			m_frames[0].m_strName = l_frame->m_name;
			Md2Vertex * l_vertices = m_frames[0].m_vertices;

			for ( int j = 0; j < m_header.m_numVertices; j++ )
			{
				l_vertices[j].m_coords[0] = ( l_frame->m_aliasVertices[j].m_vertex[0] * l_frame->m_scale[0] + l_frame->m_translate[0] ) * 1;
				l_vertices[j].m_coords[2] = ( l_frame->m_aliasVertices[j].m_vertex[1] * l_frame->m_scale[1] + l_frame->m_translate[1] ) * -1;
				l_vertices[j].m_coords[1] = ( l_frame->m_aliasVertices[j].m_vertex[2] * l_frame->m_scale[2] + l_frame->m_translate[2] ) * 1;
			}
		}
	}

	void Md2Importer::DoConvertDataStructures( Mesh & p_mesh )
	{
		SubmeshSPtr l_submesh = p_mesh.CreateSubmesh();
		std::vector< float > l_arrayNml( m_header.m_numTriangles * 9 );
		std::vector< FaceIndices > l_arrayFaces( m_header.m_numTriangles );
		BufferElementGroupSPtr l_pVertex;

		if ( m_header.m_numFrames > 0 )
		{
			//for (int i = 0 ; i < m_header.m_numVertices ; i++)
			//{
			//	l_pVertex = l_submesh->AddPoint( m_frames[0].m_vertices[i].m_coords );
			//}

			//for (int i = 0 ; i < m_header.m_numTriangles ; i++)
			//{
			//	if (m_triangles[i].m_vertexIndices[0] >= 0 && m_triangles[i].m_vertexIndices[1] >= 0 && m_triangles[i].m_vertexIndices[2] >= 0)
			//	{
			//		l_submesh->GetPoint( m_triangles[i].m_vertexIndices[0] )->SetTexCoord( real( m_texCoords[m_triangles[i].m_textureIndices[0]].u ) / real( m_header.m_skinWidth ), real( 1.0 ) - real( m_texCoords[m_triangles[i].m_textureIndices[0]].v ) / real( m_header.m_skinHeight ) );
			//		l_submesh->GetPoint( m_triangles[i].m_vertexIndices[1] )->SetTexCoord( real( m_texCoords[m_triangles[i].m_textureIndices[1]].u ) / real( m_header.m_skinWidth ), real( 1.0 ) - real( m_texCoords[m_triangles[i].m_textureIndices[1]].v ) / real( m_header.m_skinHeight ) );
			//		l_submesh->GetPoint( m_triangles[i].m_vertexIndices[2] )->SetTexCoord( real( m_texCoords[m_triangles[i].m_textureIndices[2]].u ) / real( m_header.m_skinWidth ), real( 1.0 ) - real( m_texCoords[m_triangles[i].m_textureIndices[2]].v ) / real( m_header.m_skinHeight ) );
			//		l_submesh->AddFace( m_triangles[i].m_vertexIndices[0], m_triangles[i].m_vertexIndices[1], m_triangles[i].m_vertexIndices[2] );
			//	}
			//}

			for ( int i = 0; i < m_header.m_numTriangles; i++ )
			{
				if ( m_triangles[i].m_vertexIndices[0] >= 0 && m_triangles[i].m_vertexIndices[1] >= 0 && m_triangles[i].m_vertexIndices[2] >= 0 )
				{
					Vertex::SetTexCoord( l_submesh->AddPoint( m_frames[0].m_vertices[m_triangles[i].m_vertexIndices[0]].m_coords ), real( m_texCoords[m_triangles[i].m_textureIndices[0]].u ) / real( m_header.m_skinWidth ), real( 1.0 ) - real( m_texCoords[m_triangles[i].m_textureIndices[0]].v ) / real( m_header.m_skinHeight ) );
					Vertex::SetTexCoord( l_submesh->AddPoint( m_frames[0].m_vertices[m_triangles[i].m_vertexIndices[1]].m_coords ), real( m_texCoords[m_triangles[i].m_textureIndices[1]].u ) / real( m_header.m_skinWidth ), real( 1.0 ) - real( m_texCoords[m_triangles[i].m_textureIndices[1]].v ) / real( m_header.m_skinHeight ) );
					Vertex::SetTexCoord( l_submesh->AddPoint( m_frames[0].m_vertices[m_triangles[i].m_vertexIndices[2]].m_coords ), real( m_texCoords[m_triangles[i].m_textureIndices[2]].u ) / real( m_header.m_skinWidth ), real( 1.0 ) - real( m_texCoords[m_triangles[i].m_textureIndices[2]].v ) / real( m_header.m_skinHeight ) );
					l_submesh->AddFace( i * 3 + 0, i * 3 + 1, i * 3 + 2 );
				}
			}

			delete[] m_frames[0].m_vertices;

			//for (int i = 0 ; i < m_header.m_numTriangles ; i++)
			//{
			//	if (m_triangles[i].m_vertexIndices[0] >= 0 && m_triangles[i].m_vertexIndices[1] >= 0 && m_triangles[i].m_vertexIndices[2] >= 0)
			//	{
			//		l_arrayFaces[i].m_uiVertexIndex[0] = m_triangles[i].m_vertexIndices[0];
			//		l_arrayFaces[i].m_uiVertexIndex[1] = m_triangles[i].m_vertexIndices[1];
			//		l_arrayFaces[i].m_uiVertexIndex[2] = m_triangles[i].m_vertexIndices[2];
			//		l_arrayTex.push_back( float( m_texCoords[m_triangles[i].m_textureIndices[0]].u ) / float( m_header.m_skinWidth ) );
			//		l_arrayTex.push_back( 1-float( m_texCoords[m_triangles[i].m_textureIndices[0]].v ) / float( m_header.m_skinHeight ) );
			//		l_arrayTex.push_back( float( m_texCoords[m_triangles[i].m_textureIndices[1]].u ) / float( m_header.m_skinWidth ) );
			//		l_arrayTex.push_back( 1-float( m_texCoords[m_triangles[i].m_textureIndices[1]].v ) / float( m_header.m_skinHeight ) );
			//		l_arrayTex.push_back( float( m_texCoords[m_triangles[i].m_textureIndices[2]].u ) / float( m_header.m_skinWidth ) );
			//		l_arrayTex.push_back( 1-float( m_texCoords[m_triangles[i].m_textureIndices[2]].v ) / float( m_header.m_skinHeight ) );
			//	}
			//}

			//l_submesh->AddFaceGroup( &l_arrayFaces[0], m_header.m_numTriangles, NULL, &l_arrayTex[0] );
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
