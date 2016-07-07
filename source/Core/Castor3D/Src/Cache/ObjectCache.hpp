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
#ifndef ___C3D_OBJECT_CACHE_H___
#define ___C3D_OBJECT_CACHE_H___

#include "Cache.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/10/2015
	\version	0.8.0
	\~english
	\brief		Base class for a scene element cache.
	\~french
	\brief		Classe de base pour un cache d'éléments de scène.
	*/
	template< typename ElementType, typename KeyType >
	class ObjectCacheBase
	{
	protected:
		using MyObjectCacheType = ObjectCacheBase< ElementType, KeyType >;
		using MyObjectCacheTraits = ObjectCacheTraits< ElementType, KeyType >;
		using Element = ElementType;
		using Key = KeyType;
		using Collection = Castor::Collection< Element, Key >;
		using ElementPtr = std::shared_ptr< Element >;
		using Producer = typename MyObjectCacheTraits::Producer;
		using Merger = typename MyObjectCacheTraits::Merger;
		using Initialiser = ElementInitialiser< Element >;
		using Cleaner = ElementCleaner< Element >;
		using Attacher = ElementAttacher< Element >;
		using Detacher = ElementDetacher< Element >;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_owner				The owner.
		 *\param[in]	p_rootNode			The root node.
		 *\param[in]	p_rootCameraNode	The cameras root node.
		 *\param[in]	p_rootObjectNode	The objects root node.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_owner				Le propriétaire.
		 *\param[in]	p_rootNode			Le noeud racine.
		 *\param[in]	p_rootCameraNode	Le noeud racine des caméras.
		 *\param[in]	p_rootObjectNode	Le noeud racine des objets.
		 */
		inline ObjectCacheBase( Engine & p_engine
							   , Scene & p_scene
							   , SceneNodeSPtr p_rootNode
							   , SceneNodeSPtr p_rootCameraNode
							   , SceneNodeSPtr p_rootObjectNode
							   , Producer && p_produce
							   , Initialiser && p_initialise = Initialiser{}
							   , Cleaner && p_clean = Cleaner{}
							   , Merger && p_merge = Merger{}
							   , Attacher && p_attach = Attacher{}
							   , Detacher && p_detach = Detacher{} )
			: m_engine( p_engine )
			, m_scene( p_scene )
			, m_rootNode( p_rootNode )
			, m_rootCameraNode( p_rootCameraNode )
			, m_rootObjectNode( p_rootObjectNode )
			, m_produce( std::move( p_produce ) )
			, m_initialise( std::move( p_initialise ) )
			, m_clean( std::move( p_clean ) )
			, m_merge( std::move( p_merge ) )
			, m_attach( std::move( p_attach ) )
			, m_detach( std::move( p_detach ) )
		{
		}
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		inline ~ObjectCacheBase()
		{
		}
		/**
		 *\~english
		 *\brief		Sets all the elements to be cleaned up.
		 *\~french
		 *\brief		Met tous les éléments à nettoyer.
		 */
		inline void Cleanup()
		{
			auto l_lock = Castor::make_unique_lock( m_elements );

			for ( auto l_it : m_elements )
			{
				m_detach( l_it.second );
			}

			m_onChanged();
		}
		/**
		 *\~english
		 *\brief		Flushes the collection.
		 *\~french
		 *\brief		Vide la collection.
		 */
		inline void Clear()
		{
			m_elements.clear();
		}
		/**
		 *\~english
		 *\return		\p true if the cache is empty.
		 *\~french
		 *\return		\p true si le cache est vide.
		 */
		inline bool IsEmpty()
		{
			return m_elements.empty();
		}
		/**
		 *\~english
		 *\brief		Adds an object.
		 *\param[in]	p_name	The object name.
		 *\param[in]	p_value	The object.
		 *\return		The added object, or the existing one.
		 *\~french
		 *\brief		Ajoute un objet.
		 *\param[in]	p_name	Le nom d'objet.
		 *\param[in]	p_value	L'objet.
		 *\return		L'objet ajouté, ou celui existant.
		 */
		template< typename ... Parameters >
		inline ElementPtr Add( Key const & p_name, ElementPtr p_element )
		{
			ElementPtr l_return{ p_element };

			if ( p_element )
			{
				auto l_lock = Castor::make_unique_lock( m_elements );

				if ( m_elements.has( p_name ) )
				{
					Castor::Logger::LogWarning( Castor::StringStream() << WARNING_CACHE_DUPLICATE_OBJECT << GetObjectTypeName() << cuT( ": " ) << p_name );
					l_return = m_elements.find( p_name );
				}
				else
				{
					m_elements.insert( p_name, p_element );
					m_onChanged();
				}
			}
			else
			{
				Castor::Logger::LogWarning( Castor::StringStream() << WARNING_CACHE_NULL_OBJECT << GetObjectTypeName() << cuT( ": " ) );
			}

			return l_return;
		}
		/**
		 *\~english
		 *\brief		Creates an object.
		 *\param[in]	p_name		The object name.
		 *\param[in]	p_parent	The parent scene node.
		 *\param[in]	p_params	The other constructor parameters.
		 *\return		The created object.
		 *\~french
		 *\brief		Crée un objet.
		 *\param[in]	p_name		Le nom d'objet.
		 *\param[in]	p_parent	Le noeud de scène parent.
		 *\param[in]	p_params	Les autres paramètres de construction.
		 *\return		L'objet créé.
		 */
		template< typename ... Parameters >
		inline ElementPtr Add( Key const & p_name, SceneNodeSPtr p_parent, Parameters && ... p_parameters )
		{
			auto l_lock = Castor::make_unique_lock( m_elements );
			ElementPtr l_return;

			if ( !m_elements.has( p_name ) )
			{
				l_return = m_produce( p_name, p_parent, std::forward< Parameters >( p_parameters )... );
				m_initialise( l_return );
				m_elements.insert( p_name, l_return );
				m_attach( l_return, p_parent, m_rootNode.lock(), m_rootCameraNode.lock(), m_rootObjectNode.lock() );
				Castor::Logger::LogInfo( Castor::StringStream() << INFO_CACHE_CREATED_OBJECT << GetObjectTypeName() << cuT( ": " ) << p_name );
				m_onChanged();
			}
			else
			{
				l_return = m_elements.find( p_name );
				Castor::Logger::LogWarning( Castor::StringStream() << WARNING_CACHE_DUPLICATE_OBJECT << GetObjectTypeName() << cuT( ": " ) << p_name );
			}

			return l_return;
		}
		/**
		 *\~english
		 *\brief		Removes an object, given a name.
		 *\param[in]	p_name		The object name.
		 *\~french
		 *\brief		Retire un objet à partir d'un nom.
		 *\param[in]	p_name		Le nom d'objet.
		 */
		inline void Remove( Key const & p_name )
		{
			auto l_lock = Castor::make_unique_lock( m_elements );

			if ( m_elements.has( p_name ) )
			{
				auto l_element = m_elements.find( p_name );
				m_detach( l_element );
				m_elements.erase( p_name );
				m_onChanged();
			}
		}
		/**
		 *\~english
		 *\return		Merges this cache's elements to the one given.
		 *\param[out]	p_destination		The destination cache.
		 *\~french
		 *\return		Met les éléments de ce cache dans ceux de celui donné.
		 *\param[out]	p_destination		Le cache de destination.
		 */
		inline void MergeInto( MyObjectCacheType & p_destination )
		{
			auto l_lock = Castor::make_unique_lock( m_elements );
			auto l_lockOther = Castor::make_unique_lock( p_destination.m_elements );

			for ( auto l_it : m_elements )
			{
				m_merge( *this, p_destination.m_elements, l_it.second, p_destination.m_rootCameraNode.lock(), p_destination.m_rootObjectNode.lock() );
			}

			Clear();
			m_onChanged();
		}
		/**
		 *\~english
		 *\brief		Applies a function to all the elements of this cache.
		 *\param[in]	p_func	The function.
		 *\~french
		 *\brief		Applique une fonction à tous les éléments de ce cache.
		 *\param[in]	p_func	La fonction.
		 */
		template< typename FuncType >
		inline void ForEach( FuncType p_func )const
		{
			auto l_lock = Castor::make_unique_lock( m_elements );

			for ( auto const & l_element : m_elements )
			{
				p_func( *l_element.second );
			}
		}
		/**
		 *\~english
		 *\brief		Applies a function to all the elements of this cache.
		 *\param[in]	p_func	The function.
		 *\~french
		 *\brief		Applique une fonction à tous les éléments de ce cache.
		 *\param[in]	p_func	La fonction.
		 */
		template< typename FuncType >
		inline void ForEach( FuncType p_func )
		{
			auto l_lock = Castor::make_unique_lock( m_elements );

			for ( auto & l_element : m_elements )
			{
				p_func( *l_element.second );
			}
		}
		/**
		 *\~english
		 *\return		The objects count.
		 *\~french
		 *\return		Le nombre d'objets
		 */
		inline uint32_t GetObjectCount()const
		{
			return uint32_t( m_elements.size() );
		}
		/**
		 *\~english
		 *\return		The Engine.
		 *\~french
		 *\return		L'Engine.
		 */
		inline Scene * GetScene()const
		{
			return &m_scene;
		}
		/**
		*\~english
		*\return		The Engine.
		*\~french
		*\return		L'Engine.
		*/
		inline Engine * GetEngine()const
		{
			return &m_engine;
		}
		/**
		*\~english
		*\return		The Engine.
		*\~french
		*\return		L'Engine.
		*/
		inline Castor::String const & GetObjectTypeName()const
		{
			return MyObjectCacheTraits::Name;
		}
		/**
		 *\~english
		 *\param[in]	p_name		The object name.
		 *\return		\p true if an element with given name exists.
		 *\~french
		 *\param[in]	p_name		Le nom d'objet.
		 *\return		\p true Si un élément avec le nom donné existe.
		 */
		inline bool Has( Key const & p_name )const
		{
			return m_elements.has( p_name );
		}
		/**
		 *\~english
		 *\brief		Looks for an element with given name.
		 *\param[in]	p_name		The object name.
		 *\return		The found element, nullptr if not found.
		 *\~french
		 *\brief		Cherche un élément par son nom.
		 *\param[in]	p_name		Le nom d'objet.
		 *\return		L'élément trouvé, nullptr si non trouvé.
		 */
		inline ElementPtr Find( Key const & p_name )const
		{
			return m_elements.find( p_name );
		}
		/**
		 *\~english
		 *\brief		Locks the collection mutex
		 *\~french
		 *\brief		Locke le mutex de la collection
		 */
		inline void lock()const
		{
			m_elements.lock();
		}
		/**
		 *\~english
		 *\brief		Unlocks the collection mutex
		 *\~french
		 *\brief		Délocke le mutex de la collection
		 */
		inline void unlock()const
		{
			m_elements.unlock();
		}
		/**
		 *\~english
		 *\brief		Returns an iterator to the first element of the collection
		 *\return		The iterator
		 *\~french
		 *\brief		Renvoie un itérateur sur le premier élément de la collection
		 *\return		L'itérateur
		 */
		inline auto begin()
		{
			return m_elements.begin();
		}
		/**
		 *\~english
		 *\brief		Returns an constant iterator to the first element of the collection
		 *\return		The iterator
		 *\~french
		 *\brief		Renvoie un itérateur constant sur le premier élément de la collection
		 *\return		L'itérateur
		 */
		inline auto begin()const
		{
			return m_elements.begin();
		}
		/**
		 *\~english
		 *\brief		Returns an iterator to the after last element of the collection
		 *\return		The iterator
		 *\~french
		 *\brief		Renvoie un itérateur sur l'après dernier élément de la collection
		 *\return		L'itérateur
		 */
		inline auto end()
		{
			return m_elements.end();
		}
		/**
		 *\~english
		 *\brief		Returns an constant iterator to the after last element of the collection
		 *\return		The iterator
		 *\~french
		 *\brief		Renvoie un itérateur constant sur l'après dernier élément de la collection
		 *\return		L'itérateur
		 */
		inline auto end()const
		{
			return m_elements.end();
		}

	protected:
		//!\~english	The engine.
		//!\~french		Le moteur.
		Engine & m_engine;
		//!\~english	The object attacher.
		//!\~french		L'attacheur d'objet.
		Scene & m_scene;
		//!\~english	The root node.
		//!\~french		Le noeud père de tous les noeuds de la scène.
		SceneNodeWPtr m_rootNode;
		//!\~english	The root node used only for cameras.
		//!\~french		Le noeud père de tous les noeuds de caméra.
		SceneNodeWPtr m_rootCameraNode;
		//!\~english	The root node for every object other than camera.
		//!\~french		Le noeud père de tous les noeuds d'objet.
		SceneNodeWPtr m_rootObjectNode;
		//!\~english	The elements collection.
		//!\~french		La collection d'éléments.
		mutable Collection m_elements;
		//!\~english	The element producer.
		//!\~french		Le créateur d'éléments.
		Producer m_produce;
		//!\~english	The element initialiser.
		//!\~french		L'initaliseur d'éléments.
		Initialiser m_initialise;
		//!\~english	The element cleaner.
		//!\~french		Le nettoyeur d'éléments.
		Cleaner m_clean;
		//!\~english	The objects collection merger.
		//!\~french		Le fusionneur de collection d'objets.
		Merger m_merge;
		//!\~english	The object attacher.
		//!\~french		L'attacheur d'objet.
		Attacher m_attach;
		//!\~english	The object detacher.
		//!\~french		Le détacheur d'objet.
		Detacher m_detach;
		//!\~english	The root node for every object other than camera.
		//!\~french		Le noeud père de tous les noeuds d'objet.
		Castor::Signal< std::function< void() > > m_onChanged;

		friend class Scene;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/10/2015
	\version	0.8.0
	\~english
	\brief		Base class for a scene element cache.
	\~french
	\brief		Classe de base pour un cache d'éléments de scène.
	*/
	template< typename ElementType, typename KeyType >
	class ObjectCache
		: public ObjectCacheBase< ElementType, KeyType >
	{
	protected:
		using MyObjectCacheType = ObjectCacheBase< ElementType, KeyType >;
		using Element = typename MyObjectCacheType::Element;
		using Key = typename MyObjectCacheType::Key;
		using Collection = typename MyObjectCacheType::Collection;
		using ElementPtr = typename MyObjectCacheType::ElementPtr;
		using Producer = typename MyObjectCacheType::Producer;
		using Initialiser = typename MyObjectCacheType::Initialiser;
		using Cleaner = typename MyObjectCacheType::Cleaner;
		using Merger = typename MyObjectCacheType::Merger;
		using Attacher = typename MyObjectCacheType::Attacher;
		using Detacher = typename MyObjectCacheType::Detacher;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_owner				The owner.
		 *\param[in]	p_rootNode			The root node.
		 *\param[in]	p_rootCameraNode	The cameras root node.
		 *\param[in]	p_rootObjectNode	The objects root node.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_owner				Le propriétaire.
		 *\param[in]	p_rootNode			Le noeud racine.
		 *\param[in]	p_rootCameraNode	Le noeud racine des caméras.
		 *\param[in]	p_rootObjectNode	Le noeud racine des objets.
		 */
		inline ObjectCache( Engine & p_engine
						   , Scene & p_scene
						   , SceneNodeSPtr p_rootNode
						   , SceneNodeSPtr p_rootCameraNode
						   , SceneNodeSPtr p_rootObjectNode
						   , Producer && p_produce
						   , Initialiser && p_initialise = Initialiser{}
						   , Cleaner && p_clean = Cleaner{}
						   , Merger && p_merge = Merger{}
						   , Attacher && p_attach = Attacher{}
						   , Detacher && p_detach = Detacher{} )
			: MyObjectCacheType( p_engine
								, p_scene
								, p_rootNode
								, p_rootCameraNode
								, p_rootObjectNode
								, std::move( p_produce )
								, std::move( p_initialise )
								, std::move( p_clean )
								, std::move( p_merge )
								, std::move( p_attach )
								, std::move( p_detach ) )
		{
		}
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		inline ~ObjectCache()
		{
		}
	};
	/**
	 *\~english
	 *\brief		Creates an object cache.
	 *\param[in]	p_get		The engine getter.
	 *\param[in]	p_produce	The element producer.
	 *\~french
	 *\brief		Crée un cache d'objets.
	 *\param[in]	p_get		Le récupérteur de moteur.
	 *\param[in]	p_produce	Le créateur d'objet.
	 */
	template< typename ElementType, typename KeyType >
	inline std::unique_ptr< ObjectCache< ElementType, KeyType > >
	MakeObjectCache( Engine & p_engine
					 , Scene & p_scene
					 , SceneNodeSPtr p_rootNode
					 , SceneNodeSPtr p_rootCameraNode
					 , SceneNodeSPtr p_rootObjectNode
					 , typename ObjectCacheTraits< ElementType, KeyType >::Producer && p_produce
					 , ElementInitialiser< ElementType > && p_initialise = []( std::shared_ptr< ElementType > ){}
					 , ElementCleaner< ElementType > && p_clean = []( std::shared_ptr< ElementType > ){}
					 , typename ObjectCacheTraits< ElementType, KeyType >::Merger && p_merge = []( ObjectCacheBase< ElementType, KeyType > const &
																										   , Castor::Collection< ElementType, KeyType > &
																										   , std::shared_ptr< ElementType >
																										   , SceneNodeSPtr
																										   , SceneNodeSPtr ){}
					, ElementAttacher< ElementType > && p_attach = []( std::shared_ptr< ElementType >, SceneNodeSPtr, SceneNodeSPtr, SceneNodeSPtr, SceneNodeSPtr ){}
					, ElementDetacher< ElementType > && p_detach = []( std::shared_ptr< ElementType > ){} )
	{
		return std::make_unique< ObjectCache< ElementType, KeyType > >( p_engine
																		, p_scene
																		, p_rootNode
																		, p_rootCameraNode
																		, p_rootObjectNode
																		, std::move( p_produce )
																		, std::move( p_initialise )
																		, std::move( p_clean )
																		, std::move( p_merge )
																		, std::move( p_attach )
																		, std::move( p_detach ) );
	}
}

#endif
