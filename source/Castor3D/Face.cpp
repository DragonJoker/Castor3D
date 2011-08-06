#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/Face.hpp"
#include "Castor3D/SceneFileParser.hpp"
#include "Castor3D/Submesh.hpp"

using namespace Castor3D;

unsigned long long Face::FacesCount = 0;

//*************************************************************************************************

bool Loader<Face> :: Read( Castor3D::Face & p_face, Utils::File & p_file)
{
	String l_strLine;
	bool l_bReturn = p_file.ReadLine( l_strLine, 1024) > 0;
	Point<real, 6> l_ptUVs;
	Point<real, 9> l_ptCoords;
	size_t l_uiCount;
	Point3r l_ptNormal;
	Point3r l_ptTangent;

	if (l_bReturn)
	{
		l_bReturn = SceneFileParser::ParseVector<real, 6>( l_strLine, l_ptUVs);
		l_uiCount = 0;

		for (size_t i = 0 ; i < 3 ; i++)
		{
			p_face[i].SetTexCoord( & l_ptUVs[l_uiCount * 2]);
			l_uiCount++;
		}
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.ReadLine( l_strLine, 1024) > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = SceneFileParser::ParseVector<real, 9>( l_strLine, l_ptCoords);
		l_uiCount = 0;

		for (size_t i = 0 ; i < 3 ; i++)
		{
			p_face[i].SetNormal( & l_ptCoords[l_uiCount * 3]);
			p_face.SetSmoothNormal( i, p_face[i].GetNormal());
			l_ptNormal += p_face[i].GetNormal();
			l_uiCount++;
		}
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.ReadLine( l_strLine, 1024) > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = SceneFileParser::ParseVector<real, 9>( l_strLine, l_ptCoords);
		l_uiCount = 0;

		for (size_t i = 0 ; i < 3 ; i++)
		{
			p_face[i].SetTangent( & l_ptCoords[l_uiCount * 3]);
			p_face.SetSmoothTangent( i, p_face[i].GetTangent());
			l_ptTangent += p_face[i].GetTangent();
			l_uiCount++;
		}
	}

	for (size_t i = 0 ; i < 3 ; i++)
	{
		p_face.SetFlatNormal( l_ptNormal.get_normalised());
		p_face.SetFlatTangent( l_ptTangent.get_normalised());
	}

	return l_bReturn;
}

bool Loader<Face> :: Write( const Castor3D::Face & p_face, Utils::File & p_file)
{
	bool l_bReturn = p_file.Print( 1024, cuT( "\t\t\t\tface %i %i %i\n"), p_face[0].GetIndex(), p_face[1].GetIndex(), p_face[2].GetIndex()) > 0;

	if (l_bReturn)
	{
		l_bReturn = p_file.Print( 1024, cuT( "\t\t\t\tuv %f %f %f %f %f %f\n"), p_face[0].GetTexCoord()[0], p_face[0].GetTexCoord()[1], p_face[1].GetTexCoord()[0], p_face[1].GetTexCoord()[1], p_face[2].GetTexCoord()[0], p_face[2].GetTexCoord()[1]) > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.Print( 1024, cuT( "\t\t\t\tnormals %f %f %f %f %f %f %f %f %f\n"), p_face[0].GetNormal()[0], p_face[0].GetNormal()[1], p_face[0].GetNormal()[2], p_face[1].GetNormal()[0], p_face[1].GetNormal()[1], p_face[1].GetNormal()[2], p_face[2].GetNormal()[0], p_face[2].GetNormal()[1], p_face[2].GetNormal()[2]) > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.Print( 1024, cuT( "\t\t\t\ttangents %f %f %f %f %f %f %f %f %f\n"), p_face[0].GetTangent()[0], p_face[0].GetTangent()[1], p_face[0].GetTangent()[2], p_face[1].GetTangent()[0], p_face[1].GetTangent()[1], p_face[1].GetTangent()[2], p_face[2].GetTangent()[0], p_face[2].GetTangent()[1], p_face[2].GetTangent()[2]) > 0;
	}

	return l_bReturn;
}

//*************************************************************************************************

Face :: Face( const IdPoint3r & p_v1, const IdPoint3r & p_v2, const IdPoint3r & p_v3)
	:	m_bCenterComputed( false)
{
	FacesCount++;
	m_vertex[0] = p_v1;
	m_vertex[1] = p_v2;
	m_vertex[2] = p_v3;
}

Face :: Face( const Face & p_face)
	:	m_bCenterComputed( p_face.m_bCenterComputed)
{
	m_ptFaceNormal.clopy( p_face.m_ptFaceNormal);
	m_ptFaceTangent.clopy( p_face.m_ptFaceTangent);
	m_ptCenter.clopy( p_face.m_ptCenter);
	FacesCount++;

	for (size_t i = 0 ; i < 3 ; i++)
	{
		m_vertex[i] = p_face.m_vertex[i];
		m_ptSmoothNormals[i].clopy( p_face.m_ptSmoothNormals[i]);
		m_ptSmoothTangents[i].clopy( p_face.m_ptSmoothTangents[i]);
	}
}

Face :: ~Face()
{
	FacesCount--;
}

Point3r const & Face :: GetFaceCenter()const
{
	if ( ! m_bCenterComputed)
	{
		m_ptCenter.copy( m_vertex[0].GetCoords());
		m_ptCenter += m_vertex[1].GetCoords();
		m_ptCenter += m_vertex[2].GetCoords();
		m_ptCenter /= real( 3.0);
		m_bCenterComputed = true;
	}

	return m_ptCenter;
}

void Face :: SetTextureCoords( const Point2r & p_ptUVA, const Point2r & p_ptUVB, const Point2r & p_ptUVC, bool p_bManual)
{
	m_vertex[0].SetTexCoord( p_ptUVA, p_bManual);
	m_vertex[1].SetTexCoord( p_ptUVB, p_bManual);
	m_vertex[2].SetTexCoord( p_ptUVC, p_bManual);
}

void Face :: SetVertexTexCoords( size_t p_uiVertex, real x, real y, bool p_bManual)
{
	CASTOR_ASSERT( p_uiVertex < 3);
	m_vertex[p_uiVertex].SetTexCoord( x, y, p_bManual);
}

void Face :: SetVertexTexCoords( size_t p_uiVertex, const Point2r & p_ptUV, bool p_bManual)
{
	CASTOR_ASSERT( p_uiVertex < 3);
	m_vertex[p_uiVertex].SetTexCoord( p_ptUV, p_bManual);
}

void Face :: SetVertexTexCoords( size_t p_uiVertex, real const * p_pCoords, bool p_bManual)
{
	CASTOR_ASSERT( p_uiVertex < 3);
	m_vertex[p_uiVertex].SetTexCoord( p_pCoords, p_bManual);
}

void Face :: SetNormals( Point3r const & p_ptNormalA, Point3r const & p_ptNormalB, Point3r const & p_ptNormalC, bool p_bManual)
{
	m_vertex[0].SetNormal( p_ptNormalA, p_bManual);
	m_vertex[1].SetNormal( p_ptNormalB, p_bManual);
	m_vertex[2].SetNormal( p_ptNormalC, p_bManual);
}

void Face :: SetVertexNormal( size_t p_uiVertex, real x, real y, real z, bool p_bManual)
{
	CASTOR_ASSERT( p_uiVertex < 3);
	m_vertex[p_uiVertex].SetNormal( x, y, z, p_bManual);
}

void Face :: SetVertexNormal( size_t p_uiVertex, Point3r const & p_ptNormal, bool p_bManual)
{
	CASTOR_ASSERT( p_uiVertex < 3);
	m_vertex[p_uiVertex].SetNormal( p_ptNormal, p_bManual);
}

void Face :: SetVertexNormal( size_t p_uiVertex, real const * p_pCoords, bool p_bManual)
{
	CASTOR_ASSERT( p_uiVertex < 3);
	m_vertex[p_uiVertex].SetNormal( p_pCoords, p_bManual);
}

void Face :: SetTangents( Point3r const & p_ptTangentA, Point3r const & p_ptTangentB, Point3r const & p_ptTangentC, bool p_bManual)
{
	m_vertex[0].SetTangent( p_ptTangentA, p_bManual);
	m_vertex[1].SetTangent( p_ptTangentB, p_bManual);
	m_vertex[2].SetTangent( p_ptTangentC, p_bManual);
}

void Face :: SetVertexTangent( size_t p_uiVertex, real x, real y, real z, bool p_bManual)
{
	CASTOR_ASSERT( p_uiVertex < 3);
	m_vertex[p_uiVertex].SetTangent( x, y, z, p_bManual);
}

void Face :: SetVertexTangent( size_t p_uiVertex, Point3r const & p_ptTangent, bool p_bManual)
{
	CASTOR_ASSERT( p_uiVertex < 3);
	m_vertex[p_uiVertex].SetTangent( p_ptTangent, p_bManual);
}

void Face :: SetVertexTangent( size_t p_uiVertex, real const * p_pCoords, bool p_bManual)
{
	CASTOR_ASSERT( p_uiVertex < 3);
	m_vertex[p_uiVertex].SetTangent( p_pCoords, p_bManual);
}

void Face :: SetSmoothNormals()
{
	for (size_t i = 0 ; i < 3 ; i++)
	{
		if ( ! m_vertex[i].IsManual())
		{
			m_vertex[i].SetNormal( m_ptSmoothNormals[i], false);
			m_vertex[i].SetTangent( m_ptSmoothTangents[i], false);
		}
	}
}

void Face :: SetFlatNormals()
{
	for (size_t i = 0 ; i < 3 ; i++)
	{
		if ( ! m_vertex[i].IsManual())
		{
			m_vertex[i].SetNormal( m_ptFaceNormal, false);
			m_vertex[i].SetTangent( m_ptFaceTangent, false);
		}
	}
}

void Face :: SetSmoothNormal( size_t p_uiVertex, Point3r const & p_ptNormal)
{
	CASTOR_ASSERT( p_uiVertex < 3);
	m_ptSmoothNormals[p_uiVertex].clopy( p_ptNormal);
	m_ptSmoothNormals[p_uiVertex].normalise();
}

void Face :: SetFlatNormal( Point3r const & p_ptNormal)
{
	m_ptFaceNormal.clopy( p_ptNormal);
	m_ptFaceNormal.normalise();
}

void Face :: SetSmoothTangent( size_t p_uiVertex, Point3r const & p_ptNormal)
{
	CASTOR_ASSERT( p_uiVertex < 3);
	m_ptSmoothTangents[p_uiVertex].clopy( p_ptNormal);
	m_ptSmoothTangents[p_uiVertex].normalise();
}

void Face :: SetFlatTangent( Point3r const & p_ptNormal)
{
	m_ptFaceTangent.clopy( p_ptNormal);
	m_ptFaceTangent.normalise();
}

BEGIN_SERIALISE_MAP( Face, Serialisable)
	ADD_ELEMENT( m_vertex[0])
	ADD_ELEMENT( m_vertex[1])
	ADD_ELEMENT( m_vertex[2])
END_SERIALISE_MAP()

BEGIN_POST_UNSERIALISE( Face, Serialisable)
	Point3r l_ptNormal;
	Point3r l_ptTangent;

	for (size_t i = 0 ; i < 3 ; i++)
	{
		SetSmoothNormal( 0, m_vertex[i].GetNormal());
		SetSmoothTangent( 0, m_vertex[i].GetTangent());
		l_ptNormal += m_vertex[i].GetNormal();
		l_ptTangent += m_vertex[i].GetTangent();
	}

	SetFlatNormal( l_ptNormal.get_normalised());
	SetFlatTangent( l_ptTangent.get_normalised());
END_POST_UNSERIALISE()