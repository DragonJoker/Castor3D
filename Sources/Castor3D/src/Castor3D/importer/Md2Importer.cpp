#include "PrecompiledHeader.h"

#include "importer/Module_Importer.h"

#include "importer/Md2Importer.h"
#include "material/MaterialManager.h"
#include "material/Material.h"
#include "material/Pass.h"
#include "material/TextureUnit.h"
#include "render_system/RenderSystem.h"
#include "geometry/mesh/MeshManager.h"
#include "geometry/mesh/Mesh.h"
#include "geometry/mesh/Submesh.h"
#include "geometry/primitives/Geometry.h"
#include "scene/Scene.h"
#include "scene/SceneManager.h"
#include "Log.h"

using namespace Castor3D;

Md2Importer::Md2Importer( const String & p_textureName)
	:	ExternalImporter(),
		m_textureName( p_textureName),
		m_skins( NULL),
		m_texCoords( NULL),
		m_triangles( NULL),
		m_frames( NULL),
		m_pFile( NULL)
{
	memset( & m_header, 0, sizeof( Md2Header));
}

bool Md2Importer :: _import()
{
	m_pFile = new FileIO( m_fileName, FileIO::eRead);

	size_t l_uiSlashIndex = 0;

	if (m_fileName.find_last_of( C3D_T( "\\")) != String::npos)
	{
		l_uiSlashIndex = m_fileName.find_last_of( C3D_T( "\\")) + 1;
	}

	if (m_fileName.find_last_of( C3D_T( "/")) != String::npos)
	{
		l_uiSlashIndex = max( l_uiSlashIndex, m_fileName.find_last_of( C3D_T( "/")) + 1);
	}

	size_t l_uiDotIndex = m_fileName.find_last_of( C3D_T( "."));

	UIntArray l_faces;
	FloatArray l_sizes;
	String l_name = m_fileName.substr( l_uiSlashIndex, l_uiDotIndex - l_uiSlashIndex);
	String l_meshName = m_fileName.substr( l_uiSlashIndex, l_uiDotIndex - l_uiSlashIndex);
	String l_materialName = m_fileName.substr( l_uiSlashIndex, l_uiDotIndex - l_uiSlashIndex);

	Mesh * l_pMesh = NULL;

	if (MeshManager::GetSingletonPtr()->HasElement( l_meshName))
	{
		l_pMesh = MeshManager::GetSingletonPtr()->GetElementByName( l_meshName);
	}
	else
	{
		l_pMesh = MeshManager::GetSingletonPtr()->CreateMesh( l_meshName, l_faces, l_sizes, Mesh::eCustom);
		Log::LogMessage( C3D_T( "CreatePrimitive - Mesh %s created"), l_meshName.c_str());
	}

	m_pFile->Read<Md2Header>( m_header);

	if (m_header.m_version != 8)
	{
		Log::LogMessage( C3D_T( "Invalid file format (Version not 8): ") + m_fileName);
		MeshManager::GetSingletonPtr()->DestroyElement( l_pMesh);
		_cleanUp();
		return false;
	}

	_readMD2Data();

	_convertDataStructures( l_pMesh);

	Material * l_material = MaterialManager::GetSingletonPtr()->CreateMaterial( l_materialName);
	Pass * l_pass = l_material->GetPass( 0);
	l_pass->SetAmbient( 0.0f, 0.0f, 0.0f, 1.0f);
	l_pass->SetEmissive( 0.5f, 0.5f, 0.5f, 1.0f);
	l_pass->SetShininess( 64.0);

	if ( ! m_textureName.empty())
	{
		TextureUnit * l_unit = new TextureUnit( RenderSystem::GetSingletonPtr()->CreateTextureRenderer());

		if (FileBase::FileExists( m_textureName))
		{
			l_unit->SetTexture2D( m_textureName);
			l_pass->AddTextureUnit( l_unit);
		}
		else
		{
			delete l_unit;
		}
	}

	Submesh * l_submesh;
	for (size_t i = 0 ; i < l_pMesh->GetNbSubmeshes() ; i++)
	{
		l_submesh = l_pMesh->GetSubmesh( i);
		l_submesh->SetMaterial( l_material);
	}

	l_pMesh->SetNormals();

	MaterialManager::GetSingletonPtr()->SetToInitialise( l_material);

	m_pScene = SceneManager::GetSingleton().GetElementByName( "MainScene");
	SceneNode * l_pNode = m_pScene->CreateSceneNode( l_name);

	Geometry * l_pGeometry = new Geometry( l_pMesh, l_pNode, l_name);
	Log::LogMessage( C3D_T( "PlyImporter::_import - Geometry %s created"), l_name.c_str());

	m_geometries.insert( GeometryStrMap::value_type( l_name, l_pGeometry));

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
		strcpy( m_frames[0].m_strName, l_frame->m_name);

		Md2Triangle * l_vertices = m_frames[0].m_vertices;

		for (int j = 0 ; j < m_header.m_numVertices; j++)
		{
			l_vertices[j].m_vertex[0] = l_frame->m_aliasVertices[j].m_vertex[0] * l_frame->m_scale[0] + l_frame->m_translate[0];
			l_vertices[j].m_vertex[2] = -1 * (l_frame->m_aliasVertices[j].m_vertex[1] * l_frame->m_scale[1] + l_frame->m_translate[1]);
			l_vertices[j].m_vertex[1] = l_frame->m_aliasVertices[j].m_vertex[2] * l_frame->m_scale[2] + l_frame->m_translate[2];
		}
	}
}

void Md2Importer :: _convertDataStructures( Mesh * p_pMesh)
{
	Submesh * l_pSubmesh = p_pMesh->CreateSubmesh( 1);

	for (int i = 0 ; i < m_header.m_numVertices ; i++)
	{
		l_pSubmesh->AddVertex( m_frames[0].m_vertices[i].m_vertex);
	}

	delete [] m_frames[0].m_vertices;

	std::vector < Point2D<float> > l_texVerts;

	for (int i = 0 ; i < m_header.m_numTexCoords ; i++)
	{
		l_texVerts.push_back( Point2D<float>( m_texCoords[i].u / float( m_header.m_skinWidth), 1 - m_texCoords[i].v / float( m_header.m_skinHeight)));
	}

	Face * l_pFace;

	for (int i = 0 ; i < m_header.m_numTriangles ; i++)
	{
		l_pFace = l_pSubmesh->AddFace( m_triangles[i].m_vertexIndices[0], m_triangles[i].m_vertexIndices[1], m_triangles[i].m_vertexIndices[2], 0);
		SetTexCoordV1( l_pFace, l_texVerts[m_triangles[i].m_textureIndices[0]].x, l_texVerts[m_triangles[i].m_textureIndices[0]].y);
		SetTexCoordV2( l_pFace, l_texVerts[m_triangles[i].m_textureIndices[1]].x, l_texVerts[m_triangles[i].m_textureIndices[1]].y);
		SetTexCoordV3( l_pFace, l_texVerts[m_triangles[i].m_textureIndices[2]].x, l_texVerts[m_triangles[i].m_textureIndices[2]].y);
	}

	p_pMesh->ComputeContainers();

	for (size_t i = 0 ; i < p_pMesh->GetNbSubmeshes() ; i++)
	{
		p_pMesh->GetSubmesh( i)->GenerateBuffers();
	}

	p_pMesh->SetNormals();
}

void Md2Importer :: _cleanUp()
{
	delete m_pFile;

	delete [] m_skins;
	delete m_texCoords;
	delete m_triangles;
	delete m_frames;
}