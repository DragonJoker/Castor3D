#include "CastorUtils/Graphics/Size.hpp"

namespace castor
{
	Size::Size( uint32_t width, uint32_t height )
		:	BaseType( m_data.buffer )
	{
		m_data.size.cx = width;
		m_data.size.cy = height;
	}

	Size::Size( Size const & rhs )
		:	BaseType( m_data.buffer )
	{
		m_data.size.cx = rhs.m_data.size.cx;
		m_data.size.cy = rhs.m_data.size.cy;
	}

	Size::Size( Size && rhs )
		:	BaseType( m_data.buffer )
	{
		m_data.size.cx = rhs.m_data.size.cx;
		m_data.size.cy = rhs.m_data.size.cy;
		rhs.m_data.size.cx = 0;
		rhs.m_data.size.cy = 0;
	}

	Size::~Size()
	{
	}

	Size & Size::operator =( Size const & rhs )
	{
		m_data.size.cx = rhs.m_data.size.cx;
		m_data.size.cy = rhs.m_data.size.cy;
		return *this;
	}

	Size & Size::operator =( Size && rhs )
	{
		if ( this != &rhs )
		{
			m_data.size.cx = rhs.m_data.size.cx;
			m_data.size.cy = rhs.m_data.size.cy;
			rhs.m_data.size.cx = 0;
			rhs.m_data.size.cy = 0;
		}

		return *this;
	}

	void Size::set( uint32_t width, uint32_t height )
	{
		m_data.size.cx = width;
		m_data.size.cy = height;
	}

	void Size::grow( int32_t cx, int32_t cy )
	{
		if ( cx > 0 && std::numeric_limits< uint32_t >::max() - m_data.size.cx < uint32_t( cx ) )
		{
			m_data.size.cx = std::numeric_limits< uint32_t >::max();
		}
		else if ( cx < 0 && m_data.size.cx < uint32_t( -cx ) )
		{
			m_data.size.cx = 0;
		}
		else
		{
			m_data.size.cx += cx;
		}

		if ( cy > 0 && std::numeric_limits< uint32_t >::max() - m_data.size.cy < uint32_t( cy ) )
		{
			m_data.size.cy = std::numeric_limits< uint32_t >::max();
		}
		else if ( cy < 0 && m_data.size.cy < uint32_t( -cy ) )
		{
			m_data.size.cy = 0;
		}
		else
		{
			m_data.size.cy += cy;
		}
	}

	bool operator ==( Size const & lhs, Size const & rhs )
	{
		return lhs.getWidth() == rhs.getWidth() && lhs.getHeight() == rhs.getHeight();
	}

	bool operator !=( Size const & lhs, Size const & rhs )
	{
		return lhs.getWidth() != rhs.getWidth() || lhs.getHeight() != rhs.getHeight();
	}
}
