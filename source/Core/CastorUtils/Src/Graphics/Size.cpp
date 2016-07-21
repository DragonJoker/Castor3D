#include "Graphics/Size.hpp"

namespace Castor
{
	Size::Size( uint32_t p_width, uint32_t p_height )
		:	BaseType( m_data.buffer )
	{
		m_data.size.cx	= p_width;
		m_data.size.cy	= p_height;
	}

	Size::Size( Size const & p_obj )
		:	BaseType( m_data.buffer )
	{
		m_data.size.cx	= p_obj.m_data.size.cx;
		m_data.size.cy	= p_obj.m_data.size.cy;
	}

	Size::Size( Size && p_obj )
		:	BaseType( m_data.buffer )
	{
		m_data.size.cx	= p_obj.m_data.size.cx;
		m_data.size.cy	= p_obj.m_data.size.cy;
		p_obj.m_data.size.cx = 0;
		p_obj.m_data.size.cy = 0;
	}

	Size::~Size()
	{
	}

	Size & Size::operator =( Size const & p_obj )
	{
		m_data.size.cx	= p_obj.m_data.size.cx;
		m_data.size.cy	= p_obj.m_data.size.cy;
		return *this;
	}

	Size & Size::operator =( Size && p_obj )
	{
		if ( this != &p_obj )
		{
			m_data.size.cx	= p_obj.m_data.size.cx;
			m_data.size.cy	= p_obj.m_data.size.cy;
			p_obj.m_data.size.cx = 0;
			p_obj.m_data.size.cy = 0;
		}

		return *this;
	}

	void Size::set( uint32_t p_width, uint32_t p_height )
	{
		m_data.size.cx	= p_width;
		m_data.size.cy	= p_height;
	}

	void Size::grow( int32_t p_cx, int32_t p_cy )
	{
		if ( p_cx > 0 && std::numeric_limits< uint32_t >::max() - m_data.size.cx < uint32_t( p_cx ) )
		{
			m_data.size.cx = std::numeric_limits< uint32_t >::max();
		}
		else if ( p_cx < 0 && m_data.size.cx < uint32_t( -p_cx ) )
		{
			m_data.size.cx = 0;
		}
		else
		{
			m_data.size.cx += p_cx;
		}

		if ( p_cy > 0 && std::numeric_limits< uint32_t >::max() - m_data.size.cy < uint32_t( p_cy ) )
		{
			m_data.size.cy = std::numeric_limits< uint32_t >::max();
		}
		else if ( p_cy < 0 && m_data.size.cy < uint32_t( -p_cy ) )
		{
			m_data.size.cy = 0;
		}
		else
		{
			m_data.size.cy += p_cy;
		}
	}

	bool operator ==( Size const & p_a, Size const & p_b )
	{
		return p_a.width() == p_b.width() && p_a.height() == p_b.height();
	}

	bool operator !=( Size const & p_a, Size const & p_b )
	{
		return p_a.width() != p_b.width() || p_a.height() != p_b.height();
	}
}
