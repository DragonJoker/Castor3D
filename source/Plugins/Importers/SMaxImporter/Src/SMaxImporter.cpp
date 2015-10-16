#include <RenderSystem.hpp>
#include <Buffer.hpp>
#include <SceneNode.hpp>
#include <SceneManager.hpp>
#include <Camera.hpp>
#include <Viewport.hpp>
#include <MaterialManager.hpp>
#include <MeshManager.hpp>
#include <Pass.hpp>
#include <TextureUnit.hpp>
#include <Geometry.hpp>
#include <Mesh.hpp>
#include <Submesh.hpp>
#include <Face.hpp>
#include <Version.hpp>
#include <Plugin.hpp>
#include <Engine.hpp>
#include <StaticTexture.hpp>
#include <Vertex.hpp>
#include <InitialiseEvent.hpp>

#include <Image.hpp>

#include "SMaxImporter.hpp"

using namespace SMax;
using namespace Castor3D;
using namespace Castor;

SMaxImporter::SMaxImporter( Engine & p_pEngine )
	: Importer( p_pEngine )
	, m_pFile( NULL )
	, m_iNumOfMaterials( 0 )
	, m_bIndicesFound( false )
	, m_uiNbVertex( 0 )
{
}

SceneSPtr SMaxImporter::DoImportScene()
{
	MeshSPtr l_pMesh = DoImportMesh();
	SceneSPtr l_pScene;

	if ( l_pMesh )
	{
		l_pMesh->GenerateBuffers();
		l_pScene = GetOwner()->GetSceneManager().Create( cuT( "Scene_3DS" ), *GetOwner(), cuT( "Scene_3DS" ) );
		SceneNodeSPtr l_pNode = l_pScene->CreateSceneNode( l_pMesh->GetName(), l_pScene->GetObjectRootNode() );
		GeometrySPtr l_pGeometry = l_pScene->CreateGeometry( l_pMesh->GetName() );
		l_pGeometry->AttachTo( l_pNode );
		l_pGeometry->SetMesh( l_pMesh );
	}

	return l_pScene;
}

MeshSPtr SMaxImporter::DoImportMesh()
{
	MeshSPtr l_pMesh;
	UIntArray l_faces;
	RealArray l_sizes;
	String l_nodeName = m_fileName.GetFileName();
	String l_meshName = m_fileName.GetFileName();
	String l_materialName = m_fileName.GetFileName();
	SMaxChunk l_currentChunk;
	m_pFile = new BinaryFile( m_fileName, File::eOPEN_MODE_READ );

	if ( m_pFile->IsOk() )
	{
		l_pMesh = GetOwner()->GetMeshManager().Create( l_meshName, eMESH_TYPE_CUSTOM, l_faces, l_sizes );
		DoReadChunk( &l_currentChunk );

		if ( l_currentChunk.m_eChunkId == eSMAX_CHUNK_M3DMAGIC )
		{
			DoProcessNextChunk( &l_currentChunk, l_pMesh );
			l_pMesh->ComputeNormals();
		}
	}

	delete m_pFile;
	return l_pMesh;
}

void SMaxImporter::DoProcessNextChunk( SMaxChunk * p_pChunk, MeshSPtr p_pMesh )
{
	std::vector< uint8_t > l_buffer;
	SMaxChunk l_currentChunk;
	bool l_bContinue = true;
	String l_strName;

	while ( m_pFile->IsOk() && p_pChunk->m_ulBytesRead < p_pChunk->m_ulLength && l_bContinue )
	{
		DoReadChunk( &l_currentChunk );

		if ( !DoIsValidChunk( &l_currentChunk, p_pChunk ) )
		{
//			CASTOR_ASSERT( false );
			l_bContinue = false;
		}
		else
		{
			switch ( l_currentChunk.m_eChunkId )
			{
			case eSMAX_CHUNK_M3D_VERSION:
				try
				{
					l_buffer.resize( l_currentChunk.m_ulLength - l_currentChunk.m_ulBytesRead );
					l_currentChunk.m_ulBytesRead += uint32_t( m_pFile->ReadArray( &l_buffer[0], l_currentChunk.m_ulLength - l_currentChunk.m_ulBytesRead ) );

					if ( ( l_currentChunk.m_ulLength - l_currentChunk.m_ulBytesRead == 4 ) && ( ( ( int * )&l_buffer[0] )[0] > 0x03 ) )
					{
						Logger::LogDebug( cuT( "File version is over version 3 and may load incorrectly" ) );
					}
				}
				catch ( ... )
				{
					Logger::LogDebug( cuT( "Exception caught while retrieving file version" ) );
					l_currentChunk.m_ulBytesRead = l_currentChunk.m_ulLength;
				}

				break;

			case eSMAX_CHUNK_MDATA:
				DoProcessNextChunk( &l_currentChunk, p_pMesh );
				break;

			case eSMAX_CHUNK_MAT_ENTRY:
				m_iNumOfMaterials++;
				DoProcessNextMaterialChunk( &l_currentChunk );
				break;

			case eSMAX_CHUNK_NAMED_OBJECT:
				l_currentChunk.m_ulBytesRead += DoGetString( l_strName );
				p_pMesh->ChangeName( l_strName );
				break;

			case eSMAX_CHUNK_N_TRI_OBJECT:
				DoProcessNextObjectChunk( &l_currentChunk, p_pMesh );
				break;

			case eSMAX_CHUNK_KFDATA:
				DoDiscardChunk( &l_currentChunk );
				break;

			default:
				DoDiscardChunk( &l_currentChunk );
				break;
			}
		}

		p_pChunk->m_ulBytesRead += l_currentChunk.m_ulBytesRead;
	}

	if ( !l_bContinue )
	{
		DoDiscardChunk( p_pChunk );
	}
}

void SMaxImporter::DoProcessNextObjectChunk( SMaxChunk * p_pChunk, MeshSPtr p_pMesh )
{
	SMaxChunk l_currentChunk;
	bool l_bContinue = true;
	SubmeshSPtr l_pSubmesh = p_pMesh->CreateSubmesh();

	while ( m_pFile->IsOk() && p_pChunk->m_ulBytesRead < p_pChunk->m_ulLength && l_bContinue )
	{
		DoReadChunk( &l_currentChunk );

		if ( !DoIsValidChunk( &l_currentChunk, p_pChunk ) )
		{
//			CASTOR_ASSERT( false );
			l_bContinue = false;
		}
		else
		{
			switch ( l_currentChunk.m_eChunkId )
			{
			case eSMAX_CHUNK_POINT_ARRAY:
				DoReadVertices( &l_currentChunk, l_pSubmesh );
				break;

			case eSMAX_CHUNK_FACE_ARRAY:
				DoReadVertexIndices( &l_currentChunk, l_pSubmesh );
				break;

			case eSMAX_CHUNK_MSH_MAT_GROUP:
				DoReadObjectMaterial( &l_currentChunk, l_pSubmesh );
				break;

			case eSMAX_CHUNK_TEX_VERTS:
				DoReadUVCoordinates( &l_currentChunk, l_pSubmesh );
				break;

			default:
				DoDiscardChunk( &l_currentChunk );
				break;
			}
		}

		p_pChunk->m_ulBytesRead += l_currentChunk.m_ulBytesRead;
	}

	if ( l_pSubmesh->GetPointsCount() && l_pSubmesh->GetFaceCount() )
	{
		if ( !m_bIndicesFound )
		{
			l_pSubmesh->ComputeFacesFromPolygonVertex();
		}

		l_pSubmesh->ComputeContainers();
	}

	m_bIndicesFound = false;

	if ( !l_bContinue )
	{
		DoDiscardChunk( p_pChunk );
	}
}

void SMaxImporter::DoProcessNextMaterialChunk( SMaxChunk * p_pChunk )
{
	SMaxChunk l_currentChunk;
	MaterialSPtr l_pMaterial;
	Colour l_crDiffuse( Colour::from_rgb( 0 ) );
	Colour l_crAmbient( Colour::from_rgb( 0 ) );
	Colour l_crSpecular( Colour::from_rgb( 0 ) );
	String l_strMatName;
	String l_strTextures[eTEXTURE_CHANNEL_ALL];
	bool l_bContinue = true;
	bool l_bTwoSided = false;

	while ( m_pFile->IsOk() && p_pChunk->m_ulBytesRead < p_pChunk->m_ulLength && l_bContinue )
	{
		DoReadChunk( &l_currentChunk );

		if ( !DoIsValidChunk( &l_currentChunk, p_pChunk ) )
		{
//			CASTOR_ASSERT( false );
			l_bContinue = false;
		}
		else
		{
			switch ( l_currentChunk.m_eChunkId )
			{
			case eSMAX_CHUNK_MAT_NAME:
				l_currentChunk.m_ulBytesRead += DoGetString( l_strMatName );
				l_currentChunk.m_ulBytesRead = l_currentChunk.m_ulLength;
				break;

			case eSMAX_CHUNK_MAT_AMBIENT:
				DoReadColorChunk( &l_currentChunk, l_crAmbient );
				break;

			case eSMAX_CHUNK_MAT_DIFFUSE:
				DoReadColorChunk( &l_currentChunk, l_crDiffuse );
				break;

			case eSMAX_CHUNK_MAT_SPECULAR:
				DoReadColorChunk( &l_currentChunk, l_crSpecular );
				break;

			case eSMAX_CHUNK_MAT_SHININESS:
				break;

			case eSMAX_CHUNK_MAT_TWO_SIDE:
				l_bTwoSided = true;
				break;

			case eSMAX_CHUNK_MAT_TEXMAP:
				DoProcessMaterialMapChunk( &l_currentChunk, l_strTextures[eTEXTURE_CHANNEL_DIFFUSE] );
				Logger::LogDebug( cuT( "Diffuse map : " ) + l_strTextures[eTEXTURE_CHANNEL_DIFFUSE] );
				break;

			case eSMAX_CHUNK_MAT_SPECMAP:
				DoProcessMaterialMapChunk( &l_currentChunk, l_strTextures[eTEXTURE_CHANNEL_SPECULAR] );
				Logger::LogDebug( cuT( "Specular map : " ) + l_strTextures[eTEXTURE_CHANNEL_SPECULAR] );
				break;

			case eSMAX_CHUNK_MAT_OPACMAP:
				DoProcessMaterialMapChunk( &l_currentChunk, l_strTextures[eTEXTURE_CHANNEL_OPACITY] );
				Logger::LogDebug( cuT( "Opacity map : " ) + l_strTextures[eTEXTURE_CHANNEL_OPACITY] );
				break;

			case eSMAX_CHUNK_MAT_BUMPMAP:
				DoProcessMaterialMapChunk( &l_currentChunk, l_strTextures[eTEXTURE_CHANNEL_NORMAL] );
				Logger::LogDebug( cuT( "Normal map : " ) + l_strTextures[eTEXTURE_CHANNEL_NORMAL] );
				break;

			case eSMAX_CHUNK_MAT_MAPNAME:
				l_currentChunk.m_ulBytesRead += DoGetString( l_strTextures[eTEXTURE_CHANNEL_DIFFUSE] );
				l_currentChunk.m_ulBytesRead = l_currentChunk.m_ulLength;
				Logger::LogDebug( cuT( "Texture : " ) + l_strTextures[eTEXTURE_CHANNEL_DIFFUSE] );
				break;

			default:
				DoDiscardChunk( &l_currentChunk );
				break;
			}
		}

		p_pChunk->m_ulBytesRead += l_currentChunk.m_ulBytesRead;
	}

	if ( !l_bContinue )
	{
		DoDiscardChunk( p_pChunk );
	}

	if ( !l_strMatName.empty() )
	{
		MaterialManager * l_pMtlManager = &GetOwner()->GetMaterialManager();
		l_pMaterial = l_pMtlManager->Find( l_strMatName );

		if ( ! l_pMaterial )
		{
			l_pMaterial = l_pMtlManager->Create( l_strMatName, *GetOwner(), l_strMatName );
			l_pMaterial->CreatePass();
		}

		PassSPtr l_pPass = l_pMaterial->GetPass( 0 );
		l_pPass->SetAmbient( l_crAmbient );
		l_pPass->SetDiffuse( l_crDiffuse );
		l_pPass->SetSpecular( l_crSpecular );
		l_pPass->SetTwoSided( l_bTwoSided );
		String l_strTexture;

		for ( int i = 0 ; i < eTEXTURE_CHANNEL_ALL ; i++ )
		{
			l_strTexture = l_strTextures[i];

			if ( !l_strTexture.empty() )
			{
				String l_strPath = m_pFile->GetFilePath() / l_strTexture;

				if ( !File::FileExists( l_strPath ) )
				{
					l_strPath = m_pFile->GetFilePath() / string::lower_case( l_strTexture );
				}

				ImageSPtr l_pImage = GetOwner()->GetImageManager().create( l_strPath, l_strPath );

				if ( l_pImage )
				{
					TextureUnitSPtr l_unit = l_pPass->AddTextureUnit();
					StaticTextureSPtr l_pStaTexture = GetOwner()->GetRenderSystem()->CreateStaticTexture();
					l_pStaTexture->SetType( eTEXTURE_TYPE_2D );
					l_pStaTexture->SetImage( l_pImage->GetPixels() );
					l_unit->SetTexture( l_pStaTexture );
					l_unit->SetBlendColour( Colour::from_components( 1.0, 1.0, 1.0, 1.0 ) );
					l_unit->SetChannel( eTEXTURE_CHANNEL( i ) );
				}
			}
		}

		GetOwner()->PostEvent( std::make_shared< InitialiseEvent< Material > >( *l_pMaterial ) );
	}
}

void SMaxImporter::DoProcessMaterialMapChunk( SMaxChunk * p_pChunk, String & p_strName )
{
	SMaxChunk l_currentChunk;
	bool l_bContinue = true;

	while ( m_pFile->IsOk() && p_pChunk->m_ulBytesRead < p_pChunk->m_ulLength && l_bContinue )
	{
		DoReadChunk( &l_currentChunk );

		if ( !DoIsValidChunk( &l_currentChunk, p_pChunk ) )
		{
//			CASTOR_ASSERT( false );
			l_bContinue = false;
		}
		else
		{
			switch ( l_currentChunk.m_eChunkId )
			{
			case eSMAX_CHUNK_MAT_MAPNAME:
				l_currentChunk.m_ulBytesRead += DoGetString( p_strName );
				l_currentChunk.m_ulBytesRead = l_currentChunk.m_ulLength;
				break;

			default:
				DoDiscardChunk( &l_currentChunk );
				break;
			}
		}

		p_pChunk->m_ulBytesRead += l_currentChunk.m_ulBytesRead;
	}

	if ( !l_bContinue )
	{
		DoDiscardChunk( p_pChunk );
	}
}

void SMaxImporter::DoReadChunk( SMaxChunk * p_pChunk )
{
	try
	{
		uint16_t l_usId;
		p_pChunk->m_ulBytesRead = uint32_t( m_pFile->Read( l_usId ) );
		p_pChunk->m_eChunkId = eSMAX_CHUNK( l_usId );
		p_pChunk->m_ulBytesRead += uint32_t( m_pFile->Read( p_pChunk->m_ulLength ) );
	}
	catch ( ... )
	{
		Logger::LogWarning( cuT( "Exception caught when reading chunk" ) );
	}
}

int SMaxImporter::DoGetString( String & p_strString )
{
	char l_pBuffer[255];

	try
	{
		int index = 0;
		m_pFile->Read( *l_pBuffer );

		while ( m_pFile->IsOk() && *( l_pBuffer + index++ ) != 0 )
		{
			m_pFile->Read( *( l_pBuffer + index ) );
		}

		p_strString = string::string_cast< xchar >( l_pBuffer );
	}
	catch ( ... )
	{
		Logger::LogWarning( cuT( "Exception caught when loading string" ) );
	}

	return int( strlen( l_pBuffer ) + 1 );
}

void SMaxImporter::DoReadColorChunk( SMaxChunk * p_pChunk, Colour & p_colour )
{
	SMaxChunk l_tempChunk;

	try
	{
		DoReadChunk( &l_tempChunk );

		if ( l_tempChunk.m_eChunkId == eSMAX_CHUNK_COLOR_F )
		{
			float l_pColour[3];
			l_tempChunk.m_ulBytesRead += uint32_t( m_pFile->ReadArray( l_pColour, 3 ) );
			p_colour.red() = l_pColour[0];
			p_colour.green() = l_pColour[1];
			p_colour.blue() = l_pColour[2];
		}
		else if ( l_tempChunk.m_eChunkId == eSMAX_CHUNK_COLOR_24 )
		{
			uint8_t l_pColour[3];
			l_tempChunk.m_ulBytesRead += uint32_t( m_pFile->ReadArray( l_pColour, 3 ) );
			p_colour.red() = l_pColour[0];
			p_colour.green() = l_pColour[1];
			p_colour.blue() = l_pColour[2];
		}

		p_pChunk->m_ulBytesRead += l_tempChunk.m_ulBytesRead;
	}
	catch ( ... )
	{
		Logger::LogWarning( cuT( "Exception caught when loading colour" ) );
		p_pChunk->m_ulBytesRead = p_pChunk->m_ulLength;
	}
}

void SMaxImporter::DoReadVertexIndices( SMaxChunk * p_pChunk, SubmeshSPtr p_pSubmesh )
{
	std::vector< uint32_t > l_arrayGroups;
	std::vector< stFACE_INDICES > l_arrayFaces;
	BufferElementGroupSPtr l_pV1;
	BufferElementGroupSPtr l_pV2;
	BufferElementGroupSPtr l_pV3;
	uint16_t l_usIndices[4];
	stFACE_INDICES l_face = { 0 };
	FaceSPtr l_pFace;
	SMaxChunk l_currentChunk;
	String l_strMatName;

	try
	{
		uint16_t l_usNumOfFaces = 0;
		p_pChunk->m_ulBytesRead += uint32_t( m_pFile->Read( l_usNumOfFaces ) );
		l_arrayFaces.resize( l_usNumOfFaces );
		Logger::LogDebug( StringStream() << cuT( "VertexIndices: " ) << l_usNumOfFaces );

		for ( uint16_t i = 0 ; i < l_usNumOfFaces && m_pFile->IsOk() && p_pChunk->m_ulBytesRead < p_pChunk->m_ulLength ; i++ )
		{
			p_pChunk->m_ulBytesRead += uint32_t( m_pFile->ReadArray( l_usIndices ) );
			l_arrayFaces[i].m_uiVertexIndex[0] = l_usIndices[0];
			l_arrayFaces[i].m_uiVertexIndex[1] = l_usIndices[2];
			l_arrayFaces[i].m_uiVertexIndex[2] = l_usIndices[1];
		}

		m_bIndicesFound = true;

		if ( m_pFile->IsOk() && p_pChunk->m_ulBytesRead < p_pChunk->m_ulLength )
		{
			bool l_bFace = true;

			// We have not reached the end of the chunk, so we try to retrieve remaining informations, beginning with uncounted faces (???)
			while ( m_pFile->IsOk() && p_pChunk->m_ulBytesRead + 6 < p_pChunk->m_ulLength && l_bFace )
			{
				DoReadChunk( &l_currentChunk );

				if ( l_currentChunk.m_eChunkId == eSMAX_CHUNK_SMOOTH_GROUP )
				{
					// The retrieved id tells the chunk might describe a smoothing group, so we check the length of the chunk
					// That length must be equal to (Faces count) * 4, as a smoothing group is a bitfield with 4 bytes for each face
					if ( DoIsValidChunk( &l_currentChunk, p_pChunk ) && l_currentChunk.m_ulLength == l_usNumOfFaces * sizeof( int ) )
					{
						// The chunk effectively describes a smoothing group
						l_bFace = false;
					}
					else
					{
						// The chunk doesn't describe a smoothing group, we suppose it is a face
						l_bFace = true;
					}
				}
				else if ( l_currentChunk.m_eChunkId == eSMAX_CHUNK_MSH_MAT_GROUP )
				{
					// The retrieved id tells the chunk might describe a material group, so we try to parse it
					if ( DoIsValidChunk( &l_currentChunk, p_pChunk ) )
					{
						l_bFace = false;
					}
					else
					{
						// The chunk isn't valid, we suppose it is a face
						l_bFace = true;
					}
				}

				if ( l_bFace )
				{
					// The chunk describes a triangle
					l_face.m_uiVertexIndex[0] = l_currentChunk.m_eChunkId;
					l_face.m_uiVertexIndex[1] = static_cast< uint16_t >( ( l_currentChunk.m_ulLength & 0x0000FFFF ) >>  0 );
					l_face.m_uiVertexIndex[2] = static_cast< uint16_t >( ( l_currentChunk.m_ulLength & 0xFFFF0000 ) >> 16 );

					if ( p_pChunk->m_ulBytesRead + 8 >= p_pChunk->m_ulLength )
					{
						// We can't read a face, we discard the chunk
						p_pChunk->m_ulBytesRead += l_currentChunk.m_ulBytesRead;
						DoDiscardChunk( p_pChunk );
						l_bFace = false;
					}
					else if ( l_face.m_uiVertexIndex[0] < m_uiNbVertex && l_face.m_uiVertexIndex[1] < m_uiNbVertex && l_face.m_uiVertexIndex[2] < m_uiNbVertex )
					{
						// Vertex indices are correct, so we consider we really are processing a face
						l_arrayFaces.push_back( l_face );
						p_pChunk->m_ulBytesRead += uint32_t( m_pFile->Read( l_usIndices[3] ) );
						p_pChunk->m_ulBytesRead += l_currentChunk.m_ulBytesRead;
					}
					else
					{
						// Vertex indices retrieved are incorrect, we get out from this loop, keeping the chunk for further processing
						l_bFace = false;
					}
				}
			}

			std::size_t l_uiNbFaces = l_arrayFaces.size();
			l_arrayGroups.resize( l_uiNbFaces );

			if ( p_pChunk->m_ulBytesRead < p_pChunk->m_ulLength )
			{
				// We have finished parsing remaining faces, and we have a chunk waiting for it's processing
				do
				{
					if ( l_currentChunk.m_eChunkId == eSMAX_CHUNK_SMOOTH_GROUP )
					{
						l_currentChunk.m_ulBytesRead += uint32_t( m_pFile->ReadArray( &l_arrayGroups[0], l_uiNbFaces ) );

						//for( std::size_t i = 0 ; i < l_uiNbFaces && p_pChunk->m_ulBytesRead < p_pChunk->m_ulLength ; ++i )
						//{
						//	l_itGroups = l_mapGroupsById.find( l_arrayGroups[i] );

						//	if( l_itGroups == l_mapGroupsById.end() )
						//	{
						//		l_mapGroupsById.insert( std::make_pair( l_arrayGroups[i], p_pSubmesh->AddSmoothingGroup() ) );
						//	}
						//}

						p_pChunk->m_ulBytesRead += l_currentChunk.m_ulBytesRead;

						if ( p_pChunk->m_ulBytesRead + 6 < p_pChunk->m_ulLength )
						{
							DoReadChunk( &l_currentChunk );
						}
						else
						{
							DoDiscardChunk( p_pChunk );
						}
					}
					else if ( l_currentChunk.m_eChunkId == eSMAX_CHUNK_MSH_MAT_GROUP )
					{
						DoReadObjectMaterial( &l_currentChunk, p_pSubmesh );
						p_pChunk->m_ulBytesRead += l_currentChunk.m_ulBytesRead;

						if ( p_pChunk->m_ulBytesRead + 6 < p_pChunk->m_ulLength )
						{
							DoReadChunk( &l_currentChunk );
						}
						else
						{
							DoDiscardChunk( p_pChunk );
						}
					}
					else if ( DoIsValidChunk( &l_currentChunk, p_pChunk ) )
					{
						DoDiscardChunk( &l_currentChunk );
						p_pChunk->m_ulBytesRead += l_currentChunk.m_ulBytesRead;

						if ( p_pChunk->m_ulBytesRead + 6 < p_pChunk->m_ulLength )
						{
							DoReadChunk( &l_currentChunk );
						}
						else
						{
							DoDiscardChunk( p_pChunk );
						}
					}
					else
					{
						// We decal the chunk of 2 bytes on the right
						uint16_t l_usDump = 0;
						p_pChunk->m_ulBytesRead += 2;
						l_currentChunk.m_eChunkId = eSMAX_CHUNK( static_cast< uint16_t >( l_currentChunk.m_ulLength & 0xFFFF0000 >> 16 ) );
						m_pFile->Read( l_usDump );
						l_currentChunk.m_ulLength = ( ( l_currentChunk.m_ulLength & 0x0000FFFF ) << 16 ) + l_usDump;
					}
				}
				while ( m_pFile->IsOk() && p_pChunk->m_ulBytesRead < p_pChunk->m_ulLength );
			}
		}

		for ( std::vector< stFACE_INDICES >::const_iterator l_it = l_arrayFaces.begin() ; l_it != l_arrayFaces.end() ; ++l_it )
		{
			// It seems faces are inverted in 3DS so I invert the indices to fall back in a good order
			uint32_t l_uiV1 = l_it->m_uiVertexIndex[0];
			uint32_t l_uiV2 = l_it->m_uiVertexIndex[1];
			uint32_t l_uiV3 = l_it->m_uiVertexIndex[2];
			l_pV1 = p_pSubmesh->GetPoint( l_uiV1 );
			l_pV2 = p_pSubmesh->GetPoint( l_uiV2 );
			l_pV3 = p_pSubmesh->GetPoint( l_uiV3 );
			l_pFace = p_pSubmesh->AddFace( l_uiV1, l_uiV2, l_uiV3 );

			if ( m_arrayTexVerts.size() )
			{
				Vertex::SetTexCoord( l_pV1, m_arrayTexVerts[( l_uiV1 * 2 ) + 0], m_arrayTexVerts[( l_uiV1 * 2 ) + 1] );
				Vertex::SetTexCoord( l_pV2, m_arrayTexVerts[( l_uiV2 * 2 ) + 0], m_arrayTexVerts[( l_uiV2 * 2 ) + 1] );
				Vertex::SetTexCoord( l_pV3, m_arrayTexVerts[( l_uiV3 * 2 ) + 0], m_arrayTexVerts[( l_uiV3 * 2 ) + 1] );
			}
		}
	}
	catch ( std::exception & exc )
	{
		Logger::LogWarning( std::stringstream() << "Exception caught when loading vertex indices: " << exc.what() );
		p_pChunk->m_ulBytesRead = p_pChunk->m_ulLength;
	}
	catch ( ... )
	{
		Logger::LogWarning( cuT( "Exception caught when loading vertex indices" ) );
		p_pChunk->m_ulBytesRead = p_pChunk->m_ulLength;
	}
}

void SMaxImporter::DoReadUVCoordinates( SMaxChunk * p_pChunk, SubmeshSPtr p_pSubmesh )
{
	try
	{
		uint16_t l_usNumTexVertex = 0;
		uint32_t l_uiNumTexVertex = 0;
		p_pChunk->m_ulBytesRead += uint32_t( m_pFile->Read( l_usNumTexVertex ) );
		l_uiNumTexVertex = l_usNumTexVertex;
		m_arrayTexVerts.resize( l_uiNumTexVertex * 2 );
		Logger::LogDebug( StringStream() << cuT( "TexCoords: " ) << l_usNumTexVertex );
		p_pChunk->m_ulBytesRead += uint32_t( m_pFile->ReadArray( &m_arrayTexVerts[0], l_uiNumTexVertex * 2 ) );
	}
	catch ( std::exception & exc )
	{
		Logger::LogWarning( std::stringstream() << "Exception caught when loading uv indices: " << exc.what() );
		p_pChunk->m_ulBytesRead = p_pChunk->m_ulLength;
	}
	catch ( ... )
	{
		Logger::LogWarning( cuT( "Exception caught when loading uv coordinates" ) );
		p_pChunk->m_ulBytesRead = p_pChunk->m_ulLength;
	}
}

void SMaxImporter::DoReadVertices( SMaxChunk * p_pChunk, SubmeshSPtr p_pSubmesh )
{
	try
	{
		float * l_pTmp = NULL;
		std::vector< float > l_pVertex;
		uint16_t l_usNumOfVerts = 0;
		p_pChunk->m_ulBytesRead += uint32_t( m_pFile->Read( l_usNumOfVerts ) );
		m_uiNbVertex = l_usNumOfVerts;

		if ( m_uiNbVertex > 0 )
		{
			l_pVertex.resize( m_uiNbVertex * 3 );
			l_pTmp = &l_pVertex[0];
			p_pChunk->m_ulBytesRead += uint32_t( m_pFile->ReadArray( l_pTmp, m_uiNbVertex * 3 ) );
		}

		Logger::LogDebug( StringStream() << cuT( "Vertices: " ) << l_usNumOfVerts );

		for ( uint16_t i = 0 ; i < l_usNumOfVerts ; i++ )
		{
			p_pSubmesh->AddPoint( l_pTmp[0], l_pTmp[2], -l_pTmp[1] );
			l_pTmp += 3;
		}
	}
	catch ( std::exception & exc )
	{
		Logger::LogWarning( std::stringstream() << "Exception caught when loading vertices: " << exc.what() );
		p_pChunk->m_ulBytesRead = p_pChunk->m_ulLength;
	}
	catch ( ... )
	{
		Logger::LogWarning( cuT( "Exception caught when loading vertices" ) );
		p_pChunk->m_ulBytesRead = p_pChunk->m_ulLength;
	}
}

void SMaxImporter::DoReadObjectMaterial( SMaxChunk * p_pChunk, SubmeshSPtr p_pSubmesh )
{
	String l_materialName;
	MaterialSPtr l_pMaterial;
	MaterialManager * l_pMtlManager;
	p_pChunk->m_ulBytesRead += DoGetString( l_materialName );
	l_pMtlManager = &GetOwner()->GetMaterialManager();
	l_pMaterial = l_pMtlManager->Find( l_materialName );

	if ( l_pMaterial && !p_pSubmesh->GetDefaultMaterial() )
	{
		p_pSubmesh->SetDefaultMaterial( l_pMaterial );
	}

	DoDiscardChunk( p_pChunk );
}

void SMaxImporter::DoDiscardChunk( SMaxChunk * p_pChunk )
{
	if ( p_pChunk->m_ulLength > p_pChunk->m_ulBytesRead )
	{
		uint32_t l_uiSize = p_pChunk->m_ulLength - p_pChunk->m_ulBytesRead;
		std::vector< uint8_t > l_buffer( l_uiSize );

		try
		{
			if ( l_uiSize <= uint32_t( m_pFile->GetLength() - m_pFile->Tell() ) && l_uiSize > 0 )
			{
				m_pFile->ReadArray( &l_buffer[0], l_uiSize );
			}
			else
			{
				m_pFile->ReadArray( &l_buffer[0], std::size_t( m_pFile->GetLength() - m_pFile->Tell() ) );
				throw std::range_error( "Bad chunk size" );
			}
		}
		catch ( std::exception & exc )
		{
			Logger::LogWarning( StringStream() << "Exception caught when discarding chunk: " << exc.what() );
		}
		catch ( ... )
		{
			Logger::LogWarning( cuT( "Exception caught when discarding chunk" ) );
		}

		p_pChunk->m_ulBytesRead = p_pChunk->m_ulLength;
	}
}

bool SMaxImporter::DoIsValidChunk( SMaxChunk * p_pChunk, SMaxChunk * p_pParent )
{
	bool l_bReturn = false;

	switch ( p_pChunk->m_eChunkId )
	{
	default:
		l_bReturn = false;
		break;

	case	eSMAX_CHUNK_NULL_CHUNK 				:
	case	eSMAX_CHUNK_UNKNOWN_CHUNK			:
	case	eSMAX_CHUNK_M3D_VERSION				:
	case	eSMAX_CHUNK_M3D_KFVERSION 			:
	case	eSMAX_CHUNK_COLOR_F					:
	case	eSMAX_CHUNK_COLOR_24				:
	case	eSMAX_CHUNK_LIN_COLOR_24			:
	case	eSMAX_CHUNK_LIN_COLOR_F				:
	case	eSMAX_CHUNK_INT_PERCENTAGE			:
	case	eSMAX_CHUNK_FLOAT_PERCENTAGE		:
	case	eSMAX_CHUNK_MASTER_SCALE			:
	case	eSMAX_CHUNK_CHUNKTYPE 				:
	case	eSMAX_CHUNK_CHUNKUNIQUE 			:
	case	eSMAX_CHUNK_NOTCHUNK 				:
	case	eSMAX_CHUNK_CONTAINER 				:
	case	eSMAX_CHUNK_ISCHUNK 				:
	case	eSMAX_CHUNK_C_SXP_SELFI_MASKDATA	:
	case	eSMAX_CHUNK_BIT_MAP					:
	case	eSMAX_CHUNK_USE_BIT_MAP 			:
	case	eSMAX_CHUNK_SOLID_BGND				:
	case	eSMAX_CHUNK_USE_SOLID_BGND 			:
	case	eSMAX_CHUNK_V_GRADIENT				:
	case	eSMAX_CHUNK_USE_V_GRADIENT 			:
	case	eSMAX_CHUNK_LO_SHADOW_BIAS			:
	case	eSMAX_CHUNK_HI_SHADOW_BIAS 			:
	case	eSMAX_CHUNK_SHADOW_MAP_SIZE			:
	case	eSMAX_CHUNK_SHADOW_SAMPLES 			:
	case	eSMAX_CHUNK_SHADOW_RANGE 			:
	case	eSMAX_CHUNK_SHADOW_FILTER			:
	case	eSMAX_CHUNK_RAY_BIAS				:
	case	eSMAX_CHUNK_O_CONSTS				:
	case	eSMAX_CHUNK_AMBIENT_LIGHT 			:
	case	eSMAX_CHUNK_FOG						:
	case	eSMAX_CHUNK_USE_FOG 				:
	case	eSMAX_CHUNK_FOG_BGND 				:
	case	eSMAX_CHUNK_DISTANCE_CUE			:
	case	eSMAX_CHUNK_USE_DISTANCE_CUE 		:
	case	eSMAX_CHUNK_LAYER_FOG				:
	case	eSMAX_CHUNK_USE_LAYER_FOG 			:
	case	eSMAX_CHUNK_DCUE_BGND 				:
	case	eSMAX_CHUNK_SMAGIC 					:
	case	eSMAX_CHUNK_LMAGIC					:
	case	eSMAX_CHUNK_DEFAULT_VIEW			:
	case	eSMAX_CHUNK_VIEW_TOP				:
	case	eSMAX_CHUNK_VIEW_BOTTOM				:
	case	eSMAX_CHUNK_VIEW_LEFT				:
	case	eSMAX_CHUNK_VIEW_RIGHT				:
	case	eSMAX_CHUNK_VIEW_FRONT				:
	case	eSMAX_CHUNK_VIEW_BACK				:
	case	eSMAX_CHUNK_VIEW_USER				:
	case	eSMAX_CHUNK_VIEW_CAMERA				:
	case	eSMAX_CHUNK_VIEW_WINDOW 			:
	case	eSMAX_CHUNK_MDATA					:
	case	eSMAX_CHUNK_MESH_VERSION 			:
	case	eSMAX_CHUNK_MLIBMAGIC				:
	case	eSMAX_CHUNK_PRJMAGIC				:
	case	eSMAX_CHUNK_MATMAGIC				:
	case	eSMAX_CHUNK_NAMED_OBJECT			:
	case	eSMAX_CHUNK_OBJ_HIDDEN 				:
	case	eSMAX_CHUNK_OBJ_VIS_LOFTER 			:
	case	eSMAX_CHUNK_OBJ_DOESNT_CAST 		:
	case	eSMAX_CHUNK_OBJ_MATTE 				:
	case	eSMAX_CHUNK_OBJ_FAST 				:
	case	eSMAX_CHUNK_OBJ_PROCEDURAL 			:
	case	eSMAX_CHUNK_OBJ_FROZEN 				:
	case	eSMAX_CHUNK_OBJ_DONT_RCVSHADOW 		:
	case	eSMAX_CHUNK_N_TRI_OBJECT			:
	case	eSMAX_CHUNK_POINT_ARRAY				:
	case	eSMAX_CHUNK_POINT_FLAG_ARRAY		:
	case	eSMAX_CHUNK_FACE_ARRAY				:
	case	eSMAX_CHUNK_MSH_MAT_GROUP			:
	case	eSMAX_CHUNK_OLD_MAT_GROUP 			:
	case	eSMAX_CHUNK_TEX_VERTS				:
	case	eSMAX_CHUNK_SMOOTH_GROUP			:
	case	eSMAX_CHUNK_MESH_MATRIX				:
	case	eSMAX_CHUNK_MESH_COLOR				:
	case	eSMAX_CHUNK_MESH_TEXTURE_INFO		:
	case	eSMAX_CHUNK_PROC_NAME 				:
	case	eSMAX_CHUNK_PROC_DATA 				:
	case	eSMAX_CHUNK_MSH_BOXMAP 				:
	case	eSMAX_CHUNK_N_D_L_OLD 				:
	case	eSMAX_CHUNK_N_CAM_OLD 				:
	case	eSMAX_CHUNK_N_DIRECT_LIGHT			:
	case	eSMAX_CHUNK_DL_SPOTLIGHT			:
	case	eSMAX_CHUNK_DL_OFF 					:
	case	eSMAX_CHUNK_DL_ATTENUATE 			:
	case	eSMAX_CHUNK_DL_RAYSHAD 				:
	case	eSMAX_CHUNK_DL_SHADOWED 			:
	case	eSMAX_CHUNK_DL_LOCAL_SHADOW 		:
	case	eSMAX_CHUNK_DL_LOCAL_SHADOW2 		:
	case	eSMAX_CHUNK_DL_SEE_CONE 			:
	case	eSMAX_CHUNK_DL_SPOT_RECTANGULAR 	:
	case	eSMAX_CHUNK_DL_SPOT_OVERSHOOT 		:
	case	eSMAX_CHUNK_DL_SPOT_PROJECTOR 		:
	case	eSMAX_CHUNK_DL_EXCLUDE 				:
	case	eSMAX_CHUNK_DL_RANGE 				:
	case	eSMAX_CHUNK_DL_SPOT_ROLL			:
	case	eSMAX_CHUNK_DL_SPOT_ASPECT 			:
	case	eSMAX_CHUNK_DL_RAY_BIAS				:
	case	eSMAX_CHUNK_DL_INNER_RANGE			:
	case	eSMAX_CHUNK_DL_OUTER_RANGE			:
	case	eSMAX_CHUNK_DL_MULTIPLIER			:
	case	eSMAX_CHUNK_N_AMBIENT_LIGHT 		:
	case	eSMAX_CHUNK_N_CAMERA				:
	case	eSMAX_CHUNK_CAM_SEE_CONE 			:
	case	eSMAX_CHUNK_CAM_RANGES				:
	case	eSMAX_CHUNK_M3DMAGIC				:
	case	eSMAX_CHUNK_HIERARCHY 				:
	case	eSMAX_CHUNK_PARENT_OBJECT 			:
	case	eSMAX_CHUNK_PIVOT_OBJECT 			:
	case	eSMAX_CHUNK_PIVOT_LIMITS 			:
	case	eSMAX_CHUNK_PIVOT_ORDER 			:
	case	eSMAX_CHUNK_XLATE_RANGE				:
	case	eSMAX_CHUNK_POLY_2D					:
	case	eSMAX_CHUNK_SHAPE_OK				:
	case	eSMAX_CHUNK_SHAPE_NOT_OK			:
	case	eSMAX_CHUNK_SHAPE_HOOK				:
	case	eSMAX_CHUNK_PATH_3D					:
	case	eSMAX_CHUNK_PATH_MATRIX				:
	case	eSMAX_CHUNK_SHAPE_2D				:
	case	eSMAX_CHUNK_M_SCALE					:
	case	eSMAX_CHUNK_M_TWIST					:
	case	eSMAX_CHUNK_M_TEETER				:
	case	eSMAX_CHUNK_M_FIT					:
	case	eSMAX_CHUNK_M_BEVEL					:
	case	eSMAX_CHUNK_XZ_CURVE				:
	case	eSMAX_CHUNK_YZ_CURVE				:
	case	eSMAX_CHUNK_INTERPCT				:
	case	eSMAX_CHUNK_DEFORM_LIMIT			:
	case	eSMAX_CHUNK_USE_CONTOUR				:
	case	eSMAX_CHUNK_USE_TWEEN				:
	case	eSMAX_CHUNK_USE_SCALE				:
	case	eSMAX_CHUNK_USE_TWIST				:
	case	eSMAX_CHUNK_USE_TEETER				:
	case	eSMAX_CHUNK_USE_FIT					:
	case	eSMAX_CHUNK_USE_BEVEL				:
	case	eSMAX_CHUNK_VIEWPORT_LAYOUT_OLD		:
	case	eSMAX_CHUNK_VIEWPORT_LAYOUT			:
	case	eSMAX_CHUNK_VIEWPORT_DATA_OLD		:
	case	eSMAX_CHUNK_VIEWPORT_DATA			:
	case	eSMAX_CHUNK_VIEWPORT_DATA_3			:
	case	eSMAX_CHUNK_VIEWPORT_SIZE			:
	case	eSMAX_CHUNK_NETWORK_VIEW			:
	case	eSMAX_CHUNK_XDATA_SECTION 			:
	case	eSMAX_CHUNK_XDATA_ENTRY 			:
	case	eSMAX_CHUNK_XDATA_APPNAME 			:
	case	eSMAX_CHUNK_XDATA_STRING 			:
	case	eSMAX_CHUNK_XDATA_FLOAT 			:
	case	eSMAX_CHUNK_XDATA_DOUBLE 			:
	case	eSMAX_CHUNK_XDATA_SHORT 			:
	case	eSMAX_CHUNK_XDATA_LONG 				:
	case	eSMAX_CHUNK_XDATA_VOID 				:
	case	eSMAX_CHUNK_XDATA_GROUP 			:
	case	eSMAX_CHUNK_XDATA_RFU6 				:
	case	eSMAX_CHUNK_XDATA_RFU5 				:
	case	eSMAX_CHUNK_XDATA_RFU4 				:
	case	eSMAX_CHUNK_XDATA_RFU3 				:
	case	eSMAX_CHUNK_XDATA_RFU2 				:
	case	eSMAX_CHUNK_XDATA_RFU1 				:
	case	eSMAX_CHUNK_PARENT_NAME				:
	case	eSMAX_CHUNK_MAT_NAME				:
	case	eSMAX_CHUNK_MAT_AMBIENT				:
	case	eSMAX_CHUNK_MAT_DIFFUSE				:
	case	eSMAX_CHUNK_MAT_SPECULAR			:
	case	eSMAX_CHUNK_MAT_SHININESS			:
	case	eSMAX_CHUNK_MAT_SHIN2PCT			:
	case	eSMAX_CHUNK_MAT_SHIN3PCT			:
	case	eSMAX_CHUNK_MAT_TRANSPARENCY		:
	case	eSMAX_CHUNK_MAT_XPFALL				:
	case	eSMAX_CHUNK_MAT_REFBLUR				:
	case	eSMAX_CHUNK_MAT_SELF_ILLUM 			:
	case	eSMAX_CHUNK_MAT_TWO_SIDE 			:
	case	eSMAX_CHUNK_MAT_DECAL 				:
	case	eSMAX_CHUNK_MAT_ADDITIVE 			:
	case	eSMAX_CHUNK_MAT_SELF_ILPCT			:
	case	eSMAX_CHUNK_MAT_WIRE 				:
	case	eSMAX_CHUNK_MAT_SUPERSMP 			:
	case	eSMAX_CHUNK_MAT_WIRESIZE			:
	case	eSMAX_CHUNK_MAT_FACEMAP 			:
	case	eSMAX_CHUNK_MAT_XPFALLIN 			:
	case	eSMAX_CHUNK_MAT_PHONGSOFT 			:
	case	eSMAX_CHUNK_MAT_WIREABS 			:
	case	eSMAX_CHUNK_MAT_SHADING				:
	case	eSMAX_CHUNK_MAT_TEXMAP				:
	case	eSMAX_CHUNK_MAT_SPECMAP				:
	case	eSMAX_CHUNK_MAT_OPACMAP				:
	case	eSMAX_CHUNK_MAT_REFLMAP				:
	case	eSMAX_CHUNK_MAT_BUMPMAP				:
	case	eSMAX_CHUNK_MAT_USE_XPFALL 			:
	case	eSMAX_CHUNK_MAT_USE_REFBLUR 		:
	case	eSMAX_CHUNK_MAT_BUMP_PERCENT 		:
	case	eSMAX_CHUNK_MAT_MAPNAME				:
	case	eSMAX_CHUNK_MAT_ACUBIC 				:
	case	eSMAX_CHUNK_MAT_SXP_TEXT_DATA 		:
	case	eSMAX_CHUNK_MAT_SXP_TEXT2_DATA 		:
	case	eSMAX_CHUNK_MAT_SXP_OPAC_DATA 		:
	case	eSMAX_CHUNK_MAT_SXP_BUMP_DATA 		:
	case	eSMAX_CHUNK_MAT_SXP_SPEC_DATA 		:
	case	eSMAX_CHUNK_MAT_SXP_SHIN_DATA 		:
	case	eSMAX_CHUNK_MAT_SXP_SELFI_DATA 		:
	case	eSMAX_CHUNK_MAT_SXP_TEXT_MASKDATA 	:
	case	eSMAX_CHUNK_MAT_SXP_TEXT2_MASKDATA 	:
	case	eSMAX_CHUNK_MAT_SXP_OPAC_MASKDATA 	:
	case	eSMAX_CHUNK_MAT_SXP_BUMP_MASKDATA 	:
	case	eSMAX_CHUNK_MAT_SXP_SPEC_MASKDATA 	:
	case	eSMAX_CHUNK_MAT_SXP_SHIN_MASKDATA 	:
	case	eSMAX_CHUNK_MAT_SXP_SELFI_MASKDATA 	:
	case	eSMAX_CHUNK_MAT_SXP_REFL_MASKDATA 	:
	case	eSMAX_CHUNK_MAT_TEX2MAP 			:
	case	eSMAX_CHUNK_MAT_SHINMAP 			:
	case	eSMAX_CHUNK_MAT_SELFIMAP 			:
	case	eSMAX_CHUNK_MAT_TEXMASK 			:
	case	eSMAX_CHUNK_MAT_TEX2MASK 			:
	case	eSMAX_CHUNK_MAT_OPACMASK 			:
	case	eSMAX_CHUNK_MAT_BUMPMASK 			:
	case	eSMAX_CHUNK_MAT_SHINMASK 			:
	case	eSMAX_CHUNK_MAT_SPECMASK 			:
	case	eSMAX_CHUNK_MAT_SELFIMASK 			:
	case	eSMAX_CHUNK_MAT_REFLMASK 			:
	case	eSMAX_CHUNK_MAT_MAP_TILINGOLD 		:
	case	eSMAX_CHUNK_MAT_MAP_TILING			:
	case	eSMAX_CHUNK_MAT_MAP_TEXBLUR_OLD 	:
	case	eSMAX_CHUNK_MAT_MAP_TEXBLUR			:
	case	eSMAX_CHUNK_MAT_MAP_USCALE 			:
	case	eSMAX_CHUNK_MAT_MAP_VSCALE 			:
	case	eSMAX_CHUNK_MAT_MAP_UOFFSET 		:
	case	eSMAX_CHUNK_MAT_MAP_VOFFSET 		:
	case	eSMAX_CHUNK_MAT_MAP_ANG 			:
	case	eSMAX_CHUNK_MAT_MAP_COL1 			:
	case	eSMAX_CHUNK_MAT_MAP_COL2 			:
	case	eSMAX_CHUNK_MAT_MAP_RCOL 			:
	case	eSMAX_CHUNK_MAT_MAP_GCOL 			:
	case	eSMAX_CHUNK_MAT_MAP_BCOL 			:
	case	eSMAX_CHUNK_MAT_ENTRY				:
	case	eSMAX_CHUNK_KFDATA					:
	case	eSMAX_CHUNK_AMBIENT_NODE_TAG 		:
	case	eSMAX_CHUNK_OBJECT_NODE_TAG			:
	case	eSMAX_CHUNK_CAMERA_NODE_TAG			:
	case	eSMAX_CHUNK_TARGET_NODE_TAG			:
	case	eSMAX_CHUNK_LIGHT_NODE_TAG			:
	case	eSMAX_CHUNK_L_TARGET_NODE_TAG		:
	case	eSMAX_CHUNK_SPOTLIGHT_NODE_TAG		:
	case	eSMAX_CHUNK_KFSEG					:
	case	eSMAX_CHUNK_KFCURTIME				:
	case	eSMAX_CHUNK_KFHDR					:
	case	eSMAX_CHUNK_NODE_HDR				:
	case	eSMAX_CHUNK_INSTANCE_NAME 			:
	case	eSMAX_CHUNK_PRESCALE 				:
	case	eSMAX_CHUNK_PIVOT					:
	case	eSMAX_CHUNK_BOUNDBOX 				:
	case	eSMAX_CHUNK_MORPH_SMOOTH			:
	case	eSMAX_CHUNK_POS_TRACK_TAG			:
	case	eSMAX_CHUNK_ROT_TRACK_TAG			:
	case	eSMAX_CHUNK_SCL_TRACK_TAG			:
	case	eSMAX_CHUNK_FOV_TRACK_TAG			:
	case	eSMAX_CHUNK_ROLL_TRACK_TAG			:
	case	eSMAX_CHUNK_COL_TRACK_TAG			:
	case	eSMAX_CHUNK_MORPH_TRACK_TAG			:
	case	eSMAX_CHUNK_HOT_TRACK_TAG			:
	case	eSMAX_CHUNK_FALL_TRACK_TAG			:
	case	eSMAX_CHUNK_HIDE_TRACK_TAG 			:
	case	eSMAX_CHUNK_NODE_ID					:
	case	eSMAX_CHUNK_C_MDRAWER 				:
	case	eSMAX_CHUNK_C_TDRAWER 				:
	case	eSMAX_CHUNK_C_SHPDRAWER 			:
	case	eSMAX_CHUNK_C_MODDRAWER 			:
	case	eSMAX_CHUNK_C_RIPDRAWER 			:
	case	eSMAX_CHUNK_C_TXDRAWER 				:
	case	eSMAX_CHUNK_C_PDRAWER 				:
	case	eSMAX_CHUNK_C_MTLDRAWER 			:
	case	eSMAX_CHUNK_C_FLIDRAWER 			:
	case	eSMAX_CHUNK_C_CUBDRAWER 			:
	case	eSMAX_CHUNK_C_MFILE 				:
	case	eSMAX_CHUNK_C_SHPFILE 				:
	case	eSMAX_CHUNK_C_MODFILE 				:
	case	eSMAX_CHUNK_C_RIPFILE 				:
	case	eSMAX_CHUNK_C_TXFILE 				:
	case	eSMAX_CHUNK_C_PFILE 				:
	case	eSMAX_CHUNK_C_MTLFILE 				:
	case	eSMAX_CHUNK_C_FLIFILE 				:
	case	eSMAX_CHUNK_C_PALFILE 				:
	case	eSMAX_CHUNK_C_TX_STRING 			:
	case	eSMAX_CHUNK_C_CONSTS 				:
	case	eSMAX_CHUNK_C_SNAPS 				:
	case	eSMAX_CHUNK_C_GRIDS 				:
	case	eSMAX_CHUNK_C_ASNAPS 				:
	case	eSMAX_CHUNK_C_GRID_RANGE 			:
	case	eSMAX_CHUNK_C_RENDTYPE 				:
	case	eSMAX_CHUNK_C_PROGMODE 				:
	case	eSMAX_CHUNK_C_PREVMODE 				:
	case	eSMAX_CHUNK_C_MODWMODE 				:
	case	eSMAX_CHUNK_C_MODMODEL 				:
	case	eSMAX_CHUNK_C_ALL_LINES 			:
	case	eSMAX_CHUNK_C_BACK_TYPE 			:
	case	eSMAX_CHUNK_C_MD_CS 				:
	case	eSMAX_CHUNK_C_MD_CE 				:
	case	eSMAX_CHUNK_C_MD_SML 				:
	case	eSMAX_CHUNK_C_MD_SMW 				:
	case	eSMAX_CHUNK_C_LOFT_WITH_TEXTURE 	:
	case	eSMAX_CHUNK_C_LOFT_L_REPEAT 		:
	case	eSMAX_CHUNK_C_LOFT_W_REPEAT 		:
	case	eSMAX_CHUNK_C_LOFT_UV_NORMALIZE 	:
	case	eSMAX_CHUNK_C_WELD_LOFT 			:
	case	eSMAX_CHUNK_C_MD_PDET 				:
	case	eSMAX_CHUNK_C_MD_SDET 				:
	case	eSMAX_CHUNK_C_RGB_RMODE 			:
	case	eSMAX_CHUNK_C_RGB_HIDE 				:
	case	eSMAX_CHUNK_C_RGB_MAPSW 			:
	case	eSMAX_CHUNK_C_RGB_TWOSIDE 			:
	case	eSMAX_CHUNK_C_RGB_SHADOW 			:
	case	eSMAX_CHUNK_C_RGB_AA 				:
	case	eSMAX_CHUNK_C_RGB_OVW 				:
	case	eSMAX_CHUNK_C_RGB_OVH 				:
	case	eSMAX_CHUNK_CMAGIC 					:
	case	eSMAX_CHUNK_C_RGB_PICTYPE 			:
	case	eSMAX_CHUNK_C_RGB_OUTPUT 			:
	case	eSMAX_CHUNK_C_RGB_TODISK 			:
	case	eSMAX_CHUNK_C_RGB_COMPRESS 			:
	case	eSMAX_CHUNK_C_JPEG_COMPRESSION 		:
	case	eSMAX_CHUNK_C_RGB_DISPDEV 			:
	case	eSMAX_CHUNK_C_RGB_HARDDEV 			:
	case	eSMAX_CHUNK_C_RGB_PATH 				:
	case	eSMAX_CHUNK_C_BITMAP_DRAWER 		:
	case	eSMAX_CHUNK_C_RGB_FILE 				:
	case	eSMAX_CHUNK_C_RGB_OVASPECT 			:
	case	eSMAX_CHUNK_C_RGB_ANIMTYPE 			:
	case	eSMAX_CHUNK_C_RENDER_ALL 			:
	case	eSMAX_CHUNK_C_REND_FROM 			:
	case	eSMAX_CHUNK_C_REND_TO 				:
	case	eSMAX_CHUNK_C_REND_NTH 				:
	case	eSMAX_CHUNK_C_PAL_TYPE 				:
	case	eSMAX_CHUNK_C_RND_TURBO 			:
	case	eSMAX_CHUNK_C_RND_MIP 				:
	case	eSMAX_CHUNK_C_BGND_METHOD 			:
	case	eSMAX_CHUNK_C_AUTO_REFLECT 			:
	case	eSMAX_CHUNK_C_VP_FROM 				:
	case	eSMAX_CHUNK_C_VP_TO 				:
	case	eSMAX_CHUNK_C_VP_NTH 				:
	case	eSMAX_CHUNK_C_REND_TSTEP 			:
	case	eSMAX_CHUNK_C_VP_TSTEP 				:
	case	eSMAX_CHUNK_C_SRDIAM 				:
	case	eSMAX_CHUNK_C_SRDEG 				:
	case	eSMAX_CHUNK_C_SRSEG 				:
	case	eSMAX_CHUNK_C_SRDIR 				:
	case	eSMAX_CHUNK_C_HETOP 				:
	case	eSMAX_CHUNK_C_HEBOT 				:
	case	eSMAX_CHUNK_C_HEHT 					:
	case	eSMAX_CHUNK_C_HETURNS 				:
	case	eSMAX_CHUNK_C_HEDEG 				:
	case	eSMAX_CHUNK_C_HESEG 				:
	case	eSMAX_CHUNK_C_HEDIR 				:
	case	eSMAX_CHUNK_C_QUIKSTUFF 			:
	case	eSMAX_CHUNK_C_SEE_LIGHTS 			:
	case	eSMAX_CHUNK_C_SEE_CAMERAS 			:
	case	eSMAX_CHUNK_C_SEE_3D 				:
	case	eSMAX_CHUNK_C_MESHSEL 				:
	case	eSMAX_CHUNK_C_MESHUNSEL 			:
	case	eSMAX_CHUNK_C_POLYSEL 				:
	case	eSMAX_CHUNK_C_POLYUNSEL 			:
	case	eSMAX_CHUNK_C_SHPLOCAL 				:
	case	eSMAX_CHUNK_C_MSHLOCAL 				:
	case	eSMAX_CHUNK_C_NUM_FORMAT 			:
	case	eSMAX_CHUNK_C_ARCH_DENOM 			:
	case	eSMAX_CHUNK_C_IN_DEVICE 			:
	case	eSMAX_CHUNK_C_MSCALE 				:
	case	eSMAX_CHUNK_C_COMM_PORT 			:
	case	eSMAX_CHUNK_C_TAB_BASES 			:
	case	eSMAX_CHUNK_C_TAB_DIVS 				:
	case	eSMAX_CHUNK_C_MASTER_SCALES 		:
	case	eSMAX_CHUNK_C_SHOW_1STVERT 			:
	case	eSMAX_CHUNK_C_SHAPER_OK 			:
	case	eSMAX_CHUNK_C_LOFTER_OK 			:
	case	eSMAX_CHUNK_C_EDITOR_OK 			:
	case	eSMAX_CHUNK_C_KEYFRAMER_OK 			:
	case	eSMAX_CHUNK_C_PICKSIZE 				:
	case	eSMAX_CHUNK_C_MAPTYPE 				:
	case	eSMAX_CHUNK_C_MAP_DISPLAY 			:
	case	eSMAX_CHUNK_C_TILE_XY 				:
	case	eSMAX_CHUNK_C_MAP_XYZ 				:
	case	eSMAX_CHUNK_C_MAP_SCALE 			:
	case	eSMAX_CHUNK_C_MAP_MATRIX_OLD 		:
	case	eSMAX_CHUNK_C_MAP_MATRIX 			:
	case	eSMAX_CHUNK_C_MAP_WID_HT 			:
	case	eSMAX_CHUNK_C_OBNAME 				:
	case	eSMAX_CHUNK_C_CAMNAME 				:
	case	eSMAX_CHUNK_C_LTNAME 				:
	case	eSMAX_CHUNK_C_CUR_MNAME 			:
	case	eSMAX_CHUNK_C_CURMTL_FROM_MESH 		:
	case	eSMAX_CHUNK_C_GET_SHAPE_MAKE_FACES	:
	case	eSMAX_CHUNK_C_DETAIL 				:
	case	eSMAX_CHUNK_C_VERTMARK 				:
	case	eSMAX_CHUNK_C_MSHAX 				:
	case	eSMAX_CHUNK_C_MSHCP 				:
	case	eSMAX_CHUNK_C_USERAX 				:
	case	eSMAX_CHUNK_C_SHOOK 				:
	case	eSMAX_CHUNK_C_RAX 					:
	case	eSMAX_CHUNK_C_STAPE 				:
	case	eSMAX_CHUNK_C_LTAPE 				:
	case	eSMAX_CHUNK_C_ETAPE 				:
	case	eSMAX_CHUNK_C_KTAPE 				:
	case	eSMAX_CHUNK_C_SPHSEGS 				:
	case	eSMAX_CHUNK_C_GEOSMOOTH 			:
	case	eSMAX_CHUNK_C_HEMISEGS 				:
	case	eSMAX_CHUNK_C_PRISMSEGS 			:
	case	eSMAX_CHUNK_C_PRISMSIDES 			:
	case	eSMAX_CHUNK_C_TUBESEGS 				:
	case	eSMAX_CHUNK_C_TUBESIDES 			:
	case	eSMAX_CHUNK_C_TORSEGS 				:
	case	eSMAX_CHUNK_C_TORSIDES 				:
	case	eSMAX_CHUNK_C_CONESIDES 			:
	case	eSMAX_CHUNK_C_CONESEGS 				:
	case	eSMAX_CHUNK_C_NGPARMS 				:
	case	eSMAX_CHUNK_C_PTHLEVEL 				:
	case	eSMAX_CHUNK_C_MSCSYM 				:
	case	eSMAX_CHUNK_C_MFTSYM 				:
	case	eSMAX_CHUNK_C_MTTSYM 				:
	case	eSMAX_CHUNK_C_SMOOTHING 			:
	case	eSMAX_CHUNK_C_MODICOUNT 			:
	case	eSMAX_CHUNK_C_FONTSEL 				:
	case	eSMAX_CHUNK_C_TESS_TYPE 			:
	case	eSMAX_CHUNK_C_TESS_TENSION 			:
	case	eSMAX_CHUNK_C_SEG_START 			:
	case	eSMAX_CHUNK_C_SEG_END 				:
	case	eSMAX_CHUNK_C_CURTIME 				:
	case	eSMAX_CHUNK_C_ANIMLENGTH 			:
	case	eSMAX_CHUNK_C_PV_FROM 				:
	case	eSMAX_CHUNK_C_PV_TO 				:
	case	eSMAX_CHUNK_C_PV_DOFNUM 			:
	case	eSMAX_CHUNK_C_PV_RNG 				:
	case	eSMAX_CHUNK_C_PV_NTH 				:
	case	eSMAX_CHUNK_C_PV_TYPE 				:
	case	eSMAX_CHUNK_C_PV_METHOD 			:
	case	eSMAX_CHUNK_C_PV_FPS 				:
	case	eSMAX_CHUNK_C_VTR_FRAMES 			:
	case	eSMAX_CHUNK_C_VTR_HDTL 				:
	case	eSMAX_CHUNK_C_VTR_HD 				:
	case	eSMAX_CHUNK_C_VTR_TL 				:
	case	eSMAX_CHUNK_C_VTR_IN 				:
	case	eSMAX_CHUNK_C_VTR_PK 				:
	case	eSMAX_CHUNK_C_VTR_SH 				:
	case	eSMAX_CHUNK_C_WORK_MTLS 			:
	case	eSMAX_CHUNK_C_WORK_MTLS_2 			:
	case	eSMAX_CHUNK_C_WORK_MTLS_3 			:
	case	eSMAX_CHUNK_C_WORK_MTLS_4 			:
	case	eSMAX_CHUNK_C_BGTYPE 				:
	case	eSMAX_CHUNK_C_MEDTILE 				:
	case	eSMAX_CHUNK_C_LO_CONTRAST 			:
	case	eSMAX_CHUNK_C_HI_CONTRAST 			:
	case	eSMAX_CHUNK_C_FROZ_DISPLAY 			:
	case	eSMAX_CHUNK_C_BOOLWELD 				:
	case	eSMAX_CHUNK_C_BOOLTYPE 				:
	case	eSMAX_CHUNK_C_ANG_THRESH 			:
	case	eSMAX_CHUNK_C_SS_THRESH 			:
	case	eSMAX_CHUNK_C_TEXTURE_BLUR_DEFAULT	:
	case	eSMAX_CHUNK_C_MAPDRAWER 			:
	case	eSMAX_CHUNK_C_MAPDRAWER1 			:
	case	eSMAX_CHUNK_C_MAPDRAWER2 			:
	case	eSMAX_CHUNK_C_MAPDRAWER3 			:
	case	eSMAX_CHUNK_C_MAPDRAWER4 			:
	case	eSMAX_CHUNK_C_MAPDRAWER5 			:
	case	eSMAX_CHUNK_C_MAPDRAWER6 			:
	case	eSMAX_CHUNK_C_MAPDRAWER7 			:
	case	eSMAX_CHUNK_C_MAPDRAWER8 			:
	case	eSMAX_CHUNK_C_MAPDRAWER9 			:
	case	eSMAX_CHUNK_C_MAPDRAWER_ENTRY 		:
	case	eSMAX_CHUNK_C_BACKUP_FILE 			:
	case	eSMAX_CHUNK_C_DITHER_256 			:
	case	eSMAX_CHUNK_C_SAVE_LAST 			:
	case	eSMAX_CHUNK_C_USE_ALPHA 			:
	case	eSMAX_CHUNK_C_TGA_DEPTH 			:
	case	eSMAX_CHUNK_C_REND_FIELDS 			:
	case	eSMAX_CHUNK_C_REFLIP 				:
	case	eSMAX_CHUNK_C_SEL_ITEMTOG 			:
	case	eSMAX_CHUNK_C_SEL_RESET 			:
	case	eSMAX_CHUNK_C_STICKY_KEYINF 		:
	case	eSMAX_CHUNK_C_WELD_THRESHOLD 		:
	case	eSMAX_CHUNK_C_ZCLIP_POINT 			:
	case	eSMAX_CHUNK_C_ALPHA_SPLIT 			:
	case	eSMAX_CHUNK_C_KF_SHOW_BACKFACE 		:
	case	eSMAX_CHUNK_C_OPTIMIZE_LOFT 		:
	case	eSMAX_CHUNK_C_TENS_DEFAULT 			:
	case	eSMAX_CHUNK_C_CONT_DEFAULT 			:
	case	eSMAX_CHUNK_C_BIAS_DEFAULT 			:
	case	eSMAX_CHUNK_C_DXFNAME_SRC 			:
	case	eSMAX_CHUNK_C_AUTO_WELD 			:
	case	eSMAX_CHUNK_C_AUTO_UNIFY 			:
	case	eSMAX_CHUNK_C_AUTO_SMOOTH 			:
	case	eSMAX_CHUNK_C_DXF_SMOOTH_ANG 		:
	case	eSMAX_CHUNK_C_SMOOTH_ANG 			:
	case	eSMAX_CHUNK_C_WORK_MTLS_5 			:
	case	eSMAX_CHUNK_C_WORK_MTLS_6 			:
	case	eSMAX_CHUNK_C_WORK_MTLS_7 			:
	case	eSMAX_CHUNK_C_WORK_MTLS_8 			:
	case	eSMAX_CHUNK_C_WORKMTL 				:
	case	eSMAX_CHUNK_C_SXP_TEXT_DATA 		:
	case	eSMAX_CHUNK_C_SXP_OPAC_DATA 		:
	case	eSMAX_CHUNK_C_SXP_BUMP_DATA 		:
	case	eSMAX_CHUNK_C_SXP_SHIN_DATA 		:
	case	eSMAX_CHUNK_C_SXP_TEXT2_DATA 		:
	case	eSMAX_CHUNK_C_SXP_SPEC_DATA 		:
	case	eSMAX_CHUNK_C_SXP_SELFI_DATA 		:
	case	eSMAX_CHUNK_C_SXP_TEXT_MASKDATA 	:
	case	eSMAX_CHUNK_C_SXP_TEXT2_MASKDATA	:
	case	eSMAX_CHUNK_C_SXP_OPAC_MASKDATA 	:
	case	eSMAX_CHUNK_C_SXP_BUMP_MASKDATA 	:
	case	eSMAX_CHUNK_C_SXP_SPEC_MASKDATA 	:
	case	eSMAX_CHUNK_C_SXP_SHIN_MASKDATA 	:
	case	eSMAX_CHUNK_C_SXP_REFL_MASKDATA 	:
	case	eSMAX_CHUNK_C_NET_USE_VPOST 		:
	case	eSMAX_CHUNK_C_NET_USE_GAMMA 		:
	case	eSMAX_CHUNK_C_NET_FIELD_ORDER 		:
	case	eSMAX_CHUNK_C_BLUR_FRAMES 			:
	case	eSMAX_CHUNK_C_BLUR_SAMPLES 			:
	case	eSMAX_CHUNK_C_BLUR_DUR 				:
	case	eSMAX_CHUNK_C_HOT_METHOD 			:
	case	eSMAX_CHUNK_C_HOT_CHECK 			:
	case	eSMAX_CHUNK_C_PIXEL_SIZE 			:
	case	eSMAX_CHUNK_C_DISP_GAMMA 			:
	case	eSMAX_CHUNK_C_FBUF_GAMMA 			:
	case	eSMAX_CHUNK_C_FILE_OUT_GAMMA 		:
	case	eSMAX_CHUNK_C_FILE_IN_GAMMA 		:
	case	eSMAX_CHUNK_C_GAMMA_CORRECT 		:
	case	eSMAX_CHUNK_C_APPLY_DISP_GAMMA 		:
	case	eSMAX_CHUNK_C_APPLY_FBUF_GAMMA 		:
	case	eSMAX_CHUNK_C_APPLY_FILE_GAMMA 		:
	case	eSMAX_CHUNK_C_FORCE_WIRE 			:
	case	eSMAX_CHUNK_C_RAY_SHADOWS 			:
	case	eSMAX_CHUNK_C_MASTER_AMBIENT 		:
	case	eSMAX_CHUNK_C_SUPER_SAMPLE 			:
	case	eSMAX_CHUNK_C_OBJECT_MBLUR 			:
	case	eSMAX_CHUNK_C_MBLUR_DITHER 			:
	case	eSMAX_CHUNK_C_DITHER_24 			:
	case	eSMAX_CHUNK_C_SUPER_BLACK 			:
	case	eSMAX_CHUNK_C_SAFE_FRAME 			:
	case	eSMAX_CHUNK_C_VIEW_PRES_RATIO 		:
	case	eSMAX_CHUNK_C_BGND_PRES_RATIO 		:
	case	eSMAX_CHUNK_C_NTH_SERIAL_NUM		:
	case	eSMAX_CHUNK_VPDATA					:
	case	eSMAX_CHUNK_P_QUEUE_ENTRY 			:
	case	eSMAX_CHUNK_P_QUEUE_IMAGE 			:
	case	eSMAX_CHUNK_P_QUEUE_USEIGAMMA 		:
	case	eSMAX_CHUNK_P_QUEUE_PROC 			:
	case	eSMAX_CHUNK_P_QUEUE_SOLID 			:
	case	eSMAX_CHUNK_P_QUEUE_GRADIENT 		:
	case	eSMAX_CHUNK_P_QUEUE_KF 				:
	case	eSMAX_CHUNK_P_QUEUE_MOTBLUR 		:
	case	eSMAX_CHUNK_P_QUEUE_MB_REPEAT 		:
	case	eSMAX_CHUNK_P_QUEUE_NONE 			:
	case	eSMAX_CHUNK_P_QUEUE_RESIZE 			:
	case	eSMAX_CHUNK_P_QUEUE_OFFSET 			:
	case	eSMAX_CHUNK_P_QUEUE_ALIGN 			:
	case	eSMAX_CHUNK_P_CUSTOM_SIZE 			:
	case	eSMAX_CHUNK_P_ALPH_NONE 			:
	case	eSMAX_CHUNK_P_ALPH_PSEUDO 			:
	case	eSMAX_CHUNK_P_ALPH_OP_PSEUDO 		:
	case	eSMAX_CHUNK_P_ALPH_BLUR 			:
	case	eSMAX_CHUNK_P_ALPH_PCOL 			:
	case	eSMAX_CHUNK_P_ALPH_C0 				:
	case	eSMAX_CHUNK_P_ALPH_OP_KEY 			:
	case	eSMAX_CHUNK_P_ALPH_KCOL 			:
	case	eSMAX_CHUNK_P_ALPH_OP_NOCONV 		:
	case	eSMAX_CHUNK_P_ALPH_IMAGE 			:
	case	eSMAX_CHUNK_P_ALPH_ALPHA 			:
	case	eSMAX_CHUNK_P_ALPH_QUES 			:
	case	eSMAX_CHUNK_P_ALPH_QUEIMG 			:
	case	eSMAX_CHUNK_P_ALPH_CUTOFF 			:
	case	eSMAX_CHUNK_P_ALPHANEG 				:
	case	eSMAX_CHUNK_P_TRAN_NONE 			:
	case	eSMAX_CHUNK_P_TRAN_IMAGE 			:
	case	eSMAX_CHUNK_P_TRAN_FRAMES 			:
	case	eSMAX_CHUNK_P_TRAN_FADEIN 			:
	case	eSMAX_CHUNK_P_TRAN_FADEOUT 			:
	case	eSMAX_CHUNK_P_TRANNEG 				:
	case	eSMAX_CHUNK_P_RANGES 				:
	case	eSMAX_CHUNK_P_PROC_DATA				:
	case	eSMAX_CHUNK_POS_TRACK_TAG_KEY		:
	case	eSMAX_CHUNK_ROT_TRACK_TAG_KEY 		:
	case	eSMAX_CHUNK_SCL_TRACK_TAG_KEY 		:
	case	eSMAX_CHUNK_FOV_TRACK_TAG_KEY 		:
	case	eSMAX_CHUNK_ROLL_TRACK_TAG_KEY 		:
	case	eSMAX_CHUNK_COL_TRACK_TAG_KEY 		:
	case	eSMAX_CHUNK_MORPH_TRACK_TAG_KEY 	:
	case	eSMAX_CHUNK_HOT_TRACK_TAG_KEY 		:
	case	eSMAX_CHUNK_FALL_TRACK_TAG_KEY 		:
	case	eSMAX_CHUNK_POINT_ARRAY_ENTRY 		:
	case	eSMAX_CHUNK_POINT_FLAG_ARRAY_ENTRY 	:
	case	eSMAX_CHUNK_FACE_ARRAY_ENTRY 		:
	case	eSMAX_CHUNK_MSH_MAT_GROUP_ENTRY 	:
	case	eSMAX_CHUNK_TEX_VERTS_ENTRY 		:
	case	eSMAX_CHUNK_SMOOTH_GROUP_ENTRY 		:
	case	eSMAX_CHUNK_DUMMY					:
		l_bReturn = true;
		break;
	}

	if ( l_bReturn )
	{
		l_bReturn = p_pChunk->m_ulLength + p_pParent->m_ulBytesRead <= p_pParent->m_ulLength;
	}

	return l_bReturn;
}
