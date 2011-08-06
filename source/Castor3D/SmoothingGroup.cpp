#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/SmoothingGroup.hpp"
#include "Castor3D/Face.hpp"
#include "Castor3D/Submesh.hpp"
#include "Castor3D/SceneFileParser.hpp"

using namespace Castor3D;

//*************************************************************************************************

bool Loader<SmoothingGroup> :: Write( const SmoothingGroup & p_group, File & p_file)
{
	bool l_bReturn = p_file.WriteLine( cuT( "\t\t\tsmoothing_group\n\t\t\t{\n")) > 0;

	size_t l_uiNbFaces = p_group.GetNbFaces();

	for (size_t j = 0 ; j < l_uiNbFaces && l_bReturn ; j++)
	{
		l_bReturn = Loader<Face>::Write( * p_group[j],p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteLine( cuT( "\t\t\t}\n")) > 0;
	}

	return l_bReturn;
}

//*************************************************************************************************

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
	Point3rPtrArray l_arrayNormals, l_arrayTangents; 

	//creation d'un vecteur de liste (chaque point contient la liste des faces auxquelles il appartient) 
	l_arrayFaces.reserve( p_uiNbVertex);
	l_arrayFaces.resize( p_uiNbVertex);

	l_arrayNormals.reserve( p_uiNbVertex);
	l_arrayNormals.resize( p_uiNbVertex, Point3rPtr( new Point3r()));

	l_arrayTangents.reserve( p_uiNbVertex);
	l_arrayTangents.resize( p_uiNbVertex, Point3rPtr( new Point3r()));

	Point3r l_vec1m2;
	Point3r l_vec1m3;
	Point3r l_vec2m1;
	Point3r l_vec2m3;
	Point3r l_vec3m1;
	Point3r l_vec3m2;

	//Pour chaque vertex, on stocke la liste des faces auxquelles il appartient 
	for (i = 0 ; i < m_arrayFaces.size() ; i++)
	{
		l_vec1m2.copy( m_arrayFaces[i]->GetVertex( 0).GetCoords() - m_arrayFaces[i]->GetVertex( 1).GetCoords());
		l_vec1m3.copy( m_arrayFaces[i]->GetVertex( 0).GetCoords() - m_arrayFaces[i]->GetVertex( 2).GetCoords());
		l_vec2m1.copy( m_arrayFaces[i]->GetVertex( 1).GetCoords() - m_arrayFaces[i]->GetVertex( 0).GetCoords());
		l_vec2m3.copy( m_arrayFaces[i]->GetVertex( 1).GetCoords() - m_arrayFaces[i]->GetVertex( 2).GetCoords());
		l_vec3m1.copy( m_arrayFaces[i]->GetVertex( 2).GetCoords() - m_arrayFaces[i]->GetVertex( 0).GetCoords());
		l_vec3m2.copy( m_arrayFaces[i]->GetVertex( 2).GetCoords() - m_arrayFaces[i]->GetVertex( 1).GetCoords());

		FaceAndAnglePtr l_faa1( new FaceAndAngle( m_arrayFaces[i]));
		FaceAndAnglePtr l_faa2( new FaceAndAngle( m_arrayFaces[i]));
		FaceAndAnglePtr l_faa3( new FaceAndAngle( m_arrayFaces[i]));

		l_faa1->m_rAngle = fabs( acos( real( l_vec3m1.cos_theta( l_vec2m1))));
		l_faa1->m_uiIndex = 0;
		l_faa1->m_ptVertex1.clopy( m_arrayFaces[i]->GetVertex( 2).GetCoords());
		l_faa1->m_ptVertex2.clopy( m_arrayFaces[i]->GetVertex( 1).GetCoords());
		l_arrayFaces[m_arrayFaces[i]->GetVertex( 0).GetIndex()].push_back( l_faa1);

		l_faa2->m_rAngle = fabs( acos( real( l_vec1m2.cos_theta( l_vec3m2))));
		l_faa2->m_uiIndex = 1;
		l_faa2->m_ptVertex1.clopy( m_arrayFaces[i]->GetVertex( 0).GetCoords());
		l_faa2->m_ptVertex2.clopy( m_arrayFaces[i]->GetVertex( 2).GetCoords());
		l_arrayFaces[m_arrayFaces[i]->GetVertex( 1).GetIndex()].push_back( l_faa2);

		l_faa3->m_rAngle = fabs( acos( real( l_vec1m3.cos_theta( l_vec2m3))));
		l_faa3->m_uiIndex = 2;
		l_faa3->m_ptVertex1.clopy( m_arrayFaces[i]->GetVertex( 0).GetCoords());
		l_faa3->m_ptVertex2.clopy( m_arrayFaces[i]->GetVertex( 1).GetCoords());
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

		l_normal.normalise();
		l_arrayNormals[i]->clopy( l_normal);
		
		l_tangent.normalise();
		l_arrayTangents[i]->clopy( l_tangent);
	}

	//affectation des normales au point des faces
	for (i = 0 ; i < m_arrayFaces.size() ; i++)
	{
		for (size_t j = 0 ; j < 3 ; j++)
		{
			m_arrayFaces[i]->GetVertex( j).SetNormal( * l_arrayNormals[m_arrayFaces[i]->GetVertex( j).GetIndex()], false);
			m_arrayFaces[i]->GetVertex( j).SetTangent( * l_arrayTangents[m_arrayFaces[i]->GetVertex( j).GetIndex()]);

			m_arrayFaces[i]->SetSmoothNormal( j, * l_arrayNormals[m_arrayFaces[i]->GetVertex( j).GetIndex()]);
			m_arrayFaces[i]->SetSmoothTangent( j, * l_arrayTangents[m_arrayFaces[i]->GetVertex( j).GetIndex()]);
		}
	}

	//destruction de la liste de liste de faces
	for (i = 0 ; i < p_uiNbVertex ; i++)
	{
		l_arrayFaces[i].clear();
	}

	ClearContainer( l_arrayNormals);
	ClearContainer( l_arrayTangents);
}

SmoothingGroup & SmoothingGroup :: operator =( const SmoothingGroup & p_toCopy)
{
	m_uiGroupID = p_toCopy.m_uiGroupID;
	m_arrayFaces = p_toCopy.m_arrayFaces;
	return * this;
}

BEGIN_SERIALISE_MAP( SmoothingGroup, Serialisable)
	ADD_ELEMENT( m_uiGroupID)
END_SERIALISE_MAP()

BEGIN_POST_SERIALISE( SmoothingGroup, Serialisable)
	bool l_bReturn = m_pFile->Write( GetNbFaces()) == sizeof( size_t);

	if (l_bReturn)
	{
		FacePtr l_pFace;

		for (size_t j = 0 ; j < GetNbFaces() && l_bReturn ; j++)
		{
			l_pFace = m_arrayFaces[j];

			l_bReturn = m_pFile->Write( l_pFace->GetVertex( 0).GetIndex()) == sizeof( size_t);

			if (l_bReturn)
			{
				l_bReturn = m_pFile->Write( l_pFace->GetVertex( 1).GetIndex()) == sizeof( size_t);
			}

			if (l_bReturn)
			{
				l_bReturn = m_pFile->Write( l_pFace->GetVertex( 2).GetIndex()) == sizeof( size_t);
			}

			if (l_bReturn)
			{
				l_pFace->Serialise( * m_pFile);
			}
		}
	}
END_POST_SERIALISE()

BEGIN_POST_UNSERIALISE( SmoothingGroup, Serialisable)
	size_t l_uiNbFaces;
	bool l_bResult = m_pFile->Read( l_uiNbFaces) == sizeof( size_t);

	if (l_bResult)
	{
		// Loading group faces
		size_t l_iV1, l_iV2, l_iV3;
		FacePtr l_pFace;

		for (size_t j = 0 ; j < l_uiNbFaces && l_bResult ; j++)
		{
			l_iV1 = 0;
			l_iV2 = 0;
			l_iV3 = 0;

			l_bResult = m_pFile->Read( l_iV1) == sizeof( size_t);

			if (l_bResult)
			{
				l_bResult = m_pFile->Read( l_iV2) == sizeof( size_t);
			}

			if (l_bResult)
			{
				l_bResult = m_pFile->Read( l_iV3) == sizeof( size_t);
			}

			if (l_bResult)
			{
				FacePtr l_pFace = m_pSubmesh->AddFace( l_iV1, l_iV2, l_iV3, m_uiGroupID);
				l_pFace->Unserialise( * m_pFile, this);
			}
		}
	}
END_POST_UNSERIALISE()