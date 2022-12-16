#include "CastorUtils/Graphics/Position.hpp"

namespace castor
{
	Position::Position( int32_t x, int32_t y )
		:	BaseType( m_data.buffer )
	{
		m_data.position.x = x;
		m_data.position.y = y;
	}

	Position::Position( Position const & rhs )
		:	BaseType( m_data.buffer )
	{
		m_data.position.x = rhs.m_data.position.x;
		m_data.position.y = rhs.m_data.position.y;
	}

	Position::Position( Position && rhs )
		:	BaseType( m_data.buffer )
	{
		m_data.position.x = rhs.m_data.position.x;
		m_data.position.y = rhs.m_data.position.y;
		rhs.m_data.position.x = 0;
		rhs.m_data.position.y = 0;
	}

	Position::~Position()
	{
	}

	Position & Position::operator =( Position const & rhs )
	{
		m_data.position.x = rhs.m_data.position.x;
		m_data.position.y = rhs.m_data.position.y;
		return *this;
	}

	Position & Position::operator =( Position && rhs )
	{
		if ( this != &rhs )
		{
			m_data.position.x = rhs.m_data.position.x;
			m_data.position.y = rhs.m_data.position.y;
			rhs.m_data.position.x = 0;
			rhs.m_data.position.y = 0;
		}

		return *this;
	}

	void Position::set( int32_t x, int32_t y )
	{
		m_data.position.x = x;
		m_data.position.y = y;
	}

	void Position::offset( int32_t x, int32_t y )
	{
		m_data.position.x += x;
		m_data.position.y += y;
	}

	bool operator ==( Position const & lhs, Position const & rhs )
	{
		return lhs.x() == rhs.x() && lhs.y() == rhs.y();
	}

	bool operator !=( Position const & lhs, Position const & rhs )
	{
		return lhs.x() != rhs.x() || lhs.y() != rhs.y();
	}
}
