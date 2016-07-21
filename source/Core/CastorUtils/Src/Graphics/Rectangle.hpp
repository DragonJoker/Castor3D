/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___Rectangle___
#define ___Rectangle___

#include "Graphics/Position.hpp"
#include "Graphics/Size.hpp"

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		Intersection types
	\remark		Enumerates the different intersection types between 2 rectangles
	\~french
	\brief		Types d'intersection
	\remark		Enumère les différents types d'intersection entre 2 rectangles
	*/
	typedef enum eINTERSECTION
		: uint8_t
	{
		//!\~english Completely inside	\~french Complètement à l'intérieur
		eINTERSECTION_IN,
		//!\~english Completely outside	\~french Complètement à l'extérieur
		eINTERSECTION_OUT,
		//!\~english Intersection	\~french Intersection
		eINTERSECTION_INTERSECT,
		CASTOR_ENUM_BOUNDS( eINTERSECTION, eINTERSECTION_IN )
	}	eINTERSECTION;
	//!
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		Rectangle class
	\remark		Rectangle class, inherits from Point, holds the intersection functions and specific accessors
	\~french
	\brief		Classe représentant un rectangle
	\remark		Dérive de Point, gère les intersections entre rectangles et les accesseurs spécifiques
	*/
	class Rectangle
		: public Coords4i
	{
	private:
		union
		{
			struct
			{
				int32_t left;
				int32_t top;
				int32_t right;
				int32_t bottom;
			}		rect;
			int32_t	buffer[4];
		}	m_data;

	public :
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_ptOrigin	Position
		 *\param[in]	p_size	Size
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_ptOrigin	Position
		 *\param[in]	p_size	Dimensions
		 */
		CU_API Rectangle( Position const & p_ptOrigin = Position(), Size const & p_size = Size() );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_iLeft, p_iTop		Top left point
		 *\param[in]	p_iRight, p_iBottom	Right bottom point
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_iLeft, p_iTop		Point haut gauche
		 *\param[in]	p_iRight, p_iBottom	Point bas droit
		 */
		CU_API Rectangle( int32_t p_iLeft, int32_t p_iTop, int32_t p_iRight, int32_t p_iBottom );
		/**
		 *\~english
		 *\brief		Copy constructor
		 *\param[in]	p_rhs	The other object
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_rhs	l'autre object
		 */
		CU_API Rectangle( Rectangle const & p_rhs );
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_rhs	The other object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_rhs	l'autre object
		 */
		CU_API Rectangle & operator=( Rectangle const & p_rhs );
		/**
		 *\~english
		 *\brief		Test if the givent point is onto or into this rectangle
		 *\param[in]	p_ptPoint	The point to test
		 *\return		\p eINTERSECTION_IN if onto or into, \p eINTERSECTION_OUT if not
		 *\~french
		 *\brief		Teste si le point donné est sur ou dans ce rectangle
		 *\param[in]	p_ptPoint	Le point à tester
		 *\return		\p eINTERSECTION_IN si sur ou dedans, \p eINTERSECTION_OUT sinon
		 */
		CU_API eINTERSECTION intersects( Position const & p_ptPoint )const;
		/**
		 *\~english
		 *\brief		Test if the givent rectangle intersects this rectangle
		 *\param[in]	p_rcRect	The rectangle to test
		 *\return		The intersection type between the 2 rectangles
		 *\~french
		 *\brief		Teste si le rectangle donné intersecte ce rectangle
		 *\param[in]	p_rcRect	Le rectangle à tester
		 *\return		Le type d'intersection entre les 2 rectangles
		 */
		CU_API eINTERSECTION intersects( Rectangle const & p_rcRect )const;
		/**
		 *\~english
		 *\brief		Sets the rectangle values
		 *\param[in]	p_iLeft, p_iTop		Top left point
		 *\param[in]	p_iRight, p_iBottom	Right bottom point
		 *\~french
		 *\brief		Définit le rectangle
		 *\param[in]	p_iLeft, p_iTop		Point haut gauche
		 *\param[in]	p_iRight, p_iBottom	Point bas droit
		 */
		CU_API void set( int32_t p_iLeft, int32_t p_iTop, int32_t p_iRight, int32_t p_iBottom );
		/**
		 *\~english
		 *\brief		Retrieves the rectangle size
		 *\param[out]	p_result	The rectangle size
		 *\~french
		 *\brief		Récupère les dimensions du rectangle
		 *\param[out]	p_result	Les dimensions du rectangle
		 */
		CU_API void size( Size & p_result )const;
		/**
		 *\~english
		 *\brief		Retrieves the left coordinate
		 *\return		The rectangle's left coordinate
		 *\~french
		 *\brief		Récupère la coordonnée gauche
		 *\return		La coordonnée gauche du rectangle
		 */
		inline int left()const
		{
			return m_data.rect.left;
		}
		/**
		 *\~english
		 *\brief		Retrieves the left coordinate
		 *\return		The rectangle's left coordinate
		 *\~french
		 *\brief		Récupère la coordonnée gauche
		 *\return		La coordonnée gauche du rectangle
		 */
		inline int & left()
		{
			return m_data.rect.left;
		}
		/**
		 *\~english
		 *\brief		Retrieves the right coordinate
		 *\return		The rectangle's right coordinate
		 *\~french
		 *\brief		Récupère la coordonnée droite
		 *\return		La coordonnée droite du rectangle
		 */
		inline int right()const
		{
			return m_data.rect.right;
		}
		/**
		 *\~english
		 *\brief		Retrieves the right coordinate
		 *\return		The rectangle's right coordinate
		 *\~french
		 *\brief		Récupère la coordonnée droite
		 *\return		La coordonnée droite du rectangle
		 */
		inline int & right()
		{
			return m_data.rect.right;
		}
		/**
		 *\~english
		 *\brief		Retrieves the top coordinate
		 *\return		The rectangle's top coordinate
		 *\~french
		 *\brief		Récupère la coordonnée haute
		 *\return		La coordonnée haute du rectangle
		 */
		inline int top()const
		{
			return m_data.rect.top;
		}
		/**
		 *\~english
		 *\brief		Retrieves the top coordinate
		 *\return		The rectangle's top coordinate
		 *\~french
		 *\brief		Récupère la coordonnée haute
		 *\return		La coordonnée haute du rectangle
		 */
		inline int & top()
		{
			return m_data.rect.top;
		}
		/**
		 *\~english
		 *\brief		Retrieves the bottom coordinate
		 *\return		The rectangle's bottom coordinate
		 *\~french
		 *\brief		Récupère la coordonnée basse
		 *\return		La coordonnée basse du rectangle
		 */
		inline int bottom()const
		{
			return m_data.rect.bottom;
		}
		/**
		 *\~english
		 *\brief		Retrieves the bottom coordinate
		 *\return		The rectangle's bottom coordinate
		 *\~french
		 *\brief		Récupère la coordonnée basse
		 *\return		La coordonnée basse du rectangle
		 */
		inline int & bottom()
		{
			return m_data.rect.bottom;
		}
		/**
		 *\~english
		 *\brief		Retrieves the width
		 *\return		The rectangle's width
		 *\~french
		 *\brief		Récupère la largeur
		 *\return		La largeur du rectangle
		 */
		inline int width()const
		{
			return m_data.rect.right - m_data.rect.left;
		}
		/**
		 *\~english
		 *\brief		Retrieves the height
		 *\return		The rectangle's height
		 *\~french
		 *\brief		Récupère la hauteur
		 *\return		La hauteur du rectangle
		 */
		inline int height()const
		{
			return m_data.rect.bottom - m_data.rect.top;
		}
	};
}

#endif
