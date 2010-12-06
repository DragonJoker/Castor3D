#include "PrecompiledHeader.h"
#include "geometry/mesh/Submesh.h"

#include "geometry/mesh/PNTrianglesDivider.h"
#include "geometry/mesh/LoopDivider.h"
#include "geometry/basic/SmoothingGroup.h"
#include "geometry/basic/Vertex.h"
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
	m_lines = m_pRenderer->GetLines();

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
	m_subdivisers.clear();
	m_smoothGroups.clear();
	m_vertex.clear();
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
		Logger::LogMessage( CU_T( "Material null"));
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
	if (m_vertex.size() <= 0)
	{
		return;
	}

	Point3r l_min( m_vertex[0]);
	Point3r l_max( m_vertex[0]);

	size_t l_nbVertex = m_vertex.size();

	for (size_t i = 1 ; i < l_nbVertex ; i++)
	{
		if (m_vertex[i][0] > l_max[0])
		{
			l_max[0] = m_vertex[i][0];
		}
		if (m_vertex[i][0] < l_min[0])
		{
			l_min[0] = m_vertex[i][0];
		}
		if (m_vertex[i][1] > l_max[1])
		{
			l_max[1] = m_vertex[i][1];
		}
		if (m_vertex[i][1] < l_min[1])
		{
			l_min[1] = m_vertex[i][1];
		}
		if (m_vertex[i][2] > l_max[2])
		{
			l_max[2] = m_vertex[i][2];
		}
		if (m_vertex[i][2] < l_min[2])
		{
			l_min[2] = m_vertex[i][2];
		}
	}

	m_box.Load( l_min, l_max);
	m_sphere.Load( m_box);
}

size_t Submesh :: GetNbFaces()const
{
	size_t l_nbFaces = 0;

	for (size_t i = 0 ; i < m_smoothGroups.size() ; i++)
	{
		l_nbFaces += m_smoothGroups[i].m_faces.size();
	}

	return l_nbFaces;
}

int Submesh :: IsInMyVertex( const Point3r & p_vertex)
{
	for (unsigned int i = 0 ; i < m_vertex.size() ; i++)
	{
		if (p_vertex == m_vertex[i])
		{
			return (int)i;
		}
	}
	return -1;
}

void Submesh :: ComputeFlatNormals()
{
	//On calcule la normale et la tangente de chaque face
	for (size_t i = 0 ; i < m_smoothGroups.size() ; i++)
	{
		SmoothingGroup & l_group = m_smoothGroups[i];

		for (size_t j = 0 ; j < l_group.m_faces.size() ; j++)
		{
			Face & l_face = l_group.m_faces[j];

			const Point3r & l_v1 = l_face[0].GetCoords();
			const Point3r & l_v2 = l_face[1].GetCoords();
			const Point3r & l_v3 = l_face[2].GetCoords();

			const Point2r & l_w1 = l_face[0].GetTexCoord();
			const Point2r & l_w2 = l_face[1].GetTexCoord();
			const Point2r & l_w3 = l_face[2].GetTexCoord();

			Point3r l_vEdge1 = l_v2 - l_v1;
			Point3r l_vEdge2 = l_v3 - l_v1;
			Point2r l_vEdge1uv = l_w2 - l_w1;
			Point2r l_vEdge2uv = l_w3 - l_w1;

			Point3r l_vNormal = l_vEdge2 ^ l_vEdge1;
			l_vNormal.Normalise();

			l_face.m_faceNormal = l_vNormal;

			real l_fCoeff = l_vEdge1uv[1] * l_vEdge2uv[0] - l_vEdge1uv[0] * l_vEdge2uv[1];

			if (l_fCoeff != 0.0f)
			{
				real l_fMult = 1.0f / l_fCoeff;
				l_face.m_faceTangent = (l_vEdge1 * -l_vEdge2uv[1] + l_vEdge2 * l_vEdge1uv[1]) * l_fMult;
				l_face.m_faceTangent.Normalise();
			}
		}
	}
}

void Submesh :: ComputeSmoothNormals()
{
	for (size_t i = 0 ; i < m_smoothGroups.size() ; i++)
	{
		m_smoothGroups[i].SetNormals( m_vertex.size());
	}
}

void Submesh :: ComputeNormals()
{
	ComputeFlatNormals();
	ComputeSmoothNormals();
}

void Submesh :: InitialiseBuffers()
{
	m_pRenderer->Initialise();
}

void Submesh :: SetNormals( eNORMALS_MODE p_nm)
{
	m_normalsMode = p_nm;

	unsigned int l_trianglesIndex = 0;

	switch (m_normalsMode)
	{
	case eSmooth:
		for (size_t i = 0 ; i < m_smoothGroups.size() ; i++)
		{
			for (size_t j = 0 ; j < m_smoothGroups[i].m_faces.size() ; j++)
			{
				m_smoothGroups[i].m_faces[j].SetSmoothNormals();
			}
		}
		break;

	case eFace:
		for (size_t i = 0 ; i < m_smoothGroups.size() ; i++)
		{
			for (size_t j = 0 ; j < m_smoothGroups[i].m_faces.size() ; j++)
			{
				m_smoothGroups[i].m_faces[j].SetFlatNormals();
			}
		}
		break;
	}
}

Point3r & Submesh :: AddVertex( real x, real y, real z)
{
	m_vertex.push_back( Point3r( x, y, z));
	return m_vertex[m_vertex.size() - 1];
}

Point3r & Submesh :: AddVertex( const Point3r & p_v)
{
	m_vertex.push_back( p_v);
	return m_vertex[m_vertex.size() - 1];
}

Point3r & Submesh :: AddVertex( real * p_v)
{
	m_vertex.push_back( p_v);
	return m_vertex[m_vertex.size() - 1];
}

Face & Submesh :: AddFace( size_t a, size_t b, size_t c, size_t p_sgIndex)
{
	CASTOR_ASSERT( a < m_vertex.size() && b < m_vertex.size() && c < m_vertex.size());
	Face l_face( m_vertex[a], a, m_vertex[b], b, m_vertex[c], c);
	Face & l_return = _addFace( l_face, p_sgIndex);

	m_triangles->IncreaseSize( 11 * 3, false);
	m_lines->IncreaseSize( 22 * 3, false);

	return l_return;
}

void Submesh :: AddQuadFace( size_t a, size_t b, size_t c, size_t d, size_t p_sgIndex, const Point3r & p_ptMinUV, const Point3r & p_ptMaxUV)
{
	Face & l_face1 = AddFace( a, b, c, p_sgIndex);
	l_face1.SetVertexTexCoords( 0, p_ptMinUV[0], p_ptMinUV[1]);
	l_face1.SetVertexTexCoords( 1, p_ptMaxUV[0], p_ptMinUV[1]);
	l_face1.SetVertexTexCoords( 2, p_ptMaxUV[0], p_ptMaxUV[1]);

	Face & l_face2 = AddFace( c, d, a, p_sgIndex);					  
	l_face2.SetVertexTexCoords( 0, p_ptMaxUV[0], p_ptMaxUV[1]);
	l_face2.SetVertexTexCoords( 1, p_ptMinUV[0], p_ptMaxUV[1]);
	l_face2.SetVertexTexCoords( 2, p_ptMinUV[0], p_ptMinUV[1]);
}

SmoothingGroup & Submesh :: AddSmoothingGroup()
{
	SmoothingGroup l_group( m_smoothGroups.size() + 1);
	m_smoothGroups.push_back( l_group);
	return m_smoothGroups[m_smoothGroups.size() - 1];
}

void Submesh :: AddSmoothGroup( const SmoothingGroup & p_group)
{
	m_smoothGroups.push_back( p_group);
}

void Submesh :: GenerateBuffers()
{
	m_triangles->InitialiseBuffer( m_triangles->GetSize(), 0);
	m_lines->InitialiseBuffer( m_lines->GetSize(), 0);

	for (size_t i = 0 ; i < m_smoothGroups.size() ; i++)
	{
		for (size_t j = 0 ; j < m_smoothGroups[i].m_faces.size() ; j++)
		{
			m_triangles->AddVertex( m_smoothGroups[i].m_faces[j][0], false);
			m_triangles->AddVertex( m_smoothGroups[i].m_faces[j][1], false);
			m_triangles->AddVertex( m_smoothGroups[i].m_faces[j][2], false);
		}
	}
}

SubmeshPtr Submesh :: Clone()
{
	SubmeshPtr l_clone( new Submesh());
	bool l_found;
	size_t l_index;

	size_t l_nbVertex = m_vertex.size();

	//On effectue une copie des vertex
	for (size_t i = 0 ; i < l_nbVertex ; i++)
	{
		l_clone->m_vertex.push_back( m_vertex[i]);
	}

	for (size_t i = 0 ; i < m_smoothGroups.size() ; i++)
	{
		const SmoothingGroup & l_group = m_smoothGroups[i];
		SmoothingGroup & l_newGroup = l_clone->AddSmoothingGroup();
		l_newGroup.m_idGroup = l_group.m_idGroup;

		for (size_t j = 0 ; j < l_group.m_faces.size() ; j++)
		{
			Face l_tmpFace;
			l_found = false;
			l_index = 0;

			while ( ! l_found && l_index < l_nbVertex)
			{
				if (l_group.m_faces[j][0].GetCoords() == m_vertex[l_index])
				{
					l_tmpFace[0].SetCoords( l_clone->m_vertex[l_index]);
					l_found = true;
				}
				l_index++;
			}
			l_found = false;
			l_index = 0;

			while ( ! l_found && l_index < l_nbVertex)
			{
				if (l_group.m_faces[j][1].GetCoords() == m_vertex[l_index])
				{
					l_tmpFace[1].SetCoords( l_clone->m_vertex[l_index]);
					l_found = true;
				}
				l_index++;
			}
			l_found = false;
			l_index = 0;

			while ( ! l_found && l_index < l_nbVertex)
			{
				if (l_group.m_faces[j][2].GetCoords() == m_vertex[l_index])
				{
					l_tmpFace[2].SetCoords( l_clone->m_vertex[l_index]);
					l_found = true;
				}	
				l_index++;
			}

			l_tmpFace.m_faceNormal[0] = l_group.m_faces[j].m_faceNormal[0];
			l_tmpFace.m_faceNormal[1] = l_group.m_faces[j].m_faceNormal[1];
			l_tmpFace.m_faceNormal[2] = l_group.m_faces[j].m_faceNormal[2];

			l_newGroup.m_faces.push_back( l_tmpFace);
		}
	}

	return l_clone;
}

void Submesh :: Subdivide( SubdivisionMode p_mode, Point3r * p_center)
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
	ComputeNormals();
}

void Submesh :: Render( eDRAW_TYPE p_displayMode)
{
	MaterialPtr l_pMaterial = m_material;
	PassPtrArray l_passes = l_pMaterial->GetPasses();
	PassPtr l_pass;
	TextureUnitPtrArray l_textures;

	for (size_t j = 0 ; j < l_passes.size() ; j++)
	{
		l_pass = l_passes[j];
		l_textures = l_pass->GetTextureUnits();
		l_pass->Render( p_displayMode);
		m_pRenderer->Draw( p_displayMode, l_pass);
		l_pass->EndRender();
	}
}

bool Submesh :: Write( File & p_file)const
{
	size_t l_nbVertex;
	size_t l_nbFaces;
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
			l_bReturn = m_vertex[i].Write( p_file);
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
			l_nbFaces = m_smoothGroups[i].m_faces.size();

			l_bReturn = p_file.Write( m_smoothGroups[i].m_idGroup) == sizeof( size_t);

			if (l_bReturn)
			{
				l_bReturn = p_file.Write( l_nbFaces) == sizeof( size_t);
			}

			for (size_t j = 0 ; j < l_nbFaces && l_bReturn ; j++)
			{
				const Face & l_face = m_smoothGroups[i].m_faces[j];

				for (size_t k = 0 ; k <  3 && l_bReturn ; k++)
				{
					l_bReturn = p_file.Write( l_face[k].m_index) == sizeof( size_t);
				}

				if (l_bReturn)
				{
					l_bReturn = l_face.Write( p_file);
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
	m_lines->Cleanup();

	size_t l_nbVertex;
	size_t l_nbGroups;
	size_t l_nbFaces;
	size_t l_iV1, l_iV2, l_iV3;
	size_t l_namelength;
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
		for (size_t i = 0; i < l_nbVertex && l_bReturn ; i++)
		{
			Point3r l_v;

			l_bReturn = l_v.Read( p_file);

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

		for (size_t i = 0 ; i < l_nbGroups && l_bReturn ; i++)
		{
			SmoothingGroup & l_group = AddSmoothingGroup();

			l_bReturn = p_file.Read( l_group.m_idGroup) == sizeof( size_t);

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
						l_bReturn = AddFace( l_iV1, l_iV2, l_iV3, 0).Read( p_file);
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

void Submesh :: ComputeFacesFromPolygonVertex()
{
	int l_nbFaces = m_vertex.size() - 2;

	unsigned int l_nbTrianglesCoords = 4 * l_nbFaces * 12 * 3;
	m_triangles->InitialiseBuffer( l_nbTrianglesCoords, 0);

	unsigned int l_nbLinesCoords = 4 * l_nbFaces * 24 * 3;
	m_lines->InitialiseBuffer( l_nbLinesCoords, 0);

	Point3r l_v1 = m_vertex[0];
	Point3r l_v2 = m_vertex[1];
	Point3r l_v3 = m_vertex[2];

	Face & l_face = AddFace( 0, 1, 2, 0);
	l_face.SetVertexTexCoords( 0, 0.0, 0.0);
	l_face.SetVertexTexCoords( 1, 0.0, 0.0);
	l_face.SetVertexTexCoords( 2, 0.0, 0.0);

	m_triangles->AddVertex( l_face[0], false);
	m_triangles->AddVertex( l_face[1], false);
	m_triangles->AddVertex( l_face[2], false);

	for (size_t i = 2 ; i < m_vertex.size() - 1 ; i++)
	{
		l_v2 = m_vertex[i];
		l_v3 = m_vertex[i + 1];
		Face & l_faceA = AddFace( 0, i, i + 1, 0);

		l_faceA.SetVertexTexCoords( 0, 0.0, 0.0);
		l_faceA.SetVertexTexCoords( 1, 0.0, 0.0);
		l_faceA.SetVertexTexCoords( 2, 0.0, 0.0);

		m_triangles->AddVertex( l_faceA[0], false);
		m_triangles->AddVertex( l_faceA[1], false);
		m_triangles->AddVertex( l_faceA[2], false);
	}
}

void Submesh :: _subdividePNTriangles( Point3r * p_center)
{
	m_pRenderer->Cleanup();

	m_triangles = m_pRenderer->GetTriangles();
	m_lines = m_pRenderer->GetLines();

	PNTrianglesSubdiviser l_diviser( this);
	l_diviser.Subdivide( * p_center);
}

void Submesh :: _subdivideLoop( Point3r * p_center)
{
	m_pRenderer->Cleanup();

	m_triangles = m_pRenderer->GetTriangles();
	m_lines = m_pRenderer->GetLines();

	LoopSubdiviser l_diviser( this);
	l_diviser.Subdivide( * p_center);
}

Face & Submesh :: _addFace( const Face & p_face, size_t p_sgIndex)
{
	m_smoothGroups[p_sgIndex].m_faces.push_back( p_face);
	return m_smoothGroups[p_sgIndex].m_faces[m_smoothGroups[p_sgIndex].m_faces.size() - 1];
}