#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/geometry/basic/SmoothingGroup.h"
#include "Castor3D/geometry/basic/Face.h"
#include "Castor3D/geometry/mesh/Submesh.h"
#include "Castor3D/scene/SceneFileParser.h"

using namespace Castor3D;

SmoothingGroup :: SmoothingGroup( Submesh * p_pSubmesh, size_t p_id)
	:	m_uiGroupID( p_id)
	,	m_pSubmesh( p_pSubmesh)
{
}

SmoothingGroup :: SmoothingGroup( const SmoothingGroup & p_toCopy)
	:	m_uiGroupID( p_toCopy.m_uiGroupID)
	,	m_arrayFaces( p_toCopy.m_arrayFaces)
	,	m_pSubmesh( p_toCopy.m_pSubmesh)
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

bool SmoothingGroup :: Write( File & p_file)const
{
	bool l_bReturn = p_file.WriteLine( CU_T( "\t\t\tsmoothing_group\n\t\t\t{\n")) > 0;

	size_t l_uiNbFaces = GetNbFaces();

	for (size_t j = 0 ; j < l_uiNbFaces && l_bReturn ; j++)
	{
		l_bReturn = GetFace( j)->Write( p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteLine( CU_T( "\t\t\t}\n")) > 0;
	}

	return l_bReturn;
}

bool SmoothingGroup :: Save( File & p_file)const
{
	size_t l_uiNbFaces = GetNbFaces();
	bool l_bReturn = p_file.Write( GetGroupID()) == sizeof( size_t);

	if (l_bReturn)
	{
		l_bReturn = p_file.Write( l_uiNbFaces) == sizeof( size_t);
	}

	for (size_t j = 0 ; j < l_uiNbFaces && l_bReturn ; j++)
	{
		l_bReturn = GetFace( j)->Save( p_file);
	}

	return l_bReturn;
}

bool SmoothingGroup :: Load( File & p_file)
{
	size_t l_uiID;
	size_t l_uiNbFaces;
	bool l_bReturn = p_file.Read( l_uiID) == sizeof( size_t);

	if (l_bReturn)
	{
		SetGroupID( l_uiID);
		l_bReturn = p_file.Read( l_uiNbFaces) == sizeof( size_t);
	}

	if (l_bReturn)
	{
		size_t l_iV1, l_iV2, l_iV3;

		for (size_t j = 0 ; j < l_uiNbFaces && l_bReturn ; j++)
		{
			l_iV1 = 0;
			l_iV2 = 0;
			l_iV3 = 0;

			l_bReturn = p_file.Read( l_iV1) == sizeof( size_t);

			if (l_bReturn)
			{
				l_bReturn = p_file.Read( l_iV2) == sizeof( size_t);
			}

			if (l_bReturn)
			{
				l_bReturn = p_file.Read( l_iV3) == sizeof( size_t);
			}

			if (l_bReturn)
			{
				l_bReturn = m_pSubmesh->AddFace( l_iV1, l_iV2, l_iV3, * this)->Load( p_file);
			}
		}
	}

	return l_bReturn;
}