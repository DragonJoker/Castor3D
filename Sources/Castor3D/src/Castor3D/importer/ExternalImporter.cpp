#include "PrecompiledHeader.h"

#include "importer/Module_Importer.h"

#include "importer/ExternalImporter.h"
#include "geometry/mesh/Mesh.h"
#include "geometry/mesh/Submesh.h"
#include "material/MaterialManager.h"
#include "material/Material.h"
#include "material/TextureUnit.h"
#include "material/Pass.h"
#include "geometry/primitives/Geometry.h"
#include "render_system/RenderSystem.h"
#include "render_system/MeshRenderer.h"
#include "render_system/Buffer.h"
#include "Log.h"

using namespace Castor3D;

bool ExternalImporter :: Import( const String & p_fileName)
{
	bool l_bReturn = false;

	m_fileName = p_fileName;
	size_t l_i = min( m_fileName.find_last_of( C3D_T( "/")), m_fileName.find_last_of( "\\"));
	m_filePath = m_fileName.substr( 0, l_i + 1);

	l_bReturn = _import();

	return l_bReturn;
}
/*
void ExternalImporter :: _convertToMesh( Mesh * p_mesh, Imported3DModel * p_model)
{
	ImportedMaterialInfo l_mat;
	Material * l_material;
	Pass * l_pass;
	TextureUnit * l_unit;

	for (int i = 0 ; i < p_model->m_numOfMaterials ; i++)
	{
		l_mat = p_model->m_materials[i];
		l_material = MaterialManager::GetSingletonPtr()->CreateMaterial( l_mat.m_strName);
		l_pass = l_material->GetPass( 0);
		l_pass->SetDoubleFace( true);
		l_pass->SetAmbient( l_mat.m_fAmbient[0], l_mat.m_fAmbient[1], l_mat.m_fAmbient[2], 1.0f);
		l_pass->SetDiffuse( l_mat.m_fDiffuse[0], l_mat.m_fDiffuse[1], l_mat.m_fDiffuse[2], 1.0f);
		l_pass->SetSpecular( l_mat.m_fSpecular[0], l_mat.m_fSpecular[1], l_mat.m_fSpecular[2], 1.0f);
		l_pass->SetShininess( l_mat.m_fShininess);

		if ( ! String( l_mat.m_strFile).empty())
		{
			l_unit = new TextureUnit( RenderSystem::GetSingletonPtr()->CreateTextureRenderer());
			l_pass->AddTextureUnit( l_unit);
			FILE * l_file = NULL;
			fopen_s ( & l_file, String( l_mat.m_strFile).char_str(), "rb");

			if (l_file != NULL)
			{
				fclose( l_file);
				l_unit->SetTexture2D( l_mat.m_strFile);
			}
			else
			{
				l_unit->SetTexture2D( m_filePath + String( l_mat.m_strFile));
			}
		}
	}

	Submesh * l_submesh;
	Imported3DObject l_object;
	Face * l_face;

	for (int i = 0 ; i < p_model->m_numOfObjects ; i++)
	{
		l_object = p_model->m_objects[i];

		if (l_object.m_numOfVerts > 0)
		{
			l_submesh = p_mesh->CreateSubmesh( 1);

			l_submesh->GetRenderer()->GetTriangles()->Cleanup();
			l_submesh->GetRenderer()->GetTrianglesTexCoords()->Cleanup();
			l_submesh->GetRenderer()->GetLines()->Cleanup();
			l_submesh->GetRenderer()->GetLinesTexCoords()->Cleanup();

			for (int j = 0 ; j < l_object.m_numOfVerts ; j++)
			{
				l_submesh->AddVertex( l_object.m_vertex[j].x, l_object.m_vertex[j].y, l_object.m_vertex[j].z);
			}

			for (int j = 0 ; j < l_object.m_numOfFaces ; j++)
			{
				if ((l_face = l_submesh->AddFace( l_object.m_faces[j].m_vertIndex[0], l_object.m_faces[j].m_vertIndex[1], l_object.m_faces[j].m_vertIndex[2], 0)) != NULL)
				{
					if (l_object.m_texVerts != NULL)
					{
						SetTexCoordV1( l_face, l_object.m_texVerts[l_object.m_faces[j].m_coordIndex[0]].x, l_object.m_texVerts[l_object.m_faces[j].m_coordIndex[0]].y);
						SetTexCoordV2( l_face, l_object.m_texVerts[l_object.m_faces[j].m_coordIndex[1]].x, l_object.m_texVerts[l_object.m_faces[j].m_coordIndex[1]].y);
						SetTexCoordV3( l_face, l_object.m_texVerts[l_object.m_faces[j].m_coordIndex[2]].x, l_object.m_texVerts[l_object.m_faces[j].m_coordIndex[2]].y);
					}
				}
			}

			if (l_object.m_materialID >= 0 && p_model->m_numOfMaterials > 0)
			{
				l_material = MaterialManager::GetSingletonPtr()->GetElementByName( p_model->m_materials[l_object.m_materialID].m_strName);
				l_submesh->SetMaterial( l_material);
			}

			l_submesh->ComputeContainers();
			l_submesh->GenerateBuffers();
		}

		delete [] l_object.m_faces;
		delete [] l_object.m_normals;
		delete [] l_object.m_texVerts;
		delete [] l_object.m_vertex;
	}

	p_mesh->SetNormals();
}
*/