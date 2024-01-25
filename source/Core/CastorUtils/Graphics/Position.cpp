#include "CastorUtils/Graphics/Position.hpp"

namespace castor
{
	Position::Position( int32_t x, int32_t y )
		: BaseType{ getData().buffer.data() }
	{
		getData().position.x = x;
		getData().position.y = y;
	}

	Position::Position( Position const & rhs )
		:DataHolderT{ rhs }
		, BaseType{ getData().buffer.data() }
	{
	}

	Position::Position( Position && rhs )noexcept
		: DataHolderT{ std::move( rhs ) }
		, BaseType{ getData().buffer.data() }
	{
	}

	Position & Position::operator=( Position const & rhs )
	{
		getData().position.x = rhs.getData().position.x;
		getData().position.y = rhs.getData().position.y;
		return *this;
	}

	Position & Position::operator=( Position && rhs )noexcept
	{
		if ( this != &rhs )
		{
			getData().position.x = rhs.getData().position.x;
			getData().position.y = rhs.getData().position.y;
			rhs.getData().position.x = 0;
			rhs.getData().position.y = 0;
		}

		return *this;
	}

	void Position::set( int32_t x, int32_t y )
	{
		getData().position.x = x;
		getData().position.y = y;
	}

	void Position::offset( int32_t x, int32_t y )
	{
		getData().position.x += x;
		getData().position.y += y;
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
