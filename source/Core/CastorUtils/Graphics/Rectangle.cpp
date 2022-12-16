#include "CastorUtils/Graphics/Rectangle.hpp"

namespace castor
{
	Rectangle::Rectangle( Position const & ptStart, Size const & size )
		: Coords4i( m_data.buffer )
	{
		m_data.rect.left = ptStart.x();
		m_data.rect.top = ptStart.y();
		m_data.rect.right = m_data.rect.left + int32_t( size.getWidth() );
		m_data.rect.bottom = m_data.rect.top + int32_t( size.getHeight() );
	}

	Rectangle::Rectangle( int32_t iLeft, int32_t iTop, int32_t iRight, int32_t iBottom )
		: Coords4i( m_data.buffer )
	{
		m_data.rect.left = iLeft;
		m_data.rect.top = iTop;
		m_data.rect.right = iRight;
		m_data.rect.bottom = iBottom;
	}

	Rectangle::Rectangle( Rectangle const & rhs )
		: Coords4i( m_data.buffer )
	{
		m_data.rect.left = rhs.m_data.rect.left;
		m_data.rect.top = rhs.m_data.rect.top;
		m_data.rect.right = rhs.m_data.rect.right;
		m_data.rect.bottom = rhs.m_data.rect.bottom;
	}

	Rectangle & Rectangle::operator=( Rectangle const & rhs )
	{
		m_data.rect.left = rhs.m_data.rect.left;
		m_data.rect.top = rhs.m_data.rect.top;
		m_data.rect.right = rhs.m_data.rect.right;
		m_data.rect.bottom = rhs.m_data.rect.bottom;
		return *this;
	}

	Intersection Rectangle::intersects( Position const & point )const
	{
		Intersection eReturn = Intersection::eOut;

		if ( ( point.x() >= left() ) && ( point.y() >= top() ) && ( point.x() <= right() ) && ( point.y() <= bottom() ) )
		{
			eReturn = Intersection::eIn;
		}

		return eReturn;
	}

	Intersection Rectangle::intersects( Rectangle const & rcRect ) const
	{
		// Calcul du rectangle d'intersection
		Point2i ptStart( std::max( left(), rcRect.left() ), std::max( top(), rcRect.top() ) );
		Point2i ptEnd( std::min( right(), rcRect.right() ), std::min( bottom(), rcRect.bottom() ) );
		Rectangle rcOverlap( ptStart[0], ptStart[1], ptEnd[0], ptEnd[1] );
		Intersection eReturn = Intersection::eIntersect;

		if ( ( ptStart[0] > right() ) || ( ptStart[1] > bottom() ) )
		{
			eReturn = Intersection::eOut;
		}
		else if ( ( rcOverlap == * this ) || ( rcOverlap == rcRect ) )
		{
			eReturn = Intersection::eIn;
		}

		return eReturn;
	}

	void Rectangle::set( int32_t iLeft, int32_t iTop, int32_t iRight, int32_t iBottom )
	{
		m_data.rect.left	= iLeft;
		m_data.rect.top		= iTop;
		m_data.rect.right	= iRight;
		m_data.rect.bottom	= iBottom;
	}

	void Rectangle::size( Size & result )const
	{
		result.set( uint32_t( abs( getWidth() ) )
			, uint32_t( abs( getHeight() ) ) );
	}
}
