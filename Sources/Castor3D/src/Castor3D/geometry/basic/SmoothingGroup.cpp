#include "PrecompiledHeader.h"

#include "geometry/Module_Geometry.h"

#include "geometry/basic/SmoothingGroup.h"
#include "geometry/basic/Face.h"

using namespace Castor3D;

//! The structure managing Face and Angle
/*!
This is used to compute the smoothing groups
*/
struct FaceAndAngle
{
	real m_angle;
	FacePtr m_face;
	int m_index;
	Point3rPtr m_vertex1;
	Point3rPtr m_vertex2;
};
//! FaceAndAngle attay
typedef C3DVector( FaceAndAngle)	FaceAndAngleArray;

SmoothingGroup :: SmoothingGroup( size_t p_id)
	:	m_idGroup( p_id)
{
}

SmoothingGroup :: ~SmoothingGroup()
{
//	vector::deleteAll( m_faces);
}
 
void SmoothingGroup :: SetNormals( size_t p_nbVertex, const Point3rPtrArray & p_normals, const Point3rPtrArray & p_tangents)
{
	size_t i;
	C3DVector( FaceAndAngleArray) l_facesArray;
	Point3rArray l_normalsArray, l_tangentsArray; 

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
		Point3r l_vec1m2( *m_faces[i]->m_vertex1 - *m_faces[i]->m_vertex2);
		Point3r l_vec1m3( *m_faces[i]->m_vertex1 - *m_faces[i]->m_vertex3);
		Point3r l_vec2m1( *m_faces[i]->m_vertex2 - *m_faces[i]->m_vertex1);
		Point3r l_vec2m3( *m_faces[i]->m_vertex2 - *m_faces[i]->m_vertex3);
		Point3r l_vec3m1( *m_faces[i]->m_vertex3 - *m_faces[i]->m_vertex1);
		Point3r l_vec3m2( *m_faces[i]->m_vertex3 - *m_faces[i]->m_vertex2);

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
	FaceAndAngleArray::const_iterator l_it;
	for (i = 0 ; i < l_facesArray.size() ; i++)
	{
		Point3r l_normal, l_tangent;
		for (l_it = l_facesArray[i].begin(); l_it != l_facesArray[i].end(); l_it++)
		{
			l_normal[0] += (*l_it).m_face->m_faceNormal[0] * (*l_it).m_angle;
			l_normal[1] += (*l_it).m_face->m_faceNormal[1] * (*l_it).m_angle;
			l_normal[2] += (*l_it).m_face->m_faceNormal[2] * (*l_it).m_angle;

			l_tangent[0] += (*l_it).m_face->m_faceTangent[0] * (*l_it).m_angle;
			l_tangent[1] += (*l_it).m_face->m_faceTangent[1] * (*l_it).m_angle;
			l_tangent[2] += (*l_it).m_face->m_faceTangent[2] * (*l_it).m_angle;
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
		m_faces[i]->m_vertex1Normal->m_coords[0] = l_normalsArray[m_faces[i]->m_vertex1->m_index][0];
		m_faces[i]->m_vertex1Normal->m_coords[1] = l_normalsArray[m_faces[i]->m_vertex1->m_index][1];
		m_faces[i]->m_vertex1Normal->m_coords[2] = l_normalsArray[m_faces[i]->m_vertex1->m_index][2];

		m_faces[i]->m_vertex2Normal = p_normals[m_faces[i]->m_vertex2->m_index];
		m_faces[i]->m_vertex2Normal->m_coords[0] = l_normalsArray[m_faces[i]->m_vertex2->m_index][0];
		m_faces[i]->m_vertex2Normal->m_coords[1] = l_normalsArray[m_faces[i]->m_vertex2->m_index][1];
		m_faces[i]->m_vertex2Normal->m_coords[2] = l_normalsArray[m_faces[i]->m_vertex2->m_index][2];

		m_faces[i]->m_vertex3Normal = p_normals[m_faces[i]->m_vertex3->m_index];
		m_faces[i]->m_vertex3Normal->m_coords[0] = l_normalsArray[m_faces[i]->m_vertex3->m_index][0];
		m_faces[i]->m_vertex3Normal->m_coords[1] = l_normalsArray[m_faces[i]->m_vertex3->m_index][1];
		m_faces[i]->m_vertex3Normal->m_coords[2] = l_normalsArray[m_faces[i]->m_vertex3->m_index][2];

		m_faces[i]->m_vertex1Tangent = p_tangents[m_faces[i]->m_vertex1->m_index];
		m_faces[i]->m_vertex1Tangent->m_coords[0] = l_tangentsArray[m_faces[i]->m_vertex1->m_index][0];
		m_faces[i]->m_vertex1Tangent->m_coords[1] = l_tangentsArray[m_faces[i]->m_vertex1->m_index][1];
		m_faces[i]->m_vertex1Tangent->m_coords[2] = l_tangentsArray[m_faces[i]->m_vertex1->m_index][2];

		m_faces[i]->m_vertex2Tangent = p_tangents[m_faces[i]->m_vertex2->m_index];
		m_faces[i]->m_vertex2Tangent->m_coords[0] = l_tangentsArray[m_faces[i]->m_vertex2->m_index][0];
		m_faces[i]->m_vertex2Tangent->m_coords[1] = l_tangentsArray[m_faces[i]->m_vertex2->m_index][1];
		m_faces[i]->m_vertex2Tangent->m_coords[2] = l_tangentsArray[m_faces[i]->m_vertex2->m_index][2];

		m_faces[i]->m_vertex3Tangent = p_tangents[m_faces[i]->m_vertex3->m_index];
		m_faces[i]->m_vertex3Tangent->m_coords[0] = l_tangentsArray[m_faces[i]->m_vertex3->m_index][0];
		m_faces[i]->m_vertex3Tangent->m_coords[1] = l_tangentsArray[m_faces[i]->m_vertex3->m_index][1];
		m_faces[i]->m_vertex3Tangent->m_coords[2] = l_tangentsArray[m_faces[i]->m_vertex3->m_index][2];
	}

	//destruction de la liste de liste de faces
	for (i = 0 ; i < p_nbVertex ; i++)
	{
		l_facesArray[i].clear();
	}

	l_normalsArray.clear();
	l_tangentsArray.clear();
}