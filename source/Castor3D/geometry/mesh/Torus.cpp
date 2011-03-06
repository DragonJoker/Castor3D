#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/geometry/mesh/Torus.h"
#include "Castor3D/geometry/mesh/Submesh.h"
#include "Castor3D/geometry/basic/Vertex.h"
#include "Castor3D/geometry/basic/Face.h"
#include "Castor3D/geometry/basic/Pattern.h"
#include "Castor3D/main/Root.h"
#include "Castor3D/render_system/RenderSystem.h"
#include "Castor3D/render_system/Buffer.h"



using namespace Castor3D;


Torus :: Torus( MeshManager * p_pManager, real internalRadius_p, real externalRadius_p,
				unsigned int internalNbFaces_p, unsigned int externalNbFaces_p,
				const String & p_name)
	:	Mesh( p_pManager, p_name)
	,	m_internalRadius( internalRadius_p)
	,	m_externalRadius( externalRadius_p)
	,	m_internalNbFaces( internalNbFaces_p)
	,	m_externalNbFaces( externalNbFaces_p)
{
	m_meshType = eTorus;
	GeneratePoints();
}

Torus :: ~Torus()
{
}

void Torus :: GeneratePoints()
{
	if (m_internalRadius < 0)
	{
		m_internalRadius = -m_internalRadius;
	}
	if (m_externalRadius < 0)
	{
		m_externalRadius = -m_externalRadius;
	}
	if (m_internalNbFaces < 3 || m_externalNbFaces < 3)
	{
		return;
	}

	SubmeshPtr l_submesh = CreateSubmesh( 1);

	real l_internalRotationAngle = (Angle::PiMult2) / m_internalNbFaces;
	real l_externalRotationAngle = (Angle::PiMult2) / m_externalNbFaces;
	real l_alphaInternal = 0.0;
	real l_alphaExternal = 0.0;
	IdPoint3rPattern * l_arcs = new IdPoint3rPattern[m_externalNbFaces];

	//CALCUL DE LA POSITION DES POINTS

	for (unsigned int i = 0 ; i < m_internalNbFaces ; i++)
	{
		l_alphaInternal += l_internalRotationAngle;
		IdPoint3rPtr l_vertex = l_submesh->AddPoint( (m_internalRadius * cos( l_alphaInternal ) + m_externalRadius), (m_internalRadius * sin( l_alphaInternal )), 0.0);
		l_arcs[0].AddElement( IdPoint3r( * l_vertex), -1);
	}

	size_t l_arcSize = l_arcs[0].GetSize();

	for (unsigned int i = 0 ; i < m_externalNbFaces-1 ; i++)
	{
		l_alphaExternal += l_externalRotationAngle;

		for (unsigned int j = 0 ; j < l_arcSize ; j++)
		{
			IdPoint3r l_vertex = l_arcs[0][j];
			l_vertex[2] = l_vertex[0] * sin( l_alphaExternal);
			l_vertex[0] = l_vertex[0] * cos( l_alphaExternal);
			l_arcs[i+1].AddElement( l_vertex, -1);
			l_submesh->AddPoint( l_vertex);
		}
	}

	//GENERATION DES FACES
	for (size_t j = 0 ; j < l_arcSize - 1 ; j++)
	{
		for (unsigned int i = 0 ; i < m_externalNbFaces - 1 ; i++)
		{
			l_submesh->AddQuadFace( i * l_arcSize + j, (i + 1) * l_arcSize + j, (i + 1) * l_arcSize + j + 1, i * l_arcSize + j + 1,
									0, Point3r( real( i) / real( m_externalNbFaces), real( j) / real( l_arcSize), 0.0), Point3r( real( i + 1) / real( m_externalNbFaces), real( j + 1) / real( l_arcSize), 0.0));
		}

		l_submesh->AddQuadFace( (m_externalNbFaces-1) * l_arcSize + j, j, j + 1, (m_externalNbFaces-1) * l_arcSize + j + 1,
								0, Point3r( real( m_externalNbFaces - 1) / real( m_externalNbFaces), real( j) / real( l_arcSize), 0.0), Point3r( 1.0, real( j + 1) / real( l_arcSize), 0.0));
	}

	for (unsigned int i = 0 ; i < m_externalNbFaces - 1 ; i++)
	{
		l_submesh->AddQuadFace( i * l_arcSize + l_arcSize - 1, (i + 1) * l_arcSize + l_arcSize - 1, (i + 1) * l_arcSize, i * l_arcSize,
								0, Point3r( real( i) / real( m_externalNbFaces), real( l_arcSize - 1) / real( l_arcSize), 0.0), Point3r( real( i + 1) / real( m_externalNbFaces), 1.0, 0.0));
	}

	l_submesh->AddQuadFace( (m_externalNbFaces - 1) * l_arcSize + l_arcSize - 1, l_arcSize - 1, 0, (m_externalNbFaces - 1) * l_arcSize,
							0, Point3r( real( m_externalNbFaces - 1) / real( m_externalNbFaces), real( l_arcSize - 1) / real( l_arcSize), 0.0), Point3r( 1.0, 1.0, 0.0));

	l_submesh->GenerateBuffers();

	for (unsigned int i = 0 ; i < m_externalNbFaces ; i++)
	{
//		delete l_arcs[i];
	}
	delete [] l_arcs;
	Logger::LogMessage( CU_T( "Torus - %s - NbFaces : %d - NbVertex : %d"), GetName().char_str(),
					 l_submesh->GetNbFaces(), l_submesh->GetNbPoints());

}

void Torus :: Modify( real p_internalRadius, real p_externalRadius,
						  unsigned int p_internalNbFaces, unsigned int p_externalNbFaces)
{
	m_internalRadius = p_internalRadius;
	m_externalRadius = p_externalRadius;
	m_internalNbFaces = p_internalNbFaces;
	m_externalNbFaces = p_externalNbFaces;
	Cleanup();
	GeneratePoints();
}