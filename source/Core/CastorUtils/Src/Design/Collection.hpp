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
#ifndef ___CASTOR_COLLECTION_H___
#define ___CASTOR_COLLECTION_H___

#include "Config/MultiThreadConfig.hpp"
#include "Config/SmartPtr.hpp"
#include "Templates.hpp"
#include "NonCopyable.hpp"
#include "Exception/Assertion.hpp"

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
		DECLARE_TPL_MAP( TKey, TObjSPtr, TObjPtr );
		typedef typename TObjPtrMap::value_type value_type;
		//!\~english Typedef over the key param type	\~french Typedef sur le type de la clef en paramètre de fonction
		typedef typename call_traits< TKey >::const_param_type key_param_type;

	private:
		struct Search
		{
			TKey m_key;
			TObjPtrMapConstIt m_result;
		};

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
		 *\return		\p true if the collection is empty.
		 *\~french
		 *\return		\p true si la collection est vide.
		 */
		inline bool empty()const;
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

	protected:
		/**
		 *\~english
		 *\brief		Initialises last search (empty key, invalid result).
		 *\~french
		 *\brief		Initialise la dernière rechercher (clef vide, résultat invalide).
		 */
		void do_init_last()const;
		/**
		 *\~english
		 *\brief		Updates the last search, performing the lookup in the map.
		 *\param[in]	p_key	The key to test.
		 *\~french
		 *\brief		Met à jour la dernière recherche, en effectuant la recherche dans la map.
		 *\param[in]	p_key	La clef à tester.
		 */
		inline void do_update_last( key_param_type p_key )const;

	private:
		TObjPtrMap m_objects;
		mutable std::recursive_mutex m_mutex;
		mutable bool m_locked;
		mutable Search m_last;
	};
}

#include "Collection.inl"

#endif
