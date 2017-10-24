#include "SMaxImporter.hpp"

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

namespace C3dSMax
{
	SMaxImporter::SMaxImporter( Engine & p_engine )
		: Importer( p_engine )
		, m_pFile( NULL )
		, m_iNumOfMaterials( 0 )
		, m_bIndicesFound( false )
		, m_uiNbVertex( 0 )
	{
	}

	ImporterUPtr SMaxImporter::create( Engine & p_engine )
	{
		return std::make_unique< SMaxImporter >( p_engine );
	}

	bool SMaxImporter::doImportScene( Scene & p_scene )
	{
		auto mesh = p_scene.getMeshCache().add( cuT( "Mesh_3DS" ) );
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

	bool SMaxImporter::doImportMesh( Mesh & p_mesh )
	{
		bool result{ false };
		UIntArray faces;
		RealArray sizes;
		String nodeName = m_fileName.getFileName();
		String meshName = m_fileName.getFileName();
		String materialName = m_fileName.getFileName();
		SMaxChunk currentChunk;
		m_pFile = new BinaryFile( m_fileName, File::OpenMode::eRead );

		if ( m_pFile->isOk() )
		{
			doReadChunk( &currentChunk );

			if ( currentChunk.m_eChunkId == eSMAX_CHUNK_M3DMAGIC )
			{
				doProcessNextChunk( *p_mesh.getScene(), &currentChunk, p_mesh );
				p_mesh.computeNormals();
				result = true;
			}
		}

		delete m_pFile;
		return result;
	}

	void SMaxImporter::doProcessNextChunk( Scene & p_scene, SMaxChunk * p_chunk, Mesh & p_mesh )
	{
		std::vector< uint8_t > buffer;
		SMaxChunk currentChunk;
		bool ok = true;
		String strName;

		while ( m_pFile->isOk() && p_chunk->m_ulBytesRead < p_chunk->m_ulLength && ok )
		{
			doReadChunk( &currentChunk );

			if ( !doIsValidChunk( &currentChunk, p_chunk ) )
			{
				ok = false;
			}
			else
			{
				switch ( currentChunk.m_eChunkId )
				{
				case eSMAX_CHUNK_M3D_VERSION:
					try
					{
						buffer.resize( currentChunk.m_ulLength - currentChunk.m_ulBytesRead );
						currentChunk.m_ulBytesRead += uint32_t( m_pFile->readArray( &buffer[0], currentChunk.m_ulLength - currentChunk.m_ulBytesRead ) );

						if ( ( currentChunk.m_ulLength - currentChunk.m_ulBytesRead == 4 ) && ( ( ( int * )&buffer[0] )[0] > 0x03 ) )
						{
							Logger::logDebug( cuT( "File version is over version 3 and may load incorrectly" ) );
						}
					}
					catch ( ... )
					{
						Logger::logDebug( cuT( "Exception caught while retrieving file version" ) );
						currentChunk.m_ulBytesRead = currentChunk.m_ulLength;
					}

					break;

				case eSMAX_CHUNK_MDATA:
					doProcessNextChunk( p_scene, &currentChunk, p_mesh );
					break;

				case eSMAX_CHUNK_MAT_ENTRY:
					m_iNumOfMaterials++;
					doProcessNextMaterialChunk( p_scene, &currentChunk );
					break;

				case eSMAX_CHUNK_NAMED_OBJECT:
					currentChunk.m_ulBytesRead += doGetString( strName );
					p_mesh.changeName( strName );
					break;

				case eSMAX_CHUNK_N_TRI_OBJECT:
					doProcessNextObjectChunk( p_scene, &currentChunk, p_mesh );
					break;

				case eSMAX_CHUNK_KFDATA:
					doDiscardChunk( &currentChunk );
					break;

				default:
					doDiscardChunk( &currentChunk );
					break;
				}
			}

			p_chunk->m_ulBytesRead += currentChunk.m_ulBytesRead;
		}

		if ( !ok )
		{
			doDiscardChunk( p_chunk );
		}
	}

	void SMaxImporter::doProcessNextObjectChunk( Scene & p_scene, SMaxChunk * p_chunk, Mesh & p_mesh )
	{
		SMaxChunk currentChunk;
		bool ok = true;
		SubmeshSPtr submesh = p_mesh.createSubmesh();

		while ( m_pFile->isOk() && p_chunk->m_ulBytesRead < p_chunk->m_ulLength && ok )
		{
			doReadChunk( &currentChunk );

			if ( !doIsValidChunk( &currentChunk, p_chunk ) )
			{
				ok = false;
			}
			else
			{
				switch ( currentChunk.m_eChunkId )
				{
				case eSMAX_CHUNK_POINT_ARRAY:
					doReadVertices( &currentChunk, *submesh );
					break;

				case eSMAX_CHUNK_FACE_ARRAY:
					doReadVertexIndices( p_scene, &currentChunk, *submesh );
					break;

				case eSMAX_CHUNK_MSH_MAT_GROUP:
					doReadObjectMaterial( p_scene, &currentChunk, *submesh );
					break;

				case eSMAX_CHUNK_TEX_VERTS:
					doReadUVCoordinates( &currentChunk, *submesh );
					break;

				default:
					doDiscardChunk( &currentChunk );
					break;
				}
			}

			p_chunk->m_ulBytesRead += currentChunk.m_ulBytesRead;
		}

		if ( submesh->getPointsCount() && submesh->getFaceCount() )
		{
			if ( !m_bIndicesFound )
			{
				submesh->computeFacesFromPolygonVertex();
			}

			submesh->computeContainers();
		}

		m_bIndicesFound = false;

		if ( !ok )
		{
			doDiscardChunk( p_chunk );
		}
	}

	void SMaxImporter::doProcessNextMaterialChunk( Scene & p_scene, SMaxChunk * p_chunk )
	{
		SMaxChunk currentChunk;
		MaterialSPtr pMaterial;
		Colour crDiffuse( Colour::fromRGB( 0 ) );
		Colour crAmbient( Colour::fromRGB( 0 ) );
		Colour crSpecular( Colour::fromRGB( 0 ) );
		String strMatName;
		std::map< TextureChannel, Path > strTextures;
		bool ok = true;
		bool bTwoSided = false;

		while ( m_pFile->isOk() && p_chunk->m_ulBytesRead < p_chunk->m_ulLength && ok )
		{
			doReadChunk( &currentChunk );

			if ( !doIsValidChunk( &currentChunk, p_chunk ) )
			{
				ok = false;
			}
			else
			{
				switch ( currentChunk.m_eChunkId )
				{
				case eSMAX_CHUNK_MAT_NAME:
					currentChunk.m_ulBytesRead += doGetString( strMatName );
					currentChunk.m_ulBytesRead = currentChunk.m_ulLength;
					break;

				case eSMAX_CHUNK_MAT_AMBIENT:
					doReadColorChunk( &currentChunk, crAmbient );
					break;

				case eSMAX_CHUNK_MAT_DIFFUSE:
					doReadColorChunk( &currentChunk, crDiffuse );
					break;

				case eSMAX_CHUNK_MAT_SPECULAR:
					doReadColorChunk( &currentChunk, crSpecular );
					break;

				case eSMAX_CHUNK_MAT_SHININESS:
					break;

				case eSMAX_CHUNK_MAT_TWO_SIDE:
					bTwoSided = true;
					break;

				case eSMAX_CHUNK_MAT_TEXMAP:
					doProcessMaterialMapChunk( &currentChunk, strTextures[TextureChannel::eDiffuse] );
					Logger::logDebug( cuT( "Diffuse map: " ) + strTextures[TextureChannel::eDiffuse] );
					break;

				case eSMAX_CHUNK_MAT_SPECMAP:
					doProcessMaterialMapChunk( &currentChunk, strTextures[TextureChannel::eSpecular] );
					Logger::logDebug( cuT( "Specular map: " ) + strTextures[TextureChannel::eSpecular] );
					break;

				case eSMAX_CHUNK_MAT_OPACMAP:
					doProcessMaterialMapChunk( &currentChunk, strTextures[TextureChannel::eOpacity] );
					Logger::logDebug( cuT( "Opacity map: " ) + strTextures[TextureChannel::eOpacity] );
					break;

				case eSMAX_CHUNK_MAT_BUMPMAP:
					doProcessMaterialMapChunk( &currentChunk, strTextures[TextureChannel::eNormal] );
					Logger::logDebug( cuT( "Normal map: " ) + strTextures[TextureChannel::eNormal] );
					break;

				case eSMAX_CHUNK_MAT_MAPNAME:
					currentChunk.m_ulBytesRead += doGetString( strTextures[TextureChannel::eDiffuse] );
					currentChunk.m_ulBytesRead = currentChunk.m_ulLength;
					Logger::logDebug( cuT( "Texture: " ) + strTextures[TextureChannel::eDiffuse] );
					break;

				default:
					doDiscardChunk( &currentChunk );
					break;
				}
			}

			p_chunk->m_ulBytesRead += currentChunk.m_ulBytesRead;
		}

		if ( !ok )
		{
			doDiscardChunk( p_chunk );
		}

		if ( !strMatName.empty() )
		{
			auto & cache = p_scene.getMaterialView();
			pMaterial = cache.find( strMatName );

			if ( !pMaterial )
			{
				pMaterial = cache.add( strMatName, MaterialType::eLegacy );
				pMaterial->createPass();
			}
			
			REQUIRE( pMaterial->getType() == MaterialType::eLegacy );
			auto pass = pMaterial->getTypedPass< MaterialType::eLegacy >( 0u );
			pass->setDiffuse( crDiffuse );
			pass->setSpecular( crSpecular );
			pass->setTwoSided( bTwoSided );
			String strTexture;

			for ( auto it : strTextures )
			{
				auto strTexture = it.second;

				if ( !strTexture.empty() )
				{
					loadTexture( strTexture, *pass, it.first );
				}
			}

			p_scene.getListener().postEvent( makeInitialiseEvent( *pMaterial ) );
		}
	}

	void SMaxImporter::doProcessMaterialMapChunk( SMaxChunk * p_chunk, String & p_strName )
	{
		SMaxChunk currentChunk;
		bool ok = true;

		while ( m_pFile->isOk() && p_chunk->m_ulBytesRead < p_chunk->m_ulLength && ok )
		{
			doReadChunk( &currentChunk );

			if ( !doIsValidChunk( &currentChunk, p_chunk ) )
			{
				ok = false;
			}
			else
			{
				switch ( currentChunk.m_eChunkId )
				{
				case eSMAX_CHUNK_MAT_MAPNAME:
					currentChunk.m_ulBytesRead += doGetString( p_strName );
					currentChunk.m_ulBytesRead = currentChunk.m_ulLength;
					break;

				default:
					doDiscardChunk( &currentChunk );
					break;
				}
			}

			p_chunk->m_ulBytesRead += currentChunk.m_ulBytesRead;
		}

		if ( !ok )
		{
			doDiscardChunk( p_chunk );
		}
	}

	void SMaxImporter::doReadChunk( SMaxChunk * p_chunk )
	{
		try
		{
			uint16_t usId;
			p_chunk->m_ulBytesRead = uint32_t( m_pFile->read( usId ) );
			p_chunk->m_eChunkId = eSMAX_CHUNK( usId );
			p_chunk->m_ulBytesRead += uint32_t( m_pFile->read( p_chunk->m_ulLength ) );
		}
		catch ( ... )
		{
			Logger::logWarning( cuT( "Exception caught when reading chunk" ) );
		}
	}

	int SMaxImporter::doGetString( String & p_strString )
	{
		char pBuffer[255]{};

		try
		{
			int index = 0;
			m_pFile->read( *pBuffer );

			while ( m_pFile->isOk() && *( pBuffer + index++ ) != 0 )
			{
				m_pFile->read( *( pBuffer + index ) );
			}

			p_strString = string::stringCast< xchar >( pBuffer );
		}
		catch ( ... )
		{
			Logger::logWarning( cuT( "Exception caught when loading string" ) );
		}

		return int( strlen( pBuffer ) + 1 );
	}

	void SMaxImporter::doReadColorChunk( SMaxChunk * p_chunk, Colour & p_colour )
	{
		SMaxChunk tempChunk;

		try
		{
			doReadChunk( &tempChunk );

			if ( tempChunk.m_eChunkId == eSMAX_CHUNK_COLOR_F )
			{
				float pColour[3];
				tempChunk.m_ulBytesRead += uint32_t( m_pFile->readArray( pColour, 3 ) );
				p_colour.red() = pColour[0];
				p_colour.green() = pColour[1];
				p_colour.blue() = pColour[2];
			}
			else if ( tempChunk.m_eChunkId == eSMAX_CHUNK_COLOR_24 )
			{
				uint8_t pColour[3];
				tempChunk.m_ulBytesRead += uint32_t( m_pFile->readArray( pColour, 3 ) );
				p_colour.red() = pColour[0];
				p_colour.green() = pColour[1];
				p_colour.blue() = pColour[2];
			}

			p_chunk->m_ulBytesRead += tempChunk.m_ulBytesRead;
		}
		catch ( ... )
		{
			Logger::logWarning( cuT( "Exception caught when loading colour" ) );
			p_chunk->m_ulBytesRead = p_chunk->m_ulLength;
		}
	}

	void SMaxImporter::doReadVertexIndices( Scene & p_scene, SMaxChunk * p_chunk, Submesh & p_submesh )
	{
		std::vector< uint32_t > arrayGroups;
		std::vector< FaceIndices > arrayFaces;
		BufferElementGroupSPtr pV1;
		BufferElementGroupSPtr pV2;
		BufferElementGroupSPtr pV3;
		uint16_t usIndices[4];
		FaceIndices face = { 0 };
		FaceSPtr pFace;
		SMaxChunk currentChunk;
		String strMatName;

		try
		{
			uint16_t usNumOfFaces = 0;
			p_chunk->m_ulBytesRead += uint32_t( m_pFile->read( usNumOfFaces ) );
			arrayFaces.resize( usNumOfFaces );
			Logger::logDebug( StringStream() << cuT( "VertexIndices: " ) << usNumOfFaces );

			for ( uint16_t i = 0; i < usNumOfFaces && m_pFile->isOk() && p_chunk->m_ulBytesRead < p_chunk->m_ulLength; i++ )
			{
				p_chunk->m_ulBytesRead += uint32_t( m_pFile->readArray( usIndices ) );
				arrayFaces[i].m_index[0] = usIndices[0];
				arrayFaces[i].m_index[1] = usIndices[2];
				arrayFaces[i].m_index[2] = usIndices[1];
			}

			m_bIndicesFound = true;

			if ( m_pFile->isOk() && p_chunk->m_ulBytesRead < p_chunk->m_ulLength )
			{
				bool bFace = true;

				// We have not reached the end of the chunk, so we try to retrieve remaining informations, beginning with uncounted faces (???)
				while ( m_pFile->isOk() && p_chunk->m_ulBytesRead + 6 < p_chunk->m_ulLength && bFace )
				{
					doReadChunk( &currentChunk );

					if ( currentChunk.m_eChunkId == eSMAX_CHUNK_SMOOTH_GROUP )
					{
						// The retrieved id tells the chunk might describe a smoothing group, so we check the length of the chunk
						// That length must be equal to (Faces count) * 4, as a smoothing group is a bitfield with 4 bytes for each face
						if ( doIsValidChunk( &currentChunk, p_chunk ) && currentChunk.m_ulLength == usNumOfFaces * sizeof( int ) )
						{
							// The chunk effectively describes a smoothing group
							bFace = false;
						}
						else
						{
							// The chunk doesn't describe a smoothing group, we suppose it is a face
							bFace = true;
						}
					}
					else if ( currentChunk.m_eChunkId == eSMAX_CHUNK_MSH_MAT_GROUP )
					{
						// The retrieved id tells the chunk might describe a material group, so we try to parse it
						if ( doIsValidChunk( &currentChunk, p_chunk ) )
						{
							bFace = false;
						}
						else
						{
							// The chunk isn't valid, we suppose it is a face
							bFace = true;
						}
					}

					if ( bFace )
					{
						// The chunk describes a triangle
						face.m_index[0] = currentChunk.m_eChunkId;
						face.m_index[1] = static_cast< uint16_t >( ( currentChunk.m_ulLength & 0x0000FFFF ) >> 0 );
						face.m_index[2] = static_cast< uint16_t >( ( currentChunk.m_ulLength & 0xFFFF0000 ) >> 16 );

						if ( p_chunk->m_ulBytesRead + 8 >= p_chunk->m_ulLength )
						{
							// We can't read a face, we discard the chunk
							p_chunk->m_ulBytesRead += currentChunk.m_ulBytesRead;
							doDiscardChunk( p_chunk );
							bFace = false;
						}
						else if ( face.m_index[0] < m_uiNbVertex && face.m_index[1] < m_uiNbVertex && face.m_index[2] < m_uiNbVertex )
						{
							// Vertex indices are correct, so we consider we really are processing a face
							arrayFaces.push_back( face );
							p_chunk->m_ulBytesRead += uint32_t( m_pFile->read( usIndices[3] ) );
							p_chunk->m_ulBytesRead += currentChunk.m_ulBytesRead;
						}
						else
						{
							// Vertex indices retrieved are incorrect, we get out from this loop, keeping the chunk for further processing
							bFace = false;
						}
					}
				}

				std::size_t uiNbFaces = arrayFaces.size();
				arrayGroups.resize( uiNbFaces );

				if ( p_chunk->m_ulBytesRead < p_chunk->m_ulLength )
				{
					// We have finished parsing remaining faces, and we have a chunk waiting for it's processing
					do
					{
						if ( currentChunk.m_eChunkId == eSMAX_CHUNK_SMOOTH_GROUP )
						{
							currentChunk.m_ulBytesRead += uint32_t( m_pFile->readArray( &arrayGroups[0], uiNbFaces ) );

							//for( std::size_t i = 0 ; i < uiNbFaces && p_chunk->m_ulBytesRead < p_chunk->m_ulLength ; ++i )
							//{
							//	itGroups = mapGroupsById.find( arrayGroups[i] );

							//	if( itGroups == mapGroupsById.end() )
							//	{
							//		mapGroupsById.insert( std::make_pair( arrayGroups[i], p_submesh.AddSmoothingGroup() ) );
							//	}
							//}

							p_chunk->m_ulBytesRead += currentChunk.m_ulBytesRead;

							if ( p_chunk->m_ulBytesRead + 6 < p_chunk->m_ulLength )
							{
								doReadChunk( &currentChunk );
							}
							else
							{
								doDiscardChunk( p_chunk );
							}
						}
						else if ( currentChunk.m_eChunkId == eSMAX_CHUNK_MSH_MAT_GROUP )
						{
							doReadObjectMaterial( p_scene, &currentChunk, p_submesh );
							p_chunk->m_ulBytesRead += currentChunk.m_ulBytesRead;

							if ( p_chunk->m_ulBytesRead + 6 < p_chunk->m_ulLength )
							{
								doReadChunk( &currentChunk );
							}
							else
							{
								doDiscardChunk( p_chunk );
							}
						}
						else if ( doIsValidChunk( &currentChunk, p_chunk ) )
						{
							doDiscardChunk( &currentChunk );
							p_chunk->m_ulBytesRead += currentChunk.m_ulBytesRead;

							if ( p_chunk->m_ulBytesRead + 6 < p_chunk->m_ulLength )
							{
								doReadChunk( &currentChunk );
							}
							else
							{
								doDiscardChunk( p_chunk );
							}
						}
						else
						{
							// We decal the chunk of 2 bytes on the right
							uint16_t usDump = 0;
							p_chunk->m_ulBytesRead += 2;
							currentChunk.m_eChunkId = eSMAX_CHUNK( static_cast< uint16_t >( currentChunk.m_ulLength & 0xFFFF0000 >> 16 ) );
							m_pFile->read( usDump );
							currentChunk.m_ulLength = ( ( currentChunk.m_ulLength & 0x0000FFFF ) << 16 ) + usDump;
						}
					}
					while ( m_pFile->isOk() && p_chunk->m_ulBytesRead < p_chunk->m_ulLength );
				}
			}

			for ( std::vector< FaceIndices >::const_iterator it = arrayFaces.begin(); it != arrayFaces.end(); ++it )
			{
				// It seems faces are inverted in 3DS so I invert the indices to fall back in a good order
				uint32_t uiV1 = it->m_index[0];
				uint32_t uiV2 = it->m_index[1];
				uint32_t uiV3 = it->m_index[2];
				pV1 = p_submesh.getPoint( uiV1 );
				pV2 = p_submesh.getPoint( uiV2 );
				pV3 = p_submesh.getPoint( uiV3 );
				p_submesh.addFace( uiV1, uiV2, uiV3 );

				if ( m_arrayTexVerts.size() )
				{
					Vertex::setTexCoord( pV1, m_arrayTexVerts[( uiV1 * 2 ) + 0], m_arrayTexVerts[( uiV1 * 2 ) + 1] );
					Vertex::setTexCoord( pV2, m_arrayTexVerts[( uiV2 * 2 ) + 0], m_arrayTexVerts[( uiV2 * 2 ) + 1] );
					Vertex::setTexCoord( pV3, m_arrayTexVerts[( uiV3 * 2 ) + 0], m_arrayTexVerts[( uiV3 * 2 ) + 1] );
				}
			}
		}
		catch ( std::exception & exc )
		{
			Logger::logWarning( std::stringstream() << "Exception caught when loading vertex indices: " << exc.what() );
			p_chunk->m_ulBytesRead = p_chunk->m_ulLength;
		}
		catch ( ... )
		{
			Logger::logWarning( cuT( "Exception caught when loading vertex indices" ) );
			p_chunk->m_ulBytesRead = p_chunk->m_ulLength;
		}
	}

	void SMaxImporter::doReadUVCoordinates( SMaxChunk * p_chunk, Submesh & p_submesh )
	{
		try
		{
			uint16_t usNumTexVertex = 0;
			uint32_t uiNumTexVertex = 0;
			p_chunk->m_ulBytesRead += uint32_t( m_pFile->read( usNumTexVertex ) );
			uiNumTexVertex = usNumTexVertex;
			m_arrayTexVerts.resize( uiNumTexVertex * 2 );
			Logger::logDebug( StringStream() << cuT( "TexCoords: " ) << usNumTexVertex );
			p_chunk->m_ulBytesRead += uint32_t( m_pFile->readArray( &m_arrayTexVerts[0], uiNumTexVertex * 2 ) );
		}
		catch ( std::exception & exc )
		{
			Logger::logWarning( std::stringstream() << "Exception caught when loading uv indices: " << exc.what() );
			p_chunk->m_ulBytesRead = p_chunk->m_ulLength;
		}
		catch ( ... )
		{
			Logger::logWarning( cuT( "Exception caught when loading uv coordinates" ) );
			p_chunk->m_ulBytesRead = p_chunk->m_ulLength;
		}
	}

	void SMaxImporter::doReadVertices( SMaxChunk * p_chunk, Submesh & p_submesh )
	{
		try
		{
			float * pTmp = NULL;
			std::vector< float > pVertex;
			uint16_t usNumOfVerts = 0;
			p_chunk->m_ulBytesRead += uint32_t( m_pFile->read( usNumOfVerts ) );
			m_uiNbVertex = usNumOfVerts;

			if ( m_uiNbVertex > 0 )
			{
				pVertex.resize( m_uiNbVertex * 3 );
				pTmp = &pVertex[0];
				p_chunk->m_ulBytesRead += uint32_t( m_pFile->readArray( pTmp, m_uiNbVertex * 3 ) );
			}

			Logger::logDebug( StringStream() << cuT( "Vertices: " ) << usNumOfVerts );

			for ( uint16_t i = 0; i < usNumOfVerts; i++ )
			{
				p_submesh.addPoint( pTmp[0], pTmp[2], -pTmp[1] );
				pTmp += 3;
			}
		}
		catch ( std::exception & exc )
		{
			Logger::logWarning( std::stringstream() << "Exception caught when loading vertices: " << exc.what() );
			p_chunk->m_ulBytesRead = p_chunk->m_ulLength;
		}
		catch ( ... )
		{
			Logger::logWarning( cuT( "Exception caught when loading vertices" ) );
			p_chunk->m_ulBytesRead = p_chunk->m_ulLength;
		}
	}

	void SMaxImporter::doReadObjectMaterial( Scene & p_scene, SMaxChunk * p_chunk, Submesh & p_submesh )
	{
		String materialName;
		MaterialSPtr pMaterial;
		p_chunk->m_ulBytesRead += doGetString( materialName );
		auto & cache = p_scene.getMaterialView();
		pMaterial = cache.find( materialName );

		if ( pMaterial && !p_submesh.getDefaultMaterial() )
		{
			p_submesh.setDefaultMaterial( pMaterial );
		}

		doDiscardChunk( p_chunk );
	}

	void SMaxImporter::doDiscardChunk( SMaxChunk * p_chunk )
	{
		if ( p_chunk->m_ulLength > p_chunk->m_ulBytesRead )
		{
			uint32_t uiSize = p_chunk->m_ulLength - p_chunk->m_ulBytesRead;
			std::vector< uint8_t > buffer( uiSize );

			try
			{
				if ( uiSize <= uint32_t( m_pFile->getLength() - m_pFile->tell() ) && uiSize > 0 )
				{
					m_pFile->readArray( &buffer[0], uiSize );
				}
				else
				{
					m_pFile->readArray( &buffer[0], std::size_t( m_pFile->getLength() - m_pFile->tell() ) );
					throw std::range_error( "Bad chunk size" );
				}
			}
			catch ( std::exception & exc )
			{
				Logger::logWarning( StringStream() << "Exception caught when discarding chunk: " << exc.what() );
			}
			catch ( ... )
			{
				Logger::logWarning( cuT( "Exception caught when discarding chunk" ) );
			}

			p_chunk->m_ulBytesRead = p_chunk->m_ulLength;
		}
	}

	bool SMaxImporter::doIsValidChunk( SMaxChunk * p_chunk, SMaxChunk * p_pParent )
	{
		bool bReturn = false;

		switch ( p_chunk->m_eChunkId )
		{
		default:
			bReturn = false;
			break;

		case eSMAX_CHUNK_NULL_CHUNK:
		case eSMAX_CHUNK_UNKNOWN_CHUNK:
		case eSMAX_CHUNK_M3D_VERSION:
		case eSMAX_CHUNK_M3D_KFVERSION:
		case eSMAX_CHUNK_COLOR_F:
		case eSMAX_CHUNK_COLOR_24:
		case eSMAX_CHUNK_LIN_COLOR_24:
		case eSMAX_CHUNK_LIN_COLOR_F:
		case eSMAX_CHUNK_INT_PERCENTAGE:
		case eSMAX_CHUNK_FLOAT_PERCENTAGE:
		case eSMAX_CHUNK_MASTER_SCALE:
		case eSMAX_CHUNK_CHUNKTYPE:
		case eSMAX_CHUNK_CHUNKUNIQUE:
		case eSMAX_CHUNK_NOTCHUNK:
		case eSMAX_CHUNK_CONTAINER:
		case eSMAX_CHUNK_ISCHUNK:
		case eSMAX_CHUNK_C_SXP_SELFI_MASKDATA:
		case eSMAX_CHUNK_BIT_MAP:
		case eSMAX_CHUNK_USE_BIT_MAP:
		case eSMAX_CHUNK_SOLID_BGND:
		case eSMAX_CHUNK_USE_SOLID_BGND:
		case eSMAX_CHUNK_V_GRADIENT:
		case eSMAX_CHUNK_USE_V_GRADIENT:
		case eSMAX_CHUNK_LO_SHADOW_BIAS:
		case eSMAX_CHUNK_HI_SHADOW_BIAS:
		case eSMAX_CHUNK_SHADOW_MAP_SIZE:
		case eSMAX_CHUNK_SHADOW_SAMPLES:
		case eSMAX_CHUNK_SHADOW_RANGE:
		case eSMAX_CHUNK_SHADOW_FILTER:
		case eSMAX_CHUNK_RAY_BIAS:
		case eSMAX_CHUNK_O_CONSTS:
		case eSMAX_CHUNK_AMBIENT_LIGHT:
		case eSMAX_CHUNK_FOG:
		case eSMAX_CHUNK_USE_FOG:
		case eSMAX_CHUNK_FOG_BGND:
		case eSMAX_CHUNK_DISTANCE_CUE:
		case eSMAX_CHUNK_USE_DISTANCE_CUE:
		case eSMAX_CHUNK_LAYER_FOG:
		case eSMAX_CHUNK_USE_LAYER_FOG:
		case eSMAX_CHUNK_DCUE_BGND:
		case eSMAX_CHUNK_SMAGIC:
		case eSMAX_CHUNK_LMAGIC:
		case eSMAX_CHUNK_DEFAULT_VIEW:
		case eSMAX_CHUNK_VIEW_TOP:
		case eSMAX_CHUNK_VIEW_BOTTOM:
		case eSMAX_CHUNK_VIEW_LEFT:
		case eSMAX_CHUNK_VIEW_RIGHT:
		case eSMAX_CHUNK_VIEW_FRONT:
		case eSMAX_CHUNK_VIEW_BACK:
		case eSMAX_CHUNK_VIEW_USER:
		case eSMAX_CHUNK_VIEW_CAMERA:
		case eSMAX_CHUNK_VIEW_WINDOW:
		case eSMAX_CHUNK_MDATA:
		case eSMAX_CHUNK_MESH_VERSION:
		case eSMAX_CHUNK_MLIBMAGIC:
		case eSMAX_CHUNK_PRJMAGIC:
		case eSMAX_CHUNK_MATMAGIC:
		case eSMAX_CHUNK_NAMED_OBJECT:
		case eSMAX_CHUNK_OBJ_HIDDEN:
		case eSMAX_CHUNK_OBJ_VIS_LOFTER:
		case eSMAX_CHUNK_OBJ_DOESNT_CAST:
		case eSMAX_CHUNK_OBJ_MATTE:
		case eSMAX_CHUNK_OBJ_FAST:
		case eSMAX_CHUNK_OBJ_PROCEDURAL:
		case eSMAX_CHUNK_OBJ_FROZEN:
		case eSMAX_CHUNK_OBJ_DONT_RCVSHADOW:
		case eSMAX_CHUNK_N_TRI_OBJECT:
		case eSMAX_CHUNK_POINT_ARRAY:
		case eSMAX_CHUNK_POINT_FLAG_ARRAY:
		case eSMAX_CHUNK_FACE_ARRAY:
		case eSMAX_CHUNK_MSH_MAT_GROUP:
		case eSMAX_CHUNK_OLD_MAT_GROUP:
		case eSMAX_CHUNK_TEX_VERTS:
		case eSMAX_CHUNK_SMOOTH_GROUP:
		case eSMAX_CHUNK_MESH_MATRIX:
		case eSMAX_CHUNK_MESH_COLOR:
		case eSMAX_CHUNK_MESH_TEXTURE_INFO:
		case eSMAX_CHUNK_PROC_NAME:
		case eSMAX_CHUNK_PROC_DATA:
		case eSMAX_CHUNK_MSH_BOXMAP:
		case eSMAX_CHUNK_N_D_L_OLD:
		case eSMAX_CHUNK_N_CAM_OLD:
		case eSMAX_CHUNK_N_DIRECT_LIGHT:
		case eSMAX_CHUNK_DL_SPOTLIGHT:
		case eSMAX_CHUNK_DL_OFF:
		case eSMAX_CHUNK_DL_ATTENUATE:
		case eSMAX_CHUNK_DL_RAYSHAD:
		case eSMAX_CHUNK_DL_SHADOWED:
		case eSMAX_CHUNK_DL_LOCAL_SHADOW:
		case eSMAX_CHUNK_DL_LOCAL_SHADOW2:
		case eSMAX_CHUNK_DL_SEE_CONE:
		case eSMAX_CHUNK_DL_SPOT_RECTANGULAR:
		case eSMAX_CHUNK_DL_SPOT_OVERSHOOT:
		case eSMAX_CHUNK_DL_SPOT_PROJECTOR:
		case eSMAX_CHUNK_DL_EXCLUDE:
		case eSMAX_CHUNK_DL_RANGE:
		case eSMAX_CHUNK_DL_SPOT_ROLL:
		case eSMAX_CHUNK_DL_SPOT_ASPECT:
		case eSMAX_CHUNK_DL_RAY_BIAS:
		case eSMAX_CHUNK_DL_INNER_RANGE:
		case eSMAX_CHUNK_DL_OUTER_RANGE:
		case eSMAX_CHUNK_DL_MULTIPLIER:
		case eSMAX_CHUNK_N_AMBIENT_LIGHT:
		case eSMAX_CHUNK_N_CAMERA:
		case eSMAX_CHUNK_CAM_SEE_CONE:
		case eSMAX_CHUNK_CAM_RANGES:
		case eSMAX_CHUNK_M3DMAGIC:
		case eSMAX_CHUNK_HIERARCHY:
		case eSMAX_CHUNK_PARENT_OBJECT:
		case eSMAX_CHUNK_PIVOT_OBJECT:
		case eSMAX_CHUNK_PIVOT_LIMITS:
		case eSMAX_CHUNK_PIVOT_ORDER:
		case eSMAX_CHUNK_XLATE_RANGE:
		case eSMAX_CHUNK_POLY_2D:
		case eSMAX_CHUNK_SHAPE_OK:
		case eSMAX_CHUNK_SHAPE_NOT_OK:
		case eSMAX_CHUNK_SHAPE_HOOK:
		case eSMAX_CHUNK_PATH_3D:
		case eSMAX_CHUNK_PATH_MATRIX:
		case eSMAX_CHUNK_SHAPE_2D:
		case eSMAX_CHUNK_M_SCALE:
		case eSMAX_CHUNK_M_TWIST:
		case eSMAX_CHUNK_M_TEETER:
		case eSMAX_CHUNK_M_FIT:
		case eSMAX_CHUNK_M_BEVEL:
		case eSMAX_CHUNK_XZ_CURVE:
		case eSMAX_CHUNK_YZ_CURVE:
		case eSMAX_CHUNK_INTERPCT:
		case eSMAX_CHUNK_DEFORM_LIMIT:
		case eSMAX_CHUNK_USE_CONTOUR:
		case eSMAX_CHUNK_USE_TWEEN:
		case eSMAX_CHUNK_USE_SCALE:
		case eSMAX_CHUNK_USE_TWIST:
		case eSMAX_CHUNK_USE_TEETER:
		case eSMAX_CHUNK_USE_FIT:
		case eSMAX_CHUNK_USE_BEVEL:
		case eSMAX_CHUNK_VIEWPORT_LAYOUT_OLD:
		case eSMAX_CHUNK_VIEWPORT_LAYOUT:
		case eSMAX_CHUNK_VIEWPORT_DATA_OLD:
		case eSMAX_CHUNK_VIEWPORT_DATA:
		case eSMAX_CHUNK_VIEWPORT_DATA_3:
		case eSMAX_CHUNK_VIEWPORT_SIZE:
		case eSMAX_CHUNK_NETWORK_VIEW:
		case eSMAX_CHUNK_XDATA_SECTION:
		case eSMAX_CHUNK_XDATA_ENTRY:
		case eSMAX_CHUNK_XDATA_APPNAME:
		case eSMAX_CHUNK_XDATA_STRING:
		case eSMAX_CHUNK_XDATA_FLOAT:
		case eSMAX_CHUNK_XDATA_DOUBLE:
		case eSMAX_CHUNK_XDATA_SHORT:
		case eSMAX_CHUNK_XDATA_LONG:
		case eSMAX_CHUNK_XDATA_VOID:
		case eSMAX_CHUNK_XDATA_GROUP:
		case eSMAX_CHUNK_XDATA_RFU6:
		case eSMAX_CHUNK_XDATA_RFU5:
		case eSMAX_CHUNK_XDATA_RFU4:
		case eSMAX_CHUNK_XDATA_RFU3:
		case eSMAX_CHUNK_XDATA_RFU2:
		case eSMAX_CHUNK_XDATA_RFU1:
		case eSMAX_CHUNK_PARENT_NAME:
		case eSMAX_CHUNK_MAT_NAME:
		case eSMAX_CHUNK_MAT_AMBIENT:
		case eSMAX_CHUNK_MAT_DIFFUSE:
		case eSMAX_CHUNK_MAT_SPECULAR:
		case eSMAX_CHUNK_MAT_SHININESS:
		case eSMAX_CHUNK_MAT_SHIN2PCT:
		case eSMAX_CHUNK_MAT_SHIN3PCT:
		case eSMAX_CHUNK_MAT_TRANSPARENCY:
		case eSMAX_CHUNK_MAT_XPFALL:
		case eSMAX_CHUNK_MAT_REFBLUR:
		case eSMAX_CHUNK_MAT_SELF_ILLUM:
		case eSMAX_CHUNK_MAT_TWO_SIDE:
		case eSMAX_CHUNK_MAT_DECAL:
		case eSMAX_CHUNK_MAT_ADDITIVE:
		case eSMAX_CHUNK_MAT_SELF_ILPCT:
		case eSMAX_CHUNK_MAT_WIRE:
		case eSMAX_CHUNK_MAT_SUPERSMP:
		case eSMAX_CHUNK_MAT_WIRESIZE:
		case eSMAX_CHUNK_MAT_FACEMAP:
		case eSMAX_CHUNK_MAT_XPFALLIN:
		case eSMAX_CHUNK_MAT_PHONGSOFT:
		case eSMAX_CHUNK_MAT_WIREABS:
		case eSMAX_CHUNK_MAT_SHADING:
		case eSMAX_CHUNK_MAT_TEXMAP:
		case eSMAX_CHUNK_MAT_SPECMAP:
		case eSMAX_CHUNK_MAT_OPACMAP:
		case eSMAX_CHUNK_MAT_REFLMAP:
		case eSMAX_CHUNK_MAT_BUMPMAP:
		case eSMAX_CHUNK_MAT_USE_XPFALL:
		case eSMAX_CHUNK_MAT_USE_REFBLUR:
		case eSMAX_CHUNK_MAT_BUMP_PERCENT:
		case eSMAX_CHUNK_MAT_MAPNAME:
		case eSMAX_CHUNK_MAT_ACUBIC:
		case eSMAX_CHUNK_MAT_SXP_TEXT_DATA:
		case eSMAX_CHUNK_MAT_SXP_TEXT2_DATA:
		case eSMAX_CHUNK_MAT_SXP_OPAC_DATA:
		case eSMAX_CHUNK_MAT_SXP_BUMP_DATA:
		case eSMAX_CHUNK_MAT_SXP_SPEC_DATA:
		case eSMAX_CHUNK_MAT_SXP_SHIN_DATA:
		case eSMAX_CHUNK_MAT_SXP_SELFI_DATA:
		case eSMAX_CHUNK_MAT_SXP_TEXT_MASKDATA:
		case eSMAX_CHUNK_MAT_SXP_TEXT2_MASKDATA:
		case eSMAX_CHUNK_MAT_SXP_OPAC_MASKDATA:
		case eSMAX_CHUNK_MAT_SXP_BUMP_MASKDATA:
		case eSMAX_CHUNK_MAT_SXP_SPEC_MASKDATA:
		case eSMAX_CHUNK_MAT_SXP_SHIN_MASKDATA:
		case eSMAX_CHUNK_MAT_SXP_SELFI_MASKDATA:
		case eSMAX_CHUNK_MAT_SXP_REFL_MASKDATA:
		case eSMAX_CHUNK_MAT_TEX2MAP:
		case eSMAX_CHUNK_MAT_SHINMAP:
		case eSMAX_CHUNK_MAT_SELFIMAP:
		case eSMAX_CHUNK_MAT_TEXMASK:
		case eSMAX_CHUNK_MAT_TEX2MASK:
		case eSMAX_CHUNK_MAT_OPACMASK:
		case eSMAX_CHUNK_MAT_BUMPMASK:
		case eSMAX_CHUNK_MAT_SHINMASK:
		case eSMAX_CHUNK_MAT_SPECMASK:
		case eSMAX_CHUNK_MAT_SELFIMASK:
		case eSMAX_CHUNK_MAT_REFLMASK:
		case eSMAX_CHUNK_MAT_MAP_TILINGOLD:
		case eSMAX_CHUNK_MAT_MAP_TILING:
		case eSMAX_CHUNK_MAT_MAP_TEXBLUR_OLD:
		case eSMAX_CHUNK_MAT_MAP_TEXBLUR:
		case eSMAX_CHUNK_MAT_MAP_USCALE:
		case eSMAX_CHUNK_MAT_MAP_VSCALE:
		case eSMAX_CHUNK_MAT_MAP_UOFFSET:
		case eSMAX_CHUNK_MAT_MAP_VOFFSET:
		case eSMAX_CHUNK_MAT_MAP_ANG:
		case eSMAX_CHUNK_MAT_MAP_COL1:
		case eSMAX_CHUNK_MAT_MAP_COL2:
		case eSMAX_CHUNK_MAT_MAP_RCOL:
		case eSMAX_CHUNK_MAT_MAP_GCOL:
		case eSMAX_CHUNK_MAT_MAP_BCOL:
		case eSMAX_CHUNK_MAT_ENTRY:
		case eSMAX_CHUNK_KFDATA:
		case eSMAX_CHUNK_AMBIENT_NODE_TAG:
		case eSMAX_CHUNK_OBJECT_NODE_TAG:
		case eSMAX_CHUNK_CAMERA_NODE_TAG:
		case eSMAX_CHUNK_TARGET_NODE_TAG:
		case eSMAX_CHUNK_LIGHT_NODE_TAG:
		case eSMAX_CHUNK_L_TARGET_NODE_TAG:
		case eSMAX_CHUNK_SPOTLIGHT_NODE_TAG:
		case eSMAX_CHUNK_KFSEG:
		case eSMAX_CHUNK_KFCURTIME:
		case eSMAX_CHUNK_KFHDR:
		case eSMAX_CHUNK_NODE_HDR:
		case eSMAX_CHUNK_INSTANCE_NAME:
		case eSMAX_CHUNK_PRESCALE:
		case eSMAX_CHUNK_PIVOT:
		case eSMAX_CHUNK_BOUNDBOX:
		case eSMAX_CHUNK_MORPH_SMOOTH:
		case eSMAX_CHUNK_POS_TRACK_TAG:
		case eSMAX_CHUNK_ROT_TRACK_TAG:
		case eSMAX_CHUNK_SCL_TRACK_TAG:
		case eSMAX_CHUNK_FOV_TRACK_TAG:
		case eSMAX_CHUNK_ROLL_TRACK_TAG:
		case eSMAX_CHUNK_COL_TRACK_TAG:
		case eSMAX_CHUNK_MORPH_TRACK_TAG:
		case eSMAX_CHUNK_HOT_TRACK_TAG:
		case eSMAX_CHUNK_FALL_TRACK_TAG:
		case eSMAX_CHUNK_HIDE_TRACK_TAG:
		case eSMAX_CHUNK_NODE_ID:
		case eSMAX_CHUNK_C_MDRAWER:
		case eSMAX_CHUNK_C_TDRAWER:
		case eSMAX_CHUNK_C_SHPDRAWER:
		case eSMAX_CHUNK_C_MODDRAWER:
		case eSMAX_CHUNK_C_RIPDRAWER:
		case eSMAX_CHUNK_C_TXDRAWER:
		case eSMAX_CHUNK_C_PDRAWER:
		case eSMAX_CHUNK_C_MTLDRAWER:
		case eSMAX_CHUNK_C_FLIDRAWER:
		case eSMAX_CHUNK_C_CUBDRAWER:
		case eSMAX_CHUNK_C_MFILE:
		case eSMAX_CHUNK_C_SHPFILE:
		case eSMAX_CHUNK_C_MODFILE:
		case eSMAX_CHUNK_C_RIPFILE:
		case eSMAX_CHUNK_C_TXFILE:
		case eSMAX_CHUNK_C_PFILE:
		case eSMAX_CHUNK_C_MTLFILE:
		case eSMAX_CHUNK_C_FLIFILE:
		case eSMAX_CHUNK_C_PALFILE:
		case eSMAX_CHUNK_C_TX_STRING:
		case eSMAX_CHUNK_C_CONSTS:
		case eSMAX_CHUNK_C_SNAPS:
		case eSMAX_CHUNK_C_GRIDS:
		case eSMAX_CHUNK_C_ASNAPS:
		case eSMAX_CHUNK_C_GRID_RANGE:
		case eSMAX_CHUNK_C_RENDTYPE:
		case eSMAX_CHUNK_C_PROGMODE:
		case eSMAX_CHUNK_C_PREVMODE:
		case eSMAX_CHUNK_C_MODWMODE:
		case eSMAX_CHUNK_C_MODMODEL:
		case eSMAX_CHUNK_C_ALL_LINES:
		case eSMAX_CHUNK_C_BACK_TYPE:
		case eSMAX_CHUNK_C_MD_CS:
		case eSMAX_CHUNK_C_MD_CE:
		case eSMAX_CHUNK_C_MD_SML:
		case eSMAX_CHUNK_C_MD_SMW:
		case eSMAX_CHUNK_C_LOFT_WITH_TEXTURE:
		case eSMAX_CHUNK_C_LOFT_L_REPEAT:
		case eSMAX_CHUNK_C_LOFT_W_REPEAT:
		case eSMAX_CHUNK_C_LOFT_UV_NORMALIZE:
		case eSMAX_CHUNK_C_WELD_LOFT:
		case eSMAX_CHUNK_C_MD_PDET:
		case eSMAX_CHUNK_C_MD_SDET:
		case eSMAX_CHUNK_C_RGB_RMODE:
		case eSMAX_CHUNK_C_RGB_HIDE:
		case eSMAX_CHUNK_C_RGB_MAPSW:
		case eSMAX_CHUNK_C_RGB_TWOSIDE:
		case eSMAX_CHUNK_C_RGB_SHADOW:
		case eSMAX_CHUNK_C_RGB_AA:
		case eSMAX_CHUNK_C_RGB_OVW:
		case eSMAX_CHUNK_C_RGB_OVH:
		case eSMAX_CHUNK_CMAGIC:
		case eSMAX_CHUNK_C_RGB_PICTYPE:
		case eSMAX_CHUNK_C_RGB_OUTPUT:
		case eSMAX_CHUNK_C_RGB_TODISK:
		case eSMAX_CHUNK_C_RGB_COMPRESS:
		case eSMAX_CHUNK_C_JPEG_COMPRESSION:
		case eSMAX_CHUNK_C_RGB_DISPDEV:
		case eSMAX_CHUNK_C_RGB_HARDDEV:
		case eSMAX_CHUNK_C_RGB_PATH:
		case eSMAX_CHUNK_C_BITMAP_DRAWER:
		case eSMAX_CHUNK_C_RGB_FILE:
		case eSMAX_CHUNK_C_RGB_OVASPECT:
		case eSMAX_CHUNK_C_RGB_ANIMTYPE:
		case eSMAX_CHUNK_C_RENDER_ALL:
		case eSMAX_CHUNK_C_REND_FROM:
		case eSMAX_CHUNK_C_REND_TO:
		case eSMAX_CHUNK_C_REND_NTH:
		case eSMAX_CHUNK_C_PAL_TYPE:
		case eSMAX_CHUNK_C_RND_TURBO:
		case eSMAX_CHUNK_C_RND_MIP:
		case eSMAX_CHUNK_C_BGND_METHOD:
		case eSMAX_CHUNK_C_AUTO_REFLECT:
		case eSMAX_CHUNK_C_VP_FROM:
		case eSMAX_CHUNK_C_VP_TO:
		case eSMAX_CHUNK_C_VP_NTH:
		case eSMAX_CHUNK_C_REND_TSTEP:
		case eSMAX_CHUNK_C_VP_TSTEP:
		case eSMAX_CHUNK_C_SRDIAM:
		case eSMAX_CHUNK_C_SRDEG:
		case eSMAX_CHUNK_C_SRSEG:
		case eSMAX_CHUNK_C_SRDIR:
		case eSMAX_CHUNK_C_HETOP:
		case eSMAX_CHUNK_C_HEBOT:
		case eSMAX_CHUNK_C_HEHT:
		case eSMAX_CHUNK_C_HETURNS:
		case eSMAX_CHUNK_C_HEDEG:
		case eSMAX_CHUNK_C_HESEG:
		case eSMAX_CHUNK_C_HEDIR:
		case eSMAX_CHUNK_C_QUIKSTUFF:
		case eSMAX_CHUNK_C_SEE_LIGHTS:
		case eSMAX_CHUNK_C_SEE_CAMERAS:
		case eSMAX_CHUNK_C_SEE_3D:
		case eSMAX_CHUNK_C_MESHSEL:
		case eSMAX_CHUNK_C_MESHUNSEL:
		case eSMAX_CHUNK_C_POLYSEL:
		case eSMAX_CHUNK_C_POLYUNSEL:
		case eSMAX_CHUNK_C_SHPLOCAL:
		case eSMAX_CHUNK_C_MSHLOCAL:
		case eSMAX_CHUNK_C_NUM_FORMAT:
		case eSMAX_CHUNK_C_ARCH_DENOM:
		case eSMAX_CHUNK_C_IN_DEVICE:
		case eSMAX_CHUNK_C_MSCALE:
		case eSMAX_CHUNK_C_COMM_PORT:
		case eSMAX_CHUNK_C_TAB_BASES:
		case eSMAX_CHUNK_C_TAB_DIVS:
		case eSMAX_CHUNK_C_MASTER_SCALES:
		case eSMAX_CHUNK_C_SHOW_1STVERT:
		case eSMAX_CHUNK_C_SHAPER_OK:
		case eSMAX_CHUNK_C_LOFTER_OK:
		case eSMAX_CHUNK_C_EDITOR_OK:
		case eSMAX_CHUNK_C_KEYFRAMER_OK:
		case eSMAX_CHUNK_C_PICKSIZE:
		case eSMAX_CHUNK_C_MAPTYPE:
		case eSMAX_CHUNK_C_MAP_DISPLAY:
		case eSMAX_CHUNK_C_TILE_XY:
		case eSMAX_CHUNK_C_MAP_XYZ:
		case eSMAX_CHUNK_C_MAP_SCALE:
		case eSMAX_CHUNK_C_MAP_MATRIX_OLD:
		case eSMAX_CHUNK_C_MAP_MATRIX:
		case eSMAX_CHUNK_C_MAP_WID_HT:
		case eSMAX_CHUNK_C_OBNAME:
		case eSMAX_CHUNK_C_CAMNAME:
		case eSMAX_CHUNK_C_LTNAME:
		case eSMAX_CHUNK_C_CUR_MNAME:
		case eSMAX_CHUNK_C_CURMTL_FROM_MESH:
		case eSMAX_CHUNK_C_GET_SHAPE_MAKE_FACES:
		case eSMAX_CHUNK_C_DETAIL:
		case eSMAX_CHUNK_C_VERTMARK:
		case eSMAX_CHUNK_C_MSHAX:
		case eSMAX_CHUNK_C_MSHCP:
		case eSMAX_CHUNK_C_USERAX:
		case eSMAX_CHUNK_C_SHOOK:
		case eSMAX_CHUNK_C_RAX:
		case eSMAX_CHUNK_C_STAPE:
		case eSMAX_CHUNK_C_LTAPE:
		case eSMAX_CHUNK_C_ETAPE:
		case eSMAX_CHUNK_C_KTAPE:
		case eSMAX_CHUNK_C_SPHSEGS:
		case eSMAX_CHUNK_C_GEOSMOOTH:
		case eSMAX_CHUNK_C_HEMISEGS:
		case eSMAX_CHUNK_C_PRISMSEGS:
		case eSMAX_CHUNK_C_PRISMSIDES:
		case eSMAX_CHUNK_C_TUBESEGS:
		case eSMAX_CHUNK_C_TUBESIDES:
		case eSMAX_CHUNK_C_TORSEGS:
		case eSMAX_CHUNK_C_TORSIDES:
		case eSMAX_CHUNK_C_CONESIDES:
		case eSMAX_CHUNK_C_CONESEGS:
		case eSMAX_CHUNK_C_NGPARMS:
		case eSMAX_CHUNK_C_PTHLEVEL:
		case eSMAX_CHUNK_C_MSCSYM:
		case eSMAX_CHUNK_C_MFTSYM:
		case eSMAX_CHUNK_C_MTTSYM:
		case eSMAX_CHUNK_C_SMOOTHING:
		case eSMAX_CHUNK_C_MODICOUNT:
		case eSMAX_CHUNK_C_FONTSEL:
		case eSMAX_CHUNK_C_TESS_TYPE:
		case eSMAX_CHUNK_C_TESS_TENSION:
		case eSMAX_CHUNK_C_SEG_START:
		case eSMAX_CHUNK_C_SEG_END:
		case eSMAX_CHUNK_C_CURTIME:
		case eSMAX_CHUNK_C_ANIMLENGTH:
		case eSMAX_CHUNK_C_PV_FROM:
		case eSMAX_CHUNK_C_PV_TO:
		case eSMAX_CHUNK_C_PV_DOFNUM:
		case eSMAX_CHUNK_C_PV_RNG:
		case eSMAX_CHUNK_C_PV_NTH:
		case eSMAX_CHUNK_C_PV_TYPE:
		case eSMAX_CHUNK_C_PV_METHOD:
		case eSMAX_CHUNK_C_PV_FPS:
		case eSMAX_CHUNK_C_VTR_FRAMES:
		case eSMAX_CHUNK_C_VTR_HDTL:
		case eSMAX_CHUNK_C_VTR_HD:
		case eSMAX_CHUNK_C_VTR_TL:
		case eSMAX_CHUNK_C_VTR_IN:
		case eSMAX_CHUNK_C_VTR_PK:
		case eSMAX_CHUNK_C_VTR_SH:
		case eSMAX_CHUNK_C_WORK_MTLS:
		case eSMAX_CHUNK_C_WORK_MTLS_2:
		case eSMAX_CHUNK_C_WORK_MTLS_3:
		case eSMAX_CHUNK_C_WORK_MTLS_4:
		case eSMAX_CHUNK_C_BGTYPE:
		case eSMAX_CHUNK_C_MEDTILE:
		case eSMAX_CHUNK_C_LO_CONTRAST:
		case eSMAX_CHUNK_C_HI_CONTRAST:
		case eSMAX_CHUNK_C_FROZ_DISPLAY:
		case eSMAX_CHUNK_C_BOOLWELD:
		case eSMAX_CHUNK_C_BOOLTYPE:
		case eSMAX_CHUNK_C_ANG_THRESH:
		case eSMAX_CHUNK_C_SS_THRESH:
		case eSMAX_CHUNK_C_TEXTURE_BLUR_DEFAULT:
		case eSMAX_CHUNK_C_MAPDRAWER:
		case eSMAX_CHUNK_C_MAPDRAWER1:
		case eSMAX_CHUNK_C_MAPDRAWER2:
		case eSMAX_CHUNK_C_MAPDRAWER3:
		case eSMAX_CHUNK_C_MAPDRAWER4:
		case eSMAX_CHUNK_C_MAPDRAWER5:
		case eSMAX_CHUNK_C_MAPDRAWER6:
		case eSMAX_CHUNK_C_MAPDRAWER7:
		case eSMAX_CHUNK_C_MAPDRAWER8:
		case eSMAX_CHUNK_C_MAPDRAWER9:
		case eSMAX_CHUNK_C_MAPDRAWER_ENTRY:
		case eSMAX_CHUNK_C_BACKUP_FILE:
		case eSMAX_CHUNK_C_DITHER_256:
		case eSMAX_CHUNK_C_SAVE_LAST:
		case eSMAX_CHUNK_C_USE_ALPHA:
		case eSMAX_CHUNK_C_TGA_DEPTH:
		case eSMAX_CHUNK_C_REND_FIELDS:
		case eSMAX_CHUNK_C_REFLIP:
		case eSMAX_CHUNK_C_SEL_ITEMTOG:
		case eSMAX_CHUNK_C_SEL_RESET:
		case eSMAX_CHUNK_C_STICKY_KEYINF:
		case eSMAX_CHUNK_C_WELD_THRESHOLD:
		case eSMAX_CHUNK_C_ZCLIP_POINT:
		case eSMAX_CHUNK_C_ALPHA_SPLIT:
		case eSMAX_CHUNK_C_KF_SHOW_BACKFACE:
		case eSMAX_CHUNK_C_OPTIMIZE_LOFT:
		case eSMAX_CHUNK_C_TENS_DEFAULT:
		case eSMAX_CHUNK_C_CONT_DEFAULT:
		case eSMAX_CHUNK_C_BIAS_DEFAULT:
		case eSMAX_CHUNK_C_DXFNAME_SRC:
		case eSMAX_CHUNK_C_AUTO_WELD:
		case eSMAX_CHUNK_C_AUTO_UNIFY:
		case eSMAX_CHUNK_C_AUTO_SMOOTH:
		case eSMAX_CHUNK_C_DXF_SMOOTH_ANG:
		case eSMAX_CHUNK_C_SMOOTH_ANG:
		case eSMAX_CHUNK_C_WORK_MTLS_5:
		case eSMAX_CHUNK_C_WORK_MTLS_6:
		case eSMAX_CHUNK_C_WORK_MTLS_7:
		case eSMAX_CHUNK_C_WORK_MTLS_8:
		case eSMAX_CHUNK_C_WORKMTL:
		case eSMAX_CHUNK_C_SXP_TEXT_DATA:
		case eSMAX_CHUNK_C_SXP_OPAC_DATA:
		case eSMAX_CHUNK_C_SXP_BUMP_DATA:
		case eSMAX_CHUNK_C_SXP_SHIN_DATA:
		case eSMAX_CHUNK_C_SXP_TEXT2_DATA:
		case eSMAX_CHUNK_C_SXP_SPEC_DATA:
		case eSMAX_CHUNK_C_SXP_SELFI_DATA:
		case eSMAX_CHUNK_C_SXP_TEXT_MASKDATA:
		case eSMAX_CHUNK_C_SXP_TEXT2_MASKDATA:
		case eSMAX_CHUNK_C_SXP_OPAC_MASKDATA:
		case eSMAX_CHUNK_C_SXP_BUMP_MASKDATA:
		case eSMAX_CHUNK_C_SXP_SPEC_MASKDATA:
		case eSMAX_CHUNK_C_SXP_SHIN_MASKDATA:
		case eSMAX_CHUNK_C_SXP_REFL_MASKDATA:
		case eSMAX_CHUNK_C_NET_USE_VPOST:
		case eSMAX_CHUNK_C_NET_USE_GAMMA:
		case eSMAX_CHUNK_C_NET_FIELD_ORDER:
		case eSMAX_CHUNK_C_BLUR_FRAMES:
		case eSMAX_CHUNK_C_BLUR_SAMPLES:
		case eSMAX_CHUNK_C_BLUR_DUR:
		case eSMAX_CHUNK_C_HOT_METHOD:
		case eSMAX_CHUNK_C_HOT_CHECK:
		case eSMAX_CHUNK_C_PIXEL_SIZE:
		case eSMAX_CHUNK_C_DISP_GAMMA:
		case eSMAX_CHUNK_C_FBUF_GAMMA:
		case eSMAX_CHUNK_C_FILE_OUT_GAMMA:
		case eSMAX_CHUNK_C_FILE_IN_GAMMA:
		case eSMAX_CHUNK_C_GAMMA_CORRECT:
		case eSMAX_CHUNK_C_APPLY_DISP_GAMMA:
		case eSMAX_CHUNK_C_APPLY_FBUF_GAMMA:
		case eSMAX_CHUNK_C_APPLY_FILE_GAMMA:
		case eSMAX_CHUNK_C_FORCE_WIRE:
		case eSMAX_CHUNK_C_RAY_SHADOWS:
		case eSMAX_CHUNK_C_MASTER_AMBIENT:
		case eSMAX_CHUNK_C_SUPER_SAMPLE:
		case eSMAX_CHUNK_C_OBJECT_MBLUR:
		case eSMAX_CHUNK_C_MBLUR_DITHER:
		case eSMAX_CHUNK_C_DITHER_24:
		case eSMAX_CHUNK_C_SUPER_BLACK:
		case eSMAX_CHUNK_C_SAFE_FRAME:
		case eSMAX_CHUNK_C_VIEW_PRES_RATIO:
		case eSMAX_CHUNK_C_BGND_PRES_RATIO:
		case eSMAX_CHUNK_C_NTH_SERIAL_NUM:
		case eSMAX_CHUNK_VPDATA:
		case eSMAX_CHUNK_P_QUEUE_ENTRY:
		case eSMAX_CHUNK_P_QUEUE_IMAGE:
		case eSMAX_CHUNK_P_QUEUE_USEIGAMMA:
		case eSMAX_CHUNK_P_QUEUE_PROC:
		case eSMAX_CHUNK_P_QUEUE_SOLID:
		case eSMAX_CHUNK_P_QUEUE_GRADIENT:
		case eSMAX_CHUNK_P_QUEUE_KF:
		case eSMAX_CHUNK_P_QUEUE_MOTBLUR:
		case eSMAX_CHUNK_P_QUEUE_MB_REPEAT:
		case eSMAX_CHUNK_P_QUEUE_NONE:
		case eSMAX_CHUNK_P_QUEUE_RESIZE:
		case eSMAX_CHUNK_P_QUEUE_OFFSET:
		case eSMAX_CHUNK_P_QUEUE_ALIGN:
		case eSMAX_CHUNK_P_CUSTOM_SIZE:
		case eSMAX_CHUNK_P_ALPH_NONE:
		case eSMAX_CHUNK_P_ALPH_PSEUDO:
		case eSMAX_CHUNK_P_ALPH_OP_PSEUDO:
		case eSMAX_CHUNK_P_ALPH_BLUR:
		case eSMAX_CHUNK_P_ALPH_PCOL:
		case eSMAX_CHUNK_P_ALPH_C0:
		case eSMAX_CHUNK_P_ALPH_OP_KEY:
		case eSMAX_CHUNK_P_ALPH_KCOL:
		case eSMAX_CHUNK_P_ALPH_OP_NOCONV:
		case eSMAX_CHUNK_P_ALPH_IMAGE:
		case eSMAX_CHUNK_P_ALPH_ALPHA:
		case eSMAX_CHUNK_P_ALPH_QUES:
		case eSMAX_CHUNK_P_ALPH_QUEIMG:
		case eSMAX_CHUNK_P_ALPH_CUTOFF:
		case eSMAX_CHUNK_P_ALPHANEG:
		case eSMAX_CHUNK_P_TRAN_NONE:
		case eSMAX_CHUNK_P_TRAN_IMAGE:
		case eSMAX_CHUNK_P_TRAN_FRAMES:
		case eSMAX_CHUNK_P_TRAN_FADEIN:
		case eSMAX_CHUNK_P_TRAN_FADEOUT:
		case eSMAX_CHUNK_P_TRANNEG:
		case eSMAX_CHUNK_P_RANGES:
		case eSMAX_CHUNK_P_PROC_DATA:
		case eSMAX_CHUNK_POS_TRACK_TAG_KEY:
		case eSMAX_CHUNK_ROT_TRACK_TAG_KEY:
		case eSMAX_CHUNK_SCL_TRACK_TAG_KEY:
		case eSMAX_CHUNK_FOV_TRACK_TAG_KEY:
		case eSMAX_CHUNK_ROLL_TRACK_TAG_KEY:
		case eSMAX_CHUNK_COL_TRACK_TAG_KEY:
		case eSMAX_CHUNK_MORPH_TRACK_TAG_KEY:
		case eSMAX_CHUNK_HOT_TRACK_TAG_KEY:
		case eSMAX_CHUNK_FALL_TRACK_TAG_KEY:
		case eSMAX_CHUNK_POINT_ARRAY_ENTRY:
		case eSMAX_CHUNK_POINT_FLAG_ARRAY_ENTRY:
		case eSMAX_CHUNK_FACE_ARRAY_ENTRY:
		case eSMAX_CHUNK_MSH_MAT_GROUP_ENTRY:
		case eSMAX_CHUNK_TEX_VERTS_ENTRY:
		case eSMAX_CHUNK_SMOOTH_GROUP_ENTRY:
		case eSMAX_CHUNK_DUMMY:
			bReturn = true;
			break;
		}

		if ( bReturn )
		{
			bReturn = p_chunk->m_ulLength + p_pParent->m_ulBytesRead <= p_pParent->m_ulLength;
		}

		return bReturn;
	}
}
