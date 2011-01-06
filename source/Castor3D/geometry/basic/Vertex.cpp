#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/geometry/basic/Vertex.h"

using namespace Castor3D;

unsigned long long Vertex::Count = 0;
size_t Vertex::Size = 11;

Vertex :: Vertex( real x, real y, real z, size_t p_index)
	:	m_ptCoords( x, y, z)
	,	m_bOwnBuffer( false)
	,	m_uiIndex( p_index)
	,	m_pBuffer( NULL)
	,	m_bManual( false)
{
	Count++;
	UnlinkCoords();
}

Vertex :: Vertex( const Vertex & p_vertex)
	:	m_ptCoords( p_vertex.m_ptCoords)
	,	m_ptNormal( p_vertex.m_ptNormal)
	,	m_ptTangent( p_vertex.m_ptTangent)
	,	m_ptTexCoord( p_vertex.m_ptTexCoord)
	,	m_bManual( p_vertex.m_bManual)
	,	m_bOwnBuffer( false)
	,	m_uiIndex( p_vertex.GetIndex())
	,	m_pBuffer( NULL)
{
	Count++;
	UnlinkCoords();

}

Vertex :: Vertex( const Point3r & p_point)
	:	m_ptCoords( p_point)
	,	m_bOwnBuffer( false)
	,	m_uiIndex( 0)
	,	m_pBuffer( NULL)
	,	m_bManual( false)
{
	Count++;
	UnlinkCoords();
}

Vertex :: Vertex( const IdPoint3r & p_idPoint)
	:	m_ptCoords( p_idPoint)
	,	m_bOwnBuffer( false)
	,	m_uiIndex( p_idPoint.GetIndex())
	,	m_pBuffer( NULL)
	,	m_bManual( false)
{
	Count++;
	UnlinkCoords();
}

Vertex :: Vertex( real * p_pBuffer)
	:	m_bOwnBuffer( false)
	,	m_uiIndex( 0)
	,	m_pBuffer( NULL)
	,	m_bManual( false)
{
	Count++;
	m_pBuffer = p_pBuffer;
	_link();
}

Vertex :: Vertex( const Vertex & p_v1, const Vertex & p_v2)
	:	m_ptCoords( p_v2.GetCoords() - p_v1.GetCoords())
	,	m_bOwnBuffer( false)
	,	m_uiIndex( 0)
	,	m_pBuffer( NULL)
	,	m_bManual( false)
{
	Count++;
	UnlinkCoords();
}

Vertex :: ~Vertex()
{
	if (m_bOwnBuffer && m_pBuffer != NULL)
	{
		delete [] m_pBuffer;
		m_pBuffer = NULL;
	}

	Count--;
}

void Vertex :: LinkCoords( real * p_pBuffer)
{
	Point3r l_ptCoords(		m_ptCoords);
	Point3r l_ptNormal(		m_ptNormal);
	Point3r l_ptTangent(	m_ptTangent);
	Point2r l_ptTexCoord(	m_ptTexCoord);

	m_ptCoords.UnlinkCoords();
	m_ptNormal.UnlinkCoords();
	m_ptTangent.UnlinkCoords();
	m_ptTexCoord.UnlinkCoords();

	if (m_bOwnBuffer && m_pBuffer != NULL)
	{
		delete [] m_pBuffer;
		m_pBuffer = NULL;
	}

	m_pBuffer = p_pBuffer;
	m_bOwnBuffer = false;
	_link( l_ptCoords, l_ptNormal, l_ptTangent, l_ptTexCoord);
}

void Vertex :: UnlinkCoords()
{
	Point3r l_ptCoords(		m_ptCoords);
	Point3r l_ptNormal(		m_ptNormal);
	Point3r l_ptTangent(	m_ptTangent);
	Point2r l_ptTexCoord(	m_ptTexCoord);

	if ( ! m_bOwnBuffer)
	{
		m_pBuffer = new real[Size];
	}

	m_bOwnBuffer = true;

	_link( l_ptCoords, l_ptNormal, l_ptTangent, l_ptTexCoord);
}

Vertex & Vertex :: operator =( const Vertex & p_vertex)
{
	m_ptCoords.UnlinkCoords();
	m_ptNormal.UnlinkCoords();
	m_ptTangent.UnlinkCoords();
	m_ptTexCoord.UnlinkCoords();

	m_ptCoords		= p_vertex.m_ptCoords;
	m_ptNormal		= p_vertex.m_ptNormal;
	m_ptTangent		= p_vertex.m_ptTangent;
	m_ptTexCoord	= p_vertex.m_ptTexCoord;
	m_uiIndex		= p_vertex.m_uiIndex;
	m_bManual		= p_vertex.m_bManual;

	if (m_bOwnBuffer && m_pBuffer != NULL)
	{
		delete [] m_pBuffer;
		m_pBuffer = NULL;
	}

	m_bOwnBuffer	= false;
	UnlinkCoords();

	return * this;
}

Vertex & Vertex :: operator =( const IdPoint3r & p_idPoint)
{
	m_ptCoords.UnlinkCoords();
	m_ptNormal.UnlinkCoords();
	m_ptTangent.UnlinkCoords();
	m_ptTexCoord.UnlinkCoords();

	m_ptCoords		= p_idPoint;
	m_ptNormal		= Point3r();
	m_ptTangent		= Point3r();
	m_ptTexCoord	= Point3r();
	m_uiIndex		= p_idPoint.GetIndex();
	m_bManual		= false;

	if (m_bOwnBuffer && m_pBuffer != NULL)
	{
		delete [] m_pBuffer;
		m_pBuffer = NULL;
	}

	m_bOwnBuffer	= false;
	UnlinkCoords();

	return * this;
}

void Vertex :: SetNormal( const Point3r & val, bool p_bManual)
{
	SetNormal( val[0], val[1], val[2], p_bManual);
}

void Vertex :: SetNormal( real x, real y, real z, bool p_bManual)
{
	m_ptNormal[0] = x;
	m_ptNormal[1] = y;
	m_ptNormal[2] = z;

	if (p_bManual || m_bManual)
	{
		p_bManual = true;
	}

	m_bManual = p_bManual;
}

void Vertex :: SetNormal( const real * p_pCoords, bool p_bManual)
{
	SetNormal( p_pCoords[0], p_pCoords[1], p_pCoords[2], p_bManual);
}

void Vertex :: SetTexCoord( const Point2r & val, bool p_bManual)
{
	SetTexCoord( val[0], val[1], p_bManual);
}

void Vertex :: SetTexCoord( real x, real y, bool p_bManual)
{
	m_ptTexCoord[0] = x;
	m_ptTexCoord[1] = y;

	if (p_bManual || m_bManual)
	{
		p_bManual = true;
	}

	m_bManual = p_bManual;
}

void Vertex :: SetTexCoord( const real * p_pCoords, bool p_bManual)
{
	SetTexCoord( p_pCoords[0], p_pCoords[1], p_bManual);
}

void Vertex :: SetTangent( const Point3r & val, bool p_bManual)
{
	SetTangent( val[0], val[1], val[2], p_bManual);
}

void Vertex :: SetTangent( real x, real y, real z, bool p_bManual)
{
	m_ptTangent[0] = x;
	m_ptTangent[1] = y;
	m_ptTangent[2] = z;

	if (p_bManual || m_bManual)
	{
		p_bManual = true;
	}

	m_bManual = p_bManual;
}

void Vertex :: SetTangent( const real * p_pCoords, bool p_bManual)
{
	SetTangent( p_pCoords[0], p_pCoords[1], p_pCoords[2], p_bManual);
}

void Vertex :: _link()
{
	m_ptTexCoord.LinkCoords(	& m_pBuffer[0]);
	m_ptNormal.LinkCoords(		& m_pBuffer[2]);
	m_ptCoords.LinkCoords(		& m_pBuffer[5]);
	m_ptTangent.LinkCoords( 	& m_pBuffer[8]);
}

void Vertex :: _assign( const Point3r & p_ptCoords, const Point3r & p_ptNormal, const Point3r & p_ptTangent, const Point2r & p_ptTexCoords)
{
	m_ptCoords =	p_ptCoords;
	m_ptNormal =	p_ptNormal;
	m_ptTangent =	p_ptTangent;
	m_ptTexCoord =	p_ptTexCoords;
}

void Vertex :: _link( const Point3r & p_ptCoords, const Point3r & p_ptNormal, const Point3r & p_ptTangent, const Point2r & p_ptTexCoords)
{
	_link();
	_assign( p_ptCoords, p_ptNormal, p_ptTangent, p_ptTexCoords);
}