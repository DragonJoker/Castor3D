#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/Plane.hpp"
#include "Castor3D/Submesh.hpp"
#include "Castor3D/Vertex.hpp"

using namespace Castor3D;
using namespace Castor;

Plane :: Plane()
	:	MeshCategory	( eMESH_TYPE_PLANE	)
	,	m_depth			( 0					)
	,	m_width			( 0					)
	,	m_subDivisionsW	( 0					)
	,	m_subDivisionsD	( 0					)
{
}

Plane :: ~Plane()
{
}

MeshCategorySPtr Plane :: Create()
{
	return std::make_shared< Plane >();
}

void Plane :: Generate()
{
	m_width = (m_width > 0.0 ? m_width : -m_width);
	m_depth = (m_depth > 0.0 ? m_depth : -m_depth);

	if (m_subDivisionsW < 0 || m_subDivisionsD < 0)
	{
		return;
	}

	uint32_t l_nbVertexW = m_subDivisionsW + 2;
	uint32_t l_nbVertexH = m_subDivisionsD + 2;
	real l_gapW = m_width / (m_subDivisionsW + 1);
	real l_gapH = m_depth / (m_subDivisionsD + 1);
	Point3r l_ptCurrentUV;
	Point3r l_ptPreviousUV;
	BufferElementGroupSPtr l_pVertex;
	Point3r l_ptNormal( 0.0, 1.0, 0.0 );
	Point3r l_ptTangent;
	Point2r l_ptUv;

	SubmeshSPtr l_pSubmesh = GetMesh()->CreateSubmesh();

	for (uint32_t i = 0; i < l_nbVertexW; i++)
	{
		for (uint32_t j = 0; j < l_nbVertexH; j++)
		{
			l_pVertex = l_pSubmesh->AddPoint( (i * l_gapW), (j * l_gapH), 0.0 );
			Vertex::SetTexCoord(	l_pVertex, (i * l_gapW / m_width), (j * l_gapH / m_depth) );
			Vertex::SetNormal(		l_pVertex, 0.0, 0.0, 1.0 );
		}
	}

	for (uint32_t i = 0; i < m_subDivisionsW + 1; i++)
	{
		for (uint32_t j = i * (m_subDivisionsD + 1); j < (i + 1) * (m_subDivisionsD + 1); j++)
		{
			l_pSubmesh->AddFace( j + i,							j + m_subDivisionsW + 2 + i,	j + m_subDivisionsW + 3 + i	);
			l_pSubmesh->AddFace( j + m_subDivisionsW + 3 + i,	j + i + 1,						j + i						);
		}
	}

//	ComputeNormals();
	l_pSubmesh->ComputeTangentsFromNormals();
	GetMesh()->ComputeContainers();
}

void Plane :: Initialise( UIntArray const & p_arrayFaces, RealArray const & p_arrayDimensions)
{
	m_subDivisionsW = p_arrayFaces[0];
	m_subDivisionsD = p_arrayFaces[1];
	m_width = p_arrayDimensions[0];
	m_depth = p_arrayDimensions[1];
	GetMesh()->Cleanup();
	Generate();
}
