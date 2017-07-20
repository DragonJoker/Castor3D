#include "Md3Importer.hpp"

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

namespace C3DMd3
{
	Md3Importer::Md3Importer( Engine & p_engine )
		: Importer( p_engine )
		, m_skins( NULL )
		, m_texCoords( NULL )
		, m_triangles( NULL )
		, m_bones( NULL )
		, m_pFile( NULL )
		, m_links( NULL )
		, m_vertices( NULL )
		, m_tags( NULL )
		, m_numOfTags( 0 )
	{
		memset( &m_header, 0, sizeof( Md3Header ) );
	}

	ImporterUPtr Md3Importer::Create( Engine & p_engine )
	{
		return std::make_unique< Md3Importer >( p_engine );
	}

	bool Md3Importer::DoImportScene( Scene & p_scene )
	{
		auto mesh = p_scene.GetMeshCache().Add( cuT( "Mesh_MD3" ) );
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

	bool Md3Importer::DoImportMesh( Mesh & p_mesh )
	{
		bool result{ false };
		m_pFile = new BinaryFile( m_fileName, File::OpenMode::eRead );
		String meshName = m_fileName.GetFileName();
		String materialName = m_fileName.GetFileName();
		m_pFile->Read( m_header );
		char * id = m_header.m_fileID;

		if ( ( id[0] != 'I' || id[1] != 'D' || id[2] != 'P' || id[3] != '3' ) || m_header.m_version != 15 )
		{
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
			auto pass = material->GetTypedPass< MaterialType::eLegacy >( 0u );
			pass->SetShininess( 64.0f );
			DoReadMD3Data( p_mesh, *pass );

			for ( auto submesh : p_mesh )
			{
				submesh->SetDefaultMaterial( material );
			}

			p_mesh.ComputeNormals();
			DoCleanUp();
			result = true;
		}

		delete m_pFile;
		return result;
	}

	void Md3Importer::DoReadMD3Data( Mesh & p_mesh, LegacyPass & p_pass )
	{
		int i = 0;
		uint64_t uiRead;
		TextureUnitSPtr pTexture;
		ImageSPtr pImage;
		Logger::LogInfo( StringStream() << cuT( "MD3 File: size " ) << m_pFile->GetLength() );

		if ( m_header.m_numFrames > 0 )
		{
			m_bones = new Md3Bone[m_header.m_numFrames];
			uiRead = m_pFile->ReadArray( m_bones, m_header.m_numFrames );
			Logger::LogInfo( StringStream() << cuT( "* Bones: " ) << ( uiRead / sizeof( Md3Bone ) ) << cuT( "/" ) << m_header.m_numFrames << cuT( " (" ) << uiRead << cuT( " bytes)" ) );
			delete[] m_bones;
		}

		if ( m_header.m_numTags > 0 )
		{
			m_tags = new Md3Tag[m_header.m_numFrames * m_header.m_numTags];
			uiRead = m_pFile->ReadArray( m_tags, m_header.m_numFrames * m_header.m_numTags );
			Logger::LogInfo( StringStream() << cuT( "* Tags: " ) << ( uiRead / sizeof( Md3Tag ) ) << cuT( "/" ) << m_header.m_numTags << cuT( " (" ) << uiRead << cuT( " bytes)" ) );
			m_numOfTags = m_header.m_numTags;
		}

		m_links = new MeshSPtr[m_header.m_numTags];
		//m_links = (MeshPtr *) malloc( sizeof( Mesh) * m_header.m_numTags);
		//
		//for (i = 0 ; i < m_header.m_numTags ; i++)
		//{
		//	m_links[i].reset();
		//}

		long long meshOffset = m_pFile->Tell();
		Md3MeshInfo meshHeader = { 0 };

		for ( i = 0; i < m_header.m_numMeshes; i++ )
		{
			Logger::LogInfo( StringStream() << cuT( "* Submesh: " ) << ( i + 1 ) << cuT( "/" ) << m_header.m_numMeshes << cuT( " (from " ) << meshOffset << cuT( ")" ) );
			m_pFile->Seek( meshOffset );
			m_pFile->Read( meshHeader );
			m_skins = new Md3Skin[meshHeader.m_numSkins];
			uiRead = m_pFile->ReadArray( m_skins, meshHeader.m_numSkins );
			Logger::LogInfo( StringStream() << cuT( "* * Skins: " ) << ( uiRead / sizeof( Md3Skin ) ) << cuT( "/" ) << meshHeader.m_numSkins << cuT( " (" ) << uiRead << cuT( " bytes)" ) );

			for ( int i = 0; i < meshHeader.m_numSkins && !pTexture; i++ )
			{
				Path strValue{ string::string_cast< xchar >( m_skins[i].m_strName ) };

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

			m_triangles = new Md3Face[meshHeader.m_numTriangles];
			m_pFile->Seek( meshOffset + meshHeader.m_triStart );
			uiRead = m_pFile->ReadArray( m_triangles, meshHeader.m_numTriangles );
			Logger::LogInfo( StringStream() << cuT( "* * Triangles: " ) << ( uiRead / sizeof( Md3Face ) ) << cuT( "/" ) << meshHeader.m_numTriangles << cuT( " (" ) << uiRead << cuT( " bytes, from " ) << meshHeader.m_triStart << cuT( ")" ) );
			m_texCoords = new Md3TexCoord[meshHeader.m_numVertices];
			m_pFile->Seek( meshOffset + meshHeader.m_uvStart );
			uiRead = m_pFile->ReadArray( m_texCoords, meshHeader.m_numVertices );
			Logger::LogInfo( StringStream() << cuT( "* * TexCoords: " ) << ( uiRead / sizeof( Md3TexCoord ) ) << cuT( "/" ) << meshHeader.m_numVertices << cuT( " (" ) << uiRead << cuT( " bytes, from " ) << meshHeader.m_uvStart << cuT( ")" ) );
			m_vertices = new Md3Triangle[meshHeader.m_numVertices];
			m_pFile->Seek( meshOffset + meshHeader.m_vertexStart );
			uiRead = m_pFile->ReadArray( m_vertices, meshHeader.m_numVertices );
			Logger::LogInfo( StringStream() << cuT( "* * Vertices: " ) << ( uiRead / sizeof( Md3Triangle ) ) << cuT( "/" ) << meshHeader.m_numVertices << cuT( " (" ) << uiRead << cuT( " bytes, from " ) << meshHeader.m_vertexStart << cuT( ")" ) );
			DoConvertDataStructures( p_mesh, meshHeader );
			delete[] m_skins;
			delete[] m_texCoords;
			delete[] m_triangles;
			delete[] m_vertices;
			meshOffset += meshHeader.m_meshSize;
		}

		String strFileName = m_pFile->GetFileFullPath();
		strFileName = strFileName.substr( 0, strFileName.find_last_of( '.' ) );
		DoLoadSkin( p_mesh, Path{ strFileName + cuT( ".skin" ) } );
		DoLoadShader( p_mesh, Path{ strFileName + cuT( ".shader" ) } );
		delete[] m_links;
	}

	void Md3Importer::DoConvertDataStructures( Mesh & p_mesh, Md3MeshInfo p_meshHeader )
	{
		BufferElementGroupSPtr pVertex;
		SubmeshSPtr submesh = p_mesh.CreateSubmesh();
		m_mapSubmeshesByName.insert( std::make_pair( string::string_cast< xchar >( p_meshHeader.m_strName ), submesh ) );

		for ( int i = 0; i < p_meshHeader.m_numVertices; i++ )
		{
			pVertex = submesh->AddPoint( m_vertices[i].m_vertex[0] / 64.0f, m_vertices[i].m_vertex[1] / 64.0f, m_vertices[i].m_vertex[2] / 64.0f );
			Vertex::SetTexCoord( pVertex, m_texCoords[i].m_textureCoord[0], 1 - m_texCoords[i].m_textureCoord[1] );
		}

		for ( int i = 0; i < p_meshHeader.m_numTriangles; i++ )
		{
			if ( m_triangles[i].m_vertexIndices[0] >= 0 && m_triangles[i].m_vertexIndices[1] >= 0 && m_triangles[i].m_vertexIndices[2] >= 0 )
			{
				submesh->AddFace( m_triangles[i].m_vertexIndices[0], m_triangles[i].m_vertexIndices[1], m_triangles[i].m_vertexIndices[2] );
			}
		}

		//for (int i = 0 ; i < p_meshHeader.m_numTriangles ; i++)
		//{
		//	if (m_triangles[i].m_vertexIndices[0] >= 0 && m_triangles[i].m_vertexIndices[1] >= 0 && m_triangles[i].m_vertexIndices[2] >= 0)
		//	{
		//		submesh->AddPoint( m_vertices[m_triangles[i].m_vertexIndices[0]].m_vertex[0] / 64.0f, m_vertices[m_triangles[i].m_vertexIndices[0]].m_vertex[1] / 64.0f, m_vertices[m_triangles[i].m_vertexIndices[0]].m_vertex[2] / 64.0f )->SetTexCoord( real( m_texCoords[m_triangles[i].m_textureIndices[0]].u ), real( 1.0 ) - real( m_texCoords[m_triangles[i].m_textureIndices[0]].v ) ) );
		//		submesh->AddPoint( m_vertices[m_triangles[i].m_vertexIndices[1]].m_vertex[0] / 64.0f, m_vertices[m_triangles[i].m_vertexIndices[1]].m_vertex[1] / 64.0f, m_vertices[m_triangles[i].m_vertexIndices[1]].m_vertex[2] / 64.0f )->SetTexCoord( real( m_texCoords[m_triangles[i].m_textureIndices[1]].u ), real( 1.0 ) - real( m_texCoords[m_triangles[i].m_textureIndices[1]].v ) ) );
		//		submesh->AddPoint( m_vertices[m_triangles[i].m_vertexIndices[2]].m_vertex[0] / 64.0f, m_vertices[m_triangles[i].m_vertexIndices[2]].m_vertex[1] / 64.0f, m_vertices[m_triangles[i].m_vertexIndices[2]].m_vertex[2] / 64.0f )->SetTexCoord( real( m_texCoords[m_triangles[i].m_textureIndices[2]].u ), real( 1.0 ) - real( m_texCoords[m_triangles[i].m_textureIndices[2]].v ) ) );
		//		submesh->AddFace( i * 3 + 0, i * 3 + 1, i * 3 + 2 );
		//	}
		//}

		////Point2rArray texVerts;

		////for (int i = 0 ; i < p_meshHeader.m_numVertices ; i++)
		////{
		////	texVerts.push_back( Point2r( m_texCoords[i].m_textureCoord[0], 1-m_texCoords[i].m_textureCoord[1] ) );
		////}

		////std::vector< FaceIndices > arrayFaces( p_meshHeader.m_numTriangles );
		////std::vector< float > arrayTex;

		////for (int i = 0 ; i < p_meshHeader.m_numTriangles ; i++)
		////{
		////	if (m_triangles[i].m_vertexIndices[0] >= 0 && m_triangles[i].m_vertexIndices[1] >= 0 && m_triangles[i].m_vertexIndices[2] >= 0)
		////	{
		////		arrayFaces[i].m_uiVertexIndex[0] = m_triangles[i].m_vertexIndices[0];
		////		arrayFaces[i].m_uiVertexIndex[1] = m_triangles[i].m_vertexIndices[1];
		////		arrayFaces[i].m_uiVertexIndex[2] = m_triangles[i].m_vertexIndices[2];
		////		arrayTex.push_back( texVerts[m_triangles[i].m_vertexIndices[0]][0] );
		////		arrayTex.push_back( texVerts[m_triangles[i].m_vertexIndices[0]][1] );
		////		arrayTex.push_back( texVerts[m_triangles[i].m_vertexIndices[1]][0] );
		////		arrayTex.push_back( texVerts[m_triangles[i].m_vertexIndices[1]][1] );
		////		arrayTex.push_back( texVerts[m_triangles[i].m_vertexIndices[2]][0] );
		////		arrayTex.push_back( texVerts[m_triangles[i].m_vertexIndices[2]][1] );
		////	}
		////}

		////l_submesh->AddFaceGroup( &arrayFaces[0], p_meshHeader.m_numTriangles, NULL, &arrayTex[0] );
	}

	bool Md3Importer::DoLoadSkin( Mesh & p_mesh, Path const & p_strSkin )
	{
		bool bReturn = false;

		if ( File::FileExists( p_strSkin ) )
		{
			TextFile fileIO( p_strSkin, File::OpenMode::eRead );
			String strLine, strSection, strImage;

			while ( fileIO.IsOk() )
			{
				fileIO.ReadLine( strLine, 255 );
				strSection.clear();

				if ( !strLine.empty() && strLine.find_last_of( ',' ) != String::npos )
				{
					strSection = strLine.substr( 0, strLine.find_last_of( ',' ) );
				}

				if ( m_mapSubmeshesByName.find( strSection ) != m_mapSubmeshesByName.end() )
				{
					strImage = strLine.substr( strLine.find_last_of( ',' ) + 1 );
					strImage = strImage.substr( strImage.find_last_of( '/' ) + 1 );
					MaterialSPtr material = p_mesh.GetScene()->GetMaterialView().Find( strSection );

					if ( !material )
					{
						material = p_mesh.GetScene()->GetMaterialView().Add( strSection, MaterialType::eLegacy );
						material->CreatePass();
					}
					
					REQUIRE( material->GetType() == MaterialType::eLegacy );
					auto pass = material->GetTypedPass< MaterialType::eLegacy >( 0u );

					if ( !strImage.empty() )
					{
						LoadTexture( Path{ strImage }, *pass, TextureChannel::eDiffuse );
					}

					m_mapSubmeshesByName.find( strSection )->second->SetDefaultMaterial( material );
					p_mesh.GetScene()->GetListener().PostEvent( MakeInitialiseEvent( *material ) );
				}
			}

			bReturn = true;
		}

		return bReturn;
	}

	bool Md3Importer::DoLoadShader( Mesh & p_mesh, Path const & p_strShader )
	{
		bool bReturn = false;

		if ( File::FileExists( p_strShader ) )
		{
			TextFile fileIO( p_strShader, File::OpenMode::eRead );
			String strName = fileIO.GetFileName().substr( 0, fileIO.GetFileName().find_last_of( '.' ) );
			String strLine;
			uint32_t uiIndex = 0;
			StringStream strMatName;

			while ( fileIO.IsOk() && uiIndex < p_mesh.GetSubmeshCount() )
			{
				fileIO.ReadLine( strLine, 255 );
				StringStream strMatName( strName );
				strMatName << "_" << uiIndex;
				MaterialSPtr material = p_mesh.GetScene()->GetMaterialView().Find( strMatName.str() );

				if ( !material )
				{
					material = p_mesh.GetScene()->GetMaterialView().Add( strMatName.str(), MaterialType::eLegacy );
					material->CreatePass();
				}
				
				REQUIRE( material->GetType() == MaterialType::eLegacy );
				auto pass = material->GetTypedPass< MaterialType::eLegacy >( 0u );
				pass->SetTwoSided( true );

				if ( !strLine.empty() )
				{
					LoadTexture( Path{ strLine }, *pass, TextureChannel::eDiffuse );
				}

				p_mesh.GetSubmesh( uiIndex )->SetDefaultMaterial( material );
				p_mesh.GetScene()->GetListener().PostEvent( MakeInitialiseEvent( *material ) );
				uiIndex++;
			}

			bReturn = true;
		}

		return bReturn;
	}

	void Md3Importer::DoCleanUp()
	{
	}
}
