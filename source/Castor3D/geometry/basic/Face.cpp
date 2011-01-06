#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/geometry/basic/Face.h"

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
	// on écrit la normale de la face
	if ( ! m_ptFaceNormal.Write( p_file))
	{
		return false;
	}

	// on écrit les normales par vertex de la face
	for (size_t i = 0 ; i < 3 ; i++)
	{
		if ( ! m_vertex[0].GetNormal().Write( p_file))
		{
			return false;
		}
	}

	// on écrit les coordonnées de texture de la face
	for (size_t i = 0 ; i < 3 ; i++)
	{
		if ( ! m_vertex[0].GetTexCoord().Write( p_file))
		{
			return false;
		}
	}

	return true;
}

bool Face :: Read( File & p_file)
{
	// on lit la normale de la face
	if ( ! m_ptFaceNormal.Read( p_file))
	{
		return false;
	}

	// on lit les normales par vertex de la face
	for (size_t i = 0 ; i < 3 ; i++)
	{
		if ( ! m_vertex[0].GetNormal().Read( p_file))
		{
			return false;
		}
	}

	// on lit les coordonnées de texture par vertex de la face
	for (size_t i = 0 ; i < 3 ; i++)
	{
		if ( ! m_vertex[0].GetTexCoord().Read( p_file))
		{
			return false;
		}
	}

	m_bCenterComputed = false;

	return true;
}