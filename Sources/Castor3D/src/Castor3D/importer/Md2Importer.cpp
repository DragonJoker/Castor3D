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
#include "geometry/basic/Face.h"
#include "geometry/primitives/Geometry.h"
#include "scene/Scene.h"
#include "scene/SceneNode.h"
#include "scene/SceneManager.h"


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

	if (MeshManager::HasElement( l_meshName))
	{
		l_pMesh.reset( MeshManager::GetElementByName( l_meshName));
	}
	else
	{
		l_pMesh.reset( MeshManager::CreateMesh( l_meshName, l_faces, l_sizes, Mesh::eCustom));
		Log::LogMessage( CU_T( "CreatePrimitive - Mesh %s created"), l_meshName.c_str());
	}

	m_pFile->Read<Md2Header>( m_header);

	if (m_header.m_version != 8)
	{
		Log::LogMessage( CU_T( "Invalid file format (Version not 8): ") + m_fileName);
		MeshManager::DestroyElement( l_pMesh);
		_cleanUp();
		return false;
	}

	_readMD2Data();

	_convertDataStructures( l_pMesh);

	MaterialPtr l_material = MaterialManager::CreateMaterial( l_materialName);
	PassPtr l_pass = l_material->GetPass( 0);
	l_pass->SetAmbient( 0.0f, 0.0f, 0.0f, 1.0f);
	l_pass->SetEmissive( 0.5f, 0.5f, 0.5f, 1.0f);
	l_pass->SetShininess( 64.0);

	if ( ! m_textureName.empty())
	{
		TextureUnitPtr l_unit = new TextureUnit();

		if (File::FileExists( m_textureName))
		{
			l_unit->SetTexture2D( m_textureName);
			l_pass->AddTextureUnit( l_unit);
		}
		else
		{
//			delete l_unit;
		}
	}

	SubmeshPtr l_submesh;
	for (size_t i = 0 ; i < l_pMesh->GetNbSubmeshes() ; i++)
	{
		l_submesh = l_pMesh->GetSubmesh( i);
		l_submesh->SetMaterial( l_material);
	}

	l_pMesh->SetNormals();

	MaterialManager::SetToInitialise( l_material);

	m_pScene = SceneManager::GetSingleton().GetElementByName( "MainScene");
	SceneNodePtr l_pNode = m_pScene->CreateSceneNode( l_name);

	GeometryPtr l_pGeometry = new Geometry( l_pMesh, l_pNode, l_name);
	Log::LogMessage( CU_T( "PlyImporter::_import - Geometry %s created"), l_name.c_str());

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

void Md2Importer :: _convertDataStructures( MeshPtr p_pMesh)
{
	SubmeshPtr l_pSubmesh = p_pMesh->CreateSubmesh( 1);

	for (int i = 0 ; i < m_header.m_numVertices ; i++)
	{
		l_pSubmesh->AddVertex( m_frames[0].m_vertices[i].m_vertex);
	}

	delete [] m_frames[0].m_vertices;

	Point2rArray l_texVerts;

	for (int i = 0 ; i < m_header.m_numTexCoords ; i++)
	{
		l_texVerts.push_back( Point2r( 2, m_texCoords[i].u / real( m_header.m_skinWidth), 1 - m_texCoords[i].v / real( m_header.m_skinHeight)));
	}

	FacePtr l_pFace;

	for (int i = 0 ; i < m_header.m_numTriangles ; i++)
	{
		l_pFace = l_pSubmesh->AddFace( m_triangles[i].m_vertexIndices[0], m_triangles[i].m_vertexIndices[1], m_triangles[i].m_vertexIndices[2], 0);
		l_pFace->SetTexCoordV1( l_texVerts[m_triangles[i].m_textureIndices[0]][0], l_texVerts[m_triangles[i].m_textureIndices[0]][1]);
		l_pFace->SetTexCoordV2( l_texVerts[m_triangles[i].m_textureIndices[1]][0], l_texVerts[m_triangles[i].m_textureIndices[1]][1]);
		l_pFace->SetTexCoordV3( l_texVerts[m_triangles[i].m_textureIndices[2]][0], l_texVerts[m_triangles[i].m_textureIndices[2]][1]);
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