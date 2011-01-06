#include <Castor3D/Prerequisites.h>

using namespace Castor::Templates;

#include <Castor3D/material/MaterialManager.h>
#include <Castor3D/material/Material.h>
#include <Castor3D/material/Pass.h>
#include <Castor3D/material/TextureUnit.h>
#include <Castor3D/render_system/RenderSystem.h>
#include <Castor3D/geometry/mesh/MeshManager.h>
#include <Castor3D/geometry/mesh/Mesh.h>
#include <Castor3D/geometry/mesh/Submesh.h>
#include <Castor3D/geometry/basic/Face.h>
#include <Castor3D/geometry/primitives/Geometry.h>
#include <Castor3D/main/Version.h>
#include <Castor3D/scene/Scene.h>
#include <Castor3D/scene/SceneNode.h>
#include <Castor3D/scene/SceneManager.h>

#include "Md2Importer/Md2Importer.h"

using namespace Castor3D;

//*************************************************************************************************

extern "C" C3D_Md2_API void GetRequiredVersion( Version & p_version)
{
	p_version = Version( 0, 6);
}

extern "C" C3D_Md2_API Importer * CreateImporter( SceneManager * p_pManager)
{
	Importer * l_pReturn( new Md2Importer( p_pManager));

	return l_pReturn;
}

//*************************************************************************************************

Md2Importer::Md2Importer( SceneManager * p_pManager, const String & p_textureName)
	:	Importer( p_pManager)
	,	m_textureName( p_textureName)
	,	m_skins( NULL)
	,	m_texCoords( NULL)
	,	m_triangles( NULL)
	,	m_frames( NULL)
	,	m_pFile( NULL)
{
	memset( & m_header, 0, sizeof( Md2Header));
}

bool Md2Importer :: _import()
{
	m_pFile = new File( m_fileName, File::eRead);

	size_t l_uiSlashIndex = 0;

	if (m_fileName.find_last_of( CU_T( "\\")) != String::npos)
	{
		l_uiSlashIndex = m_fileName.find_last_of( CU_T( "\\")) + 1;
	}

	if (m_fileName.find_last_of( CU_T( "/")) != String::npos)
	{
		l_uiSlashIndex = std::max( l_uiSlashIndex, m_fileName.find_last_of( CU_T( "/")) + 1);
	}

	size_t l_uiDotIndex = m_fileName.find_last_of( CU_T( "."));

	UIntArray l_faces;
	FloatArray l_sizes;
	String l_name = m_fileName.substr( l_uiSlashIndex, l_uiDotIndex - l_uiSlashIndex);
	String l_meshName = m_fileName.substr( l_uiSlashIndex, l_uiDotIndex - l_uiSlashIndex);
	String l_materialName = m_fileName.substr( l_uiSlashIndex, l_uiDotIndex - l_uiSlashIndex);

	MeshPtr l_pMesh;

	if (m_pManager->GetMeshManager()->HasElement( l_meshName))
	{
		l_pMesh = m_pManager->GetMeshManager()->GetElementByName( l_meshName);
	}
	else
	{
		l_pMesh = m_pManager->GetMeshManager()->CreateMesh( l_meshName, l_faces, l_sizes, Mesh::eCustom);
		Logger::LogMessage( CU_T( "CreatePrimitive - Mesh %s created"), l_meshName.c_str());
	}

	m_pFile->Read<Md2Header>( m_header);

	if (m_header.m_version != 8)
	{
		Logger::LogMessage( CU_T( "Invalid file format (Version not 8): ") + m_fileName);
		m_pManager->GetMeshManager()->DestroyElement( l_pMesh);
		_cleanUp();
		return false;
	}

	_readMD2Data();

	_convertDataStructures( l_pMesh);

	MaterialPtr l_material = m_pManager->GetMaterialManager()->CreateMaterial( l_materialName);
	PassPtr l_pass = l_material->GetPass( 0);
	l_pass->SetAmbient( 0.0f, 0.0f, 0.0f, 1.0f);
	l_pass->SetEmissive( 0.5f, 0.5f, 0.5f, 1.0f);
	l_pass->SetShininess( 64.0);

	if ( ! m_textureName.empty())
	{
		TextureUnitPtr l_unit = l_pass->AddTextureUnit();

		if (File::FileExists( m_textureName))
		{
			l_unit->SetTexture2D( m_pManager->GetImageManager()->CreateImage( m_textureName, m_textureName));
		}
		else
		{
			l_pass->DestroyTextureUnit( l_unit->GetIndex());
		}
	}

	SubmeshPtr l_submesh;
	for (size_t i = 0 ; i < l_pMesh->GetNbSubmeshes() ; i++)
	{
		l_submesh = l_pMesh->GetSubmesh( i);
		l_submesh->SetMaterial( l_material);
	}

	l_pMesh->ComputeNormals();

	m_pManager->GetMaterialManager()->SetToInitialise( l_material);

	m_pScene = m_pManager->GetElementByName( "MainScene");
	SceneNodePtr l_pNode = m_pScene->CreateSceneNode( l_name);

	GeometryPtr l_pGeometry( new Geometry( l_pMesh, l_pNode, l_name));
	Logger::LogMessage( CU_T( "PlyImporter::_import - Geometry %s created"), l_name.c_str());

	m_geometries.insert( GeometryPtrStrMap::value_type( l_name, l_pGeometry));

	_cleanUp();

	return true;
}

void Md2Importer :: _readMD2Data()
{
	unsigned char l_buffer[MD2_MAX_FRAMESIZE];

	if (m_header.m_numSkins > 0)
	{
		m_skins = new Md2Skin[m_header.m_numSkins];
		m_pFile->Seek( m_header.m_offsetSkins);
		m_pFile->ReadArray<Md2Skin>( m_skins, m_header.m_numSkins);
	}

	if (m_header.m_numTexCoords > 0)
	{
		m_texCoords = new Md2TexCoord[m_header.m_numTexCoords];
		m_pFile->Seek( m_header.m_offsetTexCoords);
		m_pFile->ReadArray<Md2TexCoord>( m_texCoords, m_header.m_numTexCoords);
	}

	if (m_header.m_numTriangles > 0)
	{
		m_triangles = new Md2Face[m_header.m_numTriangles];
		m_pFile->Seek( m_header.m_offsetTriangles);
		m_pFile->ReadArray<Md2Face>( m_triangles, m_header.m_numTriangles);
	}

	if (m_header.m_numFrames > 0)
	{
		m_frames = new Md2Frame[m_header.m_numFrames];
		m_pFile->Seek( m_header.m_offsetFrames);
		Md2AliasFrame * l_frame = (Md2AliasFrame *) l_buffer;
		m_frames[0].m_vertices = new Md2Triangle[m_header.m_numVertices];
		m_pFile->ReadArray<char>( (char *)l_frame, m_header.m_frameSize);
		strcpy_s( m_frames[0].m_strName, 16, l_frame->m_name);

		Md2Triangle * l_vertices = m_frames[0].m_vertices;

		for (int j = 0 ; j < m_header.m_numVertices; j++)
		{
			l_vertices[j].m_vertex[0] = l_frame->m_aliasVertices[j].m_vertex[0] * l_frame->m_scale[0] + l_frame->m_translate[0];
			l_vertices[j].m_vertex[2] = -1 * (l_frame->m_aliasVertices[j].m_vertex[1] * l_frame->m_scale[1] + l_frame->m_translate[1]);
			l_vertices[j].m_vertex[1] = l_frame->m_aliasVertices[j].m_vertex[2] * l_frame->m_scale[2] + l_frame->m_translate[2];
		}
	}
}

void Md2Importer :: _convertDataStructures( MeshPtr p_pMesh)
{
	SubmeshPtr l_pSubmesh = p_pMesh->CreateSubmesh( 1);

	for (int i = 0 ; i < m_header.m_numVertices ; i++)
	{
		l_pSubmesh->AddPoint( m_frames[0].m_vertices[i].m_vertex);
	}

	delete [] m_frames[0].m_vertices;

	Point2rArray l_texVerts;

	for (int i = 0 ; i < m_header.m_numTexCoords ; i++)
	{
		l_texVerts.push_back( Point2r( 2, m_texCoords[i].u / real( m_header.m_skinWidth), 1 - m_texCoords[i].v / real( m_header.m_skinHeight)));
	}

	for (int i = 0 ; i < m_header.m_numTriangles ; i++)
	{
		FacePtr l_face = l_pSubmesh->AddFace( m_triangles[i].m_vertexIndices[0], m_triangles[i].m_vertexIndices[1], m_triangles[i].m_vertexIndices[2], 0);
		l_face->SetVertexTexCoords( 0, l_texVerts[m_triangles[i].m_textureIndices[0]][0], l_texVerts[m_triangles[i].m_textureIndices[0]][1]);
		l_face->SetVertexTexCoords( 1, l_texVerts[m_triangles[i].m_textureIndices[1]][0], l_texVerts[m_triangles[i].m_textureIndices[1]][1]);
		l_face->SetVertexTexCoords( 2, l_texVerts[m_triangles[i].m_textureIndices[2]][0], l_texVerts[m_triangles[i].m_textureIndices[2]][1]);
	}

	p_pMesh->ComputeContainers();

	for (size_t i = 0 ; i < p_pMesh->GetNbSubmeshes() ; i++)
	{
		p_pMesh->GetSubmesh( i)->GenerateBuffers();
	}

	p_pMesh->ComputeNormals();
}

void Md2Importer :: _cleanUp()
{
	delete m_pFile;

	delete [] m_skins;
	delete m_texCoords;
	delete m_triangles;
	delete m_frames;
}