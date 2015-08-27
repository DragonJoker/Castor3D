#include <Material.hpp>
#include <Pass.hpp>
#include <TextureUnit.hpp>
#include <RenderSystem.hpp>
#include <Mesh.hpp>
#include <Submesh.hpp>
#include <Face.hpp>
#include <Geometry.hpp>
#include <Version.hpp>
#include <Engine.hpp>
#include <Scene.hpp>
#include <SceneNode.hpp>
#include <Plugin.hpp>
#include <StaticTexture.hpp>
#include <Vertex.hpp>
#include <InitialiseEvent.hpp>

#include "Md2Importer.hpp"

using namespace Castor3D;
using namespace Castor;

//*************************************************************************************************

Md2Importer::Md2Importer( Engine * p_pEngine, String const & p_textureName )
	:	Importer		( p_pEngine													)
	,	m_textureName	( p_textureName												)
	,	m_skins			( nullptr													)
	,	m_texCoords		( nullptr													)
	,	m_triangles		( nullptr													)
	,	m_frames		( nullptr													)
	,	m_pFile			( nullptr													)
{
	memset( & m_header, 0, sizeof( Md2Header ) );
}

SceneSPtr Md2Importer::DoImportScene()
{
	MeshSPtr l_pMesh = DoImportMesh();
	SceneSPtr l_pScene;

	if ( l_pMesh )
	{
		l_pMesh->GenerateBuffers();
		l_pScene = m_pEngine->CreateScene( cuT( "Scene_MD2" ) );
		SceneNodeSPtr l_pNode = l_pScene->CreateSceneNode( l_pMesh->GetName(), l_pScene->GetObjectRootNode() );
		GeometrySPtr l_pGeometry = l_pScene->CreateGeometry( l_pMesh->GetName() );
		l_pGeometry->AttachTo( l_pNode.get() );
		l_pGeometry->SetMesh( l_pMesh );
	}

	return l_pScene;
}

MeshSPtr Md2Importer::DoImportMesh()
{
	UIntArray		l_faces;
	RealArray		l_sizes;
	SubmeshSPtr		l_submesh;
	MaterialSPtr	l_material;
	PassSPtr		l_pass;
	MeshSPtr		l_pMesh;
	String			l_meshName;
	String			l_materialName;
	TextureUnitSPtr	l_unit;
	ImageSPtr		l_pImage;
	m_pFile			= new BinaryFile( m_fileName, File::eOPEN_MODE_READ );
	l_meshName		= m_fileName.GetFileName();
	l_materialName	= m_fileName.GetFileName();
	l_pMesh			= m_pEngine->CreateMesh( eMESH_TYPE_CUSTOM, l_meshName, l_faces, l_sizes );
	m_pFile->Read( m_header );

	if ( m_header.m_version != 8 )
	{
		Logger::LogMessage( cuT( "Invalid file format (Version not 8): " ) + m_fileName );
	}
	else
	{
		l_material = m_pEngine->GetMaterialManager().find( l_materialName );

		if ( !l_material )
		{
			l_material = std::make_shared< Material >( m_pEngine, l_materialName );
			l_material->CreatePass();
			m_pEngine->GetMaterialManager().insert( l_materialName, l_material );
		}

		l_pass = l_material->GetPass( 0 );
		l_pass->SetAmbient( Castor::Colour::from_components( 0.0f, 0.0f, 0.0f, 1.0f ) );
		l_pass->SetEmissive( Castor::Colour::from_components( 0.5f, 0.5f, 0.5f, 1.0f ) );
		l_pass->SetShininess( 64.0f );
		DoReadMD2Data( l_pass );
		DoConvertDataStructures( l_pMesh );
		std::for_each( l_pMesh->Begin(), l_pMesh->End(), [&]( SubmeshSPtr p_pSubmesh )
		{
			p_pSubmesh->SetDefaultMaterial( l_material );
		} );
		l_pMesh->ComputeNormals();
		m_pEngine->PostEvent( std::make_shared< InitialiseEvent< Material > >( *l_material ) );
	}

	DoCleanUp();
	return l_pMesh;
}

void Md2Importer::DoReadMD2Data( PassSPtr p_pPass )
{
	uint64_t l_uiRead;
	TextureUnitSPtr l_pTexture;
	ImageSPtr l_pImage;
	Logger::LogDebug( cuT( "MD2 File - size : %d" ), m_pFile->GetLength() );
	Logger::LogDebug( cuT( "- Vertices : %d" ), m_header.m_numVertices );
	Logger::LogDebug( cuT( "- Skins : %d" ), m_header.m_numSkins );

	if ( m_header.m_numSkins > 0 )
	{
		m_skins = new Md2Skin[m_header.m_numSkins];
		m_pFile->Seek( m_header.m_offsetSkins );
		l_uiRead = m_pFile->ReadArray( m_skins, m_header.m_numSkins );
		Logger::LogDebug( cuT( "- Skins : %d/%d (%d bytes, from %d)" ), ( l_uiRead / sizeof( Md2Skin ) ), m_header.m_numSkins, l_uiRead, m_header.m_offsetSkins );

		for ( int i = 0 ; i < m_header.m_numSkins && !l_pTexture ; i++ )
		{
			String l_strValue = str_utils::from_str( m_skins[i] );

			if ( !l_strValue.empty() )
			{
				String l_strPath = m_filePath / l_strValue;
				l_pImage = m_pEngine->GetImageManager().find( l_strValue );

				if ( !l_pImage )
				{
					if ( !File::FileExists( l_strPath ) )
					{
						l_strPath = m_filePath / cuT( "Texture" ) / l_strValue;
					}

					if ( File::FileExists( l_strPath ) )
					{
						l_pImage = std::make_shared< Image >( l_strValue, l_strPath );
						m_pEngine->GetImageManager().insert( l_strValue, l_pImage );
					}
				}
			}

			if ( l_pImage && p_pPass )
			{
				l_pTexture = p_pPass->AddTextureUnit();
				StaticTextureSPtr l_pStaTexture = m_pEngine->GetRenderSystem()->CreateStaticTexture();
				l_pStaTexture->SetDimension( eTEXTURE_DIMENSION_2D );
				l_pStaTexture->SetImage( l_pImage->GetPixels() );
				l_pTexture->SetTexture( l_pStaTexture );
				l_pTexture->SetChannel( eTEXTURE_CHANNEL_DIFFUSE );
				Logger::LogDebug( cuT( "- Texture found : " ) + l_strValue );
			}
			else
			{
				Logger::LogDebug( cuT( "- Texture not found : " ) + l_strValue );
			}
		}
	}

	if ( m_header.m_numTexCoords > 0 )
	{
		m_texCoords = new Md2TexCoord[m_header.m_numTexCoords];
		m_pFile->Seek( m_header.m_offsetTexCoords );
		l_uiRead = m_pFile->ReadArray( m_texCoords, m_header.m_numTexCoords );
		Logger::LogDebug( cuT( "- TexCoords : %d/%d (%d bytes, from %d)" ), ( l_uiRead / sizeof( Md2TexCoord ) ), m_header.m_numTexCoords, l_uiRead, m_header.m_offsetTexCoords );
	}

	if ( m_header.m_numTriangles > 0 )
	{
		m_triangles = new Md2Face[m_header.m_numTriangles];
		m_pFile->Seek( m_header.m_offsetTriangles );
		l_uiRead = m_pFile->ReadArray( m_triangles, m_header.m_numTriangles );
		Logger::LogDebug( cuT( "- Triangles : %d/%d (%d bytes, from %d)" ), ( l_uiRead / sizeof( Md2Face ) ), m_header.m_numTriangles, l_uiRead, m_header.m_offsetTriangles );
	}

	if ( m_header.m_numFrames > 0 )
	{
		uint8_t l_buffer[MD2_MAX_FRAMESIZE];
		Logger::LogDebug( cuT( "- Frames : %d" ), m_header.m_numFrames );
		m_pFile->Seek( m_header.m_offsetFrames );
		m_frames				= new Md2Frame[m_header.m_numFrames];
		Md2AliasFrame * l_frame	= reinterpret_cast< Md2AliasFrame * >( l_buffer );
		m_frames[0].m_vertices	= new Md2Vertex[m_header.m_numVertices];
		m_pFile->ReadArray< char >( ( char * )l_frame, m_header.m_frameSize );
		m_frames[0].m_strName = l_frame->m_name;
		Md2Vertex * l_vertices = m_frames[0].m_vertices;

		for ( int j = 0 ; j < m_header.m_numVertices; j++ )
		{
			l_vertices[j].m_coords[0] = ( l_frame->m_aliasVertices[j].m_vertex[0] * l_frame->m_scale[0] + l_frame->m_translate[0] ) *  1;
			l_vertices[j].m_coords[2] = ( l_frame->m_aliasVertices[j].m_vertex[1] * l_frame->m_scale[1] + l_frame->m_translate[1] ) * -1;
			l_vertices[j].m_coords[1] = ( l_frame->m_aliasVertices[j].m_vertex[2] * l_frame->m_scale[2] + l_frame->m_translate[2] ) *  1;
		}
	}
}

void Md2Importer::DoConvertDataStructures( MeshSPtr p_pMesh )
{
	SubmeshSPtr						l_pSubmesh = p_pMesh->CreateSubmesh();
	std::vector< float >			l_arrayNml( m_header.m_numTriangles * 9 );
	std::vector< stFACE_INDICES >	l_arrayFaces( m_header.m_numTriangles );
	BufferElementGroupSPtr			l_pVertex;

	if ( m_header.m_numFrames > 0 )
	{
		/*
				for (int i = 0 ; i < m_header.m_numVertices ; i++)
				{
					l_pVertex = l_pSubmesh->AddPoint( m_frames[0].m_vertices[i].m_coords );
				}

				for (int i = 0 ; i < m_header.m_numTriangles ; i++)
				{
					if (m_triangles[i].m_vertexIndices[0] >= 0 && m_triangles[i].m_vertexIndices[1] >= 0 && m_triangles[i].m_vertexIndices[2] >= 0)
					{
						l_pSubmesh->GetPoint( m_triangles[i].m_vertexIndices[0] )->SetTexCoord( real( m_texCoords[m_triangles[i].m_textureIndices[0]].u ) / real( m_header.m_skinWidth ), real( 1.0 ) - real( m_texCoords[m_triangles[i].m_textureIndices[0]].v ) / real( m_header.m_skinHeight ) );
						l_pSubmesh->GetPoint( m_triangles[i].m_vertexIndices[1] )->SetTexCoord( real( m_texCoords[m_triangles[i].m_textureIndices[1]].u ) / real( m_header.m_skinWidth ), real( 1.0 ) - real( m_texCoords[m_triangles[i].m_textureIndices[1]].v ) / real( m_header.m_skinHeight ) );
						l_pSubmesh->GetPoint( m_triangles[i].m_vertexIndices[2] )->SetTexCoord( real( m_texCoords[m_triangles[i].m_textureIndices[2]].u ) / real( m_header.m_skinWidth ), real( 1.0 ) - real( m_texCoords[m_triangles[i].m_textureIndices[2]].v ) / real( m_header.m_skinHeight ) );
						l_pSubmesh->AddFace( m_triangles[i].m_vertexIndices[0], m_triangles[i].m_vertexIndices[1], m_triangles[i].m_vertexIndices[2] );
					}
				}
		*/
		for ( int i = 0 ; i < m_header.m_numTriangles ; i++ )
		{
			if ( m_triangles[i].m_vertexIndices[0] >= 0 && m_triangles[i].m_vertexIndices[1] >= 0 && m_triangles[i].m_vertexIndices[2] >= 0 )
			{
				Vertex::SetTexCoord( l_pSubmesh->AddPoint( m_frames[0].m_vertices[m_triangles[i].m_vertexIndices[0]].m_coords ), real( m_texCoords[m_triangles[i].m_textureIndices[0]].u ) / real( m_header.m_skinWidth ), real( 1.0 ) - real( m_texCoords[m_triangles[i].m_textureIndices[0]].v ) / real( m_header.m_skinHeight ) );
				Vertex::SetTexCoord( l_pSubmesh->AddPoint( m_frames[0].m_vertices[m_triangles[i].m_vertexIndices[1]].m_coords ), real( m_texCoords[m_triangles[i].m_textureIndices[1]].u ) / real( m_header.m_skinWidth ), real( 1.0 ) - real( m_texCoords[m_triangles[i].m_textureIndices[1]].v ) / real( m_header.m_skinHeight ) );
				Vertex::SetTexCoord( l_pSubmesh->AddPoint( m_frames[0].m_vertices[m_triangles[i].m_vertexIndices[2]].m_coords ), real( m_texCoords[m_triangles[i].m_textureIndices[2]].u ) / real( m_header.m_skinWidth ), real( 1.0 ) - real( m_texCoords[m_triangles[i].m_textureIndices[2]].v ) / real( m_header.m_skinHeight ) );
				l_pSubmesh->AddFace( i * 3 + 0, i * 3 + 1, i * 3 + 2 );
			}
		}

		delete [] m_frames[0].m_vertices;
		/*
				for (int i = 0 ; i < m_header.m_numTriangles ; i++)
				{
					if (m_triangles[i].m_vertexIndices[0] >= 0 && m_triangles[i].m_vertexIndices[1] >= 0 && m_triangles[i].m_vertexIndices[2] >= 0)
					{
						l_arrayFaces[i].m_uiVertexIndex[0] = m_triangles[i].m_vertexIndices[0];
						l_arrayFaces[i].m_uiVertexIndex[1] = m_triangles[i].m_vertexIndices[1];
						l_arrayFaces[i].m_uiVertexIndex[2] = m_triangles[i].m_vertexIndices[2];
						l_arrayTex.push_back( float( m_texCoords[m_triangles[i].m_textureIndices[0]].u ) / float( m_header.m_skinWidth ) );
						l_arrayTex.push_back( 1-float( m_texCoords[m_triangles[i].m_textureIndices[0]].v ) / float( m_header.m_skinHeight ) );
						l_arrayTex.push_back( float( m_texCoords[m_triangles[i].m_textureIndices[1]].u ) / float( m_header.m_skinWidth ) );
						l_arrayTex.push_back( 1-float( m_texCoords[m_triangles[i].m_textureIndices[1]].v ) / float( m_header.m_skinHeight ) );
						l_arrayTex.push_back( float( m_texCoords[m_triangles[i].m_textureIndices[2]].u ) / float( m_header.m_skinWidth ) );
						l_arrayTex.push_back( 1-float( m_texCoords[m_triangles[i].m_textureIndices[2]].v ) / float( m_header.m_skinHeight ) );
					}
				}

				l_pSubmesh->AddFaceGroup( &l_arrayFaces[0], m_header.m_numTriangles, NULL, &l_arrayTex[0] );
		/**/
	}

	p_pMesh->ComputeContainers();
}

void Md2Importer::DoCleanUp()
{
	delete m_pFile;
	delete [] m_skins;
	delete m_texCoords;
	delete m_triangles;
	delete m_frames;
	m_pFile		= NULL;
	m_skins		= NULL;
	m_texCoords	= NULL;
	m_triangles	= NULL;
	m_frames	= NULL;
}
