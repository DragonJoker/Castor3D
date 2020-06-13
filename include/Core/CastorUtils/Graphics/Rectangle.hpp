/*
See LICENSE file in root folder
*/
#ifndef ___Rectangle___
#define ___Rectangle___

#include "CastorUtils/Graphics/Position.hpp"
#include "CastorUtils/Graphics/Size.hpp"

namespace castor
{
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
		 *\param[in]	origin	Position
		 *\param[in]	size	Size
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	origin	Position
		 *\param[in]	size	Dimensions
		 */
		CU_API Rectangle( Position const & origin = Position(), Size const & size = Size() );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	left, top		Top left point
		 *\param[in]	right, bottom	Right bottom point
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	left, top		Point haut gauche
		 *\param[in]	right, bottom	Point bas droit
		 */
		CU_API Rectangle( int32_t left, int32_t top, int32_t right, int32_t bottom );
		/**
		 *\~english
		 *\brief		Copy constructor
		 *\param[in]	rhs	The other object
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	rhs	l'autre object
		 */
		CU_API Rectangle( Rectangle const & rhs );
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	rhs	The other object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	rhs	l'autre object
		 */
		CU_API Rectangle & operator=( Rectangle const & rhs );
		/**
		 *\~english
		 *\brief		Test if the givent point is onto or into this rectangle
		 *\param[in]	point	The point to test
		 *\return		\p Intersection::eIn if onto or into, \p Intersection::eOut if not
		 *\~french
		 *\brief		Teste si le point donné est sur ou dans ce rectangle
		 *\param[in]	point	Le point à tester
		 *\return		\p Intersection::eIn si sur ou dedans, \p Intersection::eOut sinon
		 */
		CU_API Intersection intersects( Position const & point )const;
		/**
		 *\~english
		 *\brief		Test if the givent rectangle intersects this rectangle
		 *\param[in]	rect	The rectangle to test
		 *\return		The intersection type between the 2 rectangles
		 *\~french
		 *\brief		Teste si le rectangle donné intersecte ce rectangle
		 *\param[in]	rect	Le rectangle à tester
		 *\return		Le type d'intersection entre les 2 rectangles
		 */
		CU_API Intersection intersects( Rectangle const & rect )const;
		/**
		 *\~english
		 *\brief		sets the rectangle values
		 *\param[in]	left, top		Top left point
		 *\param[in]	right, bottom	Right bottom point
		 *\~french
		 *\brief		Définit le rectangle
		 *\param[in]	left, top		Point haut gauche
		 *\param[in]	right, bottom	Point bas droit
		 */
		CU_API void set( int32_t left, int32_t top, int32_t right, int32_t bottom );
		/**
		 *\~english
		 *\brief		Retrieves the rectangle size
		 *\param[out]	result	The rectangle size
		 *\~french
		 *\brief		Récupère les dimensions du rectangle
		 *\param[out]	result	Les dimensions du rectangle
		 */
		CU_API void size( Size & result )const;
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
