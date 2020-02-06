/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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
#ifndef ___CASTOR_POSITION_H___
#define ___CASTOR_POSITION_H___

#include "Coords.hpp"

namespace Castor
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
	\remark		Sorte de sp�cialisation de Coords< 2, int32_t >
	*/
	class Position
		: private Coords< int32_t, 2 >
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
		Position( int32_t p_x = 0, int32_t p_y = 0 );
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	p_obj	The object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_obj	L'objet � copier
		 */
		Position( Position const & p_obj );
		/**
		 *\~english
		 *\brief		Move assignment Constructor
		 *\param[in]	p_obj	The object to move
		 *\~french
		 *\brief		Constructeur par d�placement
		 *\param[in]	p_obj	L'objet � d�placer
		 */
		Position( Position && p_obj );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~Position();
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_obj	The object to copy
		 *\return		A reference to this object
		 *\~french
		 *\brief		Op�rateur d'affectation par copie
		 *\param[in]	p_obj	L'objet � copier
		 *\return		Une r�f�rence sur cet objet
		 */
		Position & operator =( Position const & p_obj );
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_obj	The object to move
		 *\return		A reference to this object
		 *\~french
		 *\brief		Op�rateur d'affectation par d�placement
		 *\param[in]	p_obj	L'objet � d�placer
		 *\return		Une r�f�rence sur cet objet
		 */
		Position & operator =( Position && p_obj );
		/**
		 *\~english
		 *\brief		Sets the position values
		 *\param[in]	p_x, p_y	The position
		 *\~french
		 *\brief		D�finit la position
		 *\param[in]	p_x, p_y	La position
		 */
		void set( int32_t p_x, int32_t p_y );
		/**
		 *\~english
		 *\brief		Offsets the position values
		 *\param[in]	p_x, p_y	The position offsets
		 *\~french
		 *\brief		D�place la position
		 *\param[in]	p_x, p_y	Les valeurs de d�placement
		 */
		void offset( int32_t p_x, int32_t p_y );
		/**
		 *\~english
		 *\brief		Retrieves the left coordinate
		 *\return		The rectangle's left coordinate
		 *\~french
		 *\brief		R�cup�re la coordonn�e gauche
		 *\return		La coordonn�e gauche du rectangle
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
		 *\brief		R�cup�re la coordonn�e gauche
		 *\return		La coordonn�e gauche du rectangle
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
		 *\brief		R�cup�re la coordonn�e droite
		 *\return		La coordonn�e droite du rectangle
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
		 *\brief		R�cup�re la coordonn�e droite
		 *\return		La coordonn�e droite du rectangle
		 */
		inline int32_t & x()
		{
			return m_data.position.x;
		}

		using BaseType::ptr;
		using BaseType::const_ptr;

	private:
		union
		{
			struct
			{
				int32_t x;
				int32_t y;
			}		position;
			int32_t	buffer[2];
		}	m_data;
	};
	/**
	 *\~english
	 *\brief		Equality operator
	 *\param[in]	p_a, p_b	The positions to compare
	 *\return		\p true if positions have same coordinates
	 *\~french
	 *\brief		Op�rateur d'�galit�
	 *\param[in]	p_a, p_b	Les positions � comparer
	 *\return		\p true si les positions ont les m�mes coordonn�es
	 */
	bool operator ==( Position const & p_a, Position const & p_b );
	/**
	 *\~english
	 *\brief		Difference operator
	 *\param[in]	p_a, p_b	The positions to compare
	 *\return		\p false if positions have same coordinates
	 *\~french
	 *\brief		Op�rateur de diff�rence
	 *\param[in]	p_a, p_b	Les positions � comparer
	 *\return		\p false si les positions ont les m�mes coordonn�es
	 */
	bool operator !=( Position const & p_a, Position const & p_b );
}

#endif
