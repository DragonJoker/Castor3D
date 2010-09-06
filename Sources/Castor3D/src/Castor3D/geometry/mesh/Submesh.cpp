#include "PrecompiledHeader.h"

#include "geometry/Module_Geometry.h"

#include "geometry/mesh/Submesh.h"
#include "geometry/mesh/Subdiviser.h"
#include "geometry/mesh/LoopDivider.h"
#include "geometry/mesh/PNTrianglesDivider.h"
#include "geometry/basic/Face.h"
#include "geometry/basic/SmoothingGroup.h"
#include "render_system/RenderSystem.h"
#include "render_system/MeshRenderer.h"
#include "render_system/Buffer.h"
#include "material/MaterialManager.h"
#include "material/Material.h"

#include "Log.h"

using namespace Castor3D;

Submesh :: Submesh( SubmeshRenderer * p_renderer, size_t p_sgNumber)
	:	m_box( NULL),
		m_sphere( NULL),
		m_material( MaterialManager::GetSingletonPtr()->GetDefaultMaterial()),
		m_renderer( p_renderer)
{
	m_renderer->SetTarget( this);

	m_triangles = m_renderer->GetTriangles();
	m_trianglesNormals = m_renderer->GetTrianglesNormals();
	m_trianglesTangents = m_renderer->GetTrianglesTangents();
	m_trianglesTexCoords = m_renderer->GetTrianglesTexCoords();
	m_lines = m_renderer->GetLines();
	m_linesNormals = m_renderer->GetLinesNormals();
	m_linesTexCoords = m_renderer->GetLinesTexCoords();

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
	map::deleteAll( m_subdivisers);
	vector::deleteAll( m_smoothGroups);
	vector::deleteAll( m_vertex);
	vector::deleteAll( m_tangents);
	vector::deleteAll( m_normals);

	delete m_box;
	m_box = NULL;

	delete m_sphere;
	m_sphere = NULL;
}

void Submesh :: SetMaterial( const String & p_matName)
{
	if (p_matName == C3D_T( "DefaultMaterial"))
	{
		m_material = MaterialManager::GetSingletonPtr()->GetDefaultMaterial();
	}
	else
	{
		m_material = reinterpret_cast <Material *>( MaterialManager::GetSingletonPtr()->GetElementByName( p_matName));
	}
	if (m_material == NULL)
	{
		Log::LogMessage( C3D_T( "Material null"));
	}
}

String Submesh :: GetMaterialName()const
{
	if (m_material == NULL)
	{
		return C3DEmptyString;
	}
	return m_material->GetName();
}

void Submesh :: ComputeContainers()
{
	delete m_box;
	m_box = NULL;

	delete m_sphere;
	m_sphere = NULL;

	if (m_vertex.size() <= 0)
	{
		return;
	}

	Vector3f l_min( *m_vertex[0]);
	Vector3f l_max( *m_vertex[0]);

	size_t l_nbVertex = m_vertex.size();

	for (size_t i = 1 ; i < l_nbVertex ; i++)
	{
		if (m_vertex[i]->x > l_max.x)
		{
			l_max.x = m_vertex[i]->x;
		}
		if (m_vertex[i]->x < l_min.x)
		{
			l_min.x = m_vertex[i]->x;
		}
		if (m_vertex[i]->y > l_max.y)
		{
			l_max.y = m_vertex[i]->y;
		}
		if (m_vertex[i]->y < l_min.y)
		{
			l_min.y = m_vertex[i]->y;
		}
		if (m_vertex[i]->z > l_max.z)
		{
			l_max.z = m_vertex[i]->z;
		}
		if (m_vertex[i]->z < l_min.z)
		{
			l_min.z = m_vertex[i]->z;
		}
	}

	m_box = new ComboBox( l_min, l_max);
	m_sphere = new Sphere( *m_box);
}

SmoothingGroup * Submesh :: GetSmoothGroup( size_t p_index)const
{
	SmoothingGroup * l_pReturn = NULL;

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
	Face * l_pFace;
	SmoothingGroup * l_group;

	for (size_t i = 0 ; i < m_smoothGroups.size() ; i++)
	{
		l_group = m_smoothGroups[i];

		for (size_t j = 0 ; j < l_group->m_faces.size() ; j++)
		{
			l_pFace = l_group->m_faces[j];

			const Vector3f & l_v1 = * l_pFace->m_vertex1;
			const Vector3f & l_v2 = * l_pFace->m_vertex2;
			const Vector3f & l_v3 = * l_pFace->m_vertex3;

			const Point2D<float> & l_w1 = l_pFace->m_vertex1TexCoord;
			const Point2D<float> & l_w2 = l_pFace->m_vertex2TexCoord;
			const Point2D<float> & l_w3 = l_pFace->m_vertex3TexCoord;

			Vector3f l_vEdge1 = l_v2 - l_v1;
			Vector3f l_vEdge2 = l_v3 - l_v1;
			Point2D<float> l_vEdge1uv = l_w2 - l_w1;
			Point2D<float> l_vEdge2uv = l_w3 - l_w1;

			Vector3f l_vNormal = l_vEdge2.GetNormal( l_vEdge1);

			l_pFace->m_faceNormal = l_vNormal;

			float l_fCoeff = l_vEdge1uv.v * l_vEdge2uv.u - l_vEdge1uv.u * l_vEdge2uv.v;

			if (l_fCoeff != 0.0f)
			{
				float l_fMult = 1.0f / l_fCoeff;
				l_pFace->m_faceTangent = (l_vEdge1 * -l_vEdge2uv.y + l_vEdge2 * l_vEdge1uv.y) * l_fMult;
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
	m_renderer->Initialise();
}

void Submesh :: _setBuffersForVertex( unsigned int & p_trianglesIndex,
									  unsigned int & p_linesIndex,
									  unsigned int & p_trianglesTexIndex,
									  unsigned int & p_linesTexIndex,
									  Vector3f * p_v1, Vector3f * p_v2,
									  const Point3D<float> & p_texCoord1,
									  const Point3D<float> & p_texCoord2)
{
	// Triangles
	m_trianglesTexCoords->Add( p_texCoord1.const_ptr()[0]);
	m_trianglesTexCoords->Add( p_texCoord1.const_ptr()[1]);
	m_triangles->Add( p_v1->x);
	m_triangles->Add( p_v1->y);
	m_triangles->Add( p_v1->z);

	// Lines
	m_linesTexCoords->Add( p_texCoord1.const_ptr()[0]);
	m_linesTexCoords->Add( p_texCoord1.const_ptr()[1]);
	m_lines->Add( p_v1->x);
	m_lines->Add( p_v1->y);
	m_lines->Add( p_v1->z);
	m_linesTexCoords->Add( p_texCoord2.const_ptr()[0]);
	m_linesTexCoords->Add( p_texCoord2.const_ptr()[1]);
	m_lines->Add( p_v2->x);
	m_lines->Add( p_v2->y);
	m_lines->Add( p_v2->z);
}

void Submesh :: _setBufferSmoothNormals( Face * p_pFace)
{
	m_trianglesNormals->Add( p_pFace->m_vertex1Normal->x);
	m_trianglesNormals->Add( p_pFace->m_vertex1Normal->y);
	m_trianglesNormals->Add( p_pFace->m_vertex1Normal->z);

	m_trianglesNormals->Add( p_pFace->m_vertex2Normal->x);
	m_trianglesNormals->Add( p_pFace->m_vertex2Normal->y);
	m_trianglesNormals->Add( p_pFace->m_vertex2Normal->z);

	m_trianglesNormals->Add( p_pFace->m_vertex3Normal->x);
	m_trianglesNormals->Add( p_pFace->m_vertex3Normal->y);
	m_trianglesNormals->Add( p_pFace->m_vertex3Normal->z);

	m_trianglesTangents->Add( p_pFace->m_vertex1Tangent->x);
	m_trianglesTangents->Add( p_pFace->m_vertex1Tangent->y);
	m_trianglesTangents->Add( p_pFace->m_vertex1Tangent->z);

	m_trianglesTangents->Add( p_pFace->m_vertex2Tangent->x);
	m_trianglesTangents->Add( p_pFace->m_vertex2Tangent->y);
	m_trianglesTangents->Add( p_pFace->m_vertex2Tangent->z);

	m_trianglesTangents->Add( p_pFace->m_vertex3Tangent->x);
	m_trianglesTangents->Add( p_pFace->m_vertex3Tangent->y);
	m_trianglesTangents->Add( p_pFace->m_vertex3Tangent->z);
}

void Submesh :: _setBufferFlatNormals( Face * p_pFace)
{
	m_trianglesNormals->Add( p_pFace->m_faceNormal.x);
	m_trianglesNormals->Add( p_pFace->m_faceNormal.y);
	m_trianglesNormals->Add( p_pFace->m_faceNormal.z);

	m_trianglesNormals->Add( p_pFace->m_faceNormal.x);
	m_trianglesNormals->Add( p_pFace->m_faceNormal.y);
	m_trianglesNormals->Add( p_pFace->m_faceNormal.z);
	
	m_trianglesNormals->Add( p_pFace->m_faceNormal.x);
	m_trianglesNormals->Add( p_pFace->m_faceNormal.y);
	m_trianglesNormals->Add( p_pFace->m_faceNormal.z);

	m_trianglesTangents->Add( p_pFace->m_faceTangent.x);
	m_trianglesTangents->Add( p_pFace->m_faceTangent.y);
	m_trianglesTangents->Add( p_pFace->m_faceTangent.z);

	m_trianglesTangents->Add( p_pFace->m_faceTangent.x);
	m_trianglesTangents->Add( p_pFace->m_faceTangent.y);
	m_trianglesTangents->Add( p_pFace->m_faceTangent.z);

	m_trianglesTangents->Add( p_pFace->m_faceTangent.x);
	m_trianglesTangents->Add( p_pFace->m_faceTangent.y);
	m_trianglesTangents->Add( p_pFace->m_faceTangent.z);
}

void Submesh :: SetBuffersForFace( Face * p_face, unsigned int & p_trianglesIndex,
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
	m_trianglesTangents->InitialiseBuffer( m_triangles->GetFilled(), 0);

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

int Submesh :: IsInMyVertex( const Vector3f & p_vertex)
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

Vector3f * Submesh :: AddVertex( float x, float y, float z)
{
	Vector3f * l_result = new Vector3f( x, y, z, m_vertex.size());
	return AddVertex( l_result);
}

Vector3f * Submesh :: AddVertex( Vector3f * p_v)
{
	p_v->m_index = m_vertex.size();
	m_vertex.push_back( p_v);
	m_tangents.push_back( new Vector3f);
	m_normals.push_back( new Vector3f);
	return p_v;
}

Vector3f * Submesh :: AddVertex( float * p_v)
{
	Vector3f * l_result = new Vector3f( p_v[0], p_v[1], p_v[2], m_vertex.size());
	return AddVertex( l_result);
}

Face * Submesh :: AddFace( Vector3f * a, Vector3f * b, Vector3f * c, size_t p_sgIndex)
{
	Face * l_face = new Face( a, b, c);
	_addFace( l_face, p_sgIndex);
	m_triangles->IncreaseSize( 3 * 3);
	m_trianglesTexCoords->IncreaseSize( 3 * 2);
	m_trianglesTangents->IncreaseSize( 3 * 3);
	m_lines->IncreaseSize( 6 * 3);
	m_linesTexCoords->IncreaseSize( 6 * 2);
	return l_face;
}

Face * Submesh :: AddFace( size_t a, size_t b, size_t c, size_t p_sgIndex)
{
	Vector3f * l_v1, * l_v2, * l_v3;

	if (a >= m_vertex.size() || b >= m_vertex.size() || c >= m_vertex.size())
	{
		return NULL;
	}

	l_v1 = m_vertex[a];
	l_v2 = m_vertex[b];
	l_v3 = m_vertex[c];

	return AddFace( l_v1, l_v2, l_v3, p_sgIndex);
}

void Submesh :: AddQuadFace( Vector3f * a, Vector3f * b, Vector3f * c, Vector3f * d, size_t p_sgIndex)
{
	Face * l_pFace;
	l_pFace = AddFace( b, d, a, 0);
	SetTexCoordV1( l_pFace, 1.0, 1.0);
	SetTexCoordV2( l_pFace, 0.0, 0.0);
	SetTexCoordV3( l_pFace, 0.0, 1.0);

	l_pFace = AddFace( c, d, b, 0);
	SetTexCoordV1( l_pFace, 1.0, 0.0);
	SetTexCoordV2( l_pFace, 0.0, 0.0);
	SetTexCoordV3( l_pFace, 1.0, 1.0);
}

void Submesh :: AddQuadFace( size_t a, size_t b, size_t c, size_t d, size_t p_sgIndex)
{
	Face * l_pFace;
	l_pFace = AddFace( b, d, a, 0);
	SetTexCoordV1( l_pFace, 1.0, 1.0);
	SetTexCoordV2( l_pFace, 0.0, 0.0);
	SetTexCoordV3( l_pFace, 0.0, 1.0);

	l_pFace = AddFace( c, d, b, 0);
	SetTexCoordV1( l_pFace, 1.0, 0.0);
	SetTexCoordV2( l_pFace, 0.0, 0.0);
	SetTexCoordV3( l_pFace, 1.0, 1.0);
}

SmoothingGroup * Submesh :: AddSmoothingGroup()
{
	SmoothingGroup * l_pReturn = new SmoothingGroup( m_smoothGroups.size() + 1);
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

Submesh * Submesh :: Clone()
{
	Submesh * l_clone = new Submesh( m_renderer);
	Face * l_tmpFace;
	bool l_found;
	size_t l_index;

	size_t l_nbVertex = m_vertex.size();

	//On effectue une copie des vertex
	for (size_t i = 0 ; i < l_nbVertex ; i++)
	{
		l_clone->m_vertex.push_back( new Vector3f( * m_vertex[i]));
	}

	SmoothingGroup * l_pGroup, * l_pNewGroup;

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

void Submesh :: Subdivide( SubdivisionMode p_mode, Vector3f * p_center)
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

bool Submesh :: Write( FileIO & p_file)const
{
	size_t l_nbVertex;
	size_t l_nbFaces;
	Face * l_face;

	size_t l_matNameLength = m_material->GetName().size();
	if ( ! p_file.Write<size_t>( l_matNameLength))
	{
		return false;
	}

	if ( ! p_file.WriteArray<Char>( m_material->GetName().c_str(), l_matNameLength))
	{
		return false;
	}

	l_nbVertex = m_vertex.size();
	//On écrit le nombre de vertex
	if ( ! p_file.Write<size_t>( l_nbVertex))
	{
		return false;
	}
	// on écrit les vertex
	for (size_t i = 0; i < l_nbVertex ; i++)
	{
		if ( ! WriteVertex( m_vertex[i], p_file))
		{
			Log::LogMessage( "Unable to write vertex %i", i);
			return false;
		}
	}

	// on écrit le nombre de groupes
	if ( ! p_file.Write<size_t>( m_smoothGroups.size()))
	{
		return false;
	}

	for (size_t i = 0 ; i < m_smoothGroups.size() ; i++)
	{
		l_nbFaces = m_smoothGroups[i]->m_faces.size();

		// on écrit l'ID du groupe
		if ( ! p_file.Write<size_t>( m_smoothGroups[i]->m_idGroup))
		{
			return false;
		}

		// on écrit le nombre de faces du groupe
		if ( ! p_file.Write<size_t>( l_nbFaces))
		{
			return false;
		}

		// on écrit chaque face du groupe
		for (size_t j = 0 ; j < l_nbFaces ; j++)
		{
			l_face = m_smoothGroups[i]->m_faces[j];

			if ( ! p_file.Write<size_t>( l_face->m_vertex1->m_index))
			{
				return false;
			}

			if ( ! p_file.Write<size_t>( l_face->m_vertex2->m_index))
			{
				return false;
			}

			if ( ! p_file.Write<size_t>( l_face->m_vertex3->m_index))
			{
				return false;
			}

			if ( ! WriteFace( l_face, p_file))
			{
				return false;
			}
		}// endloop on faces
	}// endloop on smoothing groups

	return true;
}

bool Submesh :: Read( FileIO & p_file)
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
	Face * l_face;
	Char l_name[256];

	if ( ! p_file.Read<size_t>( l_namelength))
	{
		return false;
	}
	if ( ! p_file.ReadArray<Char>( l_name, l_namelength))
	{
		return false;
	}
	l_name[l_namelength] = 0;
	Material * l_material = MaterialManager::GetSingletonPtr()->GetElementByName( l_name);
	if (l_material != NULL)
	{
		m_material = l_material;
	}

	if ( ! p_file.Read<size_t>( l_nbVertex))
	{
		return false;
	}

	Vector3f * l_v;
	for (size_t i = 0; i < l_nbVertex ; i++)
	{
		l_v = new Vector3f;

		if ( ! ReadVertex( l_v, p_file))
		{
			Log::LogMessage( C3D_T( "Unable to retrieve vertex %i"), i);
			return false;
		}

		AddVertex( l_v);
	}

	l_nbGroups = 0;

	if ( ! p_file.Read<size_t>( l_nbGroups))
	{
		return false;
	}

	SmoothingGroup * l_pGroup;

	for (size_t i = 0 ; i < l_nbGroups ; i++)
	{
		l_pGroup = AddSmoothingGroup();

		if ( ! p_file.Read<size_t>( l_pGroup->m_idGroup))
		{
			return false;
		}

		if ( ! p_file.Read<size_t>( l_nbFaces))
		{
			return false;
		}

		for (size_t j = 0 ; j < l_nbFaces ; j++)
		{
			l_iV1 = 0;
			l_iV2 = 0;
			l_iV3 = 0;

			if ( ! p_file.Read<size_t>( l_iV1))
			{
				return false;
			}

			if ( ! p_file.Read<size_t>( l_iV2))
			{
				return false;
			}

			if ( ! p_file.Read<size_t>( l_iV3))
			{
				return false;
			}

			l_face = AddFace( l_iV1, l_iV2, l_iV3, 0);

			if ( ! ReadFace( l_face, p_file))
			{
				return false;
			}
		}
	}

	GenerateBuffers();

	return true;
}

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

	Vector3f * l_v1, * l_v2, * l_v3;
	l_v1 = m_vertex[0];
	l_v2 = m_vertex[1];
	l_v3 = m_vertex[2];

	Face * l_face = AddFace( l_v1, l_v2, l_v3, 0);
	SetTexCoordV1( l_face, 0.0, 0.0);
	SetTexCoordV2( l_face, 0.0, 0.0);
	SetTexCoordV3( l_face, 0.0, 0.0);

	SetBuffersForFace( l_face, l_trianglesCoordsIndex, l_linesCoordsIndex, 
						l_trianglesTexCoordsIndex, l_linesTexCoordsIndex);

	for (size_t i = 2 ; i < m_vertex.size() - 1 ; i++)
	{
		l_v2 = m_vertex[i];
		l_v3 = m_vertex[i + 1];
		l_face = AddFace( l_v1, l_v2, l_v3, 0);

		SetTexCoordV1( l_face, 0.0, 0.0);
		SetTexCoordV2( l_face, 0.0, 0.0);
		SetTexCoordV3( l_face, 0.0, 0.0);

		SetBuffersForFace( l_face, l_trianglesCoordsIndex, l_linesCoordsIndex, 
							l_trianglesTexCoordsIndex, l_linesTexCoordsIndex);
	}
}

void Submesh :: _subdividePNTriangles( Vector3f * p_center)
{
	m_renderer->Cleanup();

	m_triangles = m_renderer->GetTriangles();
	m_trianglesNormals = m_renderer->GetTrianglesNormals();
	m_trianglesTangents = m_renderer->GetTrianglesTangents();
	m_trianglesTexCoords = m_renderer->GetTrianglesTexCoords();
	m_lines = m_renderer->GetLines();
	m_linesNormals = m_renderer->GetLinesNormals();
	m_linesTexCoords = m_renderer->GetLinesTexCoords();

	PNTrianglesSubdiviser * l_diviser;

	if (m_subdivisers.find( SMPNTriangles) == m_subdivisers.end())
	{
		l_diviser = new PNTrianglesSubdiviser( this);
		m_subdivisers.insert( std::map <SubdivisionMode, Subdiviser *>::value_type( SMPNTriangles, l_diviser));
	}
	else
	{
		l_diviser = static_cast <PNTrianglesSubdiviser *>( m_subdivisers.find( SMPNTriangles)->second);
	}

	l_diviser->Subdivide( p_center);
}

void Submesh :: _subdivideLoop( Vector3f * p_center)
{
	m_renderer->Cleanup();

	m_triangles = m_renderer->GetTriangles();
	m_trianglesNormals = m_renderer->GetTrianglesNormals();
	m_trianglesTangents = m_renderer->GetTrianglesTangents();
	m_trianglesTexCoords = m_renderer->GetTrianglesTexCoords();
	m_lines = m_renderer->GetLines();
	m_linesNormals = m_renderer->GetLinesNormals();
	m_linesTexCoords = m_renderer->GetLinesTexCoords();

	LoopSubdiviser l_diviser( this);
	l_diviser.Subdivide( p_center);
/*
	LoopSubdiviser * l_diviser;
	if (m_subdivisers.find( SMLoop) == m_subdivisers.end())
	{
		l_diviser = new LoopSubdiviser( this);
		m_subdivisers.insert( std::map <SubdivisionMode, Subdiviser *>::value_type( SMLoop, l_diviser));
	}
	else
	{
		l_diviser = static_cast <LoopSubdiviser *>( m_subdivisers.find( SMLoop)->second);
	}

	l_diviser->Subdivide( p_center);
*/

}