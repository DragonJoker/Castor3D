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
#ifndef ___CASTOR_POSITION_H___
#define ___CASTOR_POSITION_H___

#include "Math/Coords.hpp"

namespace castor
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		20/02/2013
	\version	0.7.0
	\~english
	\brief		Position class
	\remark		Kind of specialisation of Coords< 2, int32_t >
	\~french
	\brief		Classe de position
	\remark		Sorte de spécialisation de Coords< 2, int32_t >
	*/
	class Position
		: public Coords< int32_t, 2 >
	{
	private:
		typedef Coords< int32_t, 2 > BaseType;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_x, p_y	The position
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_x, p_y	La position
		 */
		CU_API Position( int32_t p_x = 0, int32_t p_y = 0 );
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	p_obj	The object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_obj	L'objet à copier
		 */
		CU_API Position( Position const & p_obj );
		/**
		 *\~english
		 *\brief		Move assignment Constructor
		 *\param[in]	p_obj	The object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_obj	L'objet à déplacer
		 */
		CU_API Position( Position && p_obj );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		CU_API ~Position();
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_obj	The object to copy
		 *\return		A reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_obj	L'objet à copier
		 *\return		Une référence sur cet objet
		 */
		CU_API Position & operator=( Position const & p_obj );
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_obj	The object to move
		 *\return		A reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement
		 *\param[in]	p_obj	L'objet à déplacer
		 *\return		Une référence sur cet objet
		 */
		CU_API Position & operator=( Position && p_obj );
		/**
		 *\~english
		 *\brief		sets the position values
		 *\param[in]	p_x, p_y	The position
		 *\~french
		 *\brief		Définit la position
		 *\param[in]	p_x, p_y	La position
		 */
		CU_API void set( int32_t p_x, int32_t p_y );
		/**
		 *\~english
		 *\brief		Offsets the position values
		 *\param[in]	p_x, p_y	The position offsets
		 *\~french
		 *\brief		Déplace la position
		 *\param[in]	p_x, p_y	Les valeurs de déplacement
		 */
		CU_API void offset( int32_t p_x, int32_t p_y );
		/**
		 *\~english
		 *\brief		Retrieves the left coordinate
		 *\return		The rectangle's left coordinate
		 *\~french
		 *\brief		Récupère la coordonnée gauche
		 *\return		La coordonnée gauche du rectangle
		 */
		inline int32_t y()const
		{
			return m_data.position.y;
		}
		/**
		 *\~english
		 *\brief		Retrieves the left coordinate
		 *\return		The rectangle's left coordinate
		 *\~french
		 *\brief		Récupère la coordonnée gauche
		 *\return		La coordonnée gauche du rectangle
		 */
		inline int32_t & y()
		{
			return m_data.position.y;
		}
		/**
		 *\~english
		 *\brief		Retrieves the right coordinate
		 *\return		The rectangle's right coordinate
		 *\~french
		 *\brief		Récupère la coordonnée droite
		 *\return		La coordonnée droite du rectangle
		 */
		inline int32_t x()const
		{
			return m_data.position.x;
		}
		/**
		 *\~english
		 *\brief		Retrieves the right coordinate
		 *\return		The rectangle's right coordinate
		 *\~french
		 *\brief		Récupère la coordonnée droite
		 *\return		La coordonnée droite du rectangle
		 */
		inline int32_t & x()
		{
			return m_data.position.x;
		}

		using BaseType::ptr;
		using BaseType::constPtr;

	private:
		union
		{
			struct
			{
				int32_t x;
				int32_t y;
			} position;
			int32_t buffer[2];
		}	m_data;
	};
	/**
	 *\~english
	 *\brief		Equality operator
	 *\param[in]	p_a, p_b	The positions to compare
	 *\return		\p true if positions have same coordinates
	 *\~french
	 *\brief		Opérateur d'égalité
	 *\param[in]	p_a, p_b	Les positions à comparer
	 *\return		\p true si les positions ont les mêmes coordonnées
	 */
	CU_API bool operator==( Position const & p_a, Position const & p_b );
	/**
	 *\~english
	 *\brief		Difference operator
	 *\param[in]	p_a, p_b	The positions to compare
	 *\return		\p false if positions have same coordinates
	 *\~french
	 *\brief		Opérateur de différence
	 *\param[in]	p_a, p_b	Les positions à comparer
	 *\return		\p false si les positions ont les mêmes coordonnées
	 */
	CU_API bool operator!=( Position const & p_a, Position const & p_b );
}

#endif
