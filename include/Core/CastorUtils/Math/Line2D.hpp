/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_LINE_2D_H___
#define ___CASTOR_LINE_2D_H___

#include "CastorUtils/CastorUtils.hpp"

namespace castor
{
	/*!
	\author		Sylvain DOREMUS
	\date		14/08/2010
	\~english
	\brief		2D line equation
	\remark		do you remember y = ax + b ?
	\~french
	\brief		Equation d'une ligne 2D
	\remark		Vous connaissez y = ax + b ?
	*/
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
		 *\brief		Copy Constructor
		 *\param[in]	line	The Line2D object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	line	L'objet Line2D à copier
		 */
		Line2D( Line2D const & line )
			: a( line.a )
			, b( line.b )
		{
		}
		/**
		 *\~english
		 *\brief		Move Constructor
		 *\param[in]	line	The Line2D object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	line	L'objet Line2D à déplacer
		 */
		Line2D( Line2D && line )
			: a( std::move( line.a ) )
			, b( std::move( line.b ) )
		{
			line.a = T{};
			line.b = T{};
		}
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	line	The Line2D object to copy
		 *\return		A reference to this Line2D object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	line	L'objet Line2D à copier
		 *\return		Une référence sur cet objet Line2D
		 */
		Line2D & operator=( Line2D const & line )
		{
			a = line.a;
			b = line.b;
			return * this;
		}
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	line	The Line2D object to move
		 *\return		A reference to this Line2D object
		 *\~french
		 *\brief		Opérateur d'affectation  par déplacement
		 *\param[in]	line	L'objet Line2D à déplacer
		 *\return		Une référence sur cet objet Line2D
		 */
		Line2D operator=( Line2D && line )
		{
			if ( this != &line )
			{
				a = std::move( line.a );
				b = std::move( line.b );
				line.a = T{};
				line.b = T{};
			}

			return * this;
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
