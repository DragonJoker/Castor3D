#include "PrecompiledHeader.h"
#include "geometry/mesh/Submesh.h"

#include "geometry/mesh/PNTrianglesDivider.h"
#include "geometry/mesh/LoopDivider.h"
#include "geometry/basic/SmoothingGroup.h"
#include "geometry/basic/Face.h"
#include "material/MaterialManager.h"
#include "render_system/Buffer.h"
#include "render_system/RenderSystem.h"
#include "material/Pass.h"

using namespace Castor3D;

Submesh :: Submesh( size_t p_sgNumber)
	:	m_material( MaterialManager::GetDefaultMaterial()),
		m_strMatName( "DefaultMaterial")
{
	m_triangles = m_pRenderer->GetTriangles();
	m_trianglesNormals = m_pRenderer->GetTrianglesNormals();
	m_trianglesTangents = m_pRenderer->GetTrianglesTangents();
	m_trianglesTexCoords = m_pRenderer->GetTrianglesTexCoords();
	m_lines = m_pRenderer->GetLines();
	m_linesNormals = m_pRenderer->GetLinesNormals();
	m_linesTexCoords = m_pRenderer->GetLinesTexCoords();

	for (size_t i = 0 ; i < p_sgNumber ; i++)
	{
		AddSmoothingGroup();
	}

}

Submesh :: ~Submesh()
{
	Cleanup();
}

void Submesh :: Cleanup()
{
//	map::deleteAll( m_subdivisers);
//	vector::deleteAll( m_smoothGroups);
	m_subdivisers.clear();
	m_smoothGroups.clear();
//	vector::deleteAll( m_vertex);
//	vector::deleteAll( m_tangents);
//	vector::deleteAll( m_normals);
	m_vertex.clear();
	m_tangents.clear();
	m_normals.clear();

	m_box.reset();
	m_sphere.reset();
}

void Submesh :: SetMaterial( const String & p_matName)
{
	if (p_matName == CU_T( "DefaultMaterial"))
	{
		m_material = MaterialManager::GetDefaultMaterial();
	}
	else
	{
		m_material = MaterialManager::GetElementByName( p_matName);
	}

	if (m_material.expired())
	{
		Log::LogMessage( CU_T( "Material null"));
	}
	else
	{
		m_strMatName = p_matName;
	}
}

String Submesh :: GetMaterialName()const
{
	String l_strReturn = C3DEmptyString;

	if ( ! m_material.expired())
	{
		l_strReturn = m_strMatName;
	}

	return l_strReturn;
}

void Submesh :: ComputeContainers()
{
	m_box.reset();
	m_sphere.reset();

	if (m_vertex.size() <= 0)
	{
		return;
	}

	Point3r l_min( *m_vertex[0]);
	Point3r l_max( *m_vertex[0]);

	size_t l_nbVertex = m_vertex.size();

	for (size_t i = 1 ; i < l_nbVertex ; i++)
	{
		if (m_vertex[i]->m_coords[0] > l_max[0])
		{
			l_max[0] = m_vertex[i]->m_coords[0];
		}
		if (m_vertex[i]->m_coords[0] < l_min[0])
		{
			l_min[0] = m_vertex[i]->m_coords[0];
		}
		if (m_vertex[i]->m_coords[1] > l_max[1])
		{
			l_max[1] = m_vertex[i]->m_coords[1];
		}
		if (m_vertex[i]->m_coords[1] < l_min[1])
		{
			l_min[1] = m_vertex[i]->m_coords[1];
		}
		if (m_vertex[i]->m_coords[2] > l_max[2])
		{
			l_max[2] = m_vertex[i]->m_coords[2];
		}
		if (m_vertex[i]->m_coords[2] < l_min[2])
		{
			l_min[2] = m_vertex[i]->m_coords[2];
		}
	}

	m_box = new ComboBox( l_min, l_max);
	m_sphere = new Sphere( *m_box);
}

SmoothingGroupPtr Submesh :: GetSmoothGroup( size_t p_index)const
{
	SmoothingGroupPtr l_pReturn = NULL;

	if (p_index < m_smoothGroups.size() && p_index >= 0)
	{
		l_pReturn = m_smoothGroups[p_index];
	}

	return l_pReturn;
}

size_t Submesh :: GetNbFaces()const
{
	size_t l_nbFaces = 0;

	for (size_t i = 0 ; i < m_smoothGroups.size() ; i++)
	{
		l_nbFaces += m_smoothGroups[i]->m_faces.size();
	}

	return l_nbFaces;
}

void Submesh :: SetFlatNormals()
{
	//On calcule la normale et la tangente de chaque face
	FacePtr l_pFace;
	SmoothingGroupPtr l_group;

	for (size_t i = 0 ; i < m_smoothGroups.size() ; i++)
	{
		l_group = m_smoothGroups[i];

		for (size_t j = 0 ; j < l_group->m_faces.size() ; j++)
		{
			l_pFace = l_group->m_faces[j];

			const Point3r & l_v1 = * l_pFace->m_vertex1;
			const Point3r & l_v2 = * l_pFace->m_vertex2;
			const Point3r & l_v3 = * l_pFace->m_vertex3;

			const Point3r & l_w1 = l_pFace->m_vertex1TexCoord;
			const Point3r & l_w2 = l_pFace->m_vertex2TexCoord;
			const Point3r & l_w3 = l_pFace->m_vertex3TexCoord;

			Point3r l_vEdge1 = l_v2 - l_v1;
			Point3r l_vEdge2 = l_v3 - l_v1;
			Point3r l_vEdge1uv = l_w2 - l_w1;
			Point3r l_vEdge2uv = l_w3 - l_w1;

			Point3r l_vNormal = l_vEdge2.GetNormal( l_vEdge1);

			l_pFace->m_faceNormal = l_vNormal;

			real l_fCoeff = l_vEdge1uv[1] * l_vEdge2uv[0] - l_vEdge1uv[0] * l_vEdge2uv[1];

			if (l_fCoeff != 0.0f)
			{
				real l_fMult = 1.0f / l_fCoeff;
				l_pFace->m_faceTangent = (l_vEdge1 * -l_vEdge2uv[1] + l_vEdge2 * l_vEdge1uv[1]) * l_fMult;
				l_pFace->m_faceTangent.Normalise();
			}
		}
	}
}

void Submesh :: SetSmoothNormals()
{
	for (size_t i = 0 ; i < m_smoothGroups.size() ; i++)
	{
		m_smoothGroups[i]->SetNormals( m_vertex.size(), m_normals, m_tangents);
	}
}

void Submesh :: SetNormals()
{
	SetFlatNormals();
	SetSmoothNormals();
}

void Submesh :: CreateBuffers()
{
	m_pRenderer->Initialise();
}

void Submesh :: _setBuffersForVertex( unsigned int & p_trianglesIndex,
									  unsigned int & p_linesIndex,
									  unsigned int & p_trianglesTexIndex,
									  unsigned int & p_linesTexIndex,
									  VertexPtr p_v1, VertexPtr p_v2,
									  const Point3r & p_texCoord1,
									  const Point3r & p_texCoord2)
{
	// Triangles
	m_trianglesTexCoords->Add( p_texCoord1[0]);
	m_trianglesTexCoords->Add( p_texCoord1[1]);
	m_triangles->Add( p_v1->m_coords[0]);
	m_triangles->Add( p_v1->m_coords[1]);
	m_triangles->Add( p_v1->m_coords[2]);

	// Lines
	m_linesTexCoords->Add( p_texCoord1[0]);
	m_linesTexCoords->Add( p_texCoord1[1]);
	m_lines->Add( p_v1->m_coords[0]);
	m_lines->Add( p_v1->m_coords[1]);
	m_lines->Add( p_v1->m_coords[2]);
	m_linesTexCoords->Add( p_texCoord2[0]);
	m_linesTexCoords->Add( p_texCoord2[1]);
	m_lines->Add( p_v2->m_coords[0]);
	m_lines->Add( p_v2->m_coords[1]);
	m_lines->Add( p_v2->m_coords[2]);
}

void Submesh :: _setBufferSmoothNormals( FacePtr p_pFace)
{
	m_trianglesNormals->Add( p_pFace->m_vertex1Normal->m_coords[0]);
	m_trianglesNormals->Add( p_pFace->m_vertex1Normal->m_coords[1]);
	m_trianglesNormals->Add( p_pFace->m_vertex1Normal->m_coords[2]);

	m_trianglesNormals->Add( p_pFace->m_vertex2Normal->m_coords[0]);
	m_trianglesNormals->Add( p_pFace->m_vertex2Normal->m_coords[1]);
	m_trianglesNormals->Add( p_pFace->m_vertex2Normal->m_coords[2]);

	m_trianglesNormals->Add( p_pFace->m_vertex3Normal->m_coords[0]);
	m_trianglesNormals->Add( p_pFace->m_vertex3Normal->m_coords[1]);
	m_trianglesNormals->Add( p_pFace->m_vertex3Normal->m_coords[2]);

	if (RenderSystem::UseShaders())
	{
		m_trianglesTangents->Add( p_pFace->m_vertex1Tangent->m_coords[0]);
		m_trianglesTangents->Add( p_pFace->m_vertex1Tangent->m_coords[1]);
		m_trianglesTangents->Add( p_pFace->m_vertex1Tangent->m_coords[2]);

		m_trianglesTangents->Add( p_pFace->m_vertex2Tangent->m_coords[0]);
		m_trianglesTangents->Add( p_pFace->m_vertex2Tangent->m_coords[1]);
		m_trianglesTangents->Add( p_pFace->m_vertex2Tangent->m_coords[2]);

		m_trianglesTangents->Add( p_pFace->m_vertex3Tangent->m_coords[0]);
		m_trianglesTangents->Add( p_pFace->m_vertex3Tangent->m_coords[1]);
		m_trianglesTangents->Add( p_pFace->m_vertex3Tangent->m_coords[2]);
	}
}

void Submesh :: _setBufferFlatNormals( FacePtr p_pFace)
{
	m_trianglesNormals->Add( p_pFace->m_faceNormal[0]);
	m_trianglesNormals->Add( p_pFace->m_faceNormal[1]);
	m_trianglesNormals->Add( p_pFace->m_faceNormal[2]);

	m_trianglesNormals->Add( p_pFace->m_faceNormal[0]);
	m_trianglesNormals->Add( p_pFace->m_faceNormal[1]);
	m_trianglesNormals->Add( p_pFace->m_faceNormal[2]);
	
	m_trianglesNormals->Add( p_pFace->m_faceNormal[0]);
	m_trianglesNormals->Add( p_pFace->m_faceNormal[1]);
	m_trianglesNormals->Add( p_pFace->m_faceNormal[2]);

	if (RenderSystem::UseShaders())
	{
		m_trianglesTangents->Add( p_pFace->m_faceTangent[0]);
		m_trianglesTangents->Add( p_pFace->m_faceTangent[1]);
		m_trianglesTangents->Add( p_pFace->m_faceTangent[2]);

		m_trianglesTangents->Add( p_pFace->m_faceTangent[0]);
		m_trianglesTangents->Add( p_pFace->m_faceTangent[1]);
		m_trianglesTangents->Add( p_pFace->m_faceTangent[2]);

		m_trianglesTangents->Add( p_pFace->m_faceTangent[0]);
		m_trianglesTangents->Add( p_pFace->m_faceTangent[1]);
		m_trianglesTangents->Add( p_pFace->m_faceTangent[2]);
	}
}

void Submesh :: SetBuffersForFace( FacePtr p_face, unsigned int & p_trianglesIndex,
								   unsigned int & p_linesIndex,
								   unsigned int & p_trianglesTexIndex,
								   unsigned int & p_linesTexIndex)
{
	_setBuffersForVertex( p_trianglesIndex, p_linesIndex, p_trianglesTexIndex, p_linesTexIndex,
						  p_face->m_vertex1, p_face->m_vertex2, p_face->m_vertex1TexCoord, p_face->m_vertex2TexCoord);

	_setBuffersForVertex( p_trianglesIndex, p_linesIndex, p_trianglesTexIndex, p_linesTexIndex,
						  p_face->m_vertex2, p_face->m_vertex3, p_face->m_vertex2TexCoord, p_face->m_vertex3TexCoord);

	_setBuffersForVertex( p_trianglesIndex, p_linesIndex, p_trianglesTexIndex, p_linesTexIndex,
						  p_face->m_vertex3, p_face->m_vertex1, p_face->m_vertex3TexCoord, p_face->m_vertex1TexCoord);
}

void Submesh :: CreateNormalsBuffer( NormalsMode p_nm)
{
	m_normalsMode = p_nm;
	m_trianglesNormals->InitialiseBuffer( m_triangles->GetFilled(), 0);

	if (RenderSystem::UseShaders())
	{
		m_trianglesTangents->InitialiseBuffer( m_triangles->GetFilled(), 0);
	}

	unsigned int l_trianglesIndex = 0;

	if (m_normalsMode == nmSmoothGroups)
	{
		for (size_t i = 0 ; i < m_smoothGroups.size() ; i++)
		{
			for (size_t j = 0 ; j < m_smoothGroups[i]->m_faces.size() ; j++)
			{
				_setBufferSmoothNormals( m_smoothGroups[i]->m_faces[j]);
			}
		}
	}
	else if (m_normalsMode == nmFace)
	{
		for (size_t i = 0 ; i < m_smoothGroups.size() ; i++)
		{
			for (size_t j = 0 ; j < m_smoothGroups[i]->m_faces.size() ; j++)
			{
				_setBufferFlatNormals( m_smoothGroups[i]->m_faces[j]);
			}
		}
	}
}

int Submesh :: IsInMyVertex( const Vertex & p_vertex)
{
	for (unsigned int i = 0 ; i < m_vertex.size() ; i++)
	{
		if (p_vertex == *m_vertex[i])
		{
			return (int)i;
		}
	}
	return -1;
}

VertexPtr Submesh :: AddVertex( real x, real y, real z)
{
	VertexPtr l_result = new Vertex( x, y, z, m_vertex.size());
	return AddVertex( l_result);
}

VertexPtr Submesh :: AddVertex( const Vertex & p_v)
{
	VertexPtr l_result = new Vertex( p_v);
	return AddVertex( l_result);
}

VertexPtr Submesh :: AddVertex( VertexPtr p_v)
{
	p_v->m_index = m_vertex.size();
	m_vertex.push_back( p_v);
	m_tangents.push_back( new Point3r);
	m_normals.push_back( new Point3r);
	return p_v;
}

VertexPtr Submesh :: AddVertex( real * p_v)
{
	VertexPtr l_result = new Vertex( p_v[0], p_v[1], p_v[2], m_vertex.size());
	return AddVertex( l_result);
}

FacePtr Submesh :: AddFace( VertexPtr a, VertexPtr b, VertexPtr c, size_t p_sgIndex)
{
	FacePtr l_face = new Face( a, b, c);
	_addFace( l_face, p_sgIndex);
	m_triangles->IncreaseSize( 3 * 3);
	m_trianglesTexCoords->IncreaseSize( 3 * 2);
	if (RenderSystem::UseShaders())
	{
		m_trianglesTangents->IncreaseSize( 3 * 3);
	}
	m_lines->IncreaseSize( 6 * 3);
	m_linesTexCoords->IncreaseSize( 6 * 2);
	return l_face;
}

FacePtr Submesh :: AddFace( size_t a, size_t b, size_t c, size_t p_sgIndex)
{
	VertexPtr l_v1, l_v2, l_v3;

	if (a >= m_vertex.size() || b >= m_vertex.size() || c >= m_vertex.size())
	{
		return NULL;
	}

	l_v1 = m_vertex[a];
	l_v2 = m_vertex[b];
	l_v3 = m_vertex[c];

	return AddFace( l_v1, l_v2, l_v3, p_sgIndex);
}

void Submesh :: AddQuadFace( VertexPtr a, VertexPtr b, VertexPtr c, VertexPtr d, size_t p_sgIndex, const Point3r & p_ptMinUV, const Point3r & p_ptMaxUV)
{
	FacePtr l_pFace;
	l_pFace = AddFace( a, b, c, p_sgIndex);
	l_pFace->SetTexCoordV1( p_ptMinUV[0], p_ptMinUV[1]);
	l_pFace->SetTexCoordV2( p_ptMaxUV[0], p_ptMinUV[1]);
	l_pFace->SetTexCoordV3( p_ptMaxUV[0], p_ptMaxUV[1]);

	l_pFace = AddFace( c, d, a, p_sgIndex);
	l_pFace->SetTexCoordV1( p_ptMaxUV[0], p_ptMaxUV[1]);
	l_pFace->SetTexCoordV2( p_ptMinUV[0], p_ptMaxUV[1]);
	l_pFace->SetTexCoordV3( p_ptMinUV[0], p_ptMinUV[1]);
}

void Submesh :: AddQuadFace( size_t a, size_t b, size_t c, size_t d, size_t p_sgIndex, const Point3r & p_ptMinUV, const Point3r & p_ptMaxUV)
{
	FacePtr l_pFace;
	l_pFace = AddFace( a, b, c, p_sgIndex);
	l_pFace->SetTexCoordV1( p_ptMinUV[0], p_ptMinUV[1]);
	l_pFace->SetTexCoordV2( p_ptMaxUV[0], p_ptMinUV[1]);
	l_pFace->SetTexCoordV3( p_ptMaxUV[0], p_ptMaxUV[1]);

	l_pFace = AddFace( c, d, a, p_sgIndex);
	l_pFace->SetTexCoordV1( p_ptMaxUV[0], p_ptMaxUV[1]);
	l_pFace->SetTexCoordV2( p_ptMinUV[0], p_ptMaxUV[1]);
	l_pFace->SetTexCoordV3( p_ptMinUV[0], p_ptMinUV[1]);
}

SmoothingGroupPtr Submesh :: AddSmoothingGroup()
{
	SmoothingGroupPtr l_pReturn = new SmoothingGroup( m_smoothGroups.size() + 1);
	m_smoothGroups.push_back( l_pReturn);

	return l_pReturn;
}

void Submesh :: GenerateBuffers()
{
	m_triangles->InitialiseBuffer( m_triangles->GetSize(), 0);
	m_trianglesTexCoords->InitialiseBuffer( m_trianglesTexCoords->GetSize(), 0);
	m_lines->InitialiseBuffer( m_lines->GetSize(), 0);
	m_linesTexCoords->InitialiseBuffer( m_linesTexCoords->GetSize(), 0);

	unsigned int l_trianglesCoordsIndex = 0;
	unsigned int l_trianglesTexCoordsIndex = 0;
	unsigned int l_trianglesTanCoordsIndex = 0;
	unsigned int l_linesCoordsIndex = 0;
	unsigned int l_linesTexCoordsIndex = 0;

	for (size_t i = 0 ; i < m_smoothGroups.size() ; i++)
	{
		for (size_t j = 0 ; j < m_smoothGroups[i]->m_faces.size() ; j++)
		{
			SetBuffersForFace( m_smoothGroups[i]->m_faces[j], l_trianglesCoordsIndex, l_linesCoordsIndex,  l_trianglesTexCoordsIndex, l_linesTexCoordsIndex);
		}
	}
}

SubmeshPtr Submesh :: Clone()
{
	SubmeshPtr l_clone = new Submesh();
	FacePtr l_tmpFace;
	bool l_found;
	size_t l_index;

	size_t l_nbVertex = m_vertex.size();

	//On effectue une copie des vertex
	for (size_t i = 0 ; i < l_nbVertex ; i++)
	{
		l_clone->m_vertex.push_back( new Vertex( * m_vertex[i]));
	}

	SmoothingGroupPtr l_pGroup, l_pNewGroup;

	for (size_t i = 0 ; i < m_smoothGroups.size() ; i++)
	{
		l_pGroup = m_smoothGroups[i];
		l_pNewGroup = new SmoothingGroup( l_pGroup->m_idGroup);

		for (size_t j = 0 ; j < l_pGroup->m_faces.size() ; j++)
		{
			l_tmpFace = new Face;
			l_found = false;
			l_index = 0;

			while ( ! l_found && l_index < l_nbVertex)
			{
				if (l_pGroup->m_faces[j]->m_vertex1 == m_vertex[l_index])
				{
					l_tmpFace->m_vertex1 = l_clone->m_vertex[l_index];
					l_found = true;
				}
				l_index++;
			}
			l_found = false;
			l_index = 0;

			while ( ! l_found && l_index < l_nbVertex)
			{
				if (l_pGroup->m_faces[j]->m_vertex2 == m_vertex[l_index])
				{
					l_tmpFace->m_vertex2 = l_clone->m_vertex[l_index];
					l_found = true;
				}
				l_index++;
			}
			l_found = false;
			l_index = 0;

			while ( ! l_found && l_index < l_nbVertex)
			{
				if (l_pGroup->m_faces[j]->m_vertex3 == m_vertex[l_index])
				{
					l_tmpFace->m_vertex3 = l_clone->m_vertex[l_index];
					l_found = true;
				}	
				l_index++;
			}

			l_tmpFace->m_faceNormal[0] = l_pGroup->m_faces[j]->m_faceNormal[0];
			l_tmpFace->m_faceNormal[1] = l_pGroup->m_faces[j]->m_faceNormal[1];
			l_tmpFace->m_faceNormal[2] = l_pGroup->m_faces[j]->m_faceNormal[2];

			l_pNewGroup->m_faces.push_back( l_tmpFace);
		}
	}

	return l_clone;
}

void Submesh :: Subdivide( SubdivisionMode p_mode, Point3rPtr p_center)
{
	if (p_mode == SMPNTriangles)
	{
		_subdividePNTriangles( p_center);
	}
	else if (p_mode == SMLoop)
	{
		_subdivideLoop( p_center);
	}
	else
	{
//		_subdivideQuads( p_center);
	}

	GenerateBuffers();
	SetNormals();
}

void Submesh :: Apply( eDRAW_TYPE p_displayMode)
{
	MaterialPtr l_pMaterial = m_material;
	PassPtrArray l_passes = l_pMaterial->GetPasses();
	PassPtr l_pass;
	TextureUnitPtrArray l_textures;

	for (size_t j = 0 ; j < l_passes.size() ; j++)
	{
		l_pass = l_passes[j];
		l_textures = l_pass->GetTextureUnits();
		l_pass->Apply( p_displayMode);
		m_pRenderer->Draw( p_displayMode, l_pass);
		l_pass->Remove();
	}
}

bool Submesh :: Write( File & p_file)const
{
	size_t l_nbVertex;
	size_t l_nbFaces;
	FacePtr l_face;
	size_t l_matNameLength = m_strMatName.size();

	bool l_bReturn = p_file.Write( l_matNameLength) == sizeof( size_t);

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteArray( m_strMatName.c_str(), l_matNameLength) == l_matNameLength * sizeof( Char);
	}

	l_nbVertex = m_vertex.size();

	if (l_bReturn)
	{
		l_bReturn = p_file.Write( l_nbVertex) == sizeof( size_t);
	}

	if (l_bReturn)
	{
		for (size_t i = 0; i < l_nbVertex && l_bReturn ; i++)
		{
			l_bReturn = m_vertex[i]->Write( p_file);
		}
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.Write( m_smoothGroups.size()) == sizeof( size_t);
	}

	if (l_bReturn)
	{
		for (size_t i = 0 ; i < m_smoothGroups.size() && l_bReturn ; i++)
		{
			l_nbFaces = m_smoothGroups[i]->m_faces.size();

			l_bReturn = p_file.Write( m_smoothGroups[i]->m_idGroup) == sizeof( size_t);

			if (l_bReturn)
			{
				l_bReturn = p_file.Write( l_nbFaces) == sizeof( size_t);
			}

			for (size_t j = 0 ; j < l_nbFaces && l_bReturn ; j++)
			{
				l_face = m_smoothGroups[i]->m_faces[j];

				l_bReturn = p_file.Write( l_face->m_vertex1->m_index) == sizeof( size_t);

				if (l_bReturn)
				{
					l_bReturn = p_file.Write( l_face->m_vertex2->m_index) == sizeof( size_t);
				}

				if (l_bReturn)
				{
					l_bReturn = p_file.Write( l_face->m_vertex3->m_index) == sizeof( size_t);
				}

				if (l_bReturn)
				{
					l_bReturn = l_face->Write( p_file);
				}
			}// endloop on faces
		}// endloop on smoothing groups
	}// endif l_bReturn;

	return l_bReturn;
}

bool Submesh :: Read( File & p_file)
{
	Cleanup();

	m_triangles->Cleanup();
	m_trianglesTexCoords->Cleanup();
	m_lines->Cleanup();
	m_linesTexCoords->Cleanup();

	size_t l_nbVertex;
	size_t l_nbGroups;
	size_t l_nbFaces;
	size_t l_iV1, l_iV2, l_iV3;
	size_t l_namelength;
	FacePtr l_face;
	Char l_name[256];
	bool l_bReturn = p_file.Read( l_namelength) == sizeof( size_t);

	if (l_bReturn)
	{
		l_bReturn = p_file.ReadArray( l_name, l_namelength) == l_namelength * sizeof( Char);
	}

	if (l_bReturn)
	{
		l_name[l_namelength] = 0;
		MaterialPtr l_material( MaterialManager::GetElementByName( l_name));

		if ( ! l_material.null())
		{
			m_material = l_material;
		}
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.Read( l_nbVertex) == sizeof( size_t);
	}

	if (l_bReturn)
	{
		VertexPtr l_v;
		for (size_t i = 0; i < l_nbVertex && l_bReturn ; i++)
		{
			l_v = new Vertex;

			l_bReturn = l_v->Read( p_file);

			if (l_bReturn)
			{
				AddVertex( l_v);
			}
		}
	}

	l_nbGroups = 0;

	if (l_bReturn)
	{
		l_bReturn = p_file.Read( l_nbGroups) == sizeof( size_t);
	}

	if (l_bReturn)
	{
		SmoothingGroupPtr l_pGroup;

		for (size_t i = 0 ; i < l_nbGroups && l_bReturn ; i++)
		{
			l_pGroup = AddSmoothingGroup();

			l_bReturn = p_file.Read( l_pGroup->m_idGroup) == sizeof( size_t);

			if (l_bReturn)
			{
				l_bReturn = p_file.Read( l_nbFaces) == sizeof( size_t);
			}

			if (l_bReturn)
			{
				for (size_t j = 0 ; j < l_nbFaces && l_bReturn ; j++)
				{
					l_iV1 = 0;
					l_iV2 = 0;
					l_iV3 = 0;

					l_bReturn = p_file.Read( l_iV1) == sizeof( size_t);

					if (l_bReturn)
					{
						l_bReturn = p_file.Read( l_iV2) == sizeof( size_t);
					}

					if (l_bReturn)
					{
						l_bReturn = p_file.Read( l_iV3) == sizeof( size_t);
					}

					if (l_bReturn)
					{
						l_face = AddFace( l_iV1, l_iV2, l_iV3, 0);
						l_bReturn = l_face->Read( p_file);
					}
				}
			}
		}
	}

	if (l_bReturn)
	{
		GenerateBuffers();
	}

	return l_bReturn;
}
/*
void Submesh :: InvertNormals()
{
	for (size_t i = 0 ; i < m_smoothGroups.size() ; i++)
	{
		for (size_t j = 0 ; j < m_smoothGroups[i]->m_faces.size() ; j++)
		{
			m_smoothGroups[i]->m_faces[j]->m_faceNormal.Reverse();
		}
	}

	for (size_t i = 0 ; i < m_normals.size() ; i++)
	{
		m_normals[i]->Reverse();
	}

	for (size_t i = 0 ; i < m_tangents.size() ; i++)
	{
		m_tangents[i]->Reverse();
	}

	for (unsigned int i = 0 ; i < m_trianglesNormals->GetFilled() ; i++)
	{
		m_trianglesNormals->Negate( i);
	}
}
*/
void Submesh :: ComputeFacesFromPolygonVertex()
{
	int l_nbFaces = m_vertex.size() - 2;
	unsigned int l_nbTrianglesCoords = 4 * l_nbFaces * 3 * 3;
	unsigned int l_nbTrianglesTexCoords = 4 * l_nbFaces * 3 * 2;
	m_triangles->InitialiseBuffer( l_nbTrianglesCoords, 0);
	m_trianglesTexCoords->InitialiseBuffer( l_nbTrianglesTexCoords, 0);
	unsigned int l_trianglesCoordsIndex = 0;
	unsigned int l_trianglesTexCoordsIndex = 0;

	unsigned int l_nbLinesCoords = 4 * l_nbFaces * 6 * 3;
	unsigned int l_nbLinesTexCoords = 4 * l_nbFaces * 6 * 2;
	m_lines->InitialiseBuffer( l_nbLinesCoords, 0);
	m_linesTexCoords->InitialiseBuffer( l_nbLinesTexCoords, 0);
	unsigned int l_linesCoordsIndex = 0;
	unsigned int l_linesTexCoordsIndex = 0;

	VertexPtr l_v1, l_v2, l_v3;
	l_v1 = m_vertex[0];
	l_v2 = m_vertex[1];
	l_v3 = m_vertex[2];

	FacePtr l_face = AddFace( l_v1, l_v2, l_v3, 0);
	l_face->SetTexCoordV1( 0.0, 0.0);
	l_face->SetTexCoordV2( 0.0, 0.0);
	l_face->SetTexCoordV3( 0.0, 0.0);

	SetBuffersForFace( l_face, l_trianglesCoordsIndex, l_linesCoordsIndex, 
						l_trianglesTexCoordsIndex, l_linesTexCoordsIndex);

	for (size_t i = 2 ; i < m_vertex.size() - 1 ; i++)
	{
		l_v2 = m_vertex[i];
		l_v3 = m_vertex[i + 1];
		l_face = AddFace( l_v1, l_v2, l_v3, 0);

		l_face->SetTexCoordV1( 0.0, 0.0);
		l_face->SetTexCoordV2( 0.0, 0.0);
		l_face->SetTexCoordV3( 0.0, 0.0);

		SetBuffersForFace( l_face, l_trianglesCoordsIndex, l_linesCoordsIndex, 
							l_trianglesTexCoordsIndex, l_linesTexCoordsIndex);
	}
}

void Submesh :: _subdividePNTriangles( Point3rPtr p_center)
{
	m_pRenderer->Cleanup();

	m_triangles = m_pRenderer->GetTriangles();
	m_trianglesNormals = m_pRenderer->GetTrianglesNormals();
	m_trianglesTangents = m_pRenderer->GetTrianglesTangents();
	m_trianglesTexCoords = m_pRenderer->GetTrianglesTexCoords();
	m_lines = m_pRenderer->GetLines();
	m_linesNormals = m_pRenderer->GetLinesNormals();
	m_linesTexCoords = m_pRenderer->GetLinesTexCoords();

	PNTrianglesSubdiviserPtr l_diviser;

	if (m_subdivisers.find( SMPNTriangles) == m_subdivisers.end())
	{
		l_diviser = new PNTrianglesSubdiviser( this);
		m_subdivisers.insert( SubdiviserPtrModeMap::value_type( SMPNTriangles, l_diviser));
	}
	else
	{
		l_diviser = m_subdivisers.find( SMPNTriangles)->second;
	}

	l_diviser->Subdivide( p_center);
}

void Submesh :: _subdivideLoop( Point3rPtr p_center)
{
	m_pRenderer->Cleanup();

	m_triangles = m_pRenderer->GetTriangles();
	m_trianglesNormals = m_pRenderer->GetTrianglesNormals();
	m_trianglesTangents = m_pRenderer->GetTrianglesTangents();
	m_trianglesTexCoords = m_pRenderer->GetTrianglesTexCoords();
	m_lines = m_pRenderer->GetLines();
	m_linesNormals = m_pRenderer->GetLinesNormals();
	m_linesTexCoords = m_pRenderer->GetLinesTexCoords();

	LoopSubdiviser l_diviser( this);
	l_diviser.Subdivide( p_center);
/*
	LoopSubdiviser * l_diviser;
	if (m_subdivisers.find( SMLoop) == m_subdivisers.end())
	{
		l_diviser = new LoopSubdiviser( this);
		m_subdivisers.insert( C3DMap <SubdivisionMode, Subdiviser *>::value_type( SMLoop, l_diviser));
	}
	else
	{
		l_diviser = static_cast <LoopSubdiviser *>( m_subdivisers.find( SMLoop)->second);
	}

	l_diviser->Subdivide( p_center);
*/

}

void Submesh :: _addFace( FacePtr p_face, size_t p_sgIndex)
{
	m_smoothGroups[p_sgIndex]->m_faces.push_back( p_face);
}