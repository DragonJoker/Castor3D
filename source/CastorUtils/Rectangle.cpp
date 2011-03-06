#include "CastorUtils/PrecompiledHeader.h"

#include "CastorUtils/Rectangle.h"

using namespace Castor;
using namespace Castor::Math;

//*************************************************************************************************

Rectangle :: Rectangle( const Point2i & p_ptStart, const Point2i & p_ptSize)
	:	Point4i( p_ptStart[0], p_ptStart[1], p_ptStart[0] + p_ptSize[0], p_ptStart[1] + p_ptSize[1])
{

}

Rectangle :: Rectangle( int p_iLeft, int p_iTop, int p_iWidth, int p_iHeight)
	:	Point4i( p_iLeft, p_iTop, p_iLeft + p_iWidth, p_iTop + p_iHeight)
{

}

void Rectangle :: Set( int p_iLeft, int p_iTop, int p_iWidth, int p_iHeight)
{
	at( 0) = p_iLeft;
	at( 1) = p_iTop;
	at( 2) = p_iLeft + p_iWidth;
	at( 3) = p_iTop + p_iHeight;
}

eINTERSECTION Rectangle :: Intersects( const Point2i & p_ptPoint)const
{
	eINTERSECTION l_eReturn = eIntersectionOut;

	if ((p_ptPoint[0] >= at( 0)) && (p_ptPoint[1] >= at( 1)) && (p_ptPoint[0] <= at( 2)) && (p_ptPoint[1] <= at( 3)))
	{
		l_eReturn = eIntersectionIn;
	}

	return l_eReturn;
}

eINTERSECTION Rectangle :: Intersects( const Rectangle & p_rcRect) const
{
	// Calcul du rectangle d'intersection
	Point2i l_ptStart( std::max( at( 0), p_rcRect.at( 0)), std::max( at( 1), p_rcRect.at( 1)));
	Point2i l_ptEnd( std::min( at( 2), p_rcRect.at( 2)), std::min( at( 3), p_rcRect.at( 3)));
	Rectangle l_rcOverlap( l_ptStart, l_ptEnd - l_ptStart);
	eINTERSECTION l_eReturn = eIntersectionIntersect;

	if ((l_ptStart[0] > at( 2)) || (l_ptStart[1] > at( 3)))
	{
		l_eReturn = eIntersectionOut;
	}
	else if ((l_rcOverlap == * this) || (l_rcOverlap == p_rcRect))
	{
		l_eReturn = eIntersectionIn;
	}

	return l_eReturn;
}

bool Rectangle :: operator ==( const Rectangle & p_rcRect) const
{
	return Point4i::operator ==( p_rcRect);
}

bool Rectangle :: operator !=( const Rectangle & p_rcRect) const
{
	return Point4i::operator !=( p_rcRect);
}

//*************************************************************************************************