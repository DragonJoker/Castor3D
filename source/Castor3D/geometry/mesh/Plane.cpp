#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/geometry/mesh/Plane.h"
#include "Castor3D/geometry/mesh/Submesh.h"
#include "Castor3D/geometry/basic/Vertex.h"
#include "Castor3D/geometry/basic/Face.h"
#include "Castor3D/main/Root.h"
#include "Castor3D/render_system/RenderSystem.h"
#include "Castor3D/render_system/Buffer.h"
#include "Castor3D/geometry/mesh/Plane.h"



using namespace Castor3D;

Plane :: Plane( MeshManager * p_pManager, real width_p, real depth_p, unsigned int p_subdivisionsDepth,
						unsigned int p_subdivisionsWidth, const String & p_name)
	:	Mesh( p_pManager, p_name)
	,	m_depth( depth_p)
	,	m_width( width_p)
	,	m_subDivisionsW( p_subdivisionsWidth)
	,	m_subDivisionsD( p_subdivisionsDepth)
{
	m_meshType = Mesh::ePlane;
	GeneratePoints();
}

Plane :: ~Plane()
{
}

void Plane :: GeneratePoints()
{
	m_width = (m_width > 0.0 ? m_width : -m_width);
	m_depth = (m_depth > 0.0 ? m_depth : -m_depth);

	if (m_subDivisionsW < 0 || m_subDivisionsD < 0)
	{
		return;
	}

	real l_gapW = m_width / (m_subDivisionsW + 1);
	real l_gapD = m_depth / (m_subDivisionsD + 1);
	unsigned int l_nbVertexW = m_subDivisionsW + 2;
	unsigned int l_nbVertexD = m_subDivisionsD + 2;
	Point3r l_ptCurrentUV;
	Point3r l_ptPreviousUV;

	SubmeshPtr l_submesh = CreateSubmesh( 1);

	for (unsigned int i = 0 ; i < l_nbVertexW ; i++)
	{
		for (unsigned int j = 0 ; j < l_nbVertexD ; j++)
		{
			l_submesh->AddPoint( (i * l_gapW), 0.0, (j * l_gapD));
		}
	}

	IdPoint3rPtr l_v1, l_v2, l_v3, l_v4;

	for (unsigned int i = 0 ; i < m_subDivisionsW + 1 ; i++)
	{
		l_ptCurrentUV[0] += 1.0f / real( m_subDivisionsW + 1);

		for (unsigned int j = i * (m_subDivisionsD + 1) ; j < (i + 1) * (m_subDivisionsD + 1) ; j++)
		{
			l_v1 = l_submesh->GetPoint( j + i);
			l_v2 = l_submesh->GetPoint( j + m_subDivisionsW + 2 + i);
			l_v3 = l_submesh->GetPoint( j + m_subDivisionsW + 3 + i);
			l_v4 = l_submesh->GetPoint( j + i + 1);

			l_ptCurrentUV[1] += 1.0f / real( m_subDivisionsD + 1);

			l_submesh->AddQuadFace( l_v1->GetIndex(), l_v2->GetIndex(), l_v3->GetIndex(), l_v4->GetIndex(), 0, l_ptPreviousUV, l_ptCurrentUV);

			l_ptPreviousUV[1] = l_ptCurrentUV[1];
		}

		l_ptPreviousUV[0] = l_ptCurrentUV[0];
	}

	l_submesh->GenerateBuffers();

	Logger::LogMessage( CU_T( "Plane - %s - NbVertex : %d"), GetName().char_str(), l_submesh->GetNbPoints());
}

void Plane :: Modify( real p_width, real p_depth,
						  unsigned int p_subDivX, unsigned int p_subDivY)
{
	m_subDivisionsW = p_subDivX;
	m_subDivisionsD = p_subDivY;
	m_width = p_width;
	m_depth = p_depth;
	Cleanup();
	GeneratePoints();
}