#include "CastorUtils/Graphics/Size.hpp"

namespace castor
{
	Size::Size()noexcept
		: Size{ 0u, 0u }
	{
	}

	Size::Size( uint32_t width, uint32_t height )noexcept
		: BaseType{ getData().buffer.data() }
	{
		getData().size.x = width;
		getData().size.y = height;
	}

	Size::Size( Size const & rhs )noexcept
		: BaseType{ getData().buffer.data() }
	{
		getData().size = rhs.getData().size;
	}

	Size::Size( Size && rhs )noexcept
		: BaseType{ getData().buffer.data() }
	{
		getData().size = rhs.getData().size;
		rhs.getData().size.x = 0;
		rhs.getData().size.y = 0;
	}

	Size & Size::operator=( Size const & rhs )noexcept
	{
		getData().size = rhs.getData().size;
		return *this;
	}

	Size & Size::operator=( Size && rhs )noexcept
	{
		if ( this != &rhs )
		{
			getData().size = rhs.getData().size;
			rhs.getData().size.x = 0;
			rhs.getData().size.y = 0;
		}

		return *this;
	}

	void Size::set( uint32_t width, uint32_t height )noexcept
	{
		getData().size.x = width;
		getData().size.y = height;
	}

	void Size::grow( int32_t cx, int32_t cy )noexcept
	{
		if ( cx > 0 && std::numeric_limits< uint32_t >::max() - getData().size.x < uint32_t( cx ) )
		{
			getData().size.x = std::numeric_limits< uint32_t >::max();
		}
		else if ( cx < 0 && getData().size.x < uint32_t( -cx ) )
		{
			getData().size.x = 0;
		}
		else
		{
			getData().size.x += cx;
		}

		if ( cy > 0 && std::numeric_limits< uint32_t >::max() - getData().size.y < uint32_t( cy ) )
		{
			getData().size.y = std::numeric_limits< uint32_t >::max();
		}
		else if ( cy < 0 && getData().size.y < uint32_t( -cy ) )
		{
			getData().size.y = 0;
		}
		else
		{
			getData().size.y += cy;
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
