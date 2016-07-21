#include "Graphics/Position.hpp"

namespace Castor
{
	Position::Position( int32_t p_x, int32_t p_y )
		:	BaseType( m_data.buffer )
	{
		m_data.position.x	= p_x;
		m_data.position.y	= p_y;
	}

	Position::Position( Position const & p_obj )
		:	BaseType( m_data.buffer )
	{
		m_data.position.x	= p_obj.m_data.position.x;
		m_data.position.y	= p_obj.m_data.position.y;
	}

	Position::Position( Position && p_obj )
		:	BaseType( m_data.buffer )
	{
		m_data.position.x	= p_obj.m_data.position.x;
		m_data.position.y	= p_obj.m_data.position.y;
		p_obj.m_data.position.x = 0;
		p_obj.m_data.position.y = 0;
	}

	Position::~Position()
	{
	}

	Position & Position::operator =( Position const & p_obj )
	{
		m_data.position.x	= p_obj.m_data.position.x;
		m_data.position.y	= p_obj.m_data.position.y;
		return *this;
	}

	Position & Position::operator =( Position && p_obj )
	{
		if ( this != &p_obj )
		{
			m_data.position.x	= p_obj.m_data.position.x;
			m_data.position.y	= p_obj.m_data.position.y;
			p_obj.m_data.position.x = 0;
			p_obj.m_data.position.y = 0;
		}

		return *this;
	}

	void Position::set( int32_t p_x, int32_t p_y )
	{
		m_data.position.x	= p_x;
		m_data.position.y	= p_y;
	}

	void Position::offset( int32_t p_x, int32_t p_y )
	{
		m_data.position.x	+= p_x;
		m_data.position.y	+= p_y;
	}

	bool operator ==( Position const & p_a, Position const & p_b )
	{
		return p_a.x() == p_b.x() && p_a.y() == p_b.y();
	}

	bool operator !=( Position const & p_a, Position const & p_b )
	{
		return p_a.x() != p_b.x() || p_a.y() != p_b.y();
	}
}
