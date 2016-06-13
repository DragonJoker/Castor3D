#include "Md3Importer.hpp"

#include <Colour.hpp>
#include <Image.hpp>

#include <Engine.hpp>
#include <GeometryManager.hpp>
#include <MaterialManager.hpp>
#include <MeshManager.hpp>
#include <SceneManager.hpp>
#include <SceneNodeManager.hpp>

#include <Event/Frame/InitialiseEvent.hpp>
#include <Manager/ManagerView.hpp>
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

bool _isInString( String const & p_strString, String const & p_strSubString )
{
	if ( p_strString.length() <= 0 || p_strSubString.length() <= 0 )
	{
		return false;
	}

	return p_strString.find( p_strSubString ) != String::npos;
}

Md3Importer::Md3Importer( Engine & p_pEngine )
	: Importer( p_pEngine )
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

SceneSPtr Md3Importer::DoImportScene()
{
	SceneSPtr l_scene = GetEngine()->GetSceneManager().Create( cuT( "Scene_MD3" ), *GetEngine() );
	MeshSPtr l_mesh = DoImportMesh( *l_scene );

	if ( l_mesh )
	{
		SceneNodeSPtr l_node = l_scene->GetSceneNodeManager().Create( l_mesh->GetName(), l_scene->GetObjectRootNode() );
		GeometrySPtr l_geometry = l_scene->GetGeometryManager().Create( l_mesh->GetName(), l_node );

		for ( auto l_submesh : *l_mesh )
		{
			GetEngine()->PostEvent( MakeInitialiseEvent( *l_submesh ) );
		}

		l_geometry->SetMesh( l_mesh );
	}

	return l_scene;
}

MeshSPtr Md3Importer::DoImportMesh( Scene & p_scene )
{
	m_pFile = new BinaryFile( m_fileName, File::eOPEN_MODE_READ );
	SceneSPtr l_scene = GetEngine()->GetSceneManager().Create( cuT( "Scene_MD3" ), *GetEngine() );
	UIntArray l_faces;
	RealArray l_sizes;
	MaterialSPtr l_material;
	PassSPtr l_pass;
	String l_meshName = m_fileName.GetFileName();
	String l_materialName = m_fileName.GetFileName();
	MeshSPtr l_mesh = p_scene.GetMeshManager().Create( l_meshName, eMESH_TYPE_CUSTOM, l_faces, l_sizes );
	m_pFile->Read( m_header );
	char * l_id = m_header.m_fileID;

	if ( ( l_id[0] != 'I' || l_id[1] != 'D' || l_id[2] != 'P' || l_id[3] != '3' ) || m_header.m_version != 15 )
	{
	}
	else
	{
		l_material = p_scene.GetMaterialView().Find( l_materialName );

		if ( !l_material )
		{
			l_material = p_scene.GetMaterialView().Create( l_materialName, *GetEngine() );
			l_material->CreatePass();
		}

		l_pass = l_material->GetPass( 0 );
		l_pass->SetAmbient( Castor::Colour::from_components( 0.0f, 0.0f, 0.0f, 1.0f ) );
		l_pass->SetEmissive( Castor::Colour::from_components( 0.5f, 0.5f, 0.5f, 1.0f ) );
		l_pass->SetShininess( 64.0f );
		DoReadMD3Data( p_scene, l_mesh, l_pass );

		for ( auto l_submesh : *l_mesh )
		{
			l_submesh->SetDefaultMaterial( l_material );
		}

		l_mesh->ComputeNormals();
		DoCleanUp();
	}

	delete m_pFile;
	return l_mesh;
}

void Md3Importer::DoReadMD3Data( Scene & p_scene, MeshSPtr p_pMesh, PassSPtr p_pPass )
{
	int i = 0;
	uint64_t l_uiRead;
	TextureUnitSPtr l_pTexture;
	ImageSPtr l_pImage;
	Logger::LogInfo( StringStream() << cuT( "MD3 File: size " ) << m_pFile->GetLength() );

	if ( m_header.m_numFrames > 0 )
	{
		m_bones = new Md3Bone[m_header.m_numFrames];
		l_uiRead = m_pFile->ReadArray( m_bones, m_header.m_numFrames );
		Logger::LogInfo( StringStream() << cuT( "* Bones: " ) << ( l_uiRead / sizeof( Md3Bone ) ) << cuT( "/" ) << m_header.m_numFrames << cuT( " (" ) << l_uiRead << cuT( " bytes)" ) );
		delete [] m_bones;
	}

	if ( m_header.m_numTags > 0 )
	{
		m_tags = new Md3Tag[m_header.m_numFrames * m_header.m_numTags];
		l_uiRead = m_pFile->ReadArray( m_tags, m_header.m_numFrames * m_header.m_numTags );
		Logger::LogInfo( StringStream() << cuT( "* Tags: " ) << ( l_uiRead / sizeof( Md3Tag ) ) << cuT( "/" ) << m_header.m_numTags << cuT( " (" ) << l_uiRead << cuT( " bytes)" ) );
		m_numOfTags = m_header.m_numTags;
	}

	m_links = new MeshSPtr[m_header.m_numTags];
 	//m_links = (MeshPtr *) malloc( sizeof( Mesh) * m_header.m_numTags);
	//
 	//for (i = 0 ; i < m_header.m_numTags ; i++)
 	//{
 	//	m_links[i].reset();
 	//}

	long long l_meshOffset = m_pFile->Tell();
	Md3MeshInfo l_meshHeader = { 0 };

	for ( i = 0; i < m_header.m_numMeshes; i++ )
	{
		Logger::LogInfo( StringStream() << cuT( "* Submesh: " ) << ( i + 1 ) << cuT( "/" ) << m_header.m_numMeshes << cuT( " (from " ) << l_meshOffset << cuT( ")" ) );
		m_pFile->Seek( l_meshOffset );
		m_pFile->Read( l_meshHeader );
		m_skins = new Md3Skin[l_meshHeader.m_numSkins];
		l_uiRead = m_pFile->ReadArray( m_skins, l_meshHeader.m_numSkins );
		Logger::LogInfo( StringStream() << cuT( "* * Skins: " ) << ( l_uiRead / sizeof( Md3Skin ) ) << cuT( "/" ) << l_meshHeader.m_numSkins << cuT( " (" ) << l_uiRead << cuT( " bytes)" ) );

		for ( int i = 0 ; i < l_meshHeader.m_numSkins && !l_pTexture ; i++ )
		{
			Path l_strValue{ string::string_cast< xchar >( m_skins[i].m_strName ) };

			if ( p_pPass && !l_strValue.empty() )
			{
				l_pTexture = LoadTexture( l_strValue, *p_pPass, TextureChannel::Diffuse );

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

		m_triangles = new Md3Face[l_meshHeader.m_numTriangles];
		m_pFile->Seek( l_meshOffset + l_meshHeader.m_triStart );
		l_uiRead = m_pFile->ReadArray( m_triangles, l_meshHeader.m_numTriangles );
		Logger::LogInfo( StringStream() << cuT( "* * Triangles: " ) << ( l_uiRead / sizeof( Md3Face ) ) << cuT( "/" ) << l_meshHeader.m_numTriangles << cuT( " (" ) << l_uiRead << cuT( " bytes, from " ) << l_meshHeader.m_triStart << cuT( ")" ) );
		m_texCoords = new Md3TexCoord[l_meshHeader.m_numVertices];
		m_pFile->Seek( l_meshOffset + l_meshHeader.m_uvStart );
		l_uiRead = m_pFile->ReadArray( m_texCoords, l_meshHeader.m_numVertices );
		Logger::LogInfo( StringStream() << cuT( "* * TexCoords: " ) << ( l_uiRead / sizeof( Md3TexCoord ) ) << cuT( "/" ) << l_meshHeader.m_numVertices << cuT( " (" ) << l_uiRead << cuT( " bytes, from " ) << l_meshHeader.m_uvStart << cuT( ")" ) );
		m_vertices  = new Md3Triangle[l_meshHeader.m_numVertices];
		m_pFile->Seek( l_meshOffset + l_meshHeader.m_vertexStart );
		l_uiRead = m_pFile->ReadArray( m_vertices, l_meshHeader.m_numVertices );
		Logger::LogInfo( StringStream() << cuT( "* * Vertices: " ) << ( l_uiRead / sizeof( Md3Triangle ) ) << cuT( "/" ) << l_meshHeader.m_numVertices << cuT( " (" ) << l_uiRead << cuT( " bytes, from " ) << l_meshHeader.m_vertexStart << cuT( ")" ) );
		DoConvertDataStructures( p_pMesh, l_meshHeader );
		delete [] m_skins;
		delete [] m_texCoords;
		delete [] m_triangles;
		delete [] m_vertices;
		l_meshOffset += l_meshHeader.m_meshSize;
	}

	p_pMesh->ComputeContainers();
	p_pMesh->ComputeNormals();
	String l_strFileName = m_pFile->GetFileFullPath();
	l_strFileName = l_strFileName.substr( 0, l_strFileName.find_last_of( '.' ) );
	DoLoadSkin( p_scene, Path{ l_strFileName + cuT( ".skin" ) } );
	DoLoadShader( p_scene, p_pMesh, Path{ l_strFileName + cuT( ".shader" ) } );
	delete [] m_links;
}

void Md3Importer::DoConvertDataStructures( MeshSPtr p_pMesh, Md3MeshInfo p_meshHeader )
{
	BufferElementGroupSPtr l_pVertex;
	SubmeshSPtr l_pSubmesh = p_pMesh->CreateSubmesh();
	m_mapSubmeshesByName.insert( std::make_pair( string::string_cast< xchar >( p_meshHeader.m_strName ), l_pSubmesh ) );

	for ( int i = 0 ; i < p_meshHeader.m_numVertices ; i++ )
	{
		l_pVertex = l_pSubmesh->AddPoint( m_vertices[i].m_vertex[0] / 64.0f, m_vertices[i].m_vertex[1] / 64.0f, m_vertices[i].m_vertex[2] / 64.0f );
		Vertex::SetTexCoord( l_pVertex, m_texCoords[i].m_textureCoord[0], 1 - m_texCoords[i].m_textureCoord[1] );
	}

	for ( int i = 0 ; i < p_meshHeader.m_numTriangles ; i++ )
	{
		if ( m_triangles[i].m_vertexIndices[0] >= 0 && m_triangles[i].m_vertexIndices[1] >= 0 && m_triangles[i].m_vertexIndices[2] >= 0 )
		{
			l_pSubmesh->AddFace( m_triangles[i].m_vertexIndices[0], m_triangles[i].m_vertexIndices[1], m_triangles[i].m_vertexIndices[2] );
		}
	}

	//for (int i = 0 ; i < p_meshHeader.m_numTriangles ; i++)
	//{
	//	if (m_triangles[i].m_vertexIndices[0] >= 0 && m_triangles[i].m_vertexIndices[1] >= 0 && m_triangles[i].m_vertexIndices[2] >= 0)
	//	{
	//		l_pSubmesh->AddPoint( m_vertices[m_triangles[i].m_vertexIndices[0]].m_vertex[0] / 64.0f, m_vertices[m_triangles[i].m_vertexIndices[0]].m_vertex[1] / 64.0f, m_vertices[m_triangles[i].m_vertexIndices[0]].m_vertex[2] / 64.0f )->SetTexCoord( real( m_texCoords[m_triangles[i].m_textureIndices[0]].u ), real( 1.0 ) - real( m_texCoords[m_triangles[i].m_textureIndices[0]].v ) ) );
	//		l_pSubmesh->AddPoint( m_vertices[m_triangles[i].m_vertexIndices[1]].m_vertex[0] / 64.0f, m_vertices[m_triangles[i].m_vertexIndices[1]].m_vertex[1] / 64.0f, m_vertices[m_triangles[i].m_vertexIndices[1]].m_vertex[2] / 64.0f )->SetTexCoord( real( m_texCoords[m_triangles[i].m_textureIndices[1]].u ), real( 1.0 ) - real( m_texCoords[m_triangles[i].m_textureIndices[1]].v ) ) );
	//		l_pSubmesh->AddPoint( m_vertices[m_triangles[i].m_vertexIndices[2]].m_vertex[0] / 64.0f, m_vertices[m_triangles[i].m_vertexIndices[2]].m_vertex[1] / 64.0f, m_vertices[m_triangles[i].m_vertexIndices[2]].m_vertex[2] / 64.0f )->SetTexCoord( real( m_texCoords[m_triangles[i].m_textureIndices[2]].u ), real( 1.0 ) - real( m_texCoords[m_triangles[i].m_textureIndices[2]].v ) ) );
	//		l_pSubmesh->AddFace( i * 3 + 0, i * 3 + 1, i * 3 + 2 );
	//	}
	//}

	////Point2rArray l_texVerts;

	////for (int i = 0 ; i < p_meshHeader.m_numVertices ; i++)
	////{
	////	l_texVerts.push_back( Point2r( m_texCoords[i].m_textureCoord[0], 1-m_texCoords[i].m_textureCoord[1] ) );
	////}

	////std::vector< FaceIndices > l_arrayFaces( p_meshHeader.m_numTriangles );
	////std::vector< float > l_arrayTex;

	////for (int i = 0 ; i < p_meshHeader.m_numTriangles ; i++)
	////{
	////	if (m_triangles[i].m_vertexIndices[0] >= 0 && m_triangles[i].m_vertexIndices[1] >= 0 && m_triangles[i].m_vertexIndices[2] >= 0)
	////	{
	////		l_arrayFaces[i].m_uiVertexIndex[0] = m_triangles[i].m_vertexIndices[0];
	////		l_arrayFaces[i].m_uiVertexIndex[1] = m_triangles[i].m_vertexIndices[1];
	////		l_arrayFaces[i].m_uiVertexIndex[2] = m_triangles[i].m_vertexIndices[2];
	////		l_arrayTex.push_back( l_texVerts[m_triangles[i].m_vertexIndices[0]][0] );
	////		l_arrayTex.push_back( l_texVerts[m_triangles[i].m_vertexIndices[0]][1] );
	////		l_arrayTex.push_back( l_texVerts[m_triangles[i].m_vertexIndices[1]][0] );
	////		l_arrayTex.push_back( l_texVerts[m_triangles[i].m_vertexIndices[1]][1] );
	////		l_arrayTex.push_back( l_texVerts[m_triangles[i].m_vertexIndices[2]][0] );
	////		l_arrayTex.push_back( l_texVerts[m_triangles[i].m_vertexIndices[2]][1] );
	////	}
	////}

	////l_pSubmesh->AddFaceGroup( &l_arrayFaces[0], p_meshHeader.m_numTriangles, NULL, &l_arrayTex[0] );
}

bool Md3Importer::DoLoadSkin( Scene & p_scene, Path const & p_strSkin )
{
	bool l_bReturn = false;

	if ( File::FileExists( p_strSkin ) )
	{
		TextFile l_fileIO( p_strSkin, File::eOPEN_MODE_READ );
		String l_strLine, l_strSection, l_strImage;

		while ( l_fileIO.IsOk() )
		{
			l_fileIO.ReadLine( l_strLine, 255 );
			l_strSection.clear();

			if ( ! l_strLine.empty() && l_strLine.find_last_of( ',' ) != String::npos )
			{
				l_strSection = l_strLine.substr( 0, l_strLine.find_last_of( ',' ) );
			}

			if ( m_mapSubmeshesByName.find( l_strSection ) != m_mapSubmeshesByName.end() )
			{
				l_strImage = l_strLine.substr( l_strLine.find_last_of( ',' ) + 1 );
				l_strImage = l_strImage.substr( l_strImage.find_last_of( '/' ) + 1 );
				MaterialSPtr l_material = p_scene.GetMaterialView().Find( l_strSection );

				if ( ! l_material )
				{
					l_material = p_scene.GetMaterialView().Create( l_strSection, *GetEngine() );
					l_material->CreatePass();
				}

				PassSPtr l_pass = l_material->GetPass( 0 );
				l_pass->SetAmbient( Castor::Colour::from_components( 0.0f, 0.0f, 0.0f, 1.0f ) );
				l_pass->SetEmissive( Castor::Colour::from_components( 0.5f, 0.5f, 0.5f, 1.0f ) );

				if ( !l_strImage.empty() )
				{
					LoadTexture( Path{ l_strImage }, *l_pass, TextureChannel::Diffuse );
				}

				m_mapSubmeshesByName.find( l_strSection )->second->SetDefaultMaterial( l_material );
				GetEngine()->PostEvent( std::make_shared< InitialiseEvent< Material > >( *l_material ) );
			}
		}

		l_bReturn = true;
	}

	return l_bReturn;
}

bool Md3Importer::DoLoadShader( Scene & p_scene, MeshSPtr p_pMesh, Path const & p_strShader )
{
	bool l_bReturn = false;

	if ( File::FileExists( p_strShader ) )
	{
		TextFile l_fileIO( p_strShader, File::eOPEN_MODE_READ );
		String l_strName = l_fileIO.GetFileName().substr( 0, l_fileIO.GetFileName().find_last_of( '.' ) );
		String l_strLine;
		uint32_t l_uiIndex = 0;
		StringStream l_strMatName;

		while ( l_fileIO.IsOk() && l_uiIndex < p_pMesh->GetSubmeshCount() )
		{
			l_fileIO.ReadLine( l_strLine, 255 );
			StringStream l_strMatName( l_strName );
			l_strMatName << "_" << l_uiIndex;
			MaterialSPtr l_material = p_scene.GetMaterialView().Find( l_strMatName.str() );

			if ( ! l_material )
			{
				l_material = p_scene.GetMaterialView().Create( l_strMatName.str(), *GetEngine() );
				l_material->CreatePass();
			}

			PassSPtr l_pass = l_material->GetPass( 0 );
			l_pass->SetAmbient( Castor::Colour::from_components( 0.0f, 0.0f, 0.0f, 1.0f ) );
			l_pass->SetEmissive( Castor::Colour::from_components( 0.5f, 0.5f, 0.5f, 1.0f ) );
			l_pass->SetTwoSided( true );

			if ( !l_strLine.empty() )
			{
				LoadTexture( Path{ l_strLine }, *l_pass, TextureChannel::Diffuse );
			}

			p_pMesh->GetSubmesh( l_uiIndex )->SetDefaultMaterial( l_material );
			GetEngine()->PostEvent( std::make_shared< InitialiseEvent< Material > >( *l_material ) );
			l_uiIndex++;
		}

		l_bReturn = true;
	}

	return l_bReturn;
}

void Md3Importer::DoCleanUp()
{
}
