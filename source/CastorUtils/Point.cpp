#include "CastorUtils/PrecompiledHeader.hpp"

#include "CastorUtils/Point.hpp"

using namespace Castor;

//*************************************************************************************************

Size :: Size( uint32_t p_uiWidth, uint32_t p_uiHeight )
	:	BaseType( m_data.buffer )
{
	m_data.size.cx	= p_uiWidth;
	m_data.size.cy	= p_uiHeight;
}

Size :: Size( Size const & p_obj )
	:	BaseType( m_data.buffer )
{
	m_data.size.cx	= p_obj.m_data.size.cx;
	m_data.size.cy	= p_obj.m_data.size.cy;
}

Size :: Size( Size && p_obj )
	:	BaseType( m_data.buffer )
{
	m_data.size.cx	= p_obj.m_data.size.cx;
	m_data.size.cy	= p_obj.m_data.size.cy;
	p_obj.m_data.size.cx = 0;
	p_obj.m_data.size.cy = 0;
}

Size :: ~Size()
{
}

Size & Size :: operator =( Size const & p_obj )
{
	m_data.size.cx	= p_obj.m_data.size.cx;
	m_data.size.cy	= p_obj.m_data.size.cy;
	return *this;
}

Size & Size :: operator =( Size && p_obj )
{
	if( this != &p_obj )
	{
		m_data.size.cx	= p_obj.m_data.size.cx;
		m_data.size.cy	= p_obj.m_data.size.cy;
		p_obj.m_data.size.cx = 0;
		p_obj.m_data.size.cy = 0;
	}
	
	return *this;
}

void Size :: set( uint32_t p_uiWidth, uint32_t p_uiHeight )
{
	m_data.size.cx	= p_uiWidth;
	m_data.size.cy	= p_uiHeight;
}

void Size :: grow( int32_t p_cx, int32_t p_cy )
{
	if( p_cx > 0 && std::numeric_limits< uint32_t >::max() - m_data.size.cx < uint32_t( p_cx ) )
	{
		m_data.size.cx = std::numeric_limits< uint32_t >::max();
	}
	else if( p_cx < 0 && m_data.size.cx < uint32_t( -p_cx ) )
	{
		m_data.size.cx = 0;
	}
	else
	{
		m_data.size.cx += p_cx;
	}
	
	if( p_cy > 0 && std::numeric_limits< uint32_t >::max() - m_data.size.cy < uint32_t( p_cy ) )
	{
		m_data.size.cy = std::numeric_limits< uint32_t >::max();
	}
	else if( p_cy < 0 && m_data.size.cy < uint32_t( -p_cy ) )
	{
		m_data.size.cy = 0;
	}
	else
	{
		m_data.size.cy += p_cy;
	}
}

bool Castor::operator ==( Size const & p_a, Size const & p_b )
{
	return p_a.width() == p_b.width() && p_a.height() == p_b.height();
}

bool Castor::operator !=( Size const & p_a, Size const & p_b )
{
	return p_a.width() != p_b.width() || p_a.height() != p_b.height();
}

//*************************************************************************************************

Position :: Position( int32_t p_x, int32_t p_y )
	:	BaseType( m_data.buffer )
{
	m_data.position.x	= p_x;
	m_data.position.y	= p_y;
}

Position :: Position( Position const & p_obj )
	:	BaseType( m_data.buffer )
{
	m_data.position.x	= p_obj.m_data.position.x;
	m_data.position.y	= p_obj.m_data.position.y;
}

Position :: Position( Position && p_obj )
	:	BaseType( m_data.buffer )
{
	m_data.position.x	= p_obj.m_data.position.x;
	m_data.position.y	= p_obj.m_data.position.y;
	p_obj.m_data.position.x = 0;
	p_obj.m_data.position.y = 0;
}

Position :: ~Position()
{
}

Position & Position :: operator =( Position const & p_obj )
{
	m_data.position.x	= p_obj.m_data.position.x;
	m_data.position.y	= p_obj.m_data.position.y;
	return *this;
}

Position & Position :: operator =( Position && p_obj )
{
	if( this != &p_obj )
	{
		m_data.position.x	= p_obj.m_data.position.x;
		m_data.position.y	= p_obj.m_data.position.y;
		p_obj.m_data.position.x = 0;
		p_obj.m_data.position.y = 0;
	}
	
	return *this;
}

void Position :: set( int32_t p_x, int32_t p_y )
{
	m_data.position.x	= p_x;
	m_data.position.y	= p_y;
}

void Position :: offset( int32_t p_x, int32_t p_y )
{
	m_data.position.x	+= p_x;
	m_data.position.y	+= p_y;
}

bool Castor::operator ==( Position const & p_a, Position const & p_b )
{
	return p_a.x() == p_b.x() && p_a.y() == p_b.y();
}

bool Castor::operator !=( Position const & p_a, Position const & p_b )
{
	return p_a.x() != p_b.x() || p_a.y() != p_b.y();
}

//*************************************************************************************************