#include <Castor3D/Prerequisites.hpp>

#include <Castor3D/Material.hpp>
#include <Castor3D/Pass.hpp>
#include <Castor3D/TextureUnit.hpp>
#include <Castor3D/RenderSystem.hpp>
#include <Castor3D/Mesh.hpp>
#include <Castor3D/Submesh.hpp>
#include <Castor3D/Face.hpp>
#include <Castor3D/Geometry.hpp>
#include <Castor3D/Version.hpp>
#include <Castor3D/Scene.hpp>
#include <Castor3D/SceneNode.hpp>
#include <Castor3D/Plugin.hpp>
#include <Castor3D/Engine.hpp>
#include <Castor3D/Texture.hpp>
#include <Castor3D/Vertex.hpp>

#include "Md3Importer/Md3Importer.hpp"

using namespace Castor3D;
using namespace Castor;

//*************************************************************************************************

C3D_Md3_API void GetRequiredVersion( Version & p_version)
{
	p_version = Version();
}

C3D_Md3_API ePLUGIN_TYPE GetType()
{
	return ePLUGIN_TYPE_IMPORTER;
}

C3D_Md3_API String GetName()
{
	return cuT( "MD3 Importer");
}

C3D_Md3_API ImporterPlugin::ExtensionArray GetExtensions()
{
	ImporterPlugin::ExtensionArray l_arrayReturn;
//	ImporterPlugin::Extension l_extension( cuT( "MD3" ), cuT( "Quake III" ) );
//	l_arrayReturn.push_back( l_extension );
	return l_arrayReturn;
}

C3D_Md3_API void Create( Engine * p_pEngine, ImporterPlugin * p_pPlugin )
{
	Logger::Initialise( p_pEngine->GetLoggerInstance() );
	ImporterPtr l_pImporter = std::make_shared< Md3Importer >( p_pEngine );
	p_pPlugin->AttachImporter( l_pImporter );
}

C3D_Md3_API void Destroy( ImporterPlugin * p_pPlugin )
{
	p_pPlugin->DetachImporter();
	Logger::Cleanup();
}

//*************************************************************************************************

bool _isInString( String const & p_strString, String const & p_strSubString)
{
	if (p_strString.length() <= 0 || p_strSubString.length() <= 0)
	{
		return false;
	}

	return p_strString.find( p_strSubString) != String::npos;
}

Md3Importer :: Md3Importer( Engine * p_pEngine )
	:	Importer( p_pEngine )
	,	m_skins		( NULL )
	,	m_texCoords	( NULL )
	,	m_triangles	( NULL )
	,	m_bones		( NULL )
	,	m_pFile		( NULL )
{
	memset( & m_header, 0, sizeof( Md3Header));
}

ScenePtr Md3Importer :: DoImportScene()
{
	MeshPtr l_pMesh = DoImportMesh();

	if( l_pMesh )
	{
		l_pMesh->GenerateBuffers();
		m_pScene = m_pEngine->CreateScene( cuT( "Scene_MD3" ) );
		SceneNodePtr l_pNode = m_pScene->CreateSceneNode( l_pMesh->GetName(), m_pScene->GetObjectRootNode() );
		GeometryPtr l_pGeometry = m_pScene->CreateGeometry(l_pMesh->GetName() );
		l_pGeometry->AttachTo( l_pNode.get() );
		l_pGeometry->SetMesh( l_pMesh );
	}

	return m_pScene;
}

MeshPtr Md3Importer :: DoImportMesh()
{
	m_pFile = new BinaryFile( m_fileName, File::eOPEN_MODE_READ );
	m_pScene = m_pEngine->CreateScene( cuT( "Scene_MD3" ) );

	UIntArray l_faces;
	RealArray l_sizes;
	MaterialPtr		l_material;
	PassPtr			l_pass;
	String l_meshName = m_fileName.GetFileName();
	String l_materialName	= m_fileName.GetFileName();

	MeshPtr l_pMesh = m_pEngine->CreateMesh( eMESH_TYPE_CUSTOM, l_meshName, l_faces, l_sizes);

	m_pFile->Read( m_header );

	char * l_id = m_header.m_fileID;

	if( (l_id[0] != 'I' || l_id[1] != 'D' || l_id[2] != 'P' || l_id[3] != '3') || m_header.m_version != 15 )
	{
	}
	else
	{
		l_material = m_pEngine->GetMaterialManager()->find( l_materialName);

		if( !l_material )
		{
			l_material = std::make_shared< Material >( m_pEngine, l_materialName, 1 );
			m_pEngine->GetMaterialManager()->insert( l_materialName, l_material );
		}

		l_pass = l_material->GetPass( 0 );
		l_pass->SetAmbient( 0.0f, 0.0f, 0.0f, 1.0f );
		l_pass->SetEmissive( 0.5f, 0.5f, 0.5f, 1.0f );
		l_pass->SetShininess( 64.0f );

		DoReadMD3Data( l_pMesh, l_pass );
		
		std::for_each( l_pMesh->Begin(), l_pMesh->End(), [&]( SubmeshPtr p_pSubmesh )
		{
			p_pSubmesh->SetMaterial( l_material);
		} );

		l_pMesh->ComputeNormals();
		m_pEngine->GetMaterialManager()->SetToInitialise( l_material );

		DoCleanUp();
	}

	delete m_pFile;
	return l_pMesh;
}

void Md3Importer :: DoReadMD3Data( MeshPtr p_pMesh, PassPtr p_pPass )
{
	int i = 0;
	std::size_t l_uiRead;
	TextureUnitPtr l_pTexture;
	ImagePtr l_pImage;

	Logger::LogMessage( cuT( "MD3 File : size %d" ), m_pFile->GetLength() );

	if( m_header.m_numFrames > 0 )
	{
		m_bones = new Md3Bone[m_header.m_numFrames];
		l_uiRead = m_pFile->ReadArray( m_bones, m_header.m_numFrames );
		Logger::LogMessage( cuT( "* Bones : %d/%d (%d bytes)" ), (l_uiRead / sizeof( Md3Bone )), m_header.m_numFrames, l_uiRead );
		delete [] m_bones;
	}

	if( m_header.m_numTags > 0 )
	{
		m_tags = new Md3Tag[m_header.m_numFrames * m_header.m_numTags];
		l_uiRead = m_pFile->ReadArray( m_tags, m_header.m_numFrames * m_header.m_numTags);
		Logger::LogMessage( cuT( "* Tags : %d/%d (%d bytes)" ), (l_uiRead / sizeof( Md3Tag )), m_header.m_numTags, l_uiRead );
		m_numOfTags = m_header.m_numTags;
	}

	m_links = new MeshPtr[m_header.m_numTags];
// 	m_links = (MeshPtr *) malloc( sizeof( Mesh) * m_header.m_numTags);
//
// 	for (i = 0 ; i < m_header.m_numTags ; i++)
// 	{
// 		m_links[i].reset();
// 	}

	long long l_meshOffset = m_pFile->Tell();
	String l_strFileName = m_pFile->GetFileFullPath();
	l_strFileName = l_strFileName.substr( 0, l_strFileName.find_last_of( '.'));

	Md3MeshInfo l_meshHeader = { 0 };

	for (i = 0; i < m_header.m_numMeshes; i++)
	{
		Logger::LogMessage( cuT( "* Submesh : %d/%d (from %d)" ), (i+1), m_header.m_numMeshes, l_meshOffset );
		m_pFile->Seek( l_meshOffset );
		m_pFile->Read( l_meshHeader );

		m_skins = new Md3Skin[l_meshHeader.m_numSkins];
		l_uiRead = m_pFile->ReadArray( m_skins, l_meshHeader.m_numSkins);
		Logger::LogMessage( cuT( "* * Skins : %d/%d (%d bytes)" ), (l_uiRead / sizeof( Md3Skin )), l_meshHeader.m_numSkins, l_uiRead );

		for( int i = 0 ; i < l_meshHeader.m_numSkins && !l_pTexture ; i++ )
		{
			String l_strValue = Str::from_str( m_skins[i].m_strName );

			if( !l_strValue.empty() )
			{
				String l_strPath = m_filePath / l_strValue;
				l_pImage = m_pEngine->GetImageManager()->find( l_strValue );

				if( !l_pImage )
				{
					if( !File::FileExists( l_strPath ) )
					{
						l_strPath = m_filePath / cuT( "Texture" ) / l_strValue;
					}

					if( File::FileExists( l_strPath ) )
					{
						l_pImage = std::make_shared< Image >( l_strValue, l_strPath );
						m_pEngine->GetImageManager()->insert( l_strValue, l_pImage );
					}
				}
			}

			if( l_pImage && p_pPass )
			{
				l_pTexture = p_pPass->AddTextureUnit();
				StaticTexturePtr l_pStaTexture = m_pEngine->GetRenderSystem()->CreateStaticTexture();
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

		m_triangles = new Md3Face[l_meshHeader.m_numTriangles];
		m_pFile->Seek( l_meshOffset + l_meshHeader.m_triStart );
		l_uiRead = m_pFile->ReadArray( m_triangles, l_meshHeader.m_numTriangles );
		Logger::LogMessage( cuT( "* * Triangles : %d/%d (%d bytes, from %d)" ), (l_uiRead / sizeof( Md3Face )), l_meshHeader.m_numTriangles, l_uiRead, l_meshHeader.m_triStart );

		m_texCoords = new Md3TexCoord[l_meshHeader.m_numVertices];
		m_pFile->Seek( l_meshOffset + l_meshHeader.m_uvStart );
		l_uiRead = m_pFile->ReadArray( m_texCoords, l_meshHeader.m_numVertices );
		Logger::LogMessage( cuT( "* * TexCoords : %d/%d (%d bytes, from %d)" ), (l_uiRead / sizeof( Md3TexCoord )), l_meshHeader.m_numVertices, l_uiRead, l_meshHeader.m_uvStart );

		m_vertices  = new Md3Triangle[l_meshHeader.m_numVertices];
		m_pFile->Seek( l_meshOffset + l_meshHeader.m_vertexStart );
		l_uiRead = m_pFile->ReadArray( m_vertices, l_meshHeader.m_numVertices );
		Logger::LogMessage( cuT( "* * Vertices : %d/%d (%d bytes, from %d)" ), (l_uiRead / sizeof( Md3Triangle )), l_meshHeader.m_numVertices, l_uiRead, l_meshHeader.m_vertexStart );

		DoConvertDataStructures( p_pMesh, l_meshHeader );

		delete [] m_skins;
		delete [] m_texCoords;
		delete [] m_triangles;
		delete [] m_vertices;

		l_meshOffset += l_meshHeader.m_meshSize;
	}

	p_pMesh->ComputeContainers();
	p_pMesh->ComputeNormals();

	DoLoadSkin( l_strFileName + cuT( ".skin" ) );
	DoLoadShader( p_pMesh, l_strFileName + cuT( ".shader" ) );

	delete [] m_links;
}

void Md3Importer :: DoConvertDataStructures( MeshPtr p_pMesh, Md3MeshInfo p_meshHeader)
{
	VertexPtr l_pVertex;
	SubmeshPtr l_pSubmesh = p_pMesh->CreateSubmesh();
	m_mapSubmeshesByName.insert( std::make_pair( Str::from_str( p_meshHeader.m_strName ), l_pSubmesh));

	for (int i = 0 ; i < p_meshHeader.m_numVertices ; i++)
	{
		l_pVertex = l_pSubmesh->AddPoint( m_vertices[i].m_vertex[0] / 64.0f, m_vertices[i].m_vertex[1] / 64.0f, m_vertices[i].m_vertex[2] / 64.0f);
		l_pVertex->SetTexCoord( m_texCoords[i].m_textureCoord[0], 1 - m_texCoords[i].m_textureCoord[1] );
	}

	for (int i = 0 ; i < p_meshHeader.m_numTriangles ; i++)
	{
		if (m_triangles[i].m_vertexIndices[0] >= 0 && m_triangles[i].m_vertexIndices[1] >= 0 && m_triangles[i].m_vertexIndices[2] >= 0)
		{
			l_pSubmesh->AddFace( m_triangles[i].m_vertexIndices[0], m_triangles[i].m_vertexIndices[1], m_triangles[i].m_vertexIndices[2] );
		}
	}
/*
	for (int i = 0 ; i < p_meshHeader.m_numTriangles ; i++)
	{
		if (m_triangles[i].m_vertexIndices[0] >= 0 && m_triangles[i].m_vertexIndices[1] >= 0 && m_triangles[i].m_vertexIndices[2] >= 0)
		{
			l_pSubmesh->AddPoint( m_vertices[m_triangles[i].m_vertexIndices[0]].m_vertex[0] / 64.0f, m_vertices[m_triangles[i].m_vertexIndices[0]].m_vertex[1] / 64.0f, m_vertices[m_triangles[i].m_vertexIndices[0]].m_vertex[2] / 64.0f )->SetTexCoord( real( m_texCoords[m_triangles[i].m_textureIndices[0]].u ), real( 1.0 ) - real( m_texCoords[m_triangles[i].m_textureIndices[0]].v ) ) );
			l_pSubmesh->AddPoint( m_vertices[m_triangles[i].m_vertexIndices[1]].m_vertex[0] / 64.0f, m_vertices[m_triangles[i].m_vertexIndices[1]].m_vertex[1] / 64.0f, m_vertices[m_triangles[i].m_vertexIndices[1]].m_vertex[2] / 64.0f )->SetTexCoord( real( m_texCoords[m_triangles[i].m_textureIndices[1]].u ), real( 1.0 ) - real( m_texCoords[m_triangles[i].m_textureIndices[1]].v ) ) );
			l_pSubmesh->AddPoint( m_vertices[m_triangles[i].m_vertexIndices[2]].m_vertex[0] / 64.0f, m_vertices[m_triangles[i].m_vertexIndices[2]].m_vertex[1] / 64.0f, m_vertices[m_triangles[i].m_vertexIndices[2]].m_vertex[2] / 64.0f )->SetTexCoord( real( m_texCoords[m_triangles[i].m_textureIndices[2]].u ), real( 1.0 ) - real( m_texCoords[m_triangles[i].m_textureIndices[2]].v ) ) );
			l_pSubmesh->AddFace( i * 3 + 0, i * 3 + 1, i * 3 + 2 );
		}
	}
/*
	Point2rArray l_texVerts;

	for (int i = 0 ; i < p_meshHeader.m_numVertices ; i++)
	{
		l_texVerts.push_back( Point2r( m_texCoords[i].m_textureCoord[0], 1-m_texCoords[i].m_textureCoord[1] ) );
	}

	std::vector< stFACE_INDICES >	l_arrayFaces( p_meshHeader.m_numTriangles );
	std::vector< float >			l_arrayTex;
	for (int i = 0 ; i < p_meshHeader.m_numTriangles ; i++)
	{
		if (m_triangles[i].m_vertexIndices[0] >= 0 && m_triangles[i].m_vertexIndices[1] >= 0 && m_triangles[i].m_vertexIndices[2] >= 0)
		{
			l_arrayFaces[i].m_uiVertexIndex[0] = m_triangles[i].m_vertexIndices[0];
			l_arrayFaces[i].m_uiVertexIndex[1] = m_triangles[i].m_vertexIndices[1];
			l_arrayFaces[i].m_uiVertexIndex[2] = m_triangles[i].m_vertexIndices[2];
			l_arrayTex.push_back( l_texVerts[m_triangles[i].m_vertexIndices[0]][0] );
			l_arrayTex.push_back( l_texVerts[m_triangles[i].m_vertexIndices[0]][1] );
			l_arrayTex.push_back( l_texVerts[m_triangles[i].m_vertexIndices[1]][0] );
			l_arrayTex.push_back( l_texVerts[m_triangles[i].m_vertexIndices[1]][1] );
			l_arrayTex.push_back( l_texVerts[m_triangles[i].m_vertexIndices[2]][0] );
			l_arrayTex.push_back( l_texVerts[m_triangles[i].m_vertexIndices[2]][1] );
		}
	}

	l_pSubmesh->AddFaceGroup( &l_arrayFaces[0], p_meshHeader.m_numTriangles, NULL, &l_arrayTex[0] );
/**/
}

bool Md3Importer :: DoLoadSkin( String const & p_strSkin)
{
	bool l_bReturn = false;

	if (File::FileExists( p_strSkin))
	{
		TextFile l_fileIO( p_strSkin, File::eOPEN_MODE_READ);
		String l_strLine, l_strSection, l_strImage;

		while (l_fileIO.IsOk())
		{
			l_fileIO.ReadLine( l_strLine, 255);
			l_strSection.clear();

			if ( ! l_strLine.empty() && l_strLine.find_last_of( ',') != String::npos)
			{
				l_strSection = l_strLine.substr( 0, l_strLine.find_last_of( ','));
			}

			if (m_mapSubmeshesByName.find( l_strSection) != m_mapSubmeshesByName.end())
			{
				l_strImage = l_strLine.substr( l_strLine.find_last_of( ',') + 1);
				l_strImage = l_strImage.substr( l_strImage.find_last_of( '/') + 1);

				MaterialPtr l_material = m_pEngine->GetMaterialManager()->find( l_strSection);
				if ( ! l_material)
				{
					l_material = std::make_shared< Material >( m_pEngine, l_strSection, 1 );
					m_pEngine->GetMaterialManager()->insert( l_strSection, l_material);
				}

				PassPtr l_pass = l_material->GetPass( 0);
				l_pass->SetAmbient( 0.0f, 0.0f, 0.0f, 1.0f);
				l_pass->SetEmissive( 0.5f, 0.5f, 0.5f, 1.0f);

				if ( ! l_strImage.empty())
				{
					TextureUnitPtr l_unit = l_pass->AddTextureUnit();

					if (File::FileExists( l_strImage))
					{
						Collection<Image, String> l_imgCollection;
						ImagePtr l_pImage = l_imgCollection.find( l_strImage);

						if ( ! l_pImage)
						{
							l_pImage = std::make_shared< Image >( l_strImage, l_strImage );
							l_imgCollection.insert( l_strImage, l_pImage);
						}

						StaticTexturePtr l_pStaTexture = m_pEngine->GetRenderSystem()->CreateStaticTexture();
						l_pStaTexture->SetDimension( eTEXTURE_DIMENSION_2D );
						l_pStaTexture->SetImage( l_pImage->GetPixels() );
						l_unit->SetTexture( l_pStaTexture );
					}
					else if (File::FileExists( l_fileIO.GetFilePath() / l_strImage))
					{
						ImagePtr l_pImage = m_pEngine->GetImageManager()->find( l_fileIO.GetFilePath() / l_strImage);

						if ( ! l_pImage)
						{
							l_pImage = std::make_shared< Image >( l_fileIO.GetFilePath() / l_strImage, l_fileIO.GetFilePath() / l_strImage );
							m_pEngine->GetImageManager()->insert( l_fileIO.GetFilePath() / l_strImage, l_pImage);
						}

						StaticTexturePtr l_pStaTexture = m_pEngine->GetRenderSystem()->CreateStaticTexture();
						l_pStaTexture->SetDimension( eTEXTURE_DIMENSION_2D );
						l_pStaTexture->SetImage( l_pImage->GetPixels() );
						l_unit->SetTexture( l_pStaTexture );
					}
					else
					{
						l_pass->DestroyTextureUnit( l_unit->GetIndex());
					}
				}

				m_mapSubmeshesByName.find( l_strSection)->second->SetMaterial( l_material);
				m_pEngine->GetMaterialManager()->SetToInitialise( l_material);
			}
		}

		l_bReturn = true;
	}

	return l_bReturn;
}

bool Md3Importer :: DoLoadShader( MeshPtr p_pMesh, String const & p_strShader)
{
	bool l_bReturn = false;

	if (File::FileExists( p_strShader))
	{
		TextFile l_fileIO( p_strShader, File::eOPEN_MODE_READ);
		String l_strName = l_fileIO.GetFileName().substr( 0, l_fileIO.GetFileName().find_last_of( '.'));
		String l_strLine;
		uint32_t l_uiIndex = 0;
		String l_strMatName;

		while (l_fileIO.IsOk() && l_uiIndex < p_pMesh->GetNbSubmeshes())
		{
			l_fileIO.ReadLine( l_strLine, 255);

			l_strMatName = l_strName;
			l_strMatName << "_" << l_uiIndex;

			MaterialPtr l_material = m_pEngine->GetMaterialManager()->find( l_strMatName);
			if ( ! l_material)
			{
				l_material = std::make_shared< Material >( m_pEngine, l_strMatName, 1 );
				m_pEngine->GetMaterialManager()->insert( l_strMatName, l_material);
			}

			PassPtr l_pass = l_material->GetPass( 0);
			l_pass->SetAmbient( 0.0f, 0.0f, 0.0f, 1.0f);
			l_pass->SetEmissive( 0.5f, 0.5f, 0.5f, 1.0f);
			l_pass->SetDoubleFace( true);

			if ( ! l_strLine.empty())
			{
				TextureUnitPtr l_unit = l_pass->AddTextureUnit();

				if (File::FileExists( l_strLine))
				{
					ImagePtr l_pImage = m_pEngine->GetImageManager()->find( l_strLine );

					if( !l_pImage )
					{
						l_pImage = std::make_shared< Image >( l_strLine, l_strLine );
						m_pEngine->GetImageManager()->insert( l_strLine, l_pImage );
					}

					StaticTexturePtr l_pStaTexture = m_pEngine->GetRenderSystem()->CreateStaticTexture();
					l_pStaTexture->SetDimension( eTEXTURE_DIMENSION_2D );
					l_pStaTexture->SetImage( l_pImage->GetPixels() );
					l_unit->SetTexture( l_pStaTexture );
				}
				else if (File::FileExists( l_fileIO.GetFilePath() / l_strLine))
				{
					ImagePtr l_pImage = m_pEngine->GetImageManager()->find( l_fileIO.GetFilePath() / l_strLine);

					if( !l_pImage )
					{
						l_pImage = std::make_shared< Image >( l_fileIO.GetFilePath() / l_strLine, l_fileIO.GetFilePath() / l_strLine );
						m_pEngine->GetImageManager()->insert( l_fileIO.GetFilePath() / l_strLine, l_pImage );
					}

					StaticTexturePtr l_pStaTexture = m_pEngine->GetRenderSystem()->CreateStaticTexture();
					l_pStaTexture->SetDimension( eTEXTURE_DIMENSION_2D );
					l_pStaTexture->SetImage( l_pImage->GetPixels() );
					l_unit->SetTexture( l_pStaTexture );
				}
				else
				{
					l_pass->DestroyTextureUnit( l_unit->GetIndex());
				}
			}

			p_pMesh->GetSubmesh( l_uiIndex )->SetMaterial( l_material);
			m_pEngine->GetMaterialManager()->SetToInitialise( l_material);

			l_uiIndex++;
		}

		l_bReturn = true;
	}

	return l_bReturn;
}

void Md3Importer :: DoCleanUp()
{
}
