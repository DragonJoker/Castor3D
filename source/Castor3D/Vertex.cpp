#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/Vertex.hpp"

using namespace Castor3D;

//*************************************************************************************************

bool Loader<Point2f> :: Read( Point2f & p_object, Utils::File & p_file)
{
	String l_strWord;
	bool l_bReturn = p_file.ReadLine( l_strWord, 1024, cuT( " \r\n;\t")) > 0 && l_strWord.is_floating();
	xchar l_cDump;

	if (l_bReturn)
	{
		p_object[0] = l_strWord.to_float();
		p_file.ReadChar( l_cDump);
		l_bReturn = p_file.ReadLine( l_strWord, 1024, cuT( " \r\n;\t")) > 0 && l_strWord.is_floating();
	}

	if (l_bReturn)
	{
		p_object[1] = l_strWord.to_float();
	}

	return l_bReturn;
}

bool Loader<Point2f> :: Write( const Point2f & p_object, Utils::File & p_file)
{
	String l_strWord;
	bool l_bReturn = p_file.Print( 1024, cuT( "%f %f"), p_object[0], p_object[1]) > 0;
	return l_bReturn;
}

//*************************************************************************************************

bool Loader<Point3f> :: Read( Point3f & p_object, Utils::File & p_file)
{
	String l_strWord;
	bool l_bReturn = p_file.ReadLine( l_strWord, 1024, cuT( " \r\n;\t")) > 0 && l_strWord.is_floating();
	xchar l_cDump;

	if (l_bReturn)
	{
		p_object[0] = l_strWord.to_float();
		p_file.ReadChar( l_cDump);
		l_bReturn = p_file.ReadLine( l_strWord, 1024, cuT( " \r\n;\t")) > 0 && l_strWord.is_floating();
	}

	if (l_bReturn)
	{
		p_object[1] = l_strWord.to_float();
		p_file.ReadChar( l_cDump);
		l_bReturn = p_file.ReadLine( l_strWord, 1024, cuT( " \r\n;\t")) > 0 && l_strWord.is_floating();
	}

	if (l_bReturn)
	{
		p_object[2] = l_strWord.to_float();
	}

	return l_bReturn;
}

bool Loader<Point3f> :: Write( const Point3f & p_object, Utils::File & p_file)
{
	String l_strWord;
	bool l_bReturn = p_file.Print( 1024, cuT( "%f %f %f"), p_object[0], p_object[1], p_object[2]) > 0;
	return l_bReturn;
}

//*************************************************************************************************

bool Loader<Point4f> :: Read( Point4f & p_object, Utils::File & p_file)
{
	String l_strWord;
	bool l_bReturn = p_file.ReadLine( l_strWord, 1024, cuT( " \r\n;\t")) > 0 && l_strWord.is_floating();
	xchar l_cDump;

	if (l_bReturn)
	{
		p_object[0] = l_strWord.to_float();
		p_file.ReadChar( l_cDump);
		l_bReturn = p_file.ReadLine( l_strWord, 1024, cuT( " \r\n;\t")) > 0 && l_strWord.is_floating();
	}

	if (l_bReturn)
	{
		p_object[1] = l_strWord.to_float();
		p_file.ReadChar( l_cDump);
		l_bReturn = p_file.ReadLine( l_strWord, 1024, cuT( " \r\n;\t")) > 0 && l_strWord.is_floating();
	}

	if (l_bReturn)
	{
		p_object[2] = l_strWord.to_float();
		p_file.ReadChar( l_cDump);
		l_bReturn = p_file.ReadLine( l_strWord, 1024, cuT( " \r\n;\t")) > 0 && l_strWord.is_floating();
	}

	if (l_bReturn)
	{
		p_object[3] = l_strWord.to_float();
	}

	return l_bReturn;
}

bool Loader<Point4f> :: Write( const Point4f & p_object, Utils::File & p_file)
{
	String l_strWord;
	bool l_bReturn = p_file.Print( 1024, cuT( "%f %f %f %f"), p_object[0], p_object[1], p_object[2]) > 0;
	return l_bReturn;
}

//*************************************************************************************************

bool Loader<Point2d> :: Read( Point2d & p_object, Utils::File & p_file)
{
	String l_strWord;
	bool l_bReturn = p_file.ReadLine( l_strWord, 1024, cuT( " \r\n;\t")) > 0 && l_strWord.is_floating();
	xchar l_cDump;

	if (l_bReturn)
	{
		p_object[0] = l_strWord.to_float();
		p_file.ReadChar( l_cDump);
		l_bReturn = p_file.ReadLine( l_strWord, 1024, cuT( " \r\n;\t")) > 0 && l_strWord.is_floating();
	}

	if (l_bReturn)
	{
		p_object[1] = l_strWord.to_float();
	}

	return l_bReturn;
}

bool Loader<Point2d> :: Write( const Point2d & p_object, Utils::File & p_file)
{
	String l_strWord;
	bool l_bReturn = p_file.Print( 1024, cuT( "%g %g"), p_object[0], p_object[1]) > 0;
	return l_bReturn;
}

//*************************************************************************************************

bool Loader<Point3d> :: Read( Point3d & p_object, Utils::File & p_file)
{
	String l_strWord;
	bool l_bReturn = p_file.ReadLine( l_strWord, 1024, cuT( " \r\n;\t")) > 0 && l_strWord.is_floating();
	xchar l_cDump;

	if (l_bReturn)
	{
		p_object[0] = l_strWord.to_float();
		p_file.ReadChar( l_cDump);
		l_bReturn = p_file.ReadLine( l_strWord, 1024, cuT( " \r\n;\t")) > 0 && l_strWord.is_floating();
	}

	if (l_bReturn)
	{
		p_object[1] = l_strWord.to_float();
		p_file.ReadChar( l_cDump);
		l_bReturn = p_file.ReadLine( l_strWord, 1024, cuT( " \r\n;\t")) > 0 && l_strWord.is_floating();
	}

	if (l_bReturn)
	{
		p_object[2] = l_strWord.to_float();
	}

	return l_bReturn;
}

bool Loader<Point3d> :: Write( const Point3d & p_object, Utils::File & p_file)
{
	String l_strWord;
	bool l_bReturn = p_file.Print( 1024, cuT( "%g %g %g"), p_object[0], p_object[1], p_object[2]) > 0;
	return l_bReturn;
}

//*************************************************************************************************

bool Loader<Point4d> :: Read( Point4d & p_object, Utils::File & p_file)
{
	String l_strWord;
	bool l_bReturn = p_file.ReadLine( l_strWord, 1024, cuT( " \r\n;\t")) > 0 && l_strWord.is_floating();
	xchar l_cDump;

	if (l_bReturn)
	{
		p_object[0] = l_strWord.to_float();
		p_file.ReadChar( l_cDump);
		l_bReturn = p_file.ReadLine( l_strWord, 1024, cuT( " \r\n;\t")) > 0 && l_strWord.is_floating();
	}

	if (l_bReturn)
	{
		p_object[1] = l_strWord.to_float();
		p_file.ReadChar( l_cDump);
		l_bReturn = p_file.ReadLine( l_strWord, 1024, cuT( " \r\n;\t")) > 0 && l_strWord.is_floating();
	}

	if (l_bReturn)
	{
		p_object[2] = l_strWord.to_float();
		p_file.ReadChar( l_cDump);
		l_bReturn = p_file.ReadLine( l_strWord, 1024, cuT( " \r\n;\t")) > 0 && l_strWord.is_floating();
	}

	if (l_bReturn)
	{
		p_object[3] = l_strWord.to_float();
	}

	return l_bReturn;
}

bool Loader<Point4d> :: Write( const Point4d & p_object, Utils::File & p_file)
{
	String l_strWord;
	bool l_bReturn = p_file.Print( 1024, cuT( "%g %g %g %g"), p_object[0], p_object[1], p_object[2]) > 0;
	return l_bReturn;
}

//*************************************************************************************************

bool Loader<Vertex> :: Read( Vertex & p_object, Utils::File & p_file)
{
	bool l_bReturn = Loader<Point3r>::Read( p_object.GetNormal(), p_file);

	if (l_bReturn)
	{
		l_bReturn = Loader<Point3r>::Read( p_object.GetTangent(), p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = Loader<Point2r>::Read( p_object.GetTexCoord(), p_file);
	}

	return l_bReturn;
}

bool Loader<Vertex> :: Write( const Vertex & p_object, Utils::File & p_file)
{
	bool l_bReturn = Loader<Point3r>::Write( p_object.GetNormal(), p_file);

	if (l_bReturn)
	{
		l_bReturn = Loader<Point3r>::Write( p_object.GetTangent(), p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = Loader<Point2r>::Write( p_object.GetTexCoord(), p_file);
	}

	return l_bReturn;
}

//*************************************************************************************************

unsigned long long Vertex::Count = 0;
size_t Vertex::Size = 11;
BufferElementDeclaration Vertex::VertexDeclarationElements[] =
{
	{ 0, eELEMENT_USAGE_POSITION,	eELEMENT_TYPE_3FLOATS},
	{ 0, eELEMENT_USAGE_NORMAL,		eELEMENT_TYPE_3FLOATS},
	{ 0, eELEMENT_USAGE_TANGENT,		eELEMENT_TYPE_3FLOATS},
	{ 0, eELEMENT_USAGE_TEXCOORDS0,	eELEMENT_TYPE_2FLOATS}
};

BufferDeclaration Vertex::VertexDeclaration( VertexDeclarationElements);

Vertex :: Vertex( real x, real y, real z, size_t p_index)
	:	BufferElement( & VertexDeclaration)
	,	m_uiIndex( p_index)
	,	m_bManual( false)
{
	GetElement<Point3r>( 0).copy( Point3r( x, y, z));
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

Vertex :: Vertex( Point3r const & p_point)
	:	BufferElement( & VertexDeclaration)
	,	m_uiIndex( 0)
	,	m_bManual( false)
{
	GetElement<Point3r>( 0).clopy( p_point);
	Count++;
	UnlinkCoords();
}

Vertex :: Vertex( const IdPoint3r & p_idPoint)
	:	BufferElement( & VertexDeclaration)
	,	m_uiIndex( p_idPoint.GetIndex())
	,	m_bManual( false)
{
	GetElement<Point3r>( 0).clopy( p_idPoint);
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
	GetElement<Point3r>( 0).copy( p_v2.GetCoords() - p_v1.GetCoords());
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
	GetElement<Point3r>( 0).unlink();
	GetElement<Point3r>( 1).unlink();
	GetElement<Point3r>( 2).unlink();
	GetElement<Point2r>( 3).unlink();

	GetElement<Point3r>( 0).clopy( p_idPoint);
	GetElement<Point3r>( 1).clopy( Point3r());
	GetElement<Point3r>( 2).clopy( Point3r());
	GetElement<Point2r>( 3).clopy( Point2r());

	if (m_bOwnBuffer && m_pBuffer)
	{
		delete [] m_pBuffer;
		m_pBuffer = nullptr;
	}

	m_bOwnBuffer	= false;
	UnlinkCoords();
	m_uiIndex		= p_idPoint.GetIndex();
	m_bManual		= false;
	return * this;
}

void Vertex :: SetNormal( Point3r const & val, bool p_bManual)
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

void Vertex :: SetNormal( real const * p_pCoords, bool p_bManual)
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

void Vertex :: SetTexCoord( real const * p_pCoords, bool p_bManual)
{
	SetTexCoord( p_pCoords[0], p_pCoords[1], p_bManual);
}

void Vertex :: SetTangent( Point3r const & val, bool p_bManual)
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

void Vertex :: SetTangent( real const * p_pCoords, bool p_bManual)
{
	SetTangent( p_pCoords[0], p_pCoords[1], p_pCoords[2], p_bManual);
}

void Vertex :: _link()
{
	BufferElement::_link();
}

BEGIN_SERIALISE_MAP( Vertex, Serialisable)
	ADD_POINT( real, 3, GetNormal())
	ADD_POINT( real, 3, GetTangent())
	ADD_POINT( real, 2, GetTexCoord())
END_SERIALISE_MAP()