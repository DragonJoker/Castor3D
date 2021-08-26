/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_COLLECTION_H___
#define ___CASTOR_COLLECTION_H___

#include "CastorUtils/Design/DesignModule.hpp"

#include "CastorUtils/Design/Templates.hpp"
#include "CastorUtils/Design/NonCopyable.hpp"
#include "CastorUtils/Exception/Assertion.hpp"
#include "CastorUtils/Log/LoggerInstance.hpp"

namespace castor
{
	template< typename ObjectT, typename KeyT >
	class Collection
		: public castor::NonCopyable
	{
	public:
		using ObjectPtr = std::shared_ptr< ObjectT >;
		using ObjectPtrMap = std::map< KeyT, ObjectPtr >;
		using ObjectPtrMapIt = typename ObjectPtrMap::iterator;
		using ObjectPtrMapConstIt = typename ObjectPtrMap::const_iterator;
		using ValueType = typename ObjectPtrMap::value_type;
		using KeyParamType= typename CallTraits< KeyT >::ConstParamType;

	private:
		struct Search
		{
			KeyT key;
			ObjectPtrMapConstIt result;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		explicit Collection( LoggerInstance & logger );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~Collection();
		/**
		 *\~english
		 *\return		The logger instance.
		 *\~french
		 *\return		L'instance de logger.
		 */
		inline LoggerInstance & getLogger()const;
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
		inline ObjectPtrMapIt begin();
		/**
		 *\~english
		 *\brief		Returns an constant iterator to the first element of the collection
		 *\return		The iterator
		 *\~french
		 *\brief		Renvoie un itérateur constant sur le premier élément de la collection
		 *\return		L'itérateur
		 */
		inline ObjectPtrMapConstIt begin()const;
		/**
		 *\~english
		 *\brief		Returns an iterator to the after last element of the collection
		 *\return		The iterator
		 *\~french
		 *\brief		Renvoie un itérateur sur l'après dernier élément de la collection
		 *\return		L'itérateur
		 */
		inline ObjectPtrMapIt end();
		/**
		 *\~english
		 *\brief		Returns an constant iterator to the after last element of the collection
		 *\return		The iterator
		 *\~french
		 *\brief		Renvoie un itérateur constant sur l'après dernier élément de la collection
		 *\return		L'itérateur
		 */
		inline ObjectPtrMapConstIt end()const;
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
		 *\param[in]	key	The key
		 *\return		The found object if any, \p null_ptr if none
		 *\~french
		 *\brief		Recherche dans la collection un objet situé à la clef donnée
		 *\param[in]	key	La clef
		 *\return		L'objet trouvé s'il existe, \p null_ptr sinon
		 */
		inline ObjectPtr tryFind( KeyParamType key )const;
		/**
		 *\~english
		 *\brief		Looks for a collected object at the given key
		 *\param[in]	key	The key
		 *\return		The found object if any, \p null_ptr if none
		 *\~french
		 *\brief		Recherche dans la collection un objet situé à la clef donnée
		 *\param[in]	key	La clef
		 *\return		L'objet trouvé s'il existe, \p null_ptr sinon
		 */
		inline ObjectPtr find( KeyParamType key )const;
		/**
		 *\~english
		 *\brief		Gives the collected objects count
		 *\return		The count
		 *\~french
		 *\brief		donne le nombre d'objets de la collection
		 *\return		Le compte
		 */
		inline std::size_t size()const;
		/**
		 *\~english
		 *\brief		Inserts an element at the given key into the collection
		 *\param[in]	key		The key at which element is inserted
		 *\param[in]	element	The element to insert
		 *\return		\p false if there is already one object at the given key
		 *\~french
		 *\brief		Insère un élément à la clef donnée
		 *\param[in]	key		La clef à laquelle l'élément sera associé
		 *\param[in]	element	L'élément à insérer
		 *\return		\p false si un élément est déjà associé à la clef
		 */
		inline ObjectPtr tryInsert( KeyParamType key, ObjectPtr element );
		/**
		 *\~english
		 *\brief		Inserts an element at the given key into the collection
		 *\param[in]	key		The key at which element is inserted
		 *\param[in]	element	The element to insert
		 *\return		\p false if there is already one object at the given key
		 *\~french
		 *\brief		Insère un élément à la clef donnée
		 *\param[in]	key		La clef à laquelle l'élément sera associé
		 *\param[in]	element	L'élément à insérer
		 *\return		\p false si un élément est déjà associé à la clef
		 */
		inline bool insert( KeyParamType key, ObjectPtr element );
		/**
		 *\~english
		 *\brief		Tests if there is an element associated to the key
		 *\param[in]	key	The key to test
		 *\return		The test result
		 *\~french
		 *\brief		Teste si un élément est associé à la clef donnée
		 *\param[in]	key	La clef à tester
		 *\return		Le résultat du test
		 */
		inline bool has( KeyParamType key )const;
		/**
		 *\~english
		 *\brief		Removes the element associated to the given key from the collection
		 *\param[in]	key	The key
		 *\return		The associated element, nullptr if none
		 *\~french
		 *\brief		Enlève de la collection l'élément associé à la clef donnée
		 *\param[in]	key	La clef
		 *\return		L'élément associé, null_ptr sinon
		 */
		inline ObjectPtr erase( KeyParamType key );
		/**
		 *\~english
		 *\brief		Removes the element at given iterator.
		 *\param[in]	it	The iterator
		 *\return		The iterator after erased element.
		 *\~french
		 *\brief		Enlève de la collection l'élément à l'itérateur donné.
		 *\param[in]	it	L'itérateur
		 *\return		L'itérateur après l'élément enlevé.
		 */
		inline ObjectPtrMapIt erase( ObjectPtrMapIt it );

	protected:
		/**
		 *\~english
		 *\brief		Initialises last search (empty key, invalid result).
		 *\~french
		 *\brief		Initialise la dernière rechercher (clef vide, résultat invalide).
		 */
		void doInitLast()const;
		/**
		 *\~english
		 *\brief		Updates the last search, performing the lookup in the map.
		 *\param[in]	key	The key to test.
		 *\~french
		 *\brief		Met à jour la dernière recherche, en effectuant la recherche dans la map.
		 *\param[in]	key	La clef à tester.
		 */
		inline void doUpdateLast( KeyParamType key )const;

	private:
		LoggerInstance & m_logger;
		ObjectPtrMap m_objects;
		mutable std::recursive_mutex m_mutex;
		mutable bool m_locked;
		mutable Search m_last;
	};
}

#include "Collection.inl"

#endif
