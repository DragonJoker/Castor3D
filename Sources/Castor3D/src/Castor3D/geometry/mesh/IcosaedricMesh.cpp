#include "PrecompiledHeader.h"

#include "geometry/Module_Geometry.h"

#include "geometry/mesh/IcosaedricMesh.h"
#include "geometry/mesh/Submesh.h"
#include "geometry/basic/Face.h"
#include "geometry/basic/Vertex.h"
#include "geometry/basic/SmoothingGroup.h"
#include "main/Root.h"
#include "render_system/RenderSystem.h"
#include "render_system/Buffer.h"



using namespace Castor3D;

IcosaedricMesh :: IcosaedricMesh( real radius_p, unsigned int nbFaces_p,
								  const String & p_name)
	:	Mesh( p_name),
		m_radius	( radius_p),
		m_nbFaces	( nbFaces_p)
{
	m_meshType = Mesh::eIcosaedron;
	GeneratePoints();
}

IcosaedricMesh :: ~IcosaedricMesh ()
{
}

void IcosaedricMesh :: GeneratePoints ()
{
	if (m_radius < 0)
	{
		m_radius = -m_radius;
	}
	SubmeshPtr l_submesh = CreateSubmesh( 1);
	m_submeshes.push_back( l_submesh);

	// Dessin de l'icosaèdre
	real phi = (1.0f + sqrt( 5.0f)) / 2.0f;
	real X = m_radius / sqrt( phi * sqrt( 5.0f));
	real Z = X * phi;

	// on crée les 12 points le composant
	Point3r & pt1 = l_submesh->AddVertex( -X, 0, Z);
	Point3r & pt2 = l_submesh->AddVertex( X, 0, Z);
	Point3r & pt3 = l_submesh->AddVertex( -X, 0, -Z);
	Point3r & pt4 = l_submesh->AddVertex( X, 0, -Z);
	Point3r & pt5 = l_submesh->AddVertex( 0, Z, X);
	Point3r & pt6 = l_submesh->AddVertex( 0, Z, -X);
	Point3r & pt7 = l_submesh->AddVertex( 0, -Z, X);
	Point3r & pt8 = l_submesh->AddVertex( 0, -Z, -X);
	Point3r & pt9 = l_submesh->AddVertex( Z, X, 0);
	Point3r & pt10 = l_submesh->AddVertex(-Z, X, 0);
	Point3r & pt11 = l_submesh->AddVertex( Z, -X, 0);
	Point3r & pt12 = l_submesh->AddVertex( -Z, -X, 0);
	
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
		l_submesh->Subdivide( SMPNTriangles, & l_tmp);
		ComputeNormals( false);
	}

	l_submesh->GenerateBuffers();

	Logger::LogMessage( CU_T( "Icosaedre - %s - NbFaces : %d - NbVertex : %d"), m_name.char_str(), l_submesh->GetNbFaces(), l_submesh->GetVertices().size());
}

void IcosaedricMesh :: Modify( real p_radius, unsigned int p_nbFaces)
{
	m_nbFaces = p_nbFaces;
	m_radius = p_radius;
	Cleanup();
	GeneratePoints();
}

void IcosaedricMesh :: ComputeNormals( bool p_bReverted)
{
	Mesh::ComputeNormals( p_bReverted);
	SubmeshPtr l_pSubmesh = m_submeshes[0];

	for (size_t i = 0 ; i < l_pSubmesh->GetNbSmoothGroups() ; i++)
	{
		SmoothingGroup & l_group = l_pSubmesh->GetSmoothGroup( i);

		for (size_t j = 0 ; j < l_group.m_faces.size() ; j++)
		{
			SphericalVertex l_vsVertex1( l_group.m_faces[j][0].GetNormal());
			SphericalVertex l_vsVertex2( l_group.m_faces[j][1].GetNormal());
			SphericalVertex l_vsVertex3( l_group.m_faces[j][2].GetNormal());

			l_group.m_faces[j][0].SetTexCoord( l_vsVertex1.m_phi, l_vsVertex1.m_theta);
			l_group.m_faces[j][1].SetTexCoord( l_vsVertex2.m_phi, l_vsVertex2.m_theta);
			l_group.m_faces[j][2].SetTexCoord( l_vsVertex3.m_phi, l_vsVertex3.m_theta);
		}
	}
}