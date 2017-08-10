/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___Rectangle___
#define ___Rectangle___

#include "Position.hpp"
#include "Size.hpp"

namespace castor
{
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
			} rect;
			int32_t buffer[4];
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
		 *\param[in]	p_point	The point to test
		 *\return		\p Intersection::eIn if onto or into, \p Intersection::eOut if not
		 *\~french
		 *\brief		Teste si le point donné est sur ou dans ce rectangle
		 *\param[in]	p_point	Le point à tester
		 *\return		\p Intersection::eIn si sur ou dedans, \p Intersection::eOut sinon
		 */
		CU_API Intersection intersects( Position const & p_point )const;
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
		CU_API Intersection intersects( Rectangle const & p_rcRect )const;
		/**
		 *\~english
		 *\brief		sets the rectangle values
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
		inline int getWidth()const
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
		inline int getHeight()const
		{
			return m_data.rect.bottom - m_data.rect.top;
		}
	};
}

#endif
