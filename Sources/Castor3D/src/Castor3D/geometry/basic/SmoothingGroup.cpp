/*******************************************************************************
 Auteur:  Sylvain DOREMUS & Loïc DASSONVILLE
 But:    Moteur 3D

 Fichier: SmoothingGroup.h - SmoothingGroup.cpp

 Desc:   Classe gérant les normales des faces de leur groupe
 
*******************************************************************************/
#include "PrecompiledHeader.h"

#include "geometry/Module_Geometry.h"



#include "geometry/basic/SmoothingGroup.h"

#include "geometry/basic/Face.h"


using namespace Castor3D;


SmoothingGroup :: SmoothingGroup( size_t p_id)
	:	m_idGroup( p_id)
{
}



SmoothingGroup :: ~SmoothingGroup()
{
	vector::deleteAll( m_faces);
}


 
void SmoothingGroup :: SetNormals( size_t p_nbVertex, const Vector3fPtrArray & p_normals, const Vector3fPtrArray & p_tangents)
{
	size_t i;
	std::vector < std::vector <FaceAndAngle> > l_facesArray;
	Vector3fArray l_normalsArray, l_tangentsArray; 

	//creation d'un vecteur de liste (chaque point contient la liste des faces auxquelles il appartient) 
	l_facesArray.reserve( p_nbVertex);
	l_facesArray.resize( p_nbVertex);

	l_normalsArray.reserve( p_nbVertex);
	l_normalsArray.resize( p_nbVertex);

	l_tangentsArray.reserve( p_nbVertex);
	l_tangentsArray.resize( p_nbVertex);

	//Pour chaque vertex, on stocke la liste des faces auxquelles il appartient 
	for (i = 0 ; i < m_faces.size() ; i++)
	{
		Vector3f l_vec1m2( *m_faces[i]->m_vertex2, *m_faces[i]->m_vertex1);
		Vector3f l_vec1m3( *m_faces[i]->m_vertex3, *m_faces[i]->m_vertex1);
		Vector3f l_vec2m1( *m_faces[i]->m_vertex1, *m_faces[i]->m_vertex2);
		Vector3f l_vec2m3( *m_faces[i]->m_vertex3, *m_faces[i]->m_vertex2);
		Vector3f l_vec3m1( *m_faces[i]->m_vertex1, *m_faces[i]->m_vertex3);
		Vector3f l_vec3m2( *m_faces[i]->m_vertex2, *m_faces[i]->m_vertex3);

		FaceAndAngle l_faa1;
		FaceAndAngle l_faa2;
		FaceAndAngle l_faa3;

		l_faa1.m_face = m_faces[i];
		l_faa1.m_angle = fabs( acos( l_vec3m1.GetCosTheta( l_vec2m1)));
		l_faa1.m_index = 0;
		l_faa1.m_vertex1 = m_faces[i]->m_vertex3;
		l_faa1.m_vertex2 = m_faces[i]->m_vertex2;
		l_facesArray[m_faces[i]->m_vertex1->m_index].push_back( l_faa1);

		l_faa2.m_face = m_faces[i];
		l_faa2.m_angle = fabs( acos( l_vec1m2.GetCosTheta( l_vec3m2)));
		l_faa2.m_index = 1;
		l_faa2.m_vertex1 = m_faces[i]->m_vertex1;
		l_faa2.m_vertex2 = m_faces[i]->m_vertex3;
		l_facesArray[m_faces[i]->m_vertex2->m_index].push_back( l_faa2);

		l_faa3.m_face = m_faces[i];
		l_faa3.m_angle = fabs( acos( l_vec1m3.GetCosTheta( l_vec2m3)));
		l_faa3.m_index = 2;
		l_faa3.m_vertex1 = m_faces[i]->m_vertex1;
		l_faa3.m_vertex2 = m_faces[i]->m_vertex2;
		l_facesArray[m_faces[i]->m_vertex3->m_index].push_back( l_faa3);
	}

	//On effectue la moyennes des normales 	
	std::vector<FaceAndAngle>::const_iterator l_it;
	for (i = 0 ; i < l_facesArray.size() ; i++)
	{
		Vector3f l_normal, l_tangent;
		for (l_it = l_facesArray[i].begin(); l_it != l_facesArray[i].end(); l_it++)
		{
			l_normal.x += (*l_it).m_face->m_faceNormal[0] * (*l_it).m_angle;
			l_normal.y += (*l_it).m_face->m_faceNormal[1] * (*l_it).m_angle;
			l_normal.z += (*l_it).m_face->m_faceNormal[2] * (*l_it).m_angle;

			l_tangent.x += (*l_it).m_face->m_faceTangent[0] * (*l_it).m_angle;
			l_tangent.y += (*l_it).m_face->m_faceTangent[1] * (*l_it).m_angle;
			l_tangent.z += (*l_it).m_face->m_faceTangent[2] * (*l_it).m_angle;
		}

		l_normal.Normalise();
		l_normalsArray[i] = l_normal;
		
		l_tangent.Normalise();
		l_tangentsArray[i] = l_tangent;
	}

	//affectation des normales au point des faces
	for (i = 0 ; i < m_faces.size() ; i++)
	{
		m_faces[i]->m_vertex1Normal = p_normals[m_faces[i]->m_vertex1->m_index];
		m_faces[i]->m_vertex1Normal->x = l_normalsArray[m_faces[i]->m_vertex1->m_index].x;
		m_faces[i]->m_vertex1Normal->y = l_normalsArray[m_faces[i]->m_vertex1->m_index].y;
		m_faces[i]->m_vertex1Normal->z = l_normalsArray[m_faces[i]->m_vertex1->m_index].z;

		m_faces[i]->m_vertex2Normal = p_normals[m_faces[i]->m_vertex2->m_index];
		m_faces[i]->m_vertex2Normal->x = l_normalsArray[m_faces[i]->m_vertex2->m_index].x;
		m_faces[i]->m_vertex2Normal->y = l_normalsArray[m_faces[i]->m_vertex2->m_index].y;
		m_faces[i]->m_vertex2Normal->z = l_normalsArray[m_faces[i]->m_vertex2->m_index].z;

		m_faces[i]->m_vertex3Normal = p_normals[m_faces[i]->m_vertex3->m_index];
		m_faces[i]->m_vertex3Normal->x = l_normalsArray[m_faces[i]->m_vertex3->m_index].x;
		m_faces[i]->m_vertex3Normal->y = l_normalsArray[m_faces[i]->m_vertex3->m_index].y;
		m_faces[i]->m_vertex3Normal->z = l_normalsArray[m_faces[i]->m_vertex3->m_index].z;

		m_faces[i]->m_vertex1Tangent = p_tangents[m_faces[i]->m_vertex1->m_index];
		m_faces[i]->m_vertex1Tangent->x = l_tangentsArray[m_faces[i]->m_vertex1->m_index].x;
		m_faces[i]->m_vertex1Tangent->y = l_tangentsArray[m_faces[i]->m_vertex1->m_index].y;
		m_faces[i]->m_vertex1Tangent->z = l_tangentsArray[m_faces[i]->m_vertex1->m_index].z;

		m_faces[i]->m_vertex2Tangent = p_tangents[m_faces[i]->m_vertex2->m_index];
		m_faces[i]->m_vertex2Tangent->x = l_tangentsArray[m_faces[i]->m_vertex2->m_index].x;
		m_faces[i]->m_vertex2Tangent->y = l_tangentsArray[m_faces[i]->m_vertex2->m_index].y;
		m_faces[i]->m_vertex2Tangent->z = l_tangentsArray[m_faces[i]->m_vertex2->m_index].z;

		m_faces[i]->m_vertex3Tangent = p_tangents[m_faces[i]->m_vertex3->m_index];
		m_faces[i]->m_vertex3Tangent->x = l_tangentsArray[m_faces[i]->m_vertex3->m_index].x;
		m_faces[i]->m_vertex3Tangent->y = l_tangentsArray[m_faces[i]->m_vertex3->m_index].y;
		m_faces[i]->m_vertex3Tangent->z = l_tangentsArray[m_faces[i]->m_vertex3->m_index].z;
	}

	//destruction de la liste de liste de faces
	for (i = 0 ; i < p_nbVertex ; i++)
	{
		l_facesArray[i].clear();
	}

	l_normalsArray.clear();
	l_tangentsArray.clear();
}


