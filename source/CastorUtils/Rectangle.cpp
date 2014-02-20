#include "CastorUtils/PrecompiledHeader.hpp"

#include "CastorUtils/Rectangle.hpp"

using namespace Castor;

//*************************************************************************************************

Rect :: Rect( Position const & p_ptStart, Size const & p_ptSize )
	:	Coords4i( m_data.buffer )
{
	m_data.rect.left	= p_ptStart.x();
	m_data.rect.top		= p_ptStart.y();
	m_data.rect.right	= m_data.rect.left + int32_t( p_ptSize.width() );
	m_data.rect.bottom	= m_data.rect.top + int32_t( p_ptSize.height() );
}

Rect :: Rect( int32_t p_iLeft, int32_t p_iTop, int32_t p_iRight, int32_t p_iBottom )
	:	Coords4i( m_data.buffer )
{
	m_data.rect.left	= p_iLeft;
	m_data.rect.top		= p_iTop;
	m_data.rect.right	= p_iRight;
	m_data.rect.bottom	= p_iBottom;
}

eINTERSECTION Rect :: intersects( Point2i const & p_ptPoint )const
{
	eINTERSECTION l_eReturn = eINTERSECTION_OUT;

	if( (p_ptPoint[0] >= left()) && (p_ptPoint[1] >= top()) && (p_ptPoint[0] <= right()) && (p_ptPoint[1] <= bottom()) )
	{
		l_eReturn = eINTERSECTION_IN;
	}

	return l_eReturn;
}

eINTERSECTION Rect :: intersects( Rect const & p_rcRect ) const
{
	// Calcul du rectangle d'intersection
	Point2i l_ptStart( std::max( left(), p_rcRect.left() ), std::max( top(), p_rcRect.top() ) );
	Point2i l_ptEnd( std::min( right(), p_rcRect.right() ), std::min( bottom(), p_rcRect.bottom() ) );
	Rect l_rcOverlap( l_ptStart[0], l_ptStart[1], l_ptEnd[0], l_ptEnd[1] );
	eINTERSECTION l_eReturn = eINTERSECTION_INTERSECT;

	if( (l_ptStart[0] > right()) || (l_ptStart[1] > bottom()) )
	{
		l_eReturn = eINTERSECTION_OUT;
	}
	else if( (l_rcOverlap == * this) || (l_rcOverlap == p_rcRect) )
	{
		l_eReturn = eINTERSECTION_IN;
	}

	return l_eReturn;
}

void Rect :: set( int32_t p_iLeft, int32_t p_iTop, int32_t p_iRight, int32_t p_iBottom )
{
	m_data.rect.left	= p_iLeft;
	m_data.rect.top		= p_iTop;
	m_data.rect.right	= p_iRight;
	m_data.rect.bottom	= p_iBottom;
}

void Rect :: size( Size & p_ptResult )const
{
	p_ptResult.set( width(), height() );
}

//*************************************************************************************************
/*
bool operator ==( Rectangle const & p_rcA, Rectangle const & p_rcB)
{
	return ::operator ==( p_rcA, p_rcB);
}

bool operator !=( Rectangle const & p_rcA, Rectangle const & p_rcB)
{
	return ::operator !=( p_rcA, p_rcB);
}
*/
//*************************************************************************************************
