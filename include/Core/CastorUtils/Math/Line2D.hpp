/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_LINE_2D_H___
#define ___CASTOR_LINE_2D_H___

#include "CastorUtils/Math/MathModule.hpp"

namespace castor
{
	template< typename T >
	class Line2D
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	xA, yA	The coordinates of point A of the line
		 *\param[in]	xB, yB	The coordinates of point B of the line
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	xA, yA	Les coordonnées du point A de la ligne
		 *\param[in]	xB, yB	Les coordonnées du point B de la ligne
		 */
		Line2D( T xA, T yA, T xB, T yB )
			: a( ( yA - yB ) / ( xA - xB ) )
			, b( yB - a * xB )
		{
		}
		/**
		 *\~english
		 *\brief		Computes intersection between this line and another one
		 *\param[in]	line	The other line
		 *\param[out]	x, y	Receive the intersection point coordinates
		 *\return		\p true if an intersection exists
		 *\~french
		 *\brief		Calcule l'intersection entre cette ligne et l'autre
		 *\param[in]	line	L'autre ligne
		 *\param[out]	x, y	Reçoivent les coordonnées du point d'intersection
		 *\return		\p true si une intersection existe
		 */
		bool intersects( Line2D const & line, T & x, T & y )
		{
			bool result = false;

			if ( a == line.a )
			{
				x = ( line.b - b ) / ( a - line.a );
				y = a * x + b;
				result = true;
			}

			return result;
		}

	public:
		//!\~english	The slope.
		//!\~french		La pente
		T a;
		//!\~english	The offset.
		//!\~french		L'offset.
		T b;
	};
}

#endif
