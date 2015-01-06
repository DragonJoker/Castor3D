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
#ifndef ___CASTOR_SIZE_H___
#define ___CASTOR_SIZE_H___

#include "Coords.hpp"

namespace Castor
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		20/02/2013
	\version	0.7.0
	\~english
	\brief		Size class
	\remark		Kind of specialisation of Coords< 2, uint32_t >
	\~french
	\brief		Classe de taille
	\remark		Sorte de spécialisation de Coords< 2, uint32_t >
	*/
	class Size
		: private Coords< uint32_t, 2 >
	{
	private:
		typedef Coords< uint32_t, 2 > BaseType;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_uiWidth, p_uiHeight	The dimensions
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_uiWidth, p_uiHeight	Les dimensions
		 */
		CU_API Size( uint32_t p_uiWidth = 0, uint32_t p_uiHeight = 0 );
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	p_obj	The object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_obj	L'objet à copier
		 */
		CU_API Size( Size const & p_obj );
		/**
		 *\~english
		 *\brief		Move assignment Constructor
		 *\param[in]	p_obj	The object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_obj	L'objet à déplacer
		 */
		CU_API Size( Size && p_obj );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		CU_API ~Size();
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
		CU_API Size & operator =( Size const & p_obj );
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
		CU_API Size & operator =( Size && p_obj );
		/**
		 *\~english
		 *\brief		Sets the size values
		 *\param[in]	p_uiWidth, p_uiHeight	The dimensions
		 *\~french
		 *\brief		Définit la taille
		 *\param[in]	p_uiWidth, p_uiHeight	Les dimensions
		 */
		CU_API void set( uint32_t p_uiWidth, uint32_t p_uiHeight );
		/**
		 *\~english
		 *\brief		Retrieves the width
		 *\return		The width
		 *\~french
		 *\brief		Récupère la largeur
		 *\return		La largeur
		 */
		inline uint32_t width()const
		{
			return m_data.size.cx;
		}
		/**
		 *\~english
		 *\brief		Retrieves the width
		 *\return		The width
		 *\~french
		 *\brief		Récupère la largeur
		 *\return		La largeur
		 */
		inline uint32_t & width()
		{
			return m_data.size.cx;
		}
		/**
		 *\~english
		 *\brief		Retrieves the height
		 *\return		The height
		 *\~french
		 *\brief		Récupère la hauteur
		 *\return		La hauteur
		 */
		inline uint32_t height()const
		{
			return m_data.size.cy;
		}
		/**
		 *\~english
		 *\brief		Retrieves the height
		 *\return		The height
		 *\~french
		 *\brief		Récupère la hauteur
		 *\return		La hauteur
		 */
		inline uint32_t & height()
		{
			return m_data.size.cy;
		}
		/**
		 *\~english
		 *\brief		Modifies the size
		 *\remark		If width+cx < 0 (or height+cy < 0) then width=0 (respectively height=0)
		 *\param[in]	p_cx, p_cy	The size modifications
		 *\~french
		 *\brief		Modifie la taille
		 *\remark		Si width+cx < 0 (ou height+cy < 0) alors width=0 (respectivement height=0)
		 *\param[in]	p_cx, p_cy	Les valeurs de modification
		 */
		CU_API void grow( int32_t p_cx, int32_t p_cy );

		using BaseType::ptr;
		using BaseType::const_ptr;

	private:
		union
		{
			struct
			{
				uint32_t cx;
				uint32_t cy;
			}			size;
			uint32_t	buffer[2];
		}	m_data;
	};
	/**
	 *\~english
	 *\brief		Equality operator
	 *\param[in]	p_a, p_b	The sizes to compare
	 *\return		\p true if sizes have same dimensions
	 *\~french
	 *\brief		Opérateur d'égalité
	 *\param[in]	p_a, p_b	Les tailles à comparer
	 *\return		\p true si les tailles ont les mêmes dimensions
	 */
	CU_API bool operator ==( Size const & p_a, Size const & p_b );
	/**
	 *\~english
	 *\brief		Difference operator
	 *\param[in]	p_a, p_b	The sizes to compare
	 *\return		\p false if sizes have same dimensions
	 *\~french
	 *\brief		Opérateur de différence
	 *\param[in]	p_a, p_b	Les tailles à comparer
	 *\return		\p false si les tailles ont les mêmes dimensions
	 */
	CU_API bool operator !=( Size const & p_a, Size const & p_b );
}

#endif
