#include "PrecompiledHeader.h"

#include "importer/Module_Importer.h"

#include "importer/BspImporter.h"
#include "geometry/mesh/MeshManager.h"
#include "geometry/mesh/Mesh.h"
#include "geometry/mesh/Submesh.h"
#include "geometry/basic/Face.h"
#include "material/MaterialManager.h"
#include "material/TextureUnit.h"
#include "material/Material.h"
#include "material/Pass.h"
#include "render_system/Buffer.h"
#include "render_system/MeshRenderer.h"
#include "render_system/RenderSystem.h"
#include "Log.h"

using namespace Castor3D;

extern bool g_textures;

BspImporter :: BspImporter()
{
	m_numOfVerts    = 0;	
	m_numOfFaces    = 0;
	m_numOfIndices  = 0;
	m_numOfTextures = 0;

	m_verts = NULL;
	m_faces = NULL;
	m_indices = NULL;
}

bool BspImporter :: _import()
{
	m_pFile = new File( m_fileName, File::eRead);
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

	int i = 0;

	BSPHeader l_header = {0};
	BSPLump l_lumps[kMaxLumps] = {0};

	m_pFile->Read<BSPHeader>( l_header);
	m_pFile->ReadArray<BSPLump>( l_lumps, kMaxLumps);

	m_numOfVerts = l_lumps[kVertices].m_length / sizeof( BSPVertex);
	m_verts     = new BSPVertex[m_numOfVerts];

	m_numOfFaces = l_lumps[kFaces].m_length / sizeof( BSPFace);
	m_faces     = new BSPFace[m_numOfFaces];

	m_numOfIndices = l_lumps[kIndices].m_length / sizeof( int);
	m_indices     = new int[m_numOfIndices];

	m_numOfTextures = l_lumps[kTextures].m_length / sizeof( BSPTexture);
	BSPTexture * l_textures = new BSPTexture[m_numOfTextures];

	m_pFile->Seek( l_lumps[kVertices].m_offset);

	for (i = 0 ; i < m_numOfVerts ; i++)
	{
		m_pFile->Read<BSPVertex>( m_verts[i]);
		
		float l_temp = m_verts[i].m_position.y;
		m_verts[i].m_position.y = m_verts[i].m_position.z;
		m_verts[i].m_position.z = -l_temp;
	}	

	m_pFile->Seek( l_lumps[kIndices].m_offset);
	m_pFile->ReadArray<int>( m_indices, m_numOfIndices);

	m_pFile->Seek( l_lumps[kFaces].m_offset);
	m_pFile->ReadArray<BSPFace>( m_faces, m_numOfFaces);

	m_pFile->Seek( l_lumps[kTextures].m_offset);
	m_pFile->ReadArray<BSPTexture>( l_textures, m_numOfTextures);

	m_facesDrawn.Resize( m_numOfFaces);

	_convertToMesh( l_pMesh, l_textures);

	delete [] l_textures;

	delete m_pFile;

	return true;
}

extern BspImporter g_Level;

void BspImporter :: _destroy()
{
	delete [] m_verts;
	delete [] m_faces;
	delete [] m_indices;
}

BspImporter :: ~BspImporter()
{
	_destroy();
}

void BspImporter::_convertToMesh( Mesh * p_mesh, BSPTexture * p_textures)
{
	size_t l_i = min( m_fileName.find_last_of( C3D_T( "/")), m_fileName.find_last_of( C3D_T( "\\")));
	String l_filePath = m_fileName.substr( 0, l_i + 1);

	MaterialPtrArray l_materials;
	Material * l_material;
	Pass * l_pass;
	TextureUnit * l_unit;
	for( int i = 0; i < m_numOfTextures; i++)
	{
		String l_strName;
		l_strName = p_mesh->GetName() + C3D_T( "Material_") + l_strName;
		l_strName << i;
		l_material = MaterialManager::GetSingletonPtr()->CreateMaterial( l_strName);
		l_pass = l_material->GetPass( 0);
		l_pass->SetAmbient( 0.0f, 0.0f, 0.0f, 1.0f);
		l_pass->SetEmissive( 0.5f, 0.5f, 0.5f, 1.0f);
		l_unit = new TextureUnit( RenderSystem::GetSingletonPtr()->CreateTextureRenderer());
		l_unit->SetTexture2D( l_filePath + C3D_T( "../") + String( p_textures[i].m_strName) + C3D_T( ".jpg"));
		l_pass->AddTextureUnit( l_unit);
		l_materials.push_back( l_material);
	}

	Submesh * l_submesh;
	BSPFace l_bspFace;
	for (int i = 0 ; i < m_numOfFaces ; i++)
	{
		l_bspFace = m_faces[i];
		l_submesh = p_mesh->CreateSubmesh( 1);

		for (int j = 0 ; j < l_bspFace.m_numOfVerts ; j++)
		{
			l_submesh->AddVertex( m_verts[l_bspFace.m_startIndex + j].m_position.x, m_verts[l_bspFace.m_startIndex + j].m_position.y, m_verts[l_bspFace.m_startIndex + j].m_position.z);
		}

		l_submesh->ComputeFacesFromPolygonVertex();
		l_submesh->SetMaterial( l_materials[l_bspFace.m_textureID]);
	}
}