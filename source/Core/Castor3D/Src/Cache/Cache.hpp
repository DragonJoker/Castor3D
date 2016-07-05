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
#ifndef ___C3D_CACHE_H___
#define ___C3D_CACHE_H___

#include "Castor3DPrerequisites.hpp"

#include "Engine.hpp"

#include "Event/Frame/CleanupEvent.hpp"
#include "Event/Frame/InitialiseEvent.hpp"

#include <Collection.hpp>
#include <OwnedBy.hpp>
#include <Logger.hpp>
#include <Signal.hpp>

namespace Castor3D
{
	static const xchar * INFO_CACHE_CREATED_OBJECT = cuT( "Cache::Create - Created " );
	static const xchar * WARNING_CACHE_DUPLICATE_OBJECT = cuT( "Cache::Create - Duplicate " );
	static const xchar * WARNING_CACHE_NULL_OBJECT = cuT( "Cache::Insert - nullptr " );
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/10/2015
	\version	0.8.0
	\~english
	\brief		Helper structure to enable initialisation if a type supports it.
	\remarks	Specialisation for types that support initialisation.
	\~french
	\brief		Structure permettant d'initialiser les éléments qui le supportent.
	\remarks	Spécialisation pour les types qui ne supportent pas l'initialisation.
	*/
	template< typename Elem >
	struct ElementInitialiser < Elem, typename std::enable_if < !is_initialisable< Elem >::value >::type >
	{
		using ElemPtr = std::shared_ptr< Elem >;

		inline void operator()( Engine & p_engine, ElemPtr p_element )
		{
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/10/2015
	\version	0.8.0
	\~english
	\brief		Helper structure to enable cleanup if a type supports it.
	\remarks	Specialisation for types that support cleanup.
	\~french
	\brief		Structure permettant de nettoyer les éléments qui le supportent.
	\remarks	Spécialisation pour les types qui supportent le cleanup.
	*/
	template< typename Elem >
	struct ElementInitialiser < Elem, typename std::enable_if < is_initialisable< Elem >::value && is_instant< Elem >::value >::type >
	{
		using ElemPtr = std::shared_ptr< Elem >;

		inline void operator()( Engine & p_engine, ElemPtr p_element )
		{
			p_element->Initialise();
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/10/2015
	\version	0.8.0
	\~english
	\brief		Helper structure to enable cleanup if a type supports it.
	\remarks	Specialisation for types that support cleanup.
	\~french
	\brief		Structure permettant de nettoyer les éléments qui le supportent.
	\remarks	Spécialisation pour les types qui supportent le cleanup.
	*/
	template< typename Elem >
	struct ElementInitialiser < Elem, typename std::enable_if < is_initialisable< Elem >::value && !is_instant< Elem >::value >::type >
	{
		using ElemPtr = std::shared_ptr< Elem >;

		inline void operator()( Engine & p_engine, ElemPtr p_element )
		{
			p_engine.PostEvent( MakeInitialiseEvent( *p_element ) );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/10/2015
	\version	0.8.0
	\~english
	\brief		Helper structure to enable cleanup if a type supports it.
	\remarks	Specialisation for types that don't support cleanup.
	\~french
	\brief		Structure permettant de nettoyer les éléments qui le supportent.
	\remarks	Spécialisation pour les types qui ne supportent le cleanup.
	*/
	template< typename Elem >
	struct ElementCleaner < Elem, typename std::enable_if < !is_cleanable< Elem >::value >::type >
	{
		using ElemPtr = std::shared_ptr< Elem >;

		inline void operator()( Engine & p_engine, ElemPtr p_element )
		{
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/10/2015
	\version	0.8.0
	\~english
	\brief		Helper structure to enable cleanup if a type supports it.
	\remarks	Specialisation for types that support cleanup.
	\~french
	\brief		Structure permettant de nettoyer les éléments qui le supportent.
	\remarks	Spécialisation pour les types qui supportent le cleanup.
	*/
	template< typename Elem >
	struct ElementCleaner < Elem, typename std::enable_if < is_cleanable< Elem >::value && is_instant< Elem >::value >::type >
	{
		using ElemPtr = std::shared_ptr< Elem >;

		inline void operator()( Engine & p_engine, ElemPtr p_element )
		{
			p_element->Cleanup();
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/10/2015
	\version	0.8.0
	\~english
	\brief		Helper structure to enable cleanup if a type supports it.
	\remarks	Specialisation for types that support cleanup.
	\~french
	\brief		Structure permettant de nettoyer les éléments qui le supportent.
	\remarks	Spécialisation pour les types qui supportent le cleanup.
	*/
	template< typename Elem >
	struct ElementCleaner < Elem, typename std::enable_if < is_cleanable< Elem >::value && !is_instant< Elem >::value >::type >
	{
		using ElemPtr = std::shared_ptr< Elem >;

		inline void operator()( Engine & p_engine, ElemPtr p_element )
		{
			p_engine.PostEvent( MakeCleanupEvent( *p_element ) );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/10/2015
	\version	0.8.0
	\~english
	\brief		Helper structure to retrieve the engine instance.
	\~french
	\brief		Structure permettant de récupérer le moteur.
	*/
	struct EngineGetter
	{
		inline EngineGetter( Engine & p_engine )
			: m_engine{ p_engine }
		{
		}

		inline Engine * operator()()const
		{
			return &m_engine;
		}

		inline Engine * operator()()
		{
			return &m_engine;
		}

		Engine & m_engine;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/10/2015
	\version	0.8.0
	\~english
	\brief		Base class for an element cache.
	\~french
	\brief		Classe de base pour un cache d'éléments.
	*/
	template< typename Elem, typename Key, typename ProducerType >
	class Cache
	{
	public:
		using Collection = Castor::Collection< Elem, Key >;
		using ElemPtr = std::shared_ptr< Elem >;
		using Producer = ProducerType;
		using Initialiser = ElementInitialiser< Elem >;
		using Cleaner = ElementCleaner< Elem >;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_owner	The owner.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_owner	Le propriétaire.
		 */
		inline Cache( EngineGetter && p_get
					  , Producer && p_produce
					  , Initialiser && p_initialise = Initialiser{}
					  , Cleaner && p_clean = Cleaner{} )
			: m_get{ std::move( p_get ) }
			, m_produce{ std::move( p_produce ) }
			, m_initialise{ std::move( p_initialise ) }
			, m_clean{ std::move( p_clean ) }
		{
		}
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		inline ~Cache()
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

			for ( auto l_it : this->m_elements )
			{
				m_clean( *GetEngine(), l_it.second );
			}
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
		 *\brief		Removes an object, given a name.
		 *\param[in]	p_name		The element name.
		 *\param[in]	p_element	The element.
		 *\~french
		 *\brief		Retire un objet à partir d'un nom.
		 *\param[in]	p_name		Le nom d'élément.
		 *\param[in]	p_element	L'élément.
		 */
		inline ElemPtr Add( Key const & p_name, ElemPtr p_element )
		{
			ElemPtr l_return{ p_element };

			if ( p_element )
			{
				auto l_lock = Castor::make_unique_lock( m_elements );

				if ( m_elements.has( p_name ) )
				{
					Castor::Logger::LogWarning( Castor::StringStream() << WARNING_CACHE_DUPLICATE_OBJECT << this->GetObjectTypeName() << cuT( ": " ) << p_name );
					l_return = m_elements.find( p_name );
				}
				else
				{
					m_elements.insert( p_name, p_element );
				}
			}
			else
			{
				Castor::Logger::LogWarning( Castor::StringStream() << WARNING_CACHE_NULL_OBJECT << this->GetObjectTypeName() << cuT( ": " ) );
			}

			return l_return;
		}
		/**
		 *\~english
		 *\brief		Creates an object.
		 *\param[in]	p_name		The object name.
		 *\param[in]	p_params	The other constructor parameters.
		 *\return		The created object.
		 *\~french
		 *\brief		Crée un objet.
		 *\param[in]	p_name		Le nom d'objet.
		 *\param[in]	p_params	Les autres paramètres de construction.
		 *\return		L'objet créé.
		 */
		template< typename ... Parameters >
		inline ElemPtr Add( Key const & p_name, Parameters && ... p_params )
		{
			ElemPtr l_return;
			auto l_lock = Castor::make_unique_lock( m_elements );

			if ( !m_elements.has( p_name ) )
			{
				l_return = m_produce( p_name, std::forward< Parameters >( p_params )... );
				m_initialise( *GetEngine(), l_return );
				m_elements.insert( p_name, l_return );
				Castor::Logger::LogInfo( Castor::StringStream() << INFO_CACHE_CREATED_OBJECT << this->GetObjectTypeName() << cuT( ": " ) << p_name );
			}
			else
			{
				l_return = m_elements.find( p_name );
				Castor::Logger::LogWarning( Castor::StringStream() << WARNING_CACHE_DUPLICATE_OBJECT << this->GetObjectTypeName() << cuT( ": " ) << p_name );
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
			m_elements.erase( p_name );
		}
		/**
		*\~english
		*\param[in]	p_renderSystem	The RenderSystem.
		*\~french
		*\param[in]	p_renderSystem	Le RenderSystem.
		*/
		inline void SetRenderSystem( RenderSystem * p_renderSystem )
		{
			m_renderSystem = p_renderSystem;
		}
		/**
		*\~english
		*\return		The RenderSystem.
		*\~french
		*\return		Le RenderSystem.
		*/
		inline RenderSystem * SetRenderSystem()const
		{
			return m_renderSystem;
		}
		/**
		*\~english
		*\return		The Engine.
		*\~french
		*\return		L'Engine.
		*/
		inline Engine * GetEngine()const
		{
			return m_get();
		}
		/**
		*\~english
		*\return		The Engine.
		*\~french
		*\return		L'Engine.
		*/
		inline Castor::String const & GetObjectTypeName()const
		{
			return CachedObjectNamer< Elem >::Name;
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
		inline ElemPtr Find( Key const & p_name )const
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
		//!\~english	The RenderSystem.
		//!\~french		Le RenderSystem.
		RenderSystem * m_renderSystem{ nullptr };
		//!\~english	The elements collection.
		//!\~french		La collection d'éléments.
		mutable Collection m_elements;
		//!\~english	The engine getter.
		//!\~french		Le récupérateur de moteur.
		EngineGetter m_get;
		//!\~english	The element producer.
		//!\~french		Le créateur d'éléments.
		Producer m_produce;
		//!\~english	The element initialiser.
		//!\~french		L'initaliseur d'éléments.
		Initialiser m_initialise;
		//!\~english	The element cleaner.
		//!\~french		Le nettoyeur d'éléments.
		Cleaner m_clean;
	};
	/**
	 *\~english
	 *\brief		Creates a cache.
	 *\param[in]	p_get		The engine getter.
	 *\param[in]	p_produce	The element producer.
	 *\~french
	 *\brief		Crée un cache.
	 *\param[in]	p_get		Le récupérteur de moteur.
	 *\param[in]	p_produce	Le créateur d'objet.
	 */
	template< typename Elem, typename Key, typename ProducerType >
	inline std::unique_ptr< Cache< Elem, Key, ProducerType > >
	MakeCache( EngineGetter && p_get, ProducerType && p_produce )
	{
		return std::make_unique< Cache< Elem, Key, ProducerType > >( std::move( p_get ), std::move( p_produce ) );
	}
}

#endif
