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
#ifndef ___C3D_MANAGER_H___
#define ___C3D_MANAGER_H___

#include "Castor3DPrerequisites.hpp"

#include "CleanupEvent.hpp"
#include "Engine.hpp"
#include "InitialiseEvent.hpp"

#include <Collection.hpp>
#include <OwnedBy.hpp>
#include <Logger.hpp>

namespace Castor3D
{
	static const xchar * INFO_MANAGER_CREATED_OBJECT = cuT( "Manager::Create - Created object: " );
	static const xchar * WARNING_MANAGER_DUPLICATE_OBJECT = cuT( "Manager::Create - Duplicate object: " );
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/10/2015
	\version	0.8.0
	\~english
	\brief		Helper structure to enable initialisation if a type supports it.
	\~french
	\brief		Structure permettant d'initialiser les éléments qui le supportent.
	*/
	template< typename Elem, typename Enable = void > struct ElementInitialiser;
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
		void operator()( Engine & p_engine, Elem & p_element )
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
		void operator()( Engine & p_engine, Elem & p_element )
		{
			p_element.Initialise();
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
		void operator()( Engine & p_engine, Elem & p_element )
		{
			p_engine.PostEvent( MakeInitialiseEvent( p_element ) );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/10/2015
	\version	0.8.0
	\~english
	\brief		Helper structure to enable cleanup if a type supports it.
	\~french
	\brief		Structure permettant de nettoyer les éléments qui le supportent.
	*/
	template< typename Elem, typename Enable = void > struct ElementCleaner;
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
		void operator()( Engine & p_engine, Elem & p_element )
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
		void operator()( Engine & p_engine, Elem & p_element )
		{
			p_element.Cleanup();
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
		void operator()( Engine & p_engine, Elem & p_element )
		{
			p_engine.PostEvent( MakeCleanupEvent( p_element ) );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/10/2015
	\version	0.8.0
	\~english
	\brief		Base class for an element manager.
	\~french
	\brief		Classe de base pour un gestionnaire d'éléments.
	*/
	template< typename Key, typename Elem, typename Owner >
	class Manager
		: public Castor::OwnedBy< Owner >
	{
	public:
		typedef Castor::Collection< Elem, Key > Collection;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_engine	Le moteur.
		 */
		Manager( Engine & p_engine )
			: OwnedBy< Engine >( p_engine )
			, m_renderSystem( nullptr )
		{
		}
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~Manager()
		{
		}
		/**
		 *\~english
		 *\brief		Sets all the elements to be cleaned up.
		 *\~french
		 *\brief		Met tous les éléments à nettoyer.
		 */
		virtual void Cleanup()
		{
			std::unique_lock< Collection > l_lock( m_elements );

			for ( auto && l_it : this->m_elements )
			{
				ElementCleaner< Elem >()( *GetOwner(), *l_it.second );
			}
		}
		/**
		 *\~english
		 *\brief		Flushes the collection.
		 *\~french
		 *\brief		Vide la collection.
		 */
		virtual void Clear()
		{
			m_elements.clear();
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
		virtual void Insert( Key const & p_name, std::shared_ptr< Elem > p_element )
		{
			std::unique_lock< Collection > l_lock( m_elements );

			if ( m_elements.has( p_name ) )
			{
				Castor::Logger::LogWarning( WARNING_MANAGER_DUPLICATE_OBJECT + Castor::string::to_string( p_name ) );
			}

			m_elements.insert( p_name, p_element );
		}
		/**
		 *\~english
		 *\brief		Removes an object, given a name.
		 *\param[in]	p_name		The object name.
		 *\~french
		 *\brief		Retire un objet à partir d'un nom.
		 *\param[in]	p_name		Le nom d'objet.
		 */
		virtual void Remove( Key const & p_name )
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
		 *\biref		Looks for an element with given name.
		 *\param[in]	p_name		The object name.
		 *\return		The found element, nullptr if not found.
		 *\~french
		 *\biref		Cherche un élément par son nom.
		 *\param[in]	p_name		Le nom d'objet.
		 *\return		L'élément trouvé, nullptr si non trouvé.
		 */
		inline std::shared_ptr< Elem > Find( Key const & p_name )const
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
		inline typename Collection::TObjPtrMapIt begin()
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
		inline typename Collection::TObjPtrMapConstIt begin()const
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
		inline typename Collection::TObjPtrMapIt end()
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
		inline typename Collection::TObjPtrMapConstIt end()const
		{
			return m_elements.end();
		}

#	if CASTOR_HAS_VARIADIC_TEMPLATES

		/**
		 *\~english
		 *\brief		Creates an object from a name.
		 *\param[in]	p_name		The object name.
		 *\param[in]	p_params	The constructor parameters.
		 *\return		The created object.
		 *\~french
		 *\brief		Crée un objet à partir d'un nom.
		 *\param[in]	p_name		Le nom d'objet.
		 *\param[in]	p_params	Les paramètres de construction.
		 *\return		L'objet créé.
		 */
		template< typename ... Parameters >
		inline std::shared_ptr< Elem > Create( Key const & p_name, Parameters && ... p_params )
		{
			std::unique_lock< Collection > l_lock( m_elements );
			std::shared_ptr< Elem > l_return;

			if ( !m_elements.has( p_name ) )
			{
				l_return = std::make_shared< Elem >( std::forward< Parameters >( p_params )... );
				m_elements.insert( p_name, l_return );
				ElementInitialiser< Elem >()( *GetOwner(), *l_return );
				Castor::Logger::LogInfo( INFO_MANAGER_CREATED_OBJECT + Castor::string::to_string( p_name ) );
			}
			else
			{
				l_return = m_elements.find( p_name );
				Castor::Logger::LogWarning( WARNING_MANAGER_DUPLICATE_OBJECT + Castor::string::to_string( p_name ) );
			}

			return l_return;
		}

#	endif

	protected:
		//!\~english The RenderSystem.	\~french Le RenderSystem.
		RenderSystem * m_renderSystem;
		//!\~english The elements collection.	\~french La collection d'éléments.
		mutable Collection m_elements;
	};
}

#endif
