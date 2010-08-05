/*******************************************************************************
 Auteur:  Sylvain DOREMUS + Loic DASSONVILLE
 But:    Moteur 3D

 Fichier: Icosaedre.h - Icosaedre.cpp

 Desc:   Classe gérant la primitive de l'icosaedre

*******************************************************************************/
#include "PrecompiledHeader.h"

#include "geometry/Module_Geometry.h"

#include "geometry/mesh/IcosaedricMesh.h"
#include "geometry/mesh/Submesh.h"


#include "geometry/basic/Face.h"
#include "main/Root.h"
#include "render_system/RenderSystem.h"
#include "render_system/MeshRenderer.h"
#include "render_system/Buffer.h"

#include "Log.h"

using namespace Castor3D;


IcosaedricMesh :: IcosaedricMesh( float radius_p, unsigned int nbFaces_p,
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
	Submesh * l_submesh = new Submesh( Root::GetRenderSystem()->CreateSubmeshRenderer(), 1);
	m_submeshes.push_back( l_submesh);

	// Dessin de l'icosaèdre
	float phi = (1.0f + sqrt( 5.0f)) / 2.0f;
	float X = m_radius / sqrt( phi * sqrt( 5.0f));
	float Z = X * phi;

	// on crée les 12 points le composant
	Vector3f * pt1 = l_submesh->AddVertex( -X, 0, Z);
	Vector3f * pt2 = l_submesh->AddVertex( X, 0, Z);
	Vector3f * pt3 = l_submesh->AddVertex( -X, 0, -Z);
	Vector3f * pt4 = l_submesh->AddVertex( X, 0, -Z);
	Vector3f * pt5 = l_submesh->AddVertex( 0, Z, X);
	Vector3f * pt6 = l_submesh->AddVertex( 0, Z, -X);
	Vector3f * pt7 = l_submesh->AddVertex( 0, -Z, X);
	Vector3f * pt8 = l_submesh->AddVertex( 0, -Z, -X);
	Vector3f * pt9 = l_submesh->AddVertex( Z, X, 0);
	Vector3f * pt10 = l_submesh->AddVertex(-Z, X, 0);
	Vector3f * pt11 = l_submesh->AddVertex( Z, -X, 0);
	Vector3f * pt12 = l_submesh->AddVertex( -Z, -X, 0);
	
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
		l_submesh->_subdividePNTriangles( & Vector3f::ZeroVertex);
	}

	l_submesh->GenerateBuffers();

	Log::LogMessage( C3D_T( "Icosaedre - %s - NbFaces : %d - NbVertex : %d"), m_name.c_str(), l_submesh->GetNbFaces(), l_submesh->m_vertex.size());
}



void IcosaedricMesh :: Modify( float p_radius, unsigned int p_nbFaces)
{
	m_nbFaces = p_nbFaces;
	m_radius = p_radius;
	Cleanup();
	GeneratePoints();
}