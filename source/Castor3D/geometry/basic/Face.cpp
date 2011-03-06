#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/geometry/basic/Face.h"
#include "Castor3D/scene/SceneFileParser.h"
#include "Castor3D/geometry/mesh/Submesh.h"

using namespace Castor3D;

unsigned long long Face::FacesCount = 0;

Face :: Face( const IdPoint3r & p_v1, const IdPoint3r & p_v2, const IdPoint3r & p_v3)
	:	m_bCenterComputed( false)
{
	FacesCount++;
	m_vertex[0] = p_v1;
	m_vertex[1] = p_v2;
	m_vertex[2] = p_v3;
}

Face :: Face( const Face & p_face)
	:	m_ptFaceNormal( p_face.m_ptFaceNormal)
	,	m_ptFaceTangent( p_face.m_ptFaceTangent)
	,	m_bCenterComputed( p_face.m_bCenterComputed)
	,	m_ptCenter( p_face.m_ptCenter)
{
	FacesCount++;

	for (size_t i = 0 ; i < 3 ; i++)
	{
		m_vertex[i] = p_face.m_vertex[i];
		m_ptSmoothNormals[i] = p_face.m_ptSmoothNormals[i];
		m_ptSmoothTangents[i] = p_face.m_ptSmoothTangents[i];
	}
}

Face :: ~Face()
{
	FacesCount--;
}

const Point3r & Face :: GetFaceCenter()const
{
	if ( ! m_bCenterComputed)
	{
		m_ptCenter  = m_vertex[0].GetCoords();
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

void Face :: SetVertexTexCoords( size_t p_uiVertex, const real * p_pCoords, bool p_bManual)
{
	CASTOR_ASSERT( p_uiVertex < 3);
	m_vertex[p_uiVertex].SetTexCoord( p_pCoords, p_bManual);
}

void Face :: SetNormals( const Point2r & p_ptNormalA, const Point2r & p_ptNormalB, const Point2r & p_ptNormalC, bool p_bManual)
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

void Face :: SetVertexNormal( size_t p_uiVertex, const Point3r & p_ptNormal, bool p_bManual)
{
	CASTOR_ASSERT( p_uiVertex < 3);
	m_vertex[p_uiVertex].SetNormal( p_ptNormal, p_bManual);
}

void Face :: SetVertexNormal( size_t p_uiVertex, const real * p_pCoords, bool p_bManual)
{
	CASTOR_ASSERT( p_uiVertex < 3);
	m_vertex[p_uiVertex].SetNormal( p_pCoords, p_bManual);
}

void Face :: SetTangents( const Point2r & p_ptTangentA, const Point2r & p_ptTangentB, const Point2r & p_ptTangentC, bool p_bManual)
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

void Face :: SetVertexTangent( size_t p_uiVertex, const Point3r & p_ptTangent, bool p_bManual)
{
	CASTOR_ASSERT( p_uiVertex < 3);
	m_vertex[p_uiVertex].SetTangent( p_ptTangent, p_bManual);
}

void Face :: SetVertexTangent( size_t p_uiVertex, const real * p_pCoords, bool p_bManual)
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

void Face :: SetSmoothNormal( size_t p_uiVertex, const Point3r & p_ptNormal)
{
	CASTOR_ASSERT( p_uiVertex < 3);
	m_ptSmoothNormals[p_uiVertex] = p_ptNormal;
	m_ptSmoothNormals[p_uiVertex].Normalise();
}

void Face :: SetFlatNormal( const Point3r & p_ptNormal)
{
	m_ptFaceNormal = p_ptNormal;
	m_ptFaceNormal.Normalise();
}

void Face :: SetSmoothTangent( size_t p_uiVertex, const Point3r & p_ptNormal)
{
	CASTOR_ASSERT( p_uiVertex < 3);
	m_ptSmoothTangents[p_uiVertex] = p_ptNormal;
	m_ptSmoothTangents[p_uiVertex].Normalise();
}

void Face :: SetFlatTangent( const Point3r & p_ptNormal)
{
	m_ptFaceTangent = p_ptNormal;
	m_ptFaceTangent.Normalise();
}

bool Face :: Write( File & p_file)const
{
	bool l_bReturn = p_file.Print( 1024, "\t\t\t\tface %i %i %i\n", m_vertex[0].GetIndex(), m_vertex[1].GetIndex(), m_vertex[2].GetIndex()) > 0;

	if (l_bReturn)
	{
		l_bReturn = p_file.Print( 1024, "\t\t\t\tuv %f %f %f %f %f %f\n", m_vertex[0].GetTexCoord()[0], m_vertex[0].GetTexCoord()[1], m_vertex[1].GetTexCoord()[0], m_vertex[1].GetTexCoord()[1], m_vertex[2].GetTexCoord()[0], m_vertex[2].GetTexCoord()[1]) > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.Print( 1024, "\t\t\t\tnormals %f %f %f %f %f %f %f %f %f\n", m_vertex[0].GetNormal()[0], m_vertex[0].GetNormal()[1], m_vertex[0].GetNormal()[2], m_vertex[1].GetNormal()[0], m_vertex[1].GetNormal()[1], m_vertex[1].GetNormal()[2], m_vertex[2].GetNormal()[0], m_vertex[2].GetNormal()[1], m_vertex[2].GetNormal()[2]) > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.Print( 1024, "\t\t\t\ttangents %f %f %f %f %f %f %f %f %f\n", m_vertex[0].GetTangent()[0], m_vertex[0].GetTangent()[1], m_vertex[0].GetTangent()[2], m_vertex[1].GetTangent()[0], m_vertex[1].GetTangent()[1], m_vertex[1].GetTangent()[2], m_vertex[2].GetTangent()[0], m_vertex[2].GetTangent()[1], m_vertex[2].GetTangent()[2]) > 0;
	}

	return l_bReturn;
}

bool Face :: Read( File & p_file, Submesh * p_pSubmesh)
{
	String l_strLine;
	bool l_bReturn = p_file.ReadLine( l_strLine, 1024) > 0;
	Point<real, 6> l_ptUVs;
	Point<real, 9> l_ptCoords;
	size_t l_uiCount;

	if (l_bReturn)
	{
		l_bReturn = SceneFileParser::ParseVector<real, 6>( l_strLine, l_ptUVs);
		l_uiCount = 0;

		for (size_t i = 0 ; i < 3 ; i++)
		{
			m_vertex[i].SetTexCoord( & l_ptUVs[l_uiCount * 2]);
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
			m_vertex[i].SetNormal( & l_ptCoords[l_uiCount * 3]);
			m_ptSmoothNormals[i] = m_vertex[i].GetNormal();
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
			m_vertex[i].SetTangent( & l_ptCoords[l_uiCount * 3]);
			m_ptSmoothTangents[i] = m_vertex[i].GetTangent();
			l_uiCount++;
		}
	}

	m_ptFaceNormal = Point3r( 0, 0, 0);
	m_ptFaceTangent = Point3r( 0, 0, 0);

	for (size_t i = 0 ; i < 3 ; i++)
	{
		m_ptFaceNormal += m_ptSmoothNormals[i];
		m_ptFaceTangent += m_ptSmoothTangents[i];
	}

	m_ptFaceNormal.Normalise();
	m_ptFaceTangent.Normalise();

	return l_bReturn;
}

bool Face :: Save( File & p_file)const
{
	bool l_bReturn = true;

	for (size_t i = 0 ; i < 3 && l_bReturn ; i++)
	{
		l_bReturn = p_file.Write( m_vertex[i].GetIndex()) == sizeof( size_t);
	}

	for (size_t i = 0 ; i < 3 && l_bReturn ; i++)
	{
		l_bReturn = m_vertex[i].GetNormal().Save( p_file);
	}

	for (size_t i = 0 ; i < 3 && l_bReturn ; i++)
	{
		l_bReturn = m_vertex[i].GetTangent().Save( p_file);
	}

	for (size_t i = 0 ; i < 3 && l_bReturn ; i++)
	{
		l_bReturn = m_vertex[i].GetTexCoord().Save( p_file);
	}

	return l_bReturn;
}

bool Face :: Load( File & p_file, Submesh * p_pSubmesh)
{
	bool l_bReturn = true;

	for (size_t i = 0 ; i < 3 && l_bReturn ; i++)
	{
		l_bReturn = m_vertex[i].GetNormal().Load( p_file);
		m_ptSmoothNormals[i] = m_vertex[i].GetNormal();
	}

	for (size_t i = 0 ; i < 3 && l_bReturn ; i++)
	{
		l_bReturn = m_vertex[i].GetTangent().Load( p_file);
		m_ptSmoothTangents[i] = m_vertex[i].GetTangent();
	}

	for (size_t i = 0 ; i < 3 && l_bReturn ; i++)
	{
		l_bReturn = m_vertex[0].GetTexCoord().Load( p_file);
	}

	m_bCenterComputed = false;

	return l_bReturn;
}