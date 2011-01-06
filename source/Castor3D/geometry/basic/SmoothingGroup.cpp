#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/geometry/basic/SmoothingGroup.h"
#include "Castor3D/geometry/basic/Face.h"

using namespace Castor3D;

SmoothingGroup :: SmoothingGroup( size_t p_id)
	:	m_uiGroupID( p_id)
{
}

SmoothingGroup :: SmoothingGroup( const SmoothingGroup & p_toCopy)
	:	m_uiGroupID( p_toCopy.m_uiGroupID)
	,	m_arrayFaces( p_toCopy.m_arrayFaces)
{
}

SmoothingGroup :: ~SmoothingGroup()
{
}

void SmoothingGroup :: AddFace( FacePtr p_face)
{
	m_arrayFaces.push_back( p_face);
}

void SmoothingGroup :: ClearFaces()
{
	m_arrayFaces.clear();
}
 
void SmoothingGroup :: SetNormals( size_t p_uiNbVertex)
{
	size_t i;
	Container<FaceAndAnglePtrArray>::Vector l_arrayFaces;
	Point3rArray l_arrayNormals, l_arrayTangents; 

	//creation d'un vecteur de liste (chaque point contient la liste des faces auxquelles il appartient) 
	l_arrayFaces.reserve( p_uiNbVertex);
	l_arrayFaces.resize( p_uiNbVertex);

	l_arrayNormals.reserve( p_uiNbVertex);
	l_arrayNormals.resize( p_uiNbVertex);

	l_arrayTangents.reserve( p_uiNbVertex);
	l_arrayTangents.resize( p_uiNbVertex);

	//Pour chaque vertex, on stocke la liste des faces auxquelles il appartient 
	for (i = 0 ; i < m_arrayFaces.size() ; i++)
	{
		Point3r l_vec1m2( m_arrayFaces[i]->GetVertex( 0).GetCoords() - m_arrayFaces[i]->GetVertex( 1).GetCoords());
		Point3r l_vec1m3( m_arrayFaces[i]->GetVertex( 0).GetCoords() - m_arrayFaces[i]->GetVertex( 2).GetCoords());
		Point3r l_vec2m1( m_arrayFaces[i]->GetVertex( 1).GetCoords() - m_arrayFaces[i]->GetVertex( 0).GetCoords());
		Point3r l_vec2m3( m_arrayFaces[i]->GetVertex( 1).GetCoords() - m_arrayFaces[i]->GetVertex( 2).GetCoords());
		Point3r l_vec3m1( m_arrayFaces[i]->GetVertex( 2).GetCoords() - m_arrayFaces[i]->GetVertex( 0).GetCoords());
		Point3r l_vec3m2( m_arrayFaces[i]->GetVertex( 2).GetCoords() - m_arrayFaces[i]->GetVertex( 1).GetCoords());

		FaceAndAnglePtr l_faa1( new FaceAndAngle( m_arrayFaces[i]));
		FaceAndAnglePtr l_faa2( new FaceAndAngle( m_arrayFaces[i]));
		FaceAndAnglePtr l_faa3( new FaceAndAngle( m_arrayFaces[i]));

		l_faa1->m_rAngle = fabs( acos( l_vec3m1.GetCosTheta( l_vec2m1)));
		l_faa1->m_uiIndex = 0;
		l_faa1->m_ptVertex1 = m_arrayFaces[i]->GetVertex( 2).GetCoords();
		l_faa1->m_ptVertex2 = m_arrayFaces[i]->GetVertex( 1).GetCoords();
		l_arrayFaces[m_arrayFaces[i]->GetVertex( 0).GetIndex()].push_back( l_faa1);

		l_faa2->m_rAngle = fabs( acos( l_vec1m2.GetCosTheta( l_vec3m2)));
		l_faa2->m_uiIndex = 1;
		l_faa2->m_ptVertex1 = m_arrayFaces[i]->GetVertex( 0).GetCoords();
		l_faa2->m_ptVertex2 = m_arrayFaces[i]->GetVertex( 2).GetCoords();
		l_arrayFaces[m_arrayFaces[i]->GetVertex( 1).GetIndex()].push_back( l_faa2);

		l_faa3->m_rAngle = fabs( acos( l_vec1m3.GetCosTheta( l_vec2m3)));
		l_faa3->m_uiIndex = 2;
		l_faa3->m_ptVertex1 = m_arrayFaces[i]->GetVertex( 0).GetCoords();
		l_faa3->m_ptVertex2 = m_arrayFaces[i]->GetVertex( 1).GetCoords();
		l_arrayFaces[m_arrayFaces[i]->GetVertex( 2).GetIndex()].push_back( l_faa3);
	}

	//On effectue la moyennes des normales 	
	FaceAndAnglePtrArray::const_iterator l_it;
	for (i = 0 ; i < l_arrayFaces.size() ; i++)
	{
		Point3r l_normal, l_tangent;
		for (l_it = l_arrayFaces[i].begin(); l_it != l_arrayFaces[i].end(); l_it++)
		{
			l_normal += (*l_it)->m_face->GetFlatNormal() * (*l_it)->m_rAngle;
			l_tangent += (*l_it)->m_face->GetFlatTangent() * (*l_it)->m_rAngle;
		}

		l_normal.Normalise();
		l_arrayNormals[i] = l_normal;
		
		l_tangent.Normalise();
		l_arrayTangents[i] = l_tangent;
	}

	//affectation des normales au point des faces
	for (i = 0 ; i < m_arrayFaces.size() ; i++)
	{
		for (size_t j = 0 ; j < 3 ; j++)
		{
			m_arrayFaces[i]->GetVertex( j).SetNormal( l_arrayNormals[m_arrayFaces[i]->GetVertex( j).GetIndex()], false);
			m_arrayFaces[i]->GetVertex( j).SetTangent( l_arrayTangents[m_arrayFaces[i]->GetVertex( j).GetIndex()]);

			m_arrayFaces[i]->SetSmoothNormal( j, l_arrayNormals[m_arrayFaces[i]->GetVertex( j).GetIndex()]);
			m_arrayFaces[i]->SetSmoothTangent( j, l_arrayTangents[m_arrayFaces[i]->GetVertex( j).GetIndex()]);
		}
	}

	//destruction de la liste de liste de faces
	for (i = 0 ; i < p_uiNbVertex ; i++)
	{
		l_arrayFaces[i].clear();
	}

	l_arrayNormals.clear();
	l_arrayTangents.clear();
}

SmoothingGroup & SmoothingGroup :: operator =( const SmoothingGroup & p_toCopy)
{
	m_uiGroupID = p_toCopy.m_uiGroupID;
	m_arrayFaces = p_toCopy.m_arrayFaces;
	return * this;
}