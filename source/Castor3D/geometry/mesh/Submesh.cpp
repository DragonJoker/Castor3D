#include "Castor3D/PrecompiledHeader.h"
#include "Castor3D/geometry/mesh/Submesh.h"

#include "Castor3D/geometry/basic/SmoothingGroup.h"
#include "Castor3D/geometry/basic/Vertex.h"
#include "Castor3D/geometry/basic/Face.h"
#include "Castor3D/geometry/mesh/Subdivider.h"
#include "Castor3D/material/MaterialManager.h"
#include "Castor3D/material/Material.h"
#include "Castor3D/render_system/Buffer.h"
#include "Castor3D/render_system/RenderSystem.h"
#include "Castor3D/material/Pass.h"
#include "Castor3D/main/Root.h"
#include "Castor3D/scene/SceneManager.h"

using namespace Castor3D;

//*************************************************************************************************

SubmeshRenderer :: SubmeshRenderer( SceneManager * p_pManager)
	:	Renderer<Submesh, SubmeshRenderer>( p_pManager)
	,	m_eLastDrawType( eDRAW_TYPE( -1))
	,	m_eLastNormalsMode( eNORMALS_MODE( -1))
{
}

SubmeshRenderer :: ~SubmeshRenderer()
{
	Cleanup();
}

void SubmeshRenderer :: Cleanup()
{
	BufferManager::GetSingleton().DeleteBuffer<real>( m_vertex);

	for (int i = 0 ; i < eNbDrawType ; i++)
	{
		BufferManager::GetSingleton().DeleteBuffer<unsigned int>( m_indices[i]);
	}
}

void SubmeshRenderer :: Draw( eDRAW_TYPE p_eMode, const Pass & p_pass)
{
	if (p_eMode != m_eLastDrawType)
	{
		m_eLastDrawType = p_eMode;

//		Initialise();
	}

	_drawBuffers( p_pass);
}

void SubmeshRenderer :: _createVBOs()
{
	m_vertex = RenderSystem::CreateBuffer<VertexInfosBuffer>();

	for (int i = 0 ; i < eNbDrawType ; i++)
	{
		m_indices[i] = RenderSystem::CreateBuffer<IndicesBuffer>();
	}
}

//*************************************************************************************************

Submesh :: Submesh( size_t p_sgNumber)
	:	m_strMatName( "DefaultMaterial")
{
	m_vertex = m_pRenderer->GetVertex();

	for (int i = 0 ; i < eNbDrawType ; i++)
	{
		m_indices[i] = m_pRenderer->GetIndices( eDRAW_TYPE( i));
	}

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
	m_smoothGroups.clear();
	m_points.clear();
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
	if (m_points.size() <= 0)
	{
		return;
	}

	Point3r l_min( * m_points[0]);
	Point3r l_max( * m_points[0]);

	size_t l_nbVertex = m_points.size();

	for (size_t i = 1 ; i < l_nbVertex ; i++)
	{
		if ((*m_points[i])[0] > l_max[0])
		{
			l_max[0] = (* m_points[i])[0];
		}
		if ((*m_points[i])[0] < l_min[0])
		{
			l_min[0] = (* m_points[i])[0];
		}
		if ((*m_points[i])[1] > l_max[1])
		{
			l_max[1] = (* m_points[i])[1];
		}
		if ((*m_points[i])[1] < l_min[1])
		{
			l_min[1] = (* m_points[i])[1];
		}
		if ((*m_points[i])[2] > l_max[2])
		{
			l_max[2] = (* m_points[i])[2];
		}
		if ((*m_points[i])[2] < l_min[2])
		{
			l_min[2] = (* m_points[i])[2];
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
		l_nbFaces += m_smoothGroups[i]->GetNbFaces();
	}

	return l_nbFaces;
}

int Submesh :: IsInMyPoints( const Point3r & p_vertex)
{
	int l_iReturn = -1;

	for (unsigned int i = 0 ; i < m_points.size() && l_iReturn == -1 ; i++)
	{
		if (p_vertex == * m_points[i])
		{
			l_iReturn = int( i);
		}
	}

	return l_iReturn;
}

void Submesh :: ComputeFlatNormals()
{
	//On calcule la normale et la tangente de chaque face
	for (size_t i = 0 ; i < m_smoothGroups.size() ; i++)
	{
		SmoothingGroupPtr l_group = m_smoothGroups[i];

		for (size_t j = 0 ; j < l_group->GetNbFaces() ; j++)
		{
			FacePtr l_face = l_group->GetFace( j);

			const Point3r & l_v1 = l_face->GetVertex( 0).GetCoords();
			const Point3r & l_v2 = l_face->GetVertex( 1).GetCoords();
			const Point3r & l_v3 = l_face->GetVertex( 2).GetCoords();

			const Point2r & l_w1 = l_face->GetVertex( 0).GetTexCoord();
			const Point2r & l_w2 = l_face->GetVertex( 1).GetTexCoord();
			const Point2r & l_w3 = l_face->GetVertex( 2).GetTexCoord();

			Point3r l_vEdge1 = l_v2 - l_v1;
			Point3r l_vEdge2 = l_v3 - l_v1;
			Point2r l_vEdge1uv = l_w2 - l_w1;
			Point2r l_vEdge2uv = l_w3 - l_w1;

			Point3r l_vNormal = l_vEdge2 ^ l_vEdge1;
			l_vNormal.Normalise();

			l_face->SetFlatNormal( l_vNormal);

			real l_fCoeff = l_vEdge1uv[1] * l_vEdge2uv[0] - l_vEdge1uv[0] * l_vEdge2uv[1];

			if (l_fCoeff != 0.0f)
			{
				real l_fMult = 1.0f / l_fCoeff;
				l_face->SetFlatTangent( (l_vEdge1 * -l_vEdge2uv[1] + l_vEdge2 * l_vEdge1uv[1]) * l_fMult);
			}
		}
	}
}

void Submesh :: ComputeSmoothNormals()
{
	for (size_t i = 0 ; i < m_smoothGroups.size() ; i++)
	{
		m_smoothGroups[i]->SetNormals( m_points.size());
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
			for (size_t j = 0 ; j < m_smoothGroups[i]->GetNbFaces() ; j++)
			{
				FacePtr l_face = m_smoothGroups[i]->GetFace( j);
				l_face->SetSmoothNormals();
			}
		}
		break;

	case eFace:
		for (size_t i = 0 ; i < m_smoothGroups.size() ; i++)
		{
			for (size_t j = 0 ; j < m_smoothGroups[i]->GetNbFaces() ; j++)
			{
				FacePtr l_face = m_smoothGroups[i]->GetFace( j);
				l_face->SetFlatNormals();
			}
		}
		break;
	}
}

IdPoint3rPtr Submesh :: AddPoint( real x, real y, real z)
{
	m_points.push_back( IdPoint3rPtr( new IdPoint3r( x, y, z, m_points.size())));
	return m_points[m_points.size() - 1];
}

IdPoint3rPtr Submesh :: AddPoint( const Point3r & p_v)
{
	m_points.push_back( IdPoint3rPtr( new IdPoint3r( p_v, m_points.size())));
	return m_points[m_points.size() - 1];
}

IdPoint3rPtr Submesh :: AddPoint( real * p_v)
{
	m_points.push_back( IdPoint3rPtr( new IdPoint3r( p_v, m_points.size())));
	return m_points[m_points.size() - 1];
}

FacePtr Submesh :: AddFace( size_t a, size_t b, size_t c, size_t p_sgIndex)
{
	CASTOR_ASSERT( a < m_points.size() && b < m_points.size() && c < m_points.size() && p_sgIndex < m_smoothGroups.size());

	m_smoothGroups[p_sgIndex]->AddFace( FacePtr( new Face( * m_points[a], * m_points[b], * m_points[c])));

	m_vertex->IncreaseSize( Vertex::Size * 3, false);

	return m_smoothGroups[p_sgIndex]->GetFace( m_smoothGroups[p_sgIndex]->GetNbFaces() - 1);
}

void Submesh :: AddQuadFace( size_t a, size_t b, size_t c, size_t d, size_t p_sgIndex, const Point3r & p_ptMinUV, const Point3r & p_ptMaxUV)
{
	FacePtr l_face1 = AddFace( a, b, c, p_sgIndex);
	l_face1->SetVertexTexCoords( 0, p_ptMinUV[0], p_ptMinUV[1]);
	l_face1->SetVertexTexCoords( 1, p_ptMaxUV[0], p_ptMinUV[1]);
	l_face1->SetVertexTexCoords( 2, p_ptMaxUV[0], p_ptMaxUV[1]);

	FacePtr l_face2 = AddFace( a, c, d, p_sgIndex);
	l_face2->SetVertexTexCoords( 0, p_ptMinUV[0], p_ptMinUV[1]);
	l_face2->SetVertexTexCoords( 1, p_ptMaxUV[0], p_ptMaxUV[1]);
	l_face2->SetVertexTexCoords( 2, p_ptMinUV[0], p_ptMaxUV[1]);
}

SmoothingGroupPtr Submesh :: AddSmoothingGroup()
{
	SmoothingGroupPtr l_group( new SmoothingGroup( m_smoothGroups.size() + 1));
	m_smoothGroups.push_back( l_group);
	return m_smoothGroups[m_smoothGroups.size() - 1];
}

void Submesh :: AddSmoothGroup( SmoothingGroupPtr p_group)
{
	m_smoothGroups.push_back( p_group);
}

void Submesh :: GenerateBuffers()
{
	_generateVertex();
	_generateTrianglesIndices();
	_generateLinesIndices();
	_generatePointsIndices();
}

SubmeshPtr Submesh :: Clone()
{
	SubmeshPtr l_clone( new Submesh());
	bool l_found;
	size_t l_index;

	size_t l_nbVertex = m_points.size();

	//On effectue une copie des vertex
	for (size_t i = 0 ; i < l_nbVertex ; i++)
	{
		l_clone->m_points.push_back( m_points[i]);
	}

	for (size_t i = 0 ; i < m_smoothGroups.size() ; i++)
	{
		SmoothingGroupPtr l_group = m_smoothGroups[i];
		SmoothingGroupPtr l_newGroup = l_clone->AddSmoothingGroup();
		l_newGroup->SetGroupID( l_group->GetGroupID());

		for (size_t j = 0 ; j < l_group->GetNbFaces() ; j++)
		{
			FacePtr l_tmpFace( new Face());
			l_found = false;
			l_index = 0;

			while ( ! l_found && l_index < l_nbVertex)
			{
				if (l_group->GetFace( j)->GetVertex( 0).GetCoords() == * m_points[l_index])
				{
					l_tmpFace->GetVertex( 0).SetCoords( * l_clone->m_points[l_index]);
					l_found = true;
				}
				l_index++;
			}
			l_found = false;
			l_index = 0;

			while ( ! l_found && l_index < l_nbVertex)
			{
				if (l_group->GetFace( j)->GetVertex( 1).GetCoords() == * m_points[l_index])
				{
					l_tmpFace->GetVertex( 1).SetCoords( * l_clone->m_points[l_index]);
					l_found = true;
				}
				l_index++;
			}
			l_found = false;
			l_index = 0;

			while ( ! l_found && l_index < l_nbVertex)
			{
				if (l_group->GetFace( j)->GetVertex( 2).GetCoords() == * m_points[l_index])
				{
					l_tmpFace->GetVertex( 2).SetCoords( * l_clone->m_points[l_index]);
					l_found = true;
				}	
				l_index++;
			}

			l_tmpFace->GetFlatNormal()[0] = l_group->GetFace( j)->GetFlatNormal()[0];
			l_tmpFace->GetFlatNormal()[1] = l_group->GetFace( j)->GetFlatNormal()[1];
			l_tmpFace->GetFlatNormal()[2] = l_group->GetFace( j)->GetFlatNormal()[2];

			l_newGroup->AddFace( l_tmpFace);
		}
	}

	return l_clone;
}

void Submesh :: Subdivide( SubdividerPtr p_pSubdivider, Point3r * p_center, bool p_bGenerateBuffers, bool p_bThreaded)
{
	for (size_t i = 0 ; i < m_smoothGroups.size() ; i++)
	{
		for (size_t j = 0 ; j < m_smoothGroups[i]->GetNbFaces() ; j++)
		{
			m_smoothGroups[i]->GetFace( j)->GetVertex( 0).UnlinkCoords();
			m_smoothGroups[i]->GetFace( j)->GetVertex( 1).UnlinkCoords();
			m_smoothGroups[i]->GetFace( j)->GetVertex( 2).UnlinkCoords();
		}
	}

	m_pRenderer->Cleanup();
	m_vertex = m_pRenderer->GetVertex();

	for (int i = 0 ; i < eNbDrawType ; i++)
	{
		m_indices[i] = m_pRenderer->GetIndices( eDRAW_TYPE( i));
	}

	p_pSubdivider->Subdivide( * p_center, p_bGenerateBuffers, p_bThreaded);
}

void Submesh :: Render( eDRAW_TYPE p_displayMode)
{
	MaterialPtr l_pMaterial = m_material.lock();
	size_t l_uiCount = l_pMaterial->GetNbPasses();

	for (size_t j = 0 ; j < l_uiCount ; j++)
	{
		PassPtr l_pass = l_pMaterial->GetPass( j);
		l_pass->Render( p_displayMode);
		m_pRenderer->Draw( p_displayMode, * l_pass);
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

	l_nbVertex = m_points.size();

	if (l_bReturn)
	{
		l_bReturn = p_file.Write( l_nbVertex) == sizeof( size_t);
	}

	if (l_bReturn)
	{
		for (size_t i = 0; i < l_nbVertex && l_bReturn ; i++)
		{
			l_bReturn = m_points[i]->Write( p_file);
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
			l_nbFaces = m_smoothGroups[i]->GetNbFaces();

			l_bReturn = p_file.Write( m_smoothGroups[i]->GetGroupID()) == sizeof( size_t);

			if (l_bReturn)
			{
				l_bReturn = p_file.Write( l_nbFaces) == sizeof( size_t);
			}

			for (size_t j = 0 ; j < l_nbFaces && l_bReturn ; j++)
			{
				FacePtr l_face = m_smoothGroups[i]->GetFace( j);

				for (size_t k = 0 ; k <  3 && l_bReturn ; k++)
				{
					l_bReturn = p_file.Write( l_face->GetVertex( k).GetIndex()) == sizeof( size_t);
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

	m_pRenderer->Cleanup();
	m_vertex = m_pRenderer->GetVertex();

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
		MaterialPtr l_material( Root::GetSingletonPtr()->GetSceneManager()->GetMaterialManager()->GetElementByName( l_name));

		if ( ! l_material == NULL)
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
				AddPoint( l_v);
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
			SmoothingGroupPtr l_group = AddSmoothingGroup();
			size_t l_uiID;

			l_bReturn = p_file.Read( l_uiID) == sizeof( size_t);

			if (l_bReturn)
			{
				l_group->SetGroupID( l_uiID);
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
						l_bReturn = AddFace( l_iV1, l_iV2, l_iV3, 0)->Read( p_file);
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
	int l_nbFaces = m_points.size() - 2;

	unsigned int l_nbTrianglesCoords = 4 * l_nbFaces * 12 * 3;
	m_vertex->InitialiseBuffer( l_nbTrianglesCoords, 0);

	IdPoint3rPtr l_v1 = m_points[0];
	IdPoint3rPtr l_v2 = m_points[1];
	IdPoint3rPtr l_v3 = m_points[2];

	FacePtr l_face = AddFace( 0, 1, 2, 0);
	l_face->SetVertexTexCoords( 0, 0.0, 0.0);
	l_face->SetVertexTexCoords( 1, 0.0, 0.0);
	l_face->SetVertexTexCoords( 2, 0.0, 0.0);

	m_vertex->AddVertex( l_face->GetVertex( 0), false);
	m_vertex->AddVertex( l_face->GetVertex( 1), false);
	m_vertex->AddVertex( l_face->GetVertex( 2), false);

	for (size_t i = 2 ; i < m_points.size() - 1 ; i++)
	{
		l_v2 = m_points[i];
		l_v3 = m_points[i + 1];
		FacePtr l_faceA = AddFace( 0, i, i + 1, 0);

		l_faceA->SetVertexTexCoords( 0, 0.0, 0.0);
		l_faceA->SetVertexTexCoords( 1, 0.0, 0.0);
		l_faceA->SetVertexTexCoords( 2, 0.0, 0.0);

		m_vertex->AddVertex( l_faceA->GetVertex( 0), false);
		m_vertex->AddVertex( l_faceA->GetVertex( 1), false);
		m_vertex->AddVertex( l_faceA->GetVertex( 2), false);
	}
}

void Submesh :: _generateVertex()
{
	size_t l_uiSize = m_vertex->GetSize();
	m_vertex->InitialiseBuffer( l_uiSize, 0);

	for (size_t i = 0 ; i < m_smoothGroups.size() ; i++)
	{
		for (size_t j = 0 ; j < m_smoothGroups[i]->GetNbFaces() ; j++)
		{
			FacePtr l_face = m_smoothGroups[i]->GetFace( j);
			m_vertex->AddVertex( l_face->GetVertex( 0), false);
			m_vertex->AddVertex( l_face->GetVertex( 1), false);
			m_vertex->AddVertex( l_face->GetVertex( 2), false);
		}
	}
}

void Submesh :: _generateTrianglesIndices()
{
	size_t l_uiSize = m_vertex->GetFilled() / Vertex::Size;
	m_indices[eTriangles]->InitialiseBuffer( l_uiSize, 0);

	for (size_t i = 0 ; i < l_uiSize ; i++)
	{
		m_indices[eTriangles]->Add( i);
	}
}

void Submesh :: _generateLinesIndices()
{
	size_t l_uiSize = m_vertex->GetFilled() / Vertex::Size;
	m_indices[eLines]->InitialiseBuffer( 2 * l_uiSize, 0);

	for (size_t i = 0 ; i < l_uiSize ; i += 3)
	{
		m_indices[eLines]->Add( i);
		m_indices[eLines]->Add( i + 1);

		m_indices[eLines]->Add( i + 1);
		m_indices[eLines]->Add( i + 2);

		m_indices[eLines]->Add( i + 2);
		m_indices[eLines]->Add( i);
	}
}

void Submesh :: _generatePointsIndices()
{
	size_t l_uiSize = m_vertex->GetFilled() / Vertex::Size;
	m_indices[ePoints]->InitialiseBuffer( l_uiSize, 0);

	for (size_t i = 0 ; i < l_uiSize ; i++)
	{
		m_indices[ePoints]->Add( i);
	}
}

//*************************************************************************************************