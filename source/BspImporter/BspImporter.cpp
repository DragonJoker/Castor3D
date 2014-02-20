/*
#include <Castor3D/Prerequisites.hpp>

using namespace Castor::Templates;

#include <Castor3D/Mesh.hpp>
#include <Castor3D/Submesh.hpp>
#include <Castor3D/Face.hpp>
#include <Castor3D/TextureUnit.hpp>
#include <Castor3D/Material.hpp>
#include <Castor3D/Pass.hpp>
#include <Castor3D/Version.hpp>
#include <Castor3D/Buffer.hpp>
#include <Castor3D/MeshRenderer.hpp>
#include <Castor3D/RenderSystem.hpp>
#include <Castor3D/Plugin.hpp>

#include "BspImporter/BspImporter.hpp"

using namespace Castor3D;

//*************************************************************************************************

C3D_Bsp_API void GetRequiredVersion( Version & p_version)
{
	p_version = Version();
}

C3D_Bsp_API ePLUGIN_TYPE GetType()
{
	return ePluginImporter;
}

C3D_Bsp_API String GetName()
{
	return cuT( "BSP Importer Plugin");
}

C3D_Bsp_API String GetExtension()
{
	return cuT( "BSP");
}

C3D_Bsp_API Importer * CreateImporter()
{
	Importer * l_pReturn( new BspImporter);

	return l_pReturn;
}

//*************************************************************************************************

extern bool g_textures;

BspImporter :: BspImporter()
	:	Importer()
{
	m_numOfVerts    = 0;	
	m_numOfFaces    = 0;
	m_numOfIndices  = 0;
	m_numOfTextures = 0;

	m_verts = nullptr;
	m_faces = nullptr;
	m_indices = nullptr;
}

bool BspImporter :: _import()
{
	m_pFile = new File( m_fileName, File::eRead);
	size_t l_uiSlashIndex = 0;

	if (m_fileName.find_last_of( cuT( "\\")) != String::npos)
	{
		l_uiSlashIndex = m_fileName.find_last_of( cuT( "\\")) + 1;
	}

	if (m_fileName.find_last_of( cuT( "/")) != String::npos)
	{
		l_uiSlashIndex = max( l_uiSlashIndex, m_fileName.find_last_of( cuT( "/")) + 1);
	}

	size_t l_uiDotIndex = m_fileName.find_last_of( cuT( "."));

	UIntArray l_faces;
	FloatArray l_sizes;
	String l_name = m_fileName.substr( l_uiSlashIndex, l_uiDotIndex - l_uiSlashIndex);
	String l_meshName = m_fileName.substr( l_uiSlashIndex, l_uiDotIndex - l_uiSlashIndex);
	String l_materialName = m_fileName.substr( l_uiSlashIndex, l_uiDotIndex - l_uiSlashIndex);

	MeshPtr l_pMesh = nullptr;
	Collection<Mesh, String> l_collection;

	if (l_collection.HasElement( l_meshName))
	{
		l_pMesh = l_collection.GetElement( l_meshName);
	}
	else
	{
		l_pMesh = Factory<Mesh>::Create( l_meshName, l_faces, l_sizes, Mesh::eCustom);
		Logger::LogMessage( cuT( "CreatePrimitive - Mesh %s created"), l_meshName.c_str());
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
		
		real l_temp = m_verts[i].m_position[1];
		m_verts[i].m_position[1] = m_verts[i].m_position[2];
		m_verts[i].m_position[2] = -l_temp;
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

void BspImporter::_convertToMesh( MeshPtr p_mesh, BSPTexture * p_textures)
{
	size_t l_i = min( m_fileName.find_last_of( cuT( "/")), m_fileName.find_last_of( cuT( "\\")));
	String l_filePath = m_fileName.substr( 0, l_i + 1);

	MaterialPtrArray l_materials;
	MaterialPtr l_material;
	PassPtr l_pass;
	TextureUnitPtr l_unit;
	for( int i = 0; i < m_numOfTextures; i++)
	{
		String l_strName;
		l_strName = p_mesh->GetName() + cuT( "Material_") + l_strName;
		l_strName << i;
		l_material = Factory<Material>::Create( l_strName, 1);
		l_pass = l_material->GetPass( 0);
		l_pass->SetAmbient( 0.0f, 0.0f, 0.0f, 1.0f);
		l_pass->SetEmissive( 0.5f, 0.5f, 0.5f, 1.0f);
		l_unit = new TextureUnit( RenderSystem::GetSingletonPtr()->CreateTextureRenderer());
		l_unit->SetTexture2D( l_filePath + cuT( "../") + String( p_textures[i].m_strName) + cuT( ".jpg"));
		l_pass->AddTextureUnit( l_unit);
		l_materials.push_back( l_material);
	}

	SubmeshPtr l_submesh;
	BSPFace l_bspFace;
	for (int i = 0 ; i < m_numOfFaces ; i++)
	{
		l_bspFace = m_faces[i];
		l_submesh = p_mesh->CreateSubmesh( 1);

		for (int j = 0 ; j < l_bspFace.m_numOfVerts ; j++)
		{
			l_submesh->AddVertex( m_verts[l_bspFace.m_startIndex + j].m_position[0], m_verts[l_bspFace.m_startIndex + j].m_position[1], m_verts[l_bspFace.m_startIndex + j].m_position[2]);
		}

		l_submesh->ComputeFacesFromPolygonVertex();
		l_submesh->SetMaterial( l_materials[l_bspFace.m_textureID]);
	}
}
*/