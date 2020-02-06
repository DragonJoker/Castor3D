﻿/*
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
#ifndef ___CASTOR_COLLECTION_H___
#define ___CASTOR_COLLECTION_H___

#include "MultiThreadConfig.hpp"
#include "SmartPtr.hpp"
#include "Templates.hpp"
#include "NonCopyable.hpp"
#include "Assertion.hpp"

#pragma warning( push )
#pragma warning( disable:4661 )

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		Element collection class
	\remark		A collection class, allowing you to store named objects, removing, finding or adding them as you wish.
	\~french
	\brief		Classe de collection d'éléments
	\remark		Une classe de collection, permettant de stocker des éléments nommés, les enlever, les rechercher.
	*/
	template< typename TObj, typename TKey >
	class Collection
		: public Castor::NonCopyable
	{
	public:
		DECLARE_SMART_PTR( TObj );
		typedef std::map< TKey, TObjSPtr > TObjPtrMap;
		typedef typename TObjPtrMap::iterator TObjPtrMapIt;
		typedef typename TObjPtrMap::const_iterator TObjPtrMapConstIt;
		typedef typename TObjPtrMap::reverse_iterator TObjPtrMapRIt;
		typedef typename TObjPtrMap::const_reverse_iterator TObjPtrMapConstRIt;
		typedef typename TObjPtrMap::value_type value_type;
		typedef typename call_traits< TKey >::const_param_type key_param_type;	//!\~english Typedef over the key param type	\~french Typedef sur le type de la clef en paramètre de fonction

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		Collection();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~Collection();
		/**
		 *\~english
		 *\brief		Locks the collection mutex
		 *\~french
		 *\brief		Locke le mutex de la collection
		 */
		inline void lock()const;
		/**
		 *\~english
		 *\brief		Unlocks the collection mutex
		 *\~french
		 *\brief		Délocke le mutex de la collection
		 */
		inline void unlock()const;
		/**
		 *\~english
		 *\brief		Returns an iterator to the first element of the collection
		 *\return		The iterator
		 *\~french
		 *\brief		Renvoie un itérateur sur le premier élément de la collection
		 *\return		L'itérateur
		 */
		inline TObjPtrMapIt begin();
		/**
		 *\~english
		 *\brief		Returns an constant iterator to the first element of the collection
		 *\return		The iterator
		 *\~french
		 *\brief		Renvoie un itérateur constant sur le premier élément de la collection
		 *\return		L'itérateur
		 */
		inline TObjPtrMapConstIt begin()const;
		/**
		 *\~english
		 *\brief		Returns an iterator to the after last element of the collection
		 *\return		The iterator
		 *\~french
		 *\brief		Renvoie un itérateur sur l'après dernier élément de la collection
		 *\return		L'itérateur
		 */
		inline TObjPtrMapIt end();
		/**
		 *\~english
		 *\brief		Returns an constant iterator to the after last element of the collection
		 *\return		The iterator
		 *\~french
		 *\brief		Renvoie un itérateur constant sur l'après dernier élément de la collection
		 *\return		L'itérateur
		 */
		inline TObjPtrMapConstIt end()const;
		/**
		 *\~english
		 *\brief		Clears the collection
		 *\~french
		 *\brief		Vide la collection
		 */
		inline void clear()throw();
		/**
		 *\~english
		 *\brief		Looks for a collected object at the given key
		 *\param[in]	p_key	The key
		 *\return		The found object if any, \p null_ptr if none
		 *\~french
		 *\brief		Recherche dans la collection un objet situé à la clef donnée
		 *\param[in]	p_key	La clef
		 *\return		L'objet trouvé s'il existe, \p null_ptr sinon
		 */
		inline TObjSPtr find( key_param_type p_key )const;
		/**
		 *\~english
		 *\brief		Gives the collected objects count
		 *\return		The count
		 *\~french
		 *\brief		Donne le nombre d'objets de la collection
		 *\return		Le compte
		 */
		inline std::size_t size()const;
		/**
		 *\~english
		 *\brief		Inserts an element at the given key into the collection
		 *\param[in]	p_key		The key at which element is inserted
		 *\param[in]	p_element	The element to insert
		 *\return		\p false if there is already one object at the given key
		 *\~french
		 *\brief		Insère un élément à la clef donnée
		 *\param[in]	p_key		La clef à laquelle l'élément sera associé
		 *\param[in]	p_element	L'élément à insérer
		 *\return		\p false si un élément est déjà associé à la clef
		 */
		inline bool insert( key_param_type p_key, TObjSPtr p_element );
		/**
		 *\~english
		 *\brief		Tests if there is an element associated to the key
		 *\param[in]	p_key	The key to test
		 *\return		The test result
		 *\~french
		 *\brief		Teste si un élément est associé à la clef donnée
		 *\param[in]	p_key	La clef à tester
		 *\return		Le résultat du test
		 */
		inline bool has( key_param_type p_key )const;
		/**
		 *\~english
		 *\brief		Removes the element associated to the given key from the collection
		 *\param[in]	p_key	The key
		 *\return		The associated element, nullptr if none
		 *\~french
		 *\brief		Enlève de la collection l'élément associé à la clef donnée
		 *\param[in]	p_key	La clef
		 *\return		L'élément associé, null_ptr sinon
		 */
		inline TObjSPtr erase( key_param_type p_key );

	private:
		TObjPtrMap 						m_objectMap;
		mutable std::recursive_mutex 	m_mutex;
		mutable bool 					m_bLocked;
	};

#include "Collection.inl"
}

#pragma warning( pop )

#endif
