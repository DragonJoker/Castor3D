#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/geometry/basic/Vertex.h"

using namespace Castor3D;

unsigned long long Vertex::Count = 0;
size_t Vertex::Size = 11;
BufferElementDeclaration Vertex::VertexDeclarationElements[] =
{
	{ 0, eUsagePosition,	eType3Floats},
	{ 0, eUsageNormal,		eType3Floats},
	{ 0, eUsageTangent,		eType3Floats},
	{ 0, eUsageTexCoords0,	eType2Floats}
};

BufferDeclaration Vertex::VertexDeclaration( VertexDeclarationElements);

Vertex :: Vertex( real x, real y, real z, size_t p_index)
	:	BufferElement( & VertexDeclaration)
	,	m_uiIndex( p_index)
	,	m_bManual( false)
{
	GetElement<Point3r>( 0) = Point3r( x, y, z);
	Count++;
	UnlinkCoords();
}

Vertex :: Vertex( const Vertex & p_vertex)
	:	BufferElement( p_vertex)
	,	m_bManual( p_vertex.m_bManual)
	,	m_uiIndex( p_vertex.GetIndex())
{
	Count++;
	UnlinkCoords();

}

Vertex :: Vertex( const Point3r & p_point)
	:	BufferElement( & VertexDeclaration)
	,	m_uiIndex( 0)
	,	m_bManual( false)
{
	GetElement<Point3r>( 0) = p_point;
	Count++;
	UnlinkCoords();
}

Vertex :: Vertex( const IdPoint3r & p_idPoint)
	:	BufferElement( & VertexDeclaration)
	,	m_uiIndex( p_idPoint.GetIndex())
	,	m_bManual( false)
{
	GetElement<Point3r>( 0) = p_idPoint;
	Count++;
	UnlinkCoords();
}

Vertex :: Vertex( real * p_pBuffer)
	:	BufferElement( & VertexDeclaration)
	,	m_uiIndex( 0)
	,	m_bManual( false)
{
	Count++;
	m_pBuffer = (unsigned char *)p_pBuffer;
	_link();
}

Vertex :: Vertex( const Vertex & p_v1, const Vertex & p_v2)
	:	BufferElement( & VertexDeclaration)
	,	m_uiIndex( 0)
	,	m_bManual( false)
{
	GetElement<Point3r>( 0) = p_v2.GetCoords() - p_v1.GetCoords();
	Count++;
	UnlinkCoords();
}

Vertex :: ~Vertex()
{
	Count--;
}

void Vertex :: LinkCoords( real * p_pBuffer)
{
	BufferElement::LinkCoords( p_pBuffer);
}

void Vertex :: UnlinkCoords()
{
	BufferElement::UnlinkCoords();
}

Vertex & Vertex :: operator =( const Vertex & p_vertex)
{
	BufferElement::operator =( p_vertex);
	m_uiIndex		= p_vertex.m_uiIndex;
	m_bManual		= p_vertex.m_bManual;
	return * this;
}

Vertex & Vertex :: operator =( const IdPoint3r & p_idPoint)
{
	GetElement<Point3r>( 0).UnlinkCoords();
	GetElement<Point3r>( 1).UnlinkCoords();
	GetElement<Point3r>( 2).UnlinkCoords();
	GetElement<Point2r>( 3).UnlinkCoords();

	GetElement<Point3r>( 0) = p_idPoint;
	GetElement<Point3r>( 1) = Point3r();
	GetElement<Point3r>( 2) = Point3r();
	GetElement<Point2r>( 3) = Point2r();

	if (m_bOwnBuffer && m_pBuffer != NULL)
	{
		delete [] m_pBuffer;
		m_pBuffer = NULL;
	}

	m_bOwnBuffer	= false;
	UnlinkCoords();
	m_uiIndex		= p_idPoint.GetIndex();
	m_bManual		= false;
	return * this;
}

void Vertex :: SetNormal( const Point3r & val, bool p_bManual)
{
	SetNormal( val[0], val[1], val[2], p_bManual);
}

void Vertex :: SetNormal( real x, real y, real z, bool p_bManual)
{
	GetElement<Point3r>( 1)[0] = x;
	GetElement<Point3r>( 1)[1] = y;
	GetElement<Point3r>( 1)[2] = z;

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
	GetElement<Point2r>( 3)[0] = x;
	GetElement<Point2r>( 3)[1] = y;

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
	GetElement<Point3r>( 2)[0] = x;
	GetElement<Point3r>( 2)[1] = y;
	GetElement<Point3r>( 2)[2] = z;

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
	BufferElement::_link();
}