#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/Submesh.hpp"
#include "Castor3D/SmoothingGroup.hpp"
#include "Castor3D/Vertex.hpp"
#include "Castor3D/Face.hpp"
#include "Castor3D/Subdivider.hpp"
#include "Castor3D/Material.hpp"
#include "Castor3D/Buffer.hpp"
#include "Castor3D/RenderSystem.hpp"
#include "Castor3D/BufferElement.hpp"
#include "Castor3D/Pass.hpp"
#include "Castor3D/Root.hpp"

using namespace Castor3D;

//*************************************************************************************************

bool Loader<Submesh> :: Write( const Castor3D::Submesh & p_submesh, Utils::File & p_file)
{
	bool l_bReturn = p_file.WriteLine( cuT( "\t\tsubmesh\n\t\t{\n")) > 0;

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteLine( cuT( "\t\t\tmaterial ") + p_submesh.GetMaterialName() + cuT( "\n")) > 0;
	}

	for (size_t i = 0 ; i < p_submesh.GetNbPoints() && l_bReturn ; i++)
	{
		l_bReturn = p_file.Print( 1024, cuT( "\t\t\tvertex %f %f %f\n"), (*p_submesh[i])[0], (*p_submesh[i])[1], (*p_submesh[i])[2]) > 0;
	}

	for (size_t i = 0 ; i < p_submesh.GetNbSmoothGroups() ; i++)
	{
		l_bReturn = Loader<SmoothingGroup>::Write( * p_submesh.GetSmoothGroup( i), p_file);
	}

	return l_bReturn;
}

//*************************************************************************************************

SubmeshRenderer :: SubmeshRenderer()
	:	Renderer<Submesh, SubmeshRenderer>()
	,	m_eLastDrawType( ePRIMITIVE_TYPE( -1))
	,	m_eLastNormalsMode( eNORMALS_MODE( -1))
{
}

SubmeshRenderer :: ~SubmeshRenderer()
{
	Cleanup();
}

void SubmeshRenderer :: Cleanup()
{
	CASTOR_TRACK;
	BufferManager * l_pBufferManager = Root::GetSingleton()->GetBufferManager();
	l_pBufferManager->DeleteBuffer<real>( m_vertex);

	for (int i = 0 ; i < ePRIMITIVE_TYPE_COUNT ; i++)
	{
		l_pBufferManager->DeleteBuffer<unsigned int>( m_indices[i]);
	}
}

void SubmeshRenderer :: Draw( ePRIMITIVE_TYPE p_eMode, const Pass & p_pass)
{
	CASTOR_TRACK;
	if (p_eMode != m_eLastDrawType)
	{
		m_eLastDrawType = p_eMode;
	}

	_drawBuffers( p_pass);
}

void SubmeshRenderer :: _createVbo()
{
	CASTOR_TRACK;
	m_vertex = RenderSystem::CreateVertexBuffer( Vertex::VertexDeclarationElements, 4);

	for (int i = 0 ; i < ePRIMITIVE_TYPE_COUNT ; i++)
	{
		m_indices[i] = RenderSystem::CreateIndexBuffer();
	}
}

//*************************************************************************************************

Submesh :: Submesh( size_t p_sgNumber)
	:	m_strMatName( cuT( "DefaultMaterial"))
{
	m_vertex = m_pRenderer->GetVertex();

	for (int i = 0 ; i < ePRIMITIVE_TYPE_COUNT ; i++)
	{
		m_indices[i] = m_pRenderer->GetIndices( ePRIMITIVE_TYPE( i));
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
	CASTOR_TRACK;
	m_smoothGroups.clear();
	m_points.clear();
}

String Submesh :: GetMaterialName()const
{
	String l_strReturn = cuEmptyString;

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

	Point3r l_min;
	Point3r l_max;
	l_min.copy( * m_points[0]);
	l_max.copy( * m_points[0]);

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

int Submesh :: IsInMyPoints( Point3r const & p_vertex)
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
	Point3r l_vEdge1;
	Point3r l_vEdge2;
	Point2r l_vEdge1uv;
	Point2r l_vEdge2uv;
	Point3r l_vNormal;

	//On calcule la normale et la tangente de chaque face
	for (size_t i = 0 ; i < m_smoothGroups.size() ; i++)
	{
		SmoothingGroupPtr l_group = m_smoothGroups[i];

		for (size_t j = 0 ; j < l_group->GetNbFaces() ; j++)
		{
			FacePtr l_face = l_group->GetFace( j);

			Point3r const & l_v1 = l_face->GetVertex( 0).GetCoords();
			Point3r const & l_v2 = l_face->GetVertex( 1).GetCoords();
			Point3r const & l_v3 = l_face->GetVertex( 2).GetCoords();

			const Point2r & l_w1 = l_face->GetVertex( 0).GetTexCoord();
			const Point2r & l_w2 = l_face->GetVertex( 1).GetTexCoord();
			const Point2r & l_w3 = l_face->GetVertex( 2).GetTexCoord();

			l_vEdge1.copy( l_v2 - l_v1);
			l_vEdge2.copy( l_v3 - l_v1);
			l_vEdge1uv.copy( l_w2 - l_w1);
			l_vEdge2uv.copy( l_w3 - l_w1);

			l_vNormal.copy( l_vEdge2 ^ l_vEdge1);
			l_vNormal.normalise();

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
	CASTOR_TRACK;
	ComputeFlatNormals();
	ComputeSmoothNormals();
}

void Submesh :: InitialiseBuffers()
{
	CASTOR_TRACK;
	m_pRenderer->Initialise();
}

void Submesh :: SetNormals( eNORMALS_MODE p_nm)
{
	m_normalsMode = p_nm;

	unsigned int l_trianglesIndex = 0;

	switch (m_normalsMode)
	{
	case eNORMALS_MODE_SMOOTH:
		for (size_t i = 0 ; i < m_smoothGroups.size() ; i++)
		{
			for (size_t j = 0 ; j < m_smoothGroups[i]->GetNbFaces() ; j++)
			{
				FacePtr l_face = m_smoothGroups[i]->GetFace( j);
				l_face->SetSmoothNormals();
			}
		}
		break;

	case eNORMALS_MODE_FACE:
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

IdPoint3rPtr Submesh :: AddPoint( Point3r const & p_v)
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

	m_vertex->resize( Vertex::Size * 3, false);

	return m_smoothGroups[p_sgIndex]->GetFace( m_smoothGroups[p_sgIndex]->GetNbFaces() - 1);
}

FacePtr Submesh :: AddFace( size_t a, size_t b, size_t c, SmoothingGroup & p_group)
{
	CASTOR_ASSERT( a < m_points.size() && b < m_points.size() && c < m_points.size());

	p_group.AddFace( FacePtr( new Face( * m_points[a], * m_points[b], * m_points[c])));

	m_vertex->resize( Vertex::Size * 3, false);

	return p_group.GetFace( p_group.GetNbFaces() - 1);
}

void Submesh :: AddQuadFace( size_t a, size_t b, size_t c, size_t d, size_t p_sgIndex, Point3r const & p_ptMinUV, Point3r const & p_ptMaxUV)
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
	SmoothingGroupPtr l_group( new SmoothingGroup( this, m_smoothGroups.size() + 1));
	m_smoothGroups.push_back( l_group);
	return m_smoothGroups[m_smoothGroups.size() - 1];
}

void Submesh :: AddSmoothGroup( SmoothingGroupPtr p_group)
{
	m_smoothGroups.push_back( p_group);
}

void Submesh :: GenerateBuffers()
{
	CASTOR_TRACK;
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

	for (int i = 0 ; i < ePRIMITIVE_TYPE_COUNT ; i++)
	{
		m_indices[i] = m_pRenderer->GetIndices( ePRIMITIVE_TYPE( i));
	}

	p_pSubdivider->Subdivide( * p_center, p_bGenerateBuffers, p_bThreaded);
}

void Submesh :: Render( ePRIMITIVE_TYPE p_displayMode)
{
	CASTOR_TRACK;
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

void Submesh :: ComputeFacesFromPolygonVertex()
{
	int l_nbFaces = m_points.size() - 2;

	unsigned int l_nbTrianglesCoords = 4 * l_nbFaces * 12 * 3;
	m_vertex->reserve( l_nbTrianglesCoords, 0);

	IdPoint3rPtr l_v1 = m_points[0];
	IdPoint3rPtr l_v2 = m_points[1];
	IdPoint3rPtr l_v3 = m_points[2];

	FacePtr l_face = AddFace( 0, 1, 2, 0);
	l_face->SetVertexTexCoords( 0, 0.0, 0.0);
	l_face->SetVertexTexCoords( 1, 0.0, 0.0);
	l_face->SetVertexTexCoords( 2, 0.0, 0.0);

	m_vertex->AddElement( l_face->GetVertex( 0), false);
	m_vertex->AddElement( l_face->GetVertex( 1), false);
	m_vertex->AddElement( l_face->GetVertex( 2), false);

	for (size_t i = 2 ; i < m_points.size() - 1 ; i++)
	{
		l_v2 = m_points[i];
		l_v3 = m_points[i + 1];
		FacePtr l_faceA = AddFace( 0, i, i + 1, 0);

		l_faceA->SetVertexTexCoords( 0, 0.0, 0.0);
		l_faceA->SetVertexTexCoords( 1, 0.0, 0.0);
		l_faceA->SetVertexTexCoords( 2, 0.0, 0.0);

		m_vertex->AddElement( l_faceA->GetVertex( 0), false);
		m_vertex->AddElement( l_faceA->GetVertex( 1), false);
		m_vertex->AddElement( l_faceA->GetVertex( 2), false);
	}
}

void Submesh :: _generateVertex()
{
	CASTOR_TRACK;
	size_t l_uiSize = m_vertex->capacity();
	m_vertex->reserve( l_uiSize, 0);

	for (size_t i = 0 ; i < m_smoothGroups.size() ; i++)
	{
		for (size_t j = 0 ; j < m_smoothGroups[i]->GetNbFaces() ; j++)
		{
			FacePtr l_face = m_smoothGroups[i]->GetFace( j);
			m_vertex->AddElement( l_face->GetVertex( 0), false);
			m_vertex->AddElement( l_face->GetVertex( 1), false);
			m_vertex->AddElement( l_face->GetVertex( 2), false);
		}
	}
}

void Submesh :: _generateTrianglesIndices()
{
	size_t l_uiSize = m_vertex->size() / Vertex::Size;
	m_indices[ePRIMITIVE_TYPE_TRIANGLES]->reserve( l_uiSize, 0);

	for (size_t i = 0 ; i < l_uiSize ; i++)
	{
		m_indices[ePRIMITIVE_TYPE_TRIANGLES]->push_back( i);
	}
}

void Submesh :: _generateLinesIndices()
{
	size_t l_uiSize = m_vertex->size() / Vertex::Size;
	m_indices[ePRIMITIVE_TYPE_LINES]->reserve( 2 * l_uiSize, 0);

	for (size_t i = 0 ; i < l_uiSize ; i += 3)
	{
		m_indices[ePRIMITIVE_TYPE_LINES]->push_back( i);
		m_indices[ePRIMITIVE_TYPE_LINES]->push_back( i + 1);

		m_indices[ePRIMITIVE_TYPE_LINES]->push_back( i + 1);
		m_indices[ePRIMITIVE_TYPE_LINES]->push_back( i + 2);

		m_indices[ePRIMITIVE_TYPE_LINES]->push_back( i + 2);
		m_indices[ePRIMITIVE_TYPE_LINES]->push_back( i);
	}
}

void Submesh :: _generatePointsIndices()
{
	size_t l_uiSize = m_vertex->size() / Vertex::Size;
	m_indices[ePRIMITIVE_TYPE_POINTS]->reserve( l_uiSize, 0);

	for (size_t i = 0 ; i < l_uiSize ; i++)
	{
		m_indices[ePRIMITIVE_TYPE_POINTS]->push_back( i);
	}
}

BEGIN_SERIALISE_MAP( Submesh, Serialisable)
	ADD_ELEMENT( m_strMatName)
	ADD_ELEMENT( m_points)
	ADD_ELEMENT( m_smoothGroups)
END_SERIALISE_MAP()

BEGIN_PRE_UNSERIALISE( Submesh, Serialisable)
	Cleanup();
	GetRenderer()->Cleanup();
	SetVertex( GetRenderer()->GetVertex());
END_PRE_UNSERIALISE()

BEGIN_POST_UNSERIALISE( Submesh, Serialisable)
	Root::MaterialManager * l_pManager = Root::GetSingleton()->GetMaterialManager();
	MaterialPtr l_material( l_pManager->GetElement( m_strMatName));
	SetMaterial( l_material);
	GenerateBuffers();
END_POST_UNSERIALISE()

//*************************************************************************************************
