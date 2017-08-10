#include "Vertex.hpp"

using namespace castor;

namespace castor3d
{
	Vertex::Vertex( BufferElementGroup & p_group )
		:	m_group( p_group )
	{
	}

	Vertex::Vertex( Vertex const & p_source )
		:	m_group( p_source.m_group )
	{
	}

	Vertex & Vertex::operator =( Vertex const & p_source )
	{
		m_group	= p_source.m_group;
		return *this;
	}

	Vertex::~Vertex()
	{
	}

	Coords3r Vertex::getPosition( void * p_pElement )
	{
		return Coords3r( &reinterpret_cast< real * >( p_pElement )[sm_uiOffsetPos / sizeof( real )] );
	}

	Point3r Vertex::getPosition( void const * p_pElement )
	{
		return Point3r( &reinterpret_cast< real const * >( p_pElement )[sm_uiOffsetPos / sizeof( real )] );
	}

	Coords3r Vertex::getNormal( void * p_pElement )
	{
		return Coords3r( &reinterpret_cast< real * >( p_pElement )[sm_uiOffsetNml / sizeof( real )] );
	}

	Point3r Vertex::getNormal( void const * p_pElement )
	{
		return Point3r( &reinterpret_cast< real const * >( p_pElement )[sm_uiOffsetNml / sizeof( real )] );
	}

	Coords3r Vertex::getTangent( void * p_pElement )
	{
		return Coords3r( &reinterpret_cast< real * >( p_pElement )[sm_uiOffsetTan / sizeof( real )] );
	}

	Point3r Vertex::getTangent( void const * p_pElement )
	{
		return Point3r( &reinterpret_cast< real const * >( p_pElement )[sm_uiOffsetTan / sizeof( real )] );
	}

	Coords3r Vertex::getBitangent( void * p_pElement )
	{
		return Coords3r( &reinterpret_cast< real * >( p_pElement )[sm_uiOffsetBin / sizeof( real )] );
	}

	Point3r Vertex::getBitangent( void const * p_pElement )
	{
		return Point3r( &reinterpret_cast< real const * >( p_pElement )[sm_uiOffsetBin / sizeof( real )] );
	}

	Coords3r Vertex::getTexCoord( void * p_pElement )
	{
		return Coords3r( &reinterpret_cast< real * >( p_pElement )[sm_uiOffsetTex / sizeof( real )] );
	}

	Point3r Vertex::getTexCoord( void const * p_pElement )
	{
		return Point3r( &reinterpret_cast< real const * >( p_pElement )[sm_uiOffsetTex / sizeof( real )] );
	}

	void Vertex::setPosition( void * p_buffer, real const * p_coord )
	{
		std::memcpy( getPosition( p_buffer ).ptr(), p_coord, sm_uiSizePos );
	}

	void Vertex::setPosition( void * p_buffer, real x, real y, real z )
	{
		Coords3r pData = getPosition( p_buffer );
		pData[0] = x;
		pData[1] = y;
		pData[2] = z;
	}

	void Vertex::setNormal( void * p_buffer, real const * p_coord )
	{
		std::memcpy( getNormal( p_buffer ).ptr(), p_coord, sm_uiSizeNml );
	}

	void Vertex::setNormal( void * p_buffer, real x, real y, real z )
	{
		Coords3r pData = getNormal( p_buffer );
		pData[0] = x;
		pData[1] = y;
		pData[2] = z;
	}

	void Vertex::setTangent( void * p_buffer, real const * p_coord )
	{
		std::memcpy( getTangent( p_buffer ).ptr(), p_coord, sm_uiSizeTan );
	}

	void Vertex::setTangent( void * p_buffer, real x, real y, real z )
	{
		Coords3r pData = getTangent( p_buffer );
		pData[0] = x;
		pData[1] = y;
		pData[2] = z;
	}

	void Vertex::setBitangent( void * p_buffer, real const * p_coord )
	{
		std::memcpy( getBitangent( p_buffer ).ptr(), p_coord, sm_uiSizeBin );
	}

	void Vertex::setBitangent( void * p_buffer, real x, real y, real z )
	{
		Coords3r pData = getBitangent( p_buffer );
		pData[0] = x;
		pData[1] = y;
		pData[2] = z;
	}

	void Vertex::setTexCoord( void * p_buffer, real const * p_coord )
	{
		std::memcpy( getTexCoord( p_buffer ).ptr(), p_coord, sm_uiSizeTex );
	}

	void Vertex::setTexCoord( void * p_buffer, real x, real y, real z )
	{
		Coords3r pData = getTexCoord( p_buffer );
		pData[0] = x;
		pData[1] = y;
		pData[2] = z;
	}
}
