#include "CastorUtils/Graphics/Size.hpp"

namespace castor
{
	Size::Size( uint32_t width, uint32_t height )noexcept
		:	BaseType( m_data.buffer )
	{
		m_data.size.x = width;
		m_data.size.y = height;
	}

	Size::Size( Size const & rhs )noexcept
		:	BaseType( m_data.buffer )
	{
		m_data.size = rhs.m_data.size;
	}

	Size::Size( Size && rhs )noexcept
		:	BaseType( m_data.buffer )
	{
		m_data.size = rhs.m_data.size;
		rhs.m_data.size.x = 0;
		rhs.m_data.size.y = 0;
	}

	Size::~Size()
	{
	}

	Size & Size::operator=( Size const & rhs )noexcept
	{
		m_data.size = rhs.m_data.size;
		return *this;
	}

	Size & Size::operator=( Size && rhs )noexcept
	{
		if ( this != &rhs )
		{
			m_data.size = rhs.m_data.size;
			rhs.m_data.size.x = 0;
			rhs.m_data.size.y = 0;
		}

		return *this;
	}

	void Size::set( uint32_t width, uint32_t height )noexcept
	{
		m_data.size.x = width;
		m_data.size.y = height;
	}

	void Size::grow( int32_t cx, int32_t cy )noexcept
	{
		if ( cx > 0 && std::numeric_limits< uint32_t >::max() - m_data.size.x < uint32_t( cx ) )
		{
			m_data.size.x = std::numeric_limits< uint32_t >::max();
		}
		else if ( cx < 0 && m_data.size.x < uint32_t( -cx ) )
		{
			m_data.size.x = 0;
		}
		else
		{
			m_data.size.x += cx;
		}

		if ( cy > 0 && std::numeric_limits< uint32_t >::max() - m_data.size.y < uint32_t( cy ) )
		{
			m_data.size.y = std::numeric_limits< uint32_t >::max();
		}
		else if ( cy < 0 && m_data.size.y < uint32_t( -cy ) )
		{
			m_data.size.y = 0;
		}
		else
		{
			m_data.size.y += cy;
		}
	}

	bool operator==( Size const & lhs, Size const & rhs )noexcept
	{
		return lhs.getWidth() == rhs.getWidth() && lhs.getHeight() == rhs.getHeight();
	}

	bool operator!=( Size const & lhs, Size const & rhs )noexcept
	{
		return lhs.getWidth() != rhs.getWidth() || lhs.getHeight() != rhs.getHeight();
	}
}
