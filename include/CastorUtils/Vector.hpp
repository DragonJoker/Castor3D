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
#ifndef ___Castor_Vector___
#define ___Castor_Vector___

#include "SmartPtr.hpp"
#include <vector>
#include <typeinfo>

namespace Castor
{
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		std::vector helper functions
	\~french
	\brief		Fonctions d'aide pour std::vector
	*/
	class vector
	{
	public:
		/**
		 *\~english
		 *\brief		Tells if the vector contains a given element
		 *\remark		Uses std::find
		 *\param[in]	p_vector	The vector
		 *\param[in]	p_tValue	The value to look for
		 *\return		\p true if p_tValue is in p_vector, \p false if not
		 *\~french
		 *\brief		Dit si le vecteur contient l'élément donné
		 *\remark		Utilise std::find
		 *\param[in]	p_vector	Le vecteur
		 *\param[in]	p_tValue	La valeur à rechercher
		 *\return		\p true si p_tValue est dans p_vector, \p false sinon
		 */
		template <typename T>
		static inline bool has( std::vector< T > const & p_vector, T const & p_tValue )
		{
			return (std::find( p_vector.begin(), p_vector.end(), p_tValue ) != p_vector.end());
		}
		/**
		 *\~english
		 *\brief		Gets the value at given index from a vector
		 *\param[in]	p_vector	The vector
		 *\param[in]	p_uiIndex	The value index
		 *\return		A pointer to the found value, \p nullptr if not found
		 *\~french
		 *\brief		Dit si le vecteur contient l'élément donné
		 *\param[in]	p_vector	Le vecteur
		 *\param[in]	p_uiIndex	L'index de la valeur
		 *\return		Un pointeur sur la valeur trouvée, \p nullptr si non-trouvée
		 */
		template <typename T>
		static inline T * get_or_null( std::vector< T * > const & p_vector, std::size_t p_uiIndex )
		{
			T * pReturn = nullptr;

			if( p_uiIndex < p_vector.size() )
			{
				pReturn = p_vector[p_uiIndex];
			}

			return pReturn;
		}
		/**
		 *\~english
		 *\brief		Erases the given value from the vector
		 *\remark		Uses std::remove
		 *\param[in]	p_vector	The vector
		 *\param[in]	p_tKey		The value to look for
		 *\return		\p true if The value was removed, \p false if not
		 *\~french
		 *\brief		Efface la valeur donnée du vecteur
		 *\remark		Utilise std::remove
		 *\param[in]	p_vector	Le vecteur
		 *\param[in]	p_tKey		La valeur à rechercher
		 *\return		\p true si la valeur a été enlevée, \p false sinon
		 */
		template <typename T>
		static inline bool erase_at( std::vector< T > & p_vector, T const & p_tKey )
		{
			bool l_bReturn = false;
			typename std::vector< T >::iterator l_it = std::remove( p_vector.begin(), p_vector.end(), p_tKey );

			if( l_it != p_vector.end() )
			{
				l_bReturn = true;
				p_vector.erase( l_it, p_vector.end() );
			}

			return l_bReturn;
		}
		/**
		 *\~english
		 *\brief		Erases the value at the given index from the vector
		 *\param[in]	p_vector	The vector
		 *\param[in]	p_uiIndex	The value index
		 *\param[out]	p_tResult	Receives the value removed from the vector
		 *\return		\p true if The value was removed, \p false if not
		 *\~french
		 *\brief		Efface la valeur à l'index donné du vecteur
		 *\param[in]	p_vector	Le vecteur
		 *\param[in]	p_uiIndex	L'index de la valeur
		 *\param[out]	p_tResult	Reçoit la valeur enlevée du vecteur
		 *\return		\p true si la valeur a été enlevée, \p false sinon
		 */
		template <typename T>
		static inline bool erase_at( std::vector< T > & p_vector, std::size_t p_uiIndex, T & p_tResult )
		{
			bool l_bReturn = false;

			if( p_uiIndex < p_vector.size() )
			{
				l_bReturn = true;
				p_tResult = p_vector[p_uiIndex];
				p_vector.erase( p_vector.begin() + p_uiIndex );
			}

			return l_bReturn;
		}
		/**
		 *\~english
		 *\brief		Erases the first value from the vector
		 *\remark		Uses vector::erase_at
		 *\param[in]	p_vector	The vector
		 *\param[out]	p_tResult	Receives the value removed from the vector
		 *\return		\p true if The value was removed, \p false if not
		 *\~french
		 *\brief		Efface la première valeur du vecteur
		 *\remark		Uses vector::erase_at
		 *\param[in]	p_vector	Le vecteur
		 *\param[out]	p_tResult	Reçoit la valeur enlevée du vecteur
		 *\return		\p true si la valeur a été enlevée, \p false sinon
		 */
		template <typename T>
		static inline bool erase_first( std::vector< T > & p_vector, T & p_tResult )
		{
			return vector::erase_at( p_vector, 0, p_tResult );
		}
		/**
		 *\~english
		 *\brief		Erases the last value from the vector
		 *\remark		Uses vector::erase_at
		 *\param[in]	p_vector	The vector
		 *\param[out]	p_tResult	Receives the value removed from the vector
		 *\return		\p true if The value was removed, \p false if not
		 *\~french
		 *\brief		Efface la dernière valeur du vecteur
		 *\remark		Uses vector::erase_at
		 *\param[in]	p_vector	Le vecteur
		 *\param[out]	p_tResult	Reçoit la valeur enlevée du vecteur
		 *\return		\p true si la valeur a été enlevée, \p false sinon
		 */
		template <typename T>
		static inline bool erase_last( std::vector< T > & p_vector, T & p_tResult)
		{
			return vector::erase_at( p_vector, p_vector.size() - 1, p_tResult );
		}
		/**
		 *\~english
		 *\brief		Erases and deletes the given value from the vector
		 *\remark		Uses vector::erase_at
		 *\param[in]	p_vector	The vector
		 *\param[in]	p_pKey		The value to look for
		 *\return		\p true if The value was removed, \p false if not
		 *\~french
		 *\brief		Efface et désalloue la valeur donnée du vecteur
		 *\remark		Utilise vector::erase_at
		 *\param[in]	p_vector	Le vecteur
		 *\param[in]	p_pKey		La valeur à rechercher
		 *\return		\p true si la valeur a été enlevée, \p false sinon
		 */
		template <typename T>
		static inline bool delete_at( std::vector< T * > & p_vector, T * p_pKey )
		{
			bool bReturn = vector::erase_at( p_vector, p_pKey );

			if( bReturn )
			{
				delete p_pKey;
			}

			return bReturn;
		}
		/**
		 *\~english
		 *\brief		Erases and deletes the value at the given index from the vector
		 *\remark		Uses vector::erase_at
		 *\param[in]	p_vector	The vector
		 *\param[in]	p_uiIndex	The value index
		 *\return		\p true if The value was removed, \p false if not
		 *\~french
		 *\brief		Efface et désalloue la valeur à l'index donné du vecteur
		 *\remark		Uses vector::erase_at
		 *\param[in]	p_vector	Le vecteur
		 *\param[in]	p_uiIndex	L'index de la valeur
		 *\return		\p true si la valeur a été enlevée, \p false sinon
		 */
		template <typename T>
		static inline bool delete_at( std::vector< T * > & p_vector, std::size_t p_uiIndex )
		{
			T * l_pResult;
			bool bReturn = erase_at( p_vector, p_uiIndex, l_pResult );

			if (bReturn)
			{
				delete l_pResult;
			}

			return bReturn;
		}
		/**
		 *\~english
		 *\brief		Erases and deletes the first value from the vector
		 *\remark		Uses vector::delete_at
		 *\param[in]	p_vector	The vector
		 *\return		\p true if The value was removed, \p false if not
		 *\~french
		 *\brief		Efface et désalloue la première valeur du vecteur
		 *\remark		Uses vector::delete_at
		 *\param[in]	p_vector	Le vecteur
		 *\return		\p true si la valeur a été enlevée, \p false sinon
		 */
		template <typename T>
		static inline bool delete_first( std::vector< T * > & p_vector )
		{
			return vector::delete_at( p_vector, 0 );
		}
		/**
		 *\~english
		 *\brief		Erases and deletes the last value from the vector
		 *\remark		Uses vector::delete_at
		 *\param[in]	p_vector	The vector
		 *\return		\p true if The value was removed, \p false if not
		 *\~french
		 *\brief		Efface et désalloue la dernière valeur du vecteur
		 *\remark		Uses vector::delete_at
		 *\param[in]	p_vector	Le vecteur
		 *\return		\p true si la valeur a été enlevée, \p false sinon
		 */
		template <typename T>
		static inline bool delete_last( std::vector< T * > & p_vector )
		{
			return vector::delete_at( p_vector, p_vector.size() - 1 );
		}
	};
}

#endif
