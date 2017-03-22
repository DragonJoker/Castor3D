#include "Rectangle.hpp"

namespace Castor
{
	Rectangle::Rectangle( Position const & p_ptStart, Size const & p_size )
		: Coords4i( m_data.buffer )
	{
		m_data.rect.left = p_ptStart.x();
		m_data.rect.top = p_ptStart.y();
		m_data.rect.right = m_data.rect.left + int32_t( p_size.width() );
		m_data.rect.bottom = m_data.rect.top + int32_t( p_size.height() );
	}

	Rectangle::Rectangle( int32_t p_iLeft, int32_t p_iTop, int32_t p_iRight, int32_t p_iBottom )
		: Coords4i( m_data.buffer )
	{
		m_data.rect.left = p_iLeft;
		m_data.rect.top = p_iTop;
		m_data.rect.right = p_iRight;
		m_data.rect.bottom = p_iBottom;
	}

	Rectangle::Rectangle( Rectangle const & p_rhs )
		: Coords4i( m_data.buffer )
	{
		m_data.rect.left = p_rhs.m_data.rect.left;
		m_data.rect.top = p_rhs.m_data.rect.top;
		m_data.rect.right = p_rhs.m_data.rect.right;
		m_data.rect.bottom = p_rhs.m_data.rect.bottom;
	}

	Rectangle & Rectangle::operator=( Rectangle const & p_rhs )
	{
		m_data.rect.left = p_rhs.m_data.rect.left;
		m_data.rect.top = p_rhs.m_data.rect.top;
		m_data.rect.right = p_rhs.m_data.rect.right;
		m_data.rect.bottom = p_rhs.m_data.rect.bottom;
		return *this;
	}

	Intersection Rectangle::intersects( Position const & p_point )const
	{
		Intersection l_eReturn = Intersection::eOut;

		if ( ( p_point.x() >= left() ) && ( p_point.y() >= top() ) && ( p_point.x() <= right() ) && ( p_point.y() <= bottom() ) )
		{
			l_eReturn = Intersection::eIn;
		}

		return l_eReturn;
	}

	Intersection Rectangle::intersects( Rectangle const & p_rcRect ) const
	{
		// Calcul du rectangle d'intersection
		Point2i l_ptStart( std::max( left(), p_rcRect.left() ), std::max( top(), p_rcRect.top() ) );
		Point2i l_ptEnd( std::min( right(), p_rcRect.right() ), std::min( bottom(), p_rcRect.bottom() ) );
		Rectangle l_rcOverlap( l_ptStart[0], l_ptStart[1], l_ptEnd[0], l_ptEnd[1] );
		Intersection l_eReturn = Intersection::eIntersect;

		if ( ( l_ptStart[0] > right() ) || ( l_ptStart[1] > bottom() ) )
		{
			l_eReturn = Intersection::eOut;
		}
		else if ( ( l_rcOverlap == * this ) || ( l_rcOverlap == p_rcRect ) )
		{
			l_eReturn = Intersection::eIn;
		}

		return l_eReturn;
	}

	void Rectangle::set( int32_t p_iLeft, int32_t p_iTop, int32_t p_iRight, int32_t p_iBottom )
	{
		m_data.rect.left	= p_iLeft;
		m_data.rect.top		= p_iTop;
		m_data.rect.right	= p_iRight;
		m_data.rect.bottom	= p_iBottom;
	}

	void Rectangle::size( Size & p_result )const
	{
		p_result.set( width(), height() );
	}
}
