/*******************************************************************************
 Auteur:  Sylvain DOREMUS + Loic DASSONVILLE
 But:    Moteur 3D

 Fichier: Sphere.h - Sphere.cpp

 Desc:   Classe g‚rant la primitive de la sphere

*******************************************************************************/
#include "PrecompiledHeader.h"

#include "geometry/Module_Geometry.h"

#include "geometry/mesh/SphericMesh.h"
#include "geometry/mesh/Submesh.h"

#include "geometry/basic/Arc.h"
#include "geometry/basic/Face.h"
#include "main/Root.h"
#include "render_system/RenderSystem.h"
#include "render_system/MeshRenderer.h"
#include "render_system/Buffer.h"

#include "Log.h"

using namespace Castor3D;


SphericMesh :: SphericMesh( float radius_p, unsigned int nbFaces_p,
							const String & p_name)
	:	Mesh( p_name),
		m_radius( radius_p),
		m_nbFaces( nbFaces_p)
{
//	m_preferredSubdivMode = smQuad;
	m_meshType = Mesh::eSphere;
	GeneratePoints();
}



SphericMesh :: ~SphericMesh()
{
}



void SphericMesh :: GeneratePoints()
{
	if (m_radius < 0)
	{
		m_radius = -m_radius;
	}
	if (m_nbFaces < 3)
	{
		return;
	}

	Submesh * l_submesh = CreateSubmesh( 1);

	l_submesh->GetRenderer()->GetLines()->Cleanup();
	l_submesh->GetRenderer()->GetLinesTexCoords()->Cleanup();
	l_submesh->GetRenderer()->GetTriangles()->Cleanup();
	l_submesh->GetRenderer()->GetTrianglesTexCoords()->Cleanup();

	float l_angle = (M_PI_MULT_2) / m_nbFaces;
	Arc * l_arc;
	Vector3f * l_vertex;

	if (m_nbFaces % 2 == 0)
	{
		//CALCUL DE LA POSITION DES POINTS
		float l_alpha = 0 ;
		Arc ** l_arcs = new Arc*[m_nbFaces];
		l_arc = new Arc;
		for (size_t i = 0 ; i < (size_t)((m_nbFaces/2)-1) ; i++)
		{
			l_alpha += l_angle;
			l_vertex = l_submesh->AddVertex( m_radius * cos( l_alpha), 0.0, m_radius * sin( l_alpha));
			l_arc->AddVertex( l_vertex, -1);
		}

		Vector3f * pTopCenter_l = new Vector3f( m_radius, 0.0, 0.0);
		Vector3f * pBottomCenter_l = new Vector3f( - m_radius, 0.0, 0.0);

		unsigned int l_count = 0;
		size_t l_size = l_arc->GetNumVertex();
		for (l_alpha = 0 ; l_count < m_nbFaces ; l_alpha += l_angle)
		{
			l_arcs[l_count] = new Arc;
			for (size_t j = 0 ; j < l_size ; j++)
			{
				l_vertex = l_submesh->AddVertex( l_arc->GetVertex( j)->x,
												 l_arc->GetVertex( j)->z*sin( l_alpha),
												 l_arc->GetVertex( j)->z*cos( l_alpha));
				l_arcs[l_count]->AddVertex( l_vertex, -1);
			}
			l_count++;
		}

		l_submesh->AddVertex( pTopCenter_l);
		l_submesh->AddVertex( pBottomCenter_l);

		//RECONSTITION DES FACES
			// Sommet et Base
		Face * l_face;

		for (size_t i = 0 ; i < m_nbFaces - 1 ; i++)
		{
			l_face = l_submesh->AddFace( l_arcs[i]->GetVertex( 0), l_arcs[i+1]->GetVertex( 0), pTopCenter_l, 0);
			SetTexCoordV1( l_face, ((float) i) / ((float) m_nbFaces), 1.0f / l_size);
			SetTexCoordV2( l_face, ((float) i+1) / ((float) m_nbFaces), 1.0f / l_size);
			SetTexCoordV3( l_face, ((float) i+1) / ((float) m_nbFaces), 0.0);

			l_face = l_submesh->AddFace( l_arcs[i]->GetVertex( l_size - 1), pBottomCenter_l, l_arcs[i+1]->GetVertex( l_size - 1), 0);
			SetTexCoordV1( l_face, ((float) i) / ((float) m_nbFaces), (l_size - 1.0f) / l_size);
			SetTexCoordV2( l_face, ((float) i+1) / ((float) m_nbFaces), 1.0);
			SetTexCoordV3( l_face, ((float) i+1) / ((float) m_nbFaces), (l_size - 1.0f) / l_size);
		}
		l_face = l_submesh->AddFace( l_arcs[0]->GetVertex( 0), pTopCenter_l, l_arcs[m_nbFaces-1]->GetVertex( 0), 0);
		SetTexCoordV1( l_face, 1.0, 1.0f / l_size / 2.0f);
		SetTexCoordV2( l_face, 1.0, 0.0);
		SetTexCoordV3( l_face, ((float) m_nbFaces-1) / ((float) m_nbFaces), 1.0f / l_size);

		l_face = l_submesh->AddFace( l_arcs[0]->GetVertex( l_size - 1), l_arcs[m_nbFaces-1]->GetVertex( l_size - 1), pBottomCenter_l, 0);
		SetTexCoordV1( l_face, 1.0, (l_size - 1.0f) / l_size);
		SetTexCoordV2( l_face, ((float) m_nbFaces-1) / ((float) m_nbFaces), (l_size - 1.0f) / l_size);
		SetTexCoordV3( l_face, 1.0, 1.0);

			// Autres faces
		for (size_t j = 0 ; j < l_size - 1 ; j++)
		{
			for (size_t i = 0 ; i < m_nbFaces - 1 ; i++)
			{
				l_face = l_submesh->AddFace( l_arcs[i]->GetVertex( j + 1), l_arcs[i+1]->GetVertex( j),
											 l_arcs[i]->GetVertex( j), 0);
				SetTexCoordV1( l_face, ((float)i) / ((float)m_nbFaces), ((float)j+1) / ((float)l_size));
				SetTexCoordV2( l_face, ((float)i+1) / ((float)m_nbFaces), ((float)j) / ((float)l_size));
				SetTexCoordV3( l_face, ((float)i) / ((float)m_nbFaces), ((float)j) / ((float)l_size));

				l_face = l_submesh->AddFace( l_arcs[i]->GetVertex( j + 1), l_arcs[i+1]->GetVertex( j + 1),
											 l_arcs[i+1]->GetVertex( j), 0);
				SetTexCoordV1( l_face, ((float)i) / ((float)m_nbFaces), ((float)j+1) / ((float)l_size));
				SetTexCoordV2( l_face, ((float)i+1) / ((float)m_nbFaces), ((float)j+1) / ((float)l_size));
				SetTexCoordV3( l_face, ((float)i+1) / ((float)m_nbFaces), ((float)j) / ((float)l_size));
			}
			l_face = l_submesh->AddFace( l_arcs[0]->GetVertex( j + 1), l_arcs[m_nbFaces-1]->GetVertex( j),
										 l_arcs[m_nbFaces-1]->GetVertex( j + 1), 0);
			SetTexCoordV1( l_face, 1.0, ((float)j+1) / ((float)l_size));
			SetTexCoordV2( l_face, ((float)m_nbFaces - 1) / ((float)m_nbFaces), ((float)j) / ((float)l_size));
			SetTexCoordV3( l_face, ((float)m_nbFaces - 1) / ((float)m_nbFaces), ((float)j+1) / ((float)l_size));

			l_face = l_submesh->AddFace( l_arcs[m_nbFaces-1]->GetVertex( j), l_arcs[0]->GetVertex( j + 1),
										 l_arcs[0]->GetVertex( j), 0);
			SetTexCoordV1( l_face, ((float)m_nbFaces - 1) / ((float)m_nbFaces), ((float)j) / ((float)l_size));
			SetTexCoordV2( l_face, 1.0, ((float)j+1) / ((float)l_size));
			SetTexCoordV3( l_face, 1.0, ((float)j) / ((float)l_size));
		}

		for (unsigned int i = 0 ; i < m_nbFaces ; i++)
		{
			delete l_arcs[i];
		}
		delete [] l_arcs;
	}
	else
	{
		//CALCUL DE LA POSITION DES POINTS
		float l_alpha = 0;
		l_arc = new Arc;
		for (size_t i = 0 ; i < (size_t)(m_nbFaces/2) ; i++)
		{
			l_alpha += l_angle;
			l_vertex = new Vector3f( m_radius * cos( l_alpha), 0.0, m_radius * sin( l_alpha));
			l_arc->AddVertex( l_vertex, -1);
		}
		size_t l_size = l_arc->GetNumVertex();

		Vector3f * pTopCenter_l = new Vector3f( m_radius, 0.0, 0.0);
		Vector3f * pBottomCenter_l = new Vector3f( l_arc->GetVertex( l_size - 1)->x, 0.0, 0.0);

		unsigned int l_count = 0;
		for (l_alpha = 0 ; l_count < m_nbFaces ; l_alpha += l_angle)
		{
			for (size_t j = 0; j < l_size ; j++)
			{
				l_vertex = l_submesh->AddVertex( l_arc->GetVertex( j)->x,
												 l_arc->GetVertex( j)->z * sin( l_alpha),
												 l_arc->GetVertex( j)->z * cos( l_alpha));
			}
			l_count++;
		}

		l_submesh->AddVertex( pTopCenter_l);
		l_submesh->AddVertex( pBottomCenter_l);

		//RECONSTITION DES FACES
		Face * l_face;
		Vector3fPtrArray l_vertex = l_submesh->m_vertex;

		for (size_t i = 0 ; i < l_vertex.size() - l_size ; i += l_size)
		{
			l_face = l_submesh->AddFace( l_vertex[i+l_size], l_vertex[i], pTopCenter_l, 0);
			l_face = l_submesh->AddFace( pBottomCenter_l, l_vertex[i+l_size-1], l_vertex[i+l_size+l_size-1], 0);
		}
		l_face = l_submesh->AddFace( pTopCenter_l, l_vertex[0], l_vertex[l_vertex.size() - l_size], 0);
		l_face = l_submesh->AddFace( l_vertex[l_vertex.size() - 1], l_vertex[l_size - 1], pBottomCenter_l, 0);

		for (size_t j = 0 ; j < l_size - 1 ; j++)
		{
			for (size_t i = 0 ; i < l_vertex.size() - l_size ; i += l_size)
			{
				l_face = l_submesh->AddFace( l_vertex[j+i+1], l_vertex[j+i], l_vertex[j+i+l_size], 0);
				l_face = l_submesh->AddFace( l_vertex[j+i+l_size+1], l_vertex[i+j+1], l_vertex[j+i+l_size], 0);
			}
			l_face = l_submesh->AddFace( l_vertex[j], l_vertex[j+1], l_vertex[l_vertex.size()-l_size+1+j], 0);
			l_face = l_submesh->AddFace( l_vertex[l_vertex.size()-l_size+1+j], l_vertex[l_vertex.size()-l_size+j], l_vertex[j], 0);
		}
	}

	l_submesh->GenerateBuffers();

	delete l_arc;
	Log::LogMessage( C3D_T( "SphericMesh - %s - NbFaces : %d - NbVertex : %d"), m_name.c_str(), l_submesh->GetNbFaces(), l_submesh->m_vertex.size());
}



void SphericMesh :: Modify( float p_radius, unsigned int p_nbFaces)
{
	m_nbFaces = p_nbFaces;
    m_radius = p_radius;
	Cleanup();
	GeneratePoints();
}