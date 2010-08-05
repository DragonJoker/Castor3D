/*******************************************************************************
 Auteur:  Sylvain DOREMUS & Loïc DASSONVILLE
 But:    Moteur 3D

 Fichier: Face.h - Facef.cpp

 Desc:   Classe gérant les faces, contient les arcs composant la face
         et la normale d'une face

*******************************************************************************/
#include "PrecompiledHeader.h"

#include "geometry/Module_Geometry.h"

#include "geometry/basic/Face.h"


using namespace Castor3D;

unsigned long Face :: s_faceNumber = 0;


Face :: Face( Vector3f * p_v1, Vector3f * p_v2, Vector3f * p_v3)
	:	m_vertex1( p_v1),
		m_vertex2( p_v2),
		m_vertex3( p_v3),
		m_vertex1Normal( NULL),
		m_vertex2Normal( NULL),
		m_vertex3Normal( NULL),
		m_vertex1Tangent( NULL),
		m_vertex2Tangent( NULL),
		m_vertex3Tangent( NULL)
{
	s_faceNumber++;
}


/*
Face :: Face( const Face & p_face)
	:	m_vertex1( p_face.m_vertex2),
		m_vertex2( p_face.m_vertex2),
		m_vertex3( p_face.m_vertex3),
		m_vertex1Normal( NULL),
		m_vertex2Normal( NULL),
		m_vertex3Normal( NULL),
		m_vertex1Tangent( NULL),
		m_vertex2Tangent( NULL),
		m_vertex3Tangent( NULL)
{
	s_faceNumber++;

	for (int i = 0 ; i < 3 ; i++)
	{
		m_vertex1Normal->ptr()[i] = p_face.m_vertex1Normal->const_ptr()[i];
		m_vertex2Normal->ptr()[i] = p_face.m_vertex2Normal->const_ptr()[i];
		m_vertex3Normal->ptr()[i] = p_face.m_vertex3Normal->const_ptr()[i];
		m_faceNormal.ptr()[i] = p_face.m_faceNormal.const_ptr()[i];
	}
}
*/


Face :: ~Face()
{
	s_faceNumber--;
	//m_vertex1,m_vertex2,m_vertex3 supprimés par le Submesh
}


