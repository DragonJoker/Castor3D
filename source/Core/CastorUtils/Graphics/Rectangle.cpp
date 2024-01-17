#include "CastorUtils/Graphics/Rectangle.hpp"

namespace castor
{
	Rectangle::Rectangle( Position const & ptStart, Size const & size )
		: Coords4i{ getData().buffer.data() }
	{
		getData().rect.left = ptStart.x();
		getData().rect.top = ptStart.y();
		getData().rect.right = getData().rect.left + int32_t( size.getWidth() );
		getData().rect.bottom = getData().rect.top + int32_t( size.getHeight() );
	}

	Rectangle::Rectangle( int32_t iLeft, int32_t iTop, int32_t iRight, int32_t iBottom )
		: Coords4i{ getData().buffer.data() }
	{
		getData().rect.left = iLeft;
		getData().rect.top = iTop;
		getData().rect.right = iRight;
		getData().rect.bottom = iBottom;
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
		getData().rect.left	= iLeft;
		getData().rect.top		= iTop;
		getData().rect.right	= iRight;
		getData().rect.bottom	= iBottom;
	}

	void Rectangle::size( Size & result )const
	{
		result.set( uint32_t( abs( getWidth() ) )
			, uint32_t( abs( getHeight() ) ) );
	}
}
