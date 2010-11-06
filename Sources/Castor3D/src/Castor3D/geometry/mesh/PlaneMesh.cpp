#include "PrecompiledHeader.h"

#include "geometry/Module_Geometry.h"

#include "geometry/mesh/PlaneMesh.h"
#include "geometry/mesh/Submesh.h"
#include "geometry/basic/Face.h"
#include "main/Root.h"
#include "render_system/RenderSystem.h"
#include "render_system/Buffer.h"
#include "geometry/mesh/PlaneMesh.h"



using namespace Castor3D;

PlaneMesh :: PlaneMesh( real width_p, real depth_p, unsigned int p_subdivisionsDepth,
						unsigned int p_subdivisionsWidth, const String & p_name)
	:	Mesh( p_name),
		m_depth( depth_p),
		m_width( width_p),
		m_subDivisionsW( p_subdivisionsWidth),
		m_subDivisionsD( p_subdivisionsDepth)
{
	m_meshType = Mesh::ePlane;
	GeneratePoints();
}

PlaneMesh :: ~PlaneMesh()
{
}

void PlaneMesh :: GeneratePoints()
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
	Point3rPtr l_vertex;

	for (unsigned int i = 0 ; i < l_nbVertexW ; i++)
	{
		for (unsigned int j = 0 ; j < l_nbVertexD ; j++)
		{
			l_vertex = l_submesh->AddVertex( (i * l_gapW), 0.0, (j * l_gapD));
		}
	}

	VertexPtr l_v1, l_v2, l_v3, l_v4;

	l_submesh->GetRenderer()->GetTriangles()->Cleanup();
	l_submesh->GetRenderer()->GetTrianglesNormals()->Cleanup();
	l_submesh->GetRenderer()->GetTrianglesTangents()->Cleanup();
	l_submesh->GetRenderer()->GetTrianglesTexCoords()->Cleanup();
	l_submesh->GetRenderer()->GetLines()->Cleanup();
	l_submesh->GetRenderer()->GetLinesNormals()->Cleanup();
	l_submesh->GetRenderer()->GetLinesTexCoords()->Cleanup();

	for (unsigned int i = 0 ; i < m_subDivisionsW + 1 ; i++)
	{
		l_ptCurrentUV[0] += 1.0f / real( m_subDivisionsW + 1);

		for (unsigned int j = i * (m_subDivisionsD + 1) ; j < (i + 1) * (m_subDivisionsD + 1) ; j++)
		{
			l_v1 = l_submesh->GetVertex( j + i);
			l_v2 = l_submesh->GetVertex( j + m_subDivisionsW + 2 + i);
			l_v3 = l_submesh->GetVertex( j + m_subDivisionsW + 3 + i);
			l_v4 = l_submesh->GetVertex( j + i + 1);

			l_ptCurrentUV[1] += 1.0f / real( m_subDivisionsD + 1);

			l_submesh->AddQuadFace( l_v1, l_v2, l_v3, l_v4, 0, l_ptPreviousUV, l_ptCurrentUV);

			l_ptPreviousUV[1] = l_ptCurrentUV[1];
		}

		l_ptPreviousUV[0] = l_ptCurrentUV[0];
	}

	l_submesh->GenerateBuffers();

	Log::LogMessage( CU_T( "Plane - %s - NbVertex : %d"), m_name.c_str(), l_submesh->GetNbVertex());
}

void PlaneMesh :: Modify( real p_width, real p_depth,
						  unsigned int p_subDivX, unsigned int p_subDivY)
{
	m_subDivisionsW = p_subDivX;
	m_subDivisionsD = p_subDivY;
	m_width = p_width;
	m_depth = p_depth;
	Cleanup();
	GeneratePoints();
}