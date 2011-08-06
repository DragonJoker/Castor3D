#include <Castor3D/Prerequisites.hpp>

using namespace Castor::Templates;

#include <Castor3D/RenderSystem.hpp>
#include <Castor3D/Buffer.hpp>
#include <Castor3D/SceneNode.hpp>
#include <Castor3D/Scene.hpp>
#include <Castor3D/Camera.hpp>
#include <Castor3D/Viewport.hpp>
#include <Castor3D/Material.hpp>
#include <Castor3D/Pass.hpp>
#include <Castor3D/TextureUnit.hpp>
#include <Castor3D/Geometry.hpp>
#include <Castor3D/Mesh.hpp>
#include <Castor3D/Submesh.hpp>
#include <Castor3D/SmoothingGroup.hpp>
#include <Castor3D/Face.hpp>
#include <Castor3D/Version.hpp>
#include <Castor3D/Plugin.hpp>
#include <Castor3D/Root.hpp>

#include "SMaxImporter/SMaxImporter.hpp"

using namespace Castor3D;

//*************************************************************************************************

C3D_SMax_API void GetRequiredVersion( Version & p_version)
{
	p_version = Version();
}

C3D_SMax_API ePLUGIN_TYPE GetType()
{
	return ePLUGIN_TYPE_IMPORTER;
}

C3D_SMax_API String GetName()
{
	return cuT( "3DS Importer Plugin");
}

C3D_SMax_API String GetExtension()
{
	return cuT( "3DS");
}

C3D_SMax_API Importer * CreateImporter()
{
	Importer * l_pReturn( new SMaxImporter());

	return l_pReturn;
}

//*************************************************************************************************

char g_buffer[50000] = {0};

SMaxImporter :: SMaxImporter()
	:	Importer()
	,	m_pFile( nullptr)
	,	m_iNumOfMaterials( 0)
	,	m_texVerts( nullptr)
	,	m_bIndicesFound( false)
{
}

bool SMaxImporter :: _import()
{
	bool l_bReturn = false;
	m_pFile = new File( m_fileName, File::eOPEN_MODE_READ | File::eOPEN_MODE_BINARY);

	if (m_pFile->IsOk())
	{
		UIntArray l_faces;
		FloatArray l_sizes;
		String l_nodeName = m_fileName.GetFileName();
		String l_meshName = m_fileName.GetFileName();
		String l_materialName = m_fileName.GetFileName();

		Collection<Mesh, String> l_mshCollection;
		MeshPtr l_pMesh = l_mshCollection.GetElement( l_meshName);
		if ( ! l_pMesh)
		{
			l_pMesh = MeshPtr( new Mesh( l_meshName, eMESH_TYPE_CUSTOM));
			l_mshCollection.AddElement( l_meshName, l_pMesh);
			l_pMesh->Initialise( l_faces, l_sizes);
			Logger::LogMessage( cuT( "CreatePrimitive - Mesh %s created"), l_meshName.c_str());
		}

		SMaxChunk l_currentChunk = {0};

		_readChunk( & l_currentChunk);

		if (l_currentChunk.m_id == SMAX_PRIMARY)
		{
			_processNextChunk( l_pMesh, & l_currentChunk);
			l_pMesh->ComputeNormals();
			Collection<Scene, String> l_scnCollection;
			m_pScene = l_scnCollection.GetElement( "MainScene");
			SceneNodePtr l_pNode = m_pScene->CreateSceneNode( l_nodeName);

			GeometryPtr l_pGeometry( new Geometry( m_pScene.get(), l_pMesh, l_pNode, l_meshName));
			Logger::LogMessage( cuT( "PlyImporter::_import - Geometry %s created"), l_meshName.c_str());

			m_geometries.insert( GeometryPtrStrMap::value_type( l_meshName, l_pGeometry));
			l_bReturn = true;
		}
	}

	delete m_pFile;

	return l_bReturn;
}

void SMaxImporter :: _processNextChunk( MeshPtr p_pMesh, SMaxChunk * p_previousChunk)
{
	SubmeshPtr l_pSubmesh;

	SMaxChunk l_currentChunk = {0};
	SMaxChunk l_tempChunk = {0};

	while (m_pFile->IsOk() && p_previousChunk->m_bytesRead < p_previousChunk->m_length)
	{
		_readChunk( & l_currentChunk);

		switch (l_currentChunk.m_id)
		{
		case SMAX_VERSION:
			l_currentChunk.m_bytesRead += m_pFile->ReadArray<char>( g_buffer, l_currentChunk.m_length - l_currentChunk.m_bytesRead);
			if ((l_currentChunk.m_length - l_currentChunk.m_bytesRead == 4) && (((int *)g_buffer)[0] > 0x03))
			{
				Logger::LogWarning( cuT( "File version is over version 3 and may load incorrectly"));
			}
		break;

		case SMAX_OBJECTINFO:
			_readChunk( & l_tempChunk);
			l_tempChunk.m_bytesRead += m_pFile->ReadArray<char>( g_buffer, l_tempChunk.m_length - l_tempChunk.m_bytesRead);
			l_currentChunk.m_bytesRead += l_tempChunk.m_bytesRead;
			_processNextChunk( p_pMesh, & l_currentChunk);
		break;

		case SMAX_MATERIAL:
			m_iNumOfMaterials++;
			_processNextMaterialChunk( & l_currentChunk);
		break;

		case SMAX_OBJECT:
		{
			l_pSubmesh = p_pMesh->CreateSubmesh( 1);
			String l_strName;
			l_currentChunk.m_bytesRead += _getString( l_strName);
			_processNextObjectChunk( p_pMesh, l_pSubmesh, & l_currentChunk);
			if ( ! m_bIndicesFound)
			{
				l_pSubmesh->ComputeFacesFromPolygonVertex();
			}
			l_pSubmesh->ComputeContainers();
			l_pSubmesh->GenerateBuffers();
		}
		break;

		case SMAX_EDITKEYFRAME:
			l_currentChunk.m_bytesRead += m_pFile->ReadArray<char>( g_buffer, l_currentChunk.m_length - l_currentChunk.m_bytesRead);
		break;

		default:
			l_currentChunk.m_bytesRead += m_pFile->ReadArray<char>( g_buffer, l_currentChunk.m_length - l_currentChunk.m_bytesRead);
		break;
		}

		p_previousChunk->m_bytesRead += l_currentChunk.m_bytesRead;
	}
}

void SMaxImporter :: _processNextObjectChunk( MeshPtr p_pMesh, SubmeshPtr p_pSubmesh, SMaxChunk * p_previousChunk)
{
	SMaxChunk l_currentChunk = {0};

	while (m_pFile->IsOk() && p_previousChunk->m_bytesRead < p_previousChunk->m_length)
	{
		_readChunk( & l_currentChunk);

		switch (l_currentChunk.m_id)
		{
		case SMAX_OBJECT_MESH:
			_processNextObjectChunk( p_pMesh, p_pSubmesh, & l_currentChunk);
		break;

		case SMAX_OBJECT_VERTICES:
			_readVertices( p_pSubmesh, & l_currentChunk);
		break;

		case SMAX_OBJECT_FACES:
			_readVertexIndices( p_pSubmesh, & l_currentChunk);
		break;

		case SMAX_OBJECT_MATERIAL:
			_readObjectMaterial( p_pSubmesh, & l_currentChunk);
		break;

		case SMAX_OBJECT_UV:
			_readUVCoordinates( p_pSubmesh, & l_currentChunk);
		break;

		default:
			l_currentChunk.m_bytesRead += m_pFile->ReadArray<char>( g_buffer, l_currentChunk.m_length - l_currentChunk.m_bytesRead);
		break;
		}

		p_previousChunk->m_bytesRead += l_currentChunk.m_bytesRead;
	}
}

void SMaxImporter :: _processNextMaterialChunk( SMaxChunk * p_previousChunk)
{
	SMaxChunk l_currentChunk = {0};
	MaterialPtr l_pMaterial;
	Colour l_crDiffuse( Colour::FromRGB( 0));
	Colour l_crAmbient( Colour::FromRGB( 0));
	Colour l_crSpecular( Colour::FromRGB( 0));
	real l_fShininess = 0;
	String l_strMatName;
	String l_strTexture;

	while (m_pFile->IsOk() && p_previousChunk->m_bytesRead < p_previousChunk->m_length)
	{
		_readChunk( & l_currentChunk);

		switch (l_currentChunk.m_id)
		{
		case SMAX_MATNAME:
			l_currentChunk.m_bytesRead += _getString( l_strMatName);
			l_currentChunk.m_bytesRead = l_currentChunk.m_length;
		break;

		case SMAX_MATDIFFUSE:
			_readColorChunk( l_crDiffuse, & l_currentChunk);
		break;

		case SMAX_MATSPECULAR:
			_readColorChunk( l_crSpecular, & l_currentChunk);
		break;

		case SMAX_MATSHININESS:
		break;

		case SMAX_MATTWOSIDED:
		break;

		case SMAX_MATMAP:
			_processMaterialMapChunk( l_strTexture, & l_currentChunk);
		break;

		case SMAX_MATMAPFILE:
			l_currentChunk.m_bytesRead += _getString( l_strTexture);
			l_currentChunk.m_bytesRead = l_currentChunk.m_length;
		break;

		default:
			l_currentChunk.m_bytesRead += m_pFile->ReadArray<char>( g_buffer, l_currentChunk.m_length - l_currentChunk.m_bytesRead);
		break;
		}

		p_previousChunk->m_bytesRead += l_currentChunk.m_bytesRead;
	}

	if ( ! l_strMatName.empty())
	{
		Root::MaterialManager * l_pMtlManager = Root::GetSingleton()->GetMaterialManager();

		Collection<Material, String> l_mtlCollection;
		l_pMaterial = l_mtlCollection.GetElement( l_strMatName);
		if ( ! l_pMaterial)
		{
			l_pMaterial = MaterialPtr( new Material( l_strMatName, 1));
			l_mtlCollection.AddElement( l_strMatName, l_pMaterial);
		}
		PassPtr l_pPass = l_pMaterial->GetPass( 0);
		l_pPass->SetAmbient( l_crAmbient);
		l_pPass->SetDiffuse( l_crDiffuse);
		l_pPass->SetSpecular( l_crSpecular);
		l_pPass->SetDoubleFace( true);

		if ( ! l_strTexture.empty())
		{
			TextureUnitPtr l_unit = l_pPass->AddTextureUnit();
			Collection<Image, String> l_imgCollection;
			ImagePtr l_pImage = l_imgCollection.GetElement( m_pFile->GetFilePath() / l_strTexture);
			if ( ! l_pImage)
			{
				l_pImage = ImagePtr( new Image( m_pFile->GetFilePath() / l_strTexture));
				l_imgCollection.AddElement( m_pFile->GetFilePath() / l_strTexture, l_pImage);
			}
			Loader<Image>::Load( * l_pImage, m_pFile->GetFilePath() / l_strTexture);
			l_unit->SetTexture( eTEXTURE_TYPE_2D, l_pImage);
			l_unit->SetPrimaryColour( 1.0, 1.0, 1.0, 1.0);
		}

		l_pMtlManager->SetToInitialise( l_pMaterial);
	}
}

void SMaxImporter :: _processMaterialMapChunk( String & p_strName, SMaxChunk * p_previousChunk)
{
	SMaxChunk l_currentChunk = {0};

	while (m_pFile->IsOk() && p_previousChunk->m_bytesRead < p_previousChunk->m_length)
	{
		_readChunk( & l_currentChunk);

		switch (l_currentChunk.m_id)
		{
		case SMAX_MATMAP:
			_processMaterialMapChunk( p_strName, & l_currentChunk);
		break;

		case SMAX_MATMAPFILE:
			l_currentChunk.m_bytesRead += _getString( p_strName);
			l_currentChunk.m_bytesRead = l_currentChunk.m_length;
		break;

		default:
			l_currentChunk.m_bytesRead += m_pFile->ReadArray<char>( g_buffer, l_currentChunk.m_length - l_currentChunk.m_bytesRead);
		break;
		}

		p_previousChunk->m_bytesRead += l_currentChunk.m_bytesRead;
	}
}

void SMaxImporter :: _readChunk( SMaxChunk * p_chunk)
{
	p_chunk->m_bytesRead = m_pFile->Read<unsigned short>( p_chunk->m_id);
	p_chunk->m_bytesRead += m_pFile->Read<unsigned int>( p_chunk->m_length);
}

int SMaxImporter :: _getString( xchar * p_pBuffer)
{
	xchar l_pBuffer[255];
	int index = 0;
	m_pFile->Read<xchar>( *l_pBuffer);

	while (m_pFile->IsOk() && *(l_pBuffer + index++) != 0)
	{
		m_pFile->Read<xchar>( *(l_pBuffer + index));
	}

	wcsncpy( p_pBuffer, String( l_pBuffer).c_str(), 255);

	return wcslen( l_pBuffer) + 1;
}

int SMaxImporter :: _getString( String & p_strString)
{
	xchar l_pBuffer[255];
	int index = 0;
	m_pFile->Read<xchar>( * l_pBuffer);

	while (m_pFile->IsOk() && *(l_pBuffer + index++) != 0)
	{
		m_pFile->Read<xchar>( *(l_pBuffer + index));
	}

	p_strString = l_pBuffer;

	return wcslen( l_pBuffer) + 1;
}

void SMaxImporter :: _readColorChunk( Colour & p_colour, SMaxChunk * p_chunk)
{
	SMaxChunk l_tempChunk = {0};
	_readChunk( & l_tempChunk);
	unsigned char l_pColour[3];
	l_tempChunk.m_bytesRead += m_pFile->ReadArray( l_pColour, 3);
	p_colour[0] = float( l_pColour[0]) / 255;
	p_colour[1] = float( l_pColour[1]) / 255;
	p_colour[2] = float( l_pColour[2]) / 255;
	p_chunk->m_bytesRead += l_tempChunk.m_bytesRead;
}

void SMaxImporter :: _readVertexIndices( SubmeshPtr p_pSubmesh, SMaxChunk * p_chunk)
{
	unsigned short l_index1 = 0;
	unsigned short l_index2 = 0;
	unsigned short l_index3 = 0;
	unsigned short l_dump = 0;
	int l_iNumOfFaces = 0;
	p_chunk->m_bytesRead += m_pFile->Read<short>( (short &)l_iNumOfFaces);

	for (int i = 0 ; i < l_iNumOfFaces ; i++)
	{
		p_chunk->m_bytesRead += m_pFile->Read( l_index1);
		p_chunk->m_bytesRead += m_pFile->Read( l_index2);
		p_chunk->m_bytesRead += m_pFile->Read( l_index3);
		p_chunk->m_bytesRead += m_pFile->Read( l_dump);

		FacePtr l_face = p_pSubmesh->AddFace( l_index1, l_index2, l_index3, 0);

		if (m_texVerts)
		{
			l_face->SetVertexTexCoords( 0, m_texVerts[l_index1][0], m_texVerts[l_index1][1]);
			l_face->SetVertexTexCoords( 1, m_texVerts[l_index2][0], m_texVerts[l_index2][1]);
			l_face->SetVertexTexCoords( 2, m_texVerts[l_index3][0], m_texVerts[l_index3][1]);
		}
	}

	m_bIndicesFound = true;
}

void SMaxImporter :: _readUVCoordinates( SubmeshPtr p_pSubmesh, SMaxChunk * p_chunk)
{
	short l_sNumTexVertex = 0;
	p_chunk->m_bytesRead += m_pFile->Read<short>( l_sNumTexVertex);
	m_texVerts = new Point2r[l_sNumTexVertex];

	for (short i = 0 ; i < l_sNumTexVertex ; i++)
	{
		p_chunk->m_bytesRead += m_pFile->ReadArray( m_texVerts[i].ptr(), sizeof( real) * 2);
	}
}

void SMaxImporter :: _readVertices( SubmeshPtr p_pSubmesh, SMaxChunk * p_chunk)
{
	short l_sNumOfVerts = 0;
	p_chunk->m_bytesRead += m_pFile->Read<short>( l_sNumOfVerts);

	for (short i = 0 ; i < l_sNumOfVerts && p_chunk->m_bytesRead < p_chunk->m_length ; i++)
	{
		Point3r l_ptVertex;
		p_chunk->m_bytesRead += m_pFile->ReadArray( l_ptVertex.ptr(), 3);
		p_pSubmesh->AddPoint( l_ptVertex[0], l_ptVertex[2], -l_ptVertex[1]);
	}
}

void SMaxImporter :: _readObjectMaterial( SubmeshPtr p_pSubmesh, SMaxChunk * p_chunk)
{
	String l_materialName;
	p_chunk->m_bytesRead += _getString( l_materialName);
	Root::MaterialManager * l_pMtlManager = Root::GetSingleton()->GetMaterialManager();
	MaterialPtr l_pMaterial = l_pMtlManager->GetElement( l_materialName);

	if (l_pMaterial)
	{
		p_pSubmesh->SetMaterial( l_pMaterial);
	}

	unsigned char * l_pBuffer = new unsigned char[p_chunk->m_length - p_chunk->m_bytesRead];

	try
	{
		p_chunk->m_bytesRead += m_pFile->ReadArray( l_pBuffer, p_chunk->m_length - p_chunk->m_bytesRead);
	}
	catch ( ... )
	{
	}

	delete [] l_pBuffer;
}

//*************************************************************************************************
