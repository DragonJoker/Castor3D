#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/geometry/mesh/Icosaedron.h"
#include "Castor3D/geometry/mesh/Submesh.h"
#include "Castor3D/geometry/basic/Face.h"
#include "Castor3D/geometry/basic/Vertex.h"
#include "Castor3D/geometry/basic/SmoothingGroup.h"
#include "Castor3D/geometry/mesh/PnTrianglesDivider.h"
#include "Castor3D/main/Root.h"
#include "Castor3D/render_system/RenderSystem.h"
#include "Castor3D/render_system/Buffer.h"



using namespace Castor3D;

Icosaedron :: Icosaedron( MeshManager * p_pManager, real radius_p, unsigned int nbFaces_p,
								  const String & p_name)
	:	Mesh( p_pManager, p_name),
		m_radius	( radius_p),
		m_nbFaces	( nbFaces_p)
{
	m_meshType = eIcosaedron;
	GeneratePoints();
}

Icosaedron :: ~Icosaedron ()
{
}

void Icosaedron :: GeneratePoints ()
{
	if (m_radius < 0)
	{
		m_radius = -m_radius;
	}
	SubmeshPtr l_submesh = CreateSubmesh( 1);

	// Dessin de l'icosaèdre
	real phi = (1.0f + sqrt( 5.0f)) / 2.0f;
	real X = m_radius / sqrt( phi * sqrt( 5.0f));
	real Z = X * phi;

	// on crée les 12 points le composant
	l_submesh->AddPoint( -X, 0, Z);
	l_submesh->AddPoint( X, 0, Z);
	l_submesh->AddPoint( -X, 0, -Z);
	l_submesh->AddPoint( X, 0, -Z);
	l_submesh->AddPoint( 0, Z, X);
	l_submesh->AddPoint( 0, Z, -X);
	l_submesh->AddPoint( 0, -Z, X);
	l_submesh->AddPoint( 0, -Z, -X);
	l_submesh->AddPoint( Z, X, 0);
	l_submesh->AddPoint(-Z, X, 0);
	l_submesh->AddPoint( Z, -X, 0);
	l_submesh->AddPoint( -Z, -X, 0);

	// on construit toutes les faces de l'isocaèdre
	l_submesh->AddFace( 1, 0, 4, 0);
	l_submesh->AddFace( 0, 9, 4, 0);
	l_submesh->AddFace( 4, 9, 5, 0);
	l_submesh->AddFace( 8, 4, 5, 0);
	l_submesh->AddFace( 1, 4, 8, 0);
	l_submesh->AddFace( 10, 1, 8, 0);
	l_submesh->AddFace( 8, 3, 10, 0);
	l_submesh->AddFace( 5, 3, 8, 0);
	l_submesh->AddFace( 5, 2, 3, 0);
	l_submesh->AddFace( 2, 7, 3, 0);
	l_submesh->AddFace( 10, 3, 7, 0);
	l_submesh->AddFace( 7, 6, 10, 0);
	l_submesh->AddFace( 11, 6, 7, 0);
	l_submesh->AddFace( 6, 11, 0, 0);
	l_submesh->AddFace( 0, 1, 6, 0);
	l_submesh->AddFace( 6, 1, 10, 0);
	l_submesh->AddFace( 9, 0, 11, 0);
	l_submesh->AddFace( 9, 11, 2, 0);
	l_submesh->AddFace( 5, 9, 2, 0);
	l_submesh->AddFace( 7, 2, 11, 0);

	ComputeNormals( false);

	Point3r l_tmp;

	// Division des faces de l'icosèdre
	for (unsigned int i = 1 ; i < m_nbFaces ; i++)
	{
		PnTrianglesDividerPtr l_pDivider( new PnTrianglesDivider( l_submesh.get()));
		l_submesh->Subdivide( l_pDivider, & l_tmp, false);
	}

	l_submesh->GenerateBuffers();
	ComputeNormals( false);

	Logger::LogMessage( CU_T( "Icosaedre - %s - NbFaces : %d - NbVertex : %d"), GetName().char_str(), l_submesh->GetNbFaces(), l_submesh->GetNbPoints());
}

void Icosaedron :: Modify( real p_radius, unsigned int p_nbFaces)
{
	m_nbFaces = p_nbFaces;
	m_radius = p_radius;
	Cleanup();
	GeneratePoints();
}

void Icosaedron :: ComputeNormals( bool p_bReverted)
{
	Mesh::ComputeNormals( p_bReverted);
	SubmeshPtr l_pSubmesh = m_submeshes[0];

	for (size_t i = 0 ; i < l_pSubmesh->GetNbSmoothGroups() ; i++)
	{
		SmoothingGroupPtr l_group = l_pSubmesh->GetSmoothGroup( i);

		for (size_t j = 0 ; j < l_group->GetNbFaces() ; j++)
		{
			SphericalVertex l_vsVertex1( l_group->GetFace( j)->GetVertex( 0).GetNormal());
			SphericalVertex l_vsVertex2( l_group->GetFace( j)->GetVertex( 1).GetNormal());
			SphericalVertex l_vsVertex3( l_group->GetFace( j)->GetVertex( 2).GetNormal());

			l_group->GetFace( j)->GetVertex( 0).SetTexCoord( l_vsVertex1.m_phi, l_vsVertex1.m_theta, false);
			l_group->GetFace( j)->GetVertex( 1).SetTexCoord( l_vsVertex2.m_phi, l_vsVertex2.m_theta, false);
			l_group->GetFace( j)->GetVertex( 2).SetTexCoord( l_vsVertex3.m_phi, l_vsVertex3.m_theta, false);
		}
	}
}