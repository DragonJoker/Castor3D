#include "PrecompiledHeader.h"

#include "geometry/Module_Geometry.h"

#include "geometry/mesh/IcosaedricMesh.h"
#include "geometry/mesh/Submesh.h"
#include "geometry/basic/Face.h"
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
	VertexPtr pt1 = l_submesh->AddVertex( -X, 0, Z);
	VertexPtr pt2 = l_submesh->AddVertex( X, 0, Z);
	VertexPtr pt3 = l_submesh->AddVertex( -X, 0, -Z);
	VertexPtr pt4 = l_submesh->AddVertex( X, 0, -Z);
	VertexPtr pt5 = l_submesh->AddVertex( 0, Z, X);
	VertexPtr pt6 = l_submesh->AddVertex( 0, Z, -X);
	VertexPtr pt7 = l_submesh->AddVertex( 0, -Z, X);
	VertexPtr pt8 = l_submesh->AddVertex( 0, -Z, -X);
	VertexPtr pt9 = l_submesh->AddVertex( Z, X, 0);
	VertexPtr pt10 = l_submesh->AddVertex(-Z, X, 0);
	VertexPtr pt11 = l_submesh->AddVertex( Z, -X, 0);
	VertexPtr pt12 = l_submesh->AddVertex( -Z, -X, 0);
	
	// on construit toutes les faces de l'isocaèdre
	l_submesh->AddFace( pt2, pt1, pt5, 0);
	l_submesh->AddFace( pt1, pt10, pt5, 0);
	l_submesh->AddFace( pt5, pt10, pt6, 0);
	l_submesh->AddFace( pt9, pt5, pt6, 0);
	l_submesh->AddFace( pt2, pt5, pt9, 0);
	l_submesh->AddFace( pt11, pt2, pt9, 0);
	l_submesh->AddFace( pt9, pt4, pt11, 0);
	l_submesh->AddFace( pt6, pt4, pt9, 0);
	l_submesh->AddFace( pt6, pt3, pt4, 0);
	l_submesh->AddFace( pt3, pt8, pt4, 0);
	l_submesh->AddFace( pt11, pt4, pt8, 0);
	l_submesh->AddFace( pt8, pt7, pt11, 0);
	l_submesh->AddFace( pt12, pt7, pt8, 0);
	l_submesh->AddFace( pt7, pt12, pt1, 0);
	l_submesh->AddFace( pt1, pt2, pt7, 0);
	l_submesh->AddFace( pt7, pt2, pt11, 0);
	l_submesh->AddFace( pt10, pt1, pt12, 0);
	l_submesh->AddFace( pt10, pt12, pt3, 0);
	l_submesh->AddFace( pt6, pt10, pt3, 0);
	l_submesh->AddFace( pt8, pt3, pt12, 0);

	// Division des faces de l'icosèdre
	for (unsigned int i = 1 ; i < m_nbFaces ; i++)
	{
		l_submesh->_subdividePNTriangles( & Vertex::ZeroVertex);
	}

	l_submesh->GenerateBuffers();

	Log::LogMessage( CU_T( "Icosaedre - %s - NbFaces : %d - NbVertex : %d"), m_name.c_str(), l_submesh->GetNbFaces(), l_submesh->m_vertex.size());
}

void IcosaedricMesh :: Modify( real p_radius, unsigned int p_nbFaces)
{
	m_nbFaces = p_nbFaces;
	m_radius = p_radius;
	Cleanup();
	GeneratePoints();
}

void IcosaedricMesh :: SetNormals( bool p_bReverted)
{
	Mesh::SetNormals( p_bReverted);
	SubmeshPtr l_pSubmesh = m_submeshes[0];

	for (size_t i = 0 ; i < l_pSubmesh->m_smoothGroups.size() ; i++)
	{
		for (size_t j = 0 ; j < l_pSubmesh->m_smoothGroups[i]->m_faces.size() ; j++)
		{
			VertexSpherical l_vsVertex1( *l_pSubmesh->m_smoothGroups[i]->m_faces[j]->m_vertex1Normal);
			VertexSpherical l_vsVertex2( *l_pSubmesh->m_smoothGroups[i]->m_faces[j]->m_vertex2Normal);
			VertexSpherical l_vsVertex3( *l_pSubmesh->m_smoothGroups[i]->m_faces[j]->m_vertex3Normal);

			l_pSubmesh->m_smoothGroups[i]->m_faces[j]->m_vertex1TexCoord = Point3r( 3, l_vsVertex1.m_phi, l_vsVertex1.m_theta, 0.0);
			l_pSubmesh->m_smoothGroups[i]->m_faces[j]->m_vertex2TexCoord = Point3r( 3, l_vsVertex2.m_phi, l_vsVertex2.m_theta, 0.0);
			l_pSubmesh->m_smoothGroups[i]->m_faces[j]->m_vertex3TexCoord = Point3r( 3, l_vsVertex3.m_phi, l_vsVertex3.m_theta, 0.0);
		}
	}
}