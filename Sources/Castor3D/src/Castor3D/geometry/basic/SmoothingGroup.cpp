#include "PrecompiledHeader.h"

#include "geometry/Module_Geometry.h"

#include "geometry/basic/SmoothingGroup.h"
#include "geometry/basic/Face.h"
#include "geometry/basic/Vertex.h"

using namespace Castor3D;

class FaceAndAngle
{
public:
	real m_angle;
	Face & m_face;
	int m_index;
	Point3r m_vertex1;
	Point3r m_vertex2;

	FaceAndAngle( Face & p_face)
		:	m_face( p_face)
	{
	}
};
typedef SharedPtr<FaceAndAngle>	FaceAndAnglePtr;
typedef C3DVector( FaceAndAnglePtr)	FaceAndAnglePtrArray;

SmoothingGroup :: SmoothingGroup( size_t p_id)
	:	m_idGroup( p_id)
{
}

SmoothingGroup :: ~SmoothingGroup()
{
//	vector::deleteAll( m_faces);
}
 
void SmoothingGroup :: SetNormals( size_t p_nbVertex)
{
	size_t i;
	C3DVector( FaceAndAnglePtrArray) l_facesArray;
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
		Point3r l_vec1m2( m_faces[i][0].GetCoords() - m_faces[i][1].GetCoords());
		Point3r l_vec1m3( m_faces[i][0].GetCoords() - m_faces[i][2].GetCoords());
		Point3r l_vec2m1( m_faces[i][1].GetCoords() - m_faces[i][0].GetCoords());
		Point3r l_vec2m3( m_faces[i][1].GetCoords() - m_faces[i][2].GetCoords());
		Point3r l_vec3m1( m_faces[i][2].GetCoords() - m_faces[i][0].GetCoords());
		Point3r l_vec3m2( m_faces[i][2].GetCoords() - m_faces[i][1].GetCoords());

		FaceAndAnglePtr l_faa1( new FaceAndAngle( m_faces[i]));
		FaceAndAnglePtr l_faa2( new FaceAndAngle( m_faces[i]));
		FaceAndAnglePtr l_faa3( new FaceAndAngle( m_faces[i]));

		l_faa1->m_angle = fabs( acos( l_vec3m1.GetCosTheta( l_vec2m1)));
		l_faa1->m_index = 0;
		l_faa1->m_vertex1 = m_faces[i][2].GetCoords();
		l_faa1->m_vertex2 = m_faces[i][1].GetCoords();
		l_facesArray[m_faces[i][0].m_index].push_back( l_faa1);

		l_faa2->m_angle = fabs( acos( l_vec1m2.GetCosTheta( l_vec3m2)));
		l_faa2->m_index = 1;
		l_faa2->m_vertex1 = m_faces[i][0].GetCoords();
		l_faa2->m_vertex2 = m_faces[i][2].GetCoords();
		l_facesArray[m_faces[i][1].m_index].push_back( l_faa2);

		l_faa3->m_angle = fabs( acos( l_vec1m3.GetCosTheta( l_vec2m3)));
		l_faa3->m_index = 2;
		l_faa3->m_vertex1 = m_faces[i][0].GetCoords();
		l_faa3->m_vertex2 = m_faces[i][1].GetCoords();
		l_facesArray[m_faces[i][2].m_index].push_back( l_faa3);
	}

	//On effectue la moyennes des normales 	
	FaceAndAnglePtrArray::const_iterator l_it;
	for (i = 0 ; i < l_facesArray.size() ; i++)
	{
		Point3r l_normal, l_tangent;
		for (l_it = l_facesArray[i].begin(); l_it != l_facesArray[i].end(); l_it++)
		{
			l_normal[0] += (*l_it)->m_face.m_faceNormal[0] * (*l_it)->m_angle;
			l_normal[1] += (*l_it)->m_face.m_faceNormal[1] * (*l_it)->m_angle;
			l_normal[2] += (*l_it)->m_face.m_faceNormal[2] * (*l_it)->m_angle;

			l_tangent[0] += (*l_it)->m_face.m_faceTangent[0] * (*l_it)->m_angle;
			l_tangent[1] += (*l_it)->m_face.m_faceTangent[1] * (*l_it)->m_angle;
			l_tangent[2] += (*l_it)->m_face.m_faceTangent[2] * (*l_it)->m_angle;
		}

		l_normal.Normalise();
		l_normalsArray[i] = l_normal;
		
		l_tangent.Normalise();
		l_tangentsArray[i] = l_tangent;
	}

	//affectation des normales au point des faces
	for (i = 0 ; i < m_faces.size() ; i++)
	{
		for (size_t j = 0 ; j < 3 ; j++)
		{
			m_faces[i][j].GetNormal()[0] = l_normalsArray[m_faces[i][j].m_index][0];
			m_faces[i][j].GetNormal()[1] = l_normalsArray[m_faces[i][j].m_index][1];
			m_faces[i][j].GetNormal()[2] = l_normalsArray[m_faces[i][j].m_index][2];

			m_faces[i][j].GetTangent()[0] = l_tangentsArray[m_faces[i][j].m_index][0];
			m_faces[i][j].GetTangent()[1] = l_tangentsArray[m_faces[i][j].m_index][1];
			m_faces[i][j].GetTangent()[2] = l_tangentsArray[m_faces[i][j].m_index][2];

			m_faces[i].m_smoothNormals[j] = m_faces[i][j].GetNormal();
			m_faces[i].m_smoothTangents[j] = m_faces[i][j].GetTangent();
		}
	}

	//destruction de la liste de liste de faces
	for (i = 0 ; i < p_nbVertex ; i++)
	{
		l_facesArray[i].clear();
	}

	l_normalsArray.clear();
	l_tangentsArray.clear();
}