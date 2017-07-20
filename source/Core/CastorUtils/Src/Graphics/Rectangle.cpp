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
		Intersection eReturn = Intersection::eOut;

		if ( ( p_point.x() >= left() ) && ( p_point.y() >= top() ) && ( p_point.x() <= right() ) && ( p_point.y() <= bottom() ) )
		{
			eReturn = Intersection::eIn;
		}

		return eReturn;
	}

	Intersection Rectangle::intersects( Rectangle const & p_rcRect ) const
	{
		// Calcul du rectangle d'intersection
		Point2i ptStart( std::max( left(), p_rcRect.left() ), std::max( top(), p_rcRect.top() ) );
		Point2i ptEnd( std::min( right(), p_rcRect.right() ), std::min( bottom(), p_rcRect.bottom() ) );
		Rectangle rcOverlap( ptStart[0], ptStart[1], ptEnd[0], ptEnd[1] );
		Intersection eReturn = Intersection::eIntersect;

		if ( ( ptStart[0] > right() ) || ( ptStart[1] > bottom() ) )
		{
			eReturn = Intersection::eOut;
		}
		else if ( ( rcOverlap == * this ) || ( rcOverlap == p_rcRect ) )
		{
			eReturn = Intersection::eIn;
		}

		return eReturn;
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
