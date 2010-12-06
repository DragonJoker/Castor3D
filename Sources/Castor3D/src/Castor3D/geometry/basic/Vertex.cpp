#include "PrecompiledHeader.h"

#include "geometry/basic/Vertex.h"

using namespace Castor3D;

unsigned long Vertex :: s_vertexNumber = 0;
size_t Vertex :: Size = 11;

Vertex :: Vertex( real x, real y, real z, size_t p_index)
	:	m_ptCoords( x, y, z),
		m_index	( p_index),
		m_bOwnBuffer( false)
{
	s_vertexNumber++;
	UnlinkCoords();
}

Vertex :: Vertex( const Vertex & p_vertex)
	:	m_ptCoords( p_vertex.m_ptCoords),
		m_ptNormal( p_vertex.m_ptNormal),
		m_ptTangent( p_vertex.m_ptTangent),
		m_ptTexCoord( p_vertex.m_ptTexCoord),
		m_index( 0),
		m_bOwnBuffer( false)
{
	s_vertexNumber++;
	UnlinkCoords();

}

Vertex :: Vertex( const Point3r & p_vertex)
	:	m_ptCoords( p_vertex),
		m_index( 0),
		m_bOwnBuffer( false)
{
	s_vertexNumber++;
	UnlinkCoords();
}

Vertex :: Vertex( real * p_pBuffer)
	:	m_index( 0),
		m_bOwnBuffer( false)
{
	m_pBuffer = p_pBuffer;
	m_ptTexCoord.LinkCoords(	& m_pBuffer[0]);
	m_ptNormal.LinkCoords(		& m_pBuffer[2]);
	m_ptCoords.LinkCoords(		& m_pBuffer[5]);
	m_ptTangent.LinkCoords( 	& m_pBuffer[8]);
}

Vertex :: Vertex( const Vertex & p_v1, const Vertex & p_v2)
	:	m_ptCoords( p_v2.m_ptCoords - p_v1.m_ptCoords),
		m_index( 0),
		m_bOwnBuffer( false)
{
	s_vertexNumber++;
	UnlinkCoords();
}

Vertex :: ~Vertex()
{
	if (m_bOwnBuffer && m_pBuffer != NULL)
	{
		delete [] m_pBuffer;
		m_pBuffer = NULL;
	}

	s_vertexNumber--;
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
		m_pBuffer = new real[12];
	}

	m_bOwnBuffer = true;
	_link( l_ptCoords, l_ptNormal, l_ptTangent, l_ptTexCoord);
}

void Vertex :: _link( const Point3r & p_ptCoords, const Point3r & p_ptNormal, const Point3r & p_ptTangent, const Point2r & p_ptTexCoords)
{
	m_ptTexCoord.LinkCoords(	& m_pBuffer[0]);
	m_ptNormal.LinkCoords(		& m_pBuffer[2]);
	m_ptCoords.LinkCoords(		& m_pBuffer[5]);
	m_ptTangent.LinkCoords( 	& m_pBuffer[8]);

	m_ptCoords =	p_ptCoords;
	m_ptNormal =	p_ptNormal;
	m_ptTangent =	p_ptTangent;
	m_ptTexCoord =	p_ptTexCoords;
}