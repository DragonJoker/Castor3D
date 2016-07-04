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
#ifndef ___C3D_OBJECT_MANAGER_H___
#define ___C3D_OBJECT_MANAGER_H___

#include "Manager.hpp"

#include "Scene/Scene.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		29/01/2016
	\version	0.8.0
	\~english
	\brief		Helper structure to enable attaching if a type supports it.
	\~french
	\brief		Structure permettant d'attacher les éléments qui le supportent.
	*/
	template< typename Elem, typename Enable = void >
	struct ElementAttacher;
	/*!
	\author 	Sylvain DOREMUS
	\date 		29/01/2016
	\version	0.8.0
	\~english
	\brief		Helper structure to enable detaching if a type supports it.
	\~french
	\brief		Structure permettant de détacher les éléments qui le supportent.
	*/
	template< typename Elem, typename Enable = void >
	struct ElementDetacher;
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/10/2015
	\version	0.8.0
	\~english
	\brief		Helper structure to enable moving elements from a manager to another.
	\~french
	\brief		Structure permettant de déplacer les éléments d'un gestionnaire à l'autre.
	*/
	template< typename Elem, typename Key, typename Enable = void >
	struct ElementMerger;
	/*!
	\author 	Sylvain DOREMUS
	\date 		29/01/2016
	\version	0.8.0
	\~english
	\brief		Helper structure to enable attaching if a type supports it.
	\remarks	Specialisation for non attachable object types.
	\~french
	\brief		Structure permettant d'attacher les éléments qui le supportent.
	\remarks	Spécialisation pour les types d'objet non attachables.
	*/
	template< typename Elem >
	struct ElementAttacher < Elem, typename std::enable_if < !is_detachable< Elem >::value >::type >
	{
		/**
		 *\~english
		 *\brief		Attaches an element to the appropriate parent node.
		 *\param[in]	p_element			The scene node.
		 *\param[in]	p_parent			The parent scene node.
		 *\param[in]	p_rootNode			The root node.
		 *\param[in]	p_rootCameraNode	The cameras root node.
		 *\param[in]	p_rootObjectNode	The objects root node.
		 *\~french
		 *\brief		Attache un élément au parent approprié.
		 *\param[in]	p_element			Le noeud de scène.
		 *\param[in]	p_parent			Le noeud de scène parent.
		 *\param[in]	p_rootNode			Le noeud racine.
		 *\param[in]	p_rootCameraNode	Le noeud racine des caméras.
		 *\param[in]	p_rootObjectNode	Le noeud racine des objets.
		 */
		void operator()( std::shared_ptr< Elem > p_element, SceneNodeSPtr p_parent, SceneNodeSPtr p_rootNode, SceneNodeSPtr p_rootCameraNode, SceneNodeSPtr p_rootObjectNode )
		{
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		29/01/2016
	\version	0.8.0
	\~english
	\brief		Helper structure to enable detaching if a type supports it.
	\remarks	Specialisation for non detachable object types.
	\~french
	\brief		Structure permettant de détacher les éléments qui le supportent.
	\remarks	Spécialisation pour les types d'objet non détachables.
	*/
	template< typename Elem >
	struct ElementDetacher < Elem, typename std::enable_if < !is_detachable< Elem >::value >::type >
	{
		void operator()( Elem & p_element )
		{
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/10/2015
	\version	0.8.0
	\~english
	\brief		Helper structure to enable detaching if a type supports it.
	\remarks	Specialisation for detachable object types.
	\~french
	\brief		Structure permettant de détacher les éléments qui le supportent.
	\remarks	Spécialisation pour les types d'objet détachables.
	*/
	template< typename Elem >
	struct ElementDetacher< Elem, typename std::enable_if< is_detachable< Elem >::value >::type >
	{
		void operator()( Elem & p_element )
		{
			p_element.Detach();
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
	struct SceneGetter
	{
		SceneGetter( Scene & p_scene )
			: m_scene{ p_scene }
		{
		}

		Scene * operator()()const
		{
			return &m_scene;
		}

		Scene * operator()()
		{
			return &m_scene;
		}

		Scene & m_scene;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/10/2015
	\version	0.8.0
	\~english
	\brief		Base class for a scene element manager.
	\~french
	\brief		Classe de base pour un gestionnaire d'éléments de scène.
	*/
	template< typename Elem, typename Key, typename ProducerType >
	class ObjectCache
		: public Cache< Elem, Key, ProducerType >
	{
		using MyCacheType = Cache< Elem, Key, ProducerType >;
		using Initialiser = typename MyCache::Initialiser;
		using Cleaner = typename MyCache::Cleaner;
		using Attacher = ElementAttacher< Elem >;
		using Detacher = ElementDetacher< Elem >;
		using Merger = ElementMerger< Elem, Key >;

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
		ObjectCache( SceneNodeSPtr p_rootNode
					, SceneNodeSPtr p_rootCameraNode
					, SceneNodeSPtr p_rootObjectNode
					, SceneGetter && p_get
					, ProducerType && p_produce
					, Initialiser && p_initialise = Initialiser{}
					, Cleaner && p_clean = Cleaner{}
					, Attacher && p_attach = Attacher{}
					, Detacher && p_detach = Detacher{}
					, Merger && p_merge = Merger{} )
			: MyCacheType( EngineGetter{ p_get()->GetEngine() }, std::move( p_produce ) )
			, m_scene( std::move( p_get ) )
			, m_rootNode( p_rootNode )
			, m_rootCameraNode( p_rootCameraNode )
			, m_rootObjectNode( p_rootObjectNode )
			, m_attach( p_attach )
			, m_detach( p_detach )
			, m_merge( p_merge )
		{
			this->m_renderSystem = p_get()->GetRenderSystem();
		}
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~ObjectCache()
		{
		}
		/**
		*\~english
		*\return		The Engine.
		*\~french
		*\return		L'Engine.
		*/
		inline Scene * GetScene()const
		{
			return m_scene();
		}
		/**
		 *\~english
		 *\brief		Sets all the elements to be cleaned up.
		 *\~french
		 *\brief		Met tous les éléments à nettoyer.
		 */
		inline void Cleanup()
		{
			auto l_lock = Castor::make_unique_lock( this->m_elements );

			for ( auto l_it : this->m_elements )
			{
				m_detach( *l_it.second );
			}

			this->GetScene()->SetChanged();
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
			auto l_lock = Castor::make_unique_lock( this->m_elements );

			if ( this->m_elements.has( p_name ) )
			{
				auto l_element = this->m_elements.find( p_name );
				m_detach( *l_element );
				this->m_elements.erase( p_name );
				this->GetScene()->SetChanged();
			}
		}
		/**
		 *\~english
		 *\return		Merges this manager's elements to the one given.
		 *\param[out]	p_destination		The destination manager.
		 *\~french
		 *\return		Met les éléments de ce gestionnaire dans ceux de celui donné.
		 *\param[out]	p_destination		Le gestionnaire de destination.
		 */
		inline void MergeInto( ObjectCache< Elem, Key, ProducerType > & p_destination )
		{
			auto l_lock = Castor::make_unique_lock( this->m_elements );
			auto l_lockOther = Castor::make_unique_lock( p_destination.m_elements );

			for ( auto l_it : this->m_elements )
			{
				m_merge( *this, p_destination.m_elements, l_it.second, p_destination.m_rootCameraNode.lock(), p_destination.m_rootObjectNode.lock() );
			}

			MyCacheType::Clear();
			p_destination.GetScene()->SetChanged();
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
		inline std::shared_ptr< Elem > Add( Key const & p_name, SceneNodeSPtr p_parent = nullptr, Parameters && ... p_params )
		{
			auto l_lock = Castor::make_unique_lock( this->m_elements );
			std::shared_ptr< Elem > l_return;

			if ( !this->m_elements.has( p_name ) )
			{
				l_return = m_produce( p_name, *this->GetScene(), p_parent, std::forward< Parameters >( p_params )... );
				this->m_elements.insert( p_name, l_return );
				m_attach( l_return, p_parent, m_rootNode.lock(), m_rootCameraNode.lock(), m_rootObjectNode.lock() );
				Castor::Logger::LogInfo( Castor::StringStream() << INFO_CACHE_CREATED_OBJECT << this->GetObjectTypeName() << cuT( ": " ) << p_name );
				this->GetScene()->SetChanged();
			}
			else
			{
				l_return = this->m_elements.find( p_name );
				Castor::Logger::LogWarning( Castor::StringStream() << WARNING_CACHE_DUPLICATE_OBJECT << this->GetObjectTypeName() << cuT( ": " ) << p_name );
			}

			return l_return;
		}
		/**
		 *\~english
		 *\return		The objects count.
		 *\~french
		 *\return		Le nombre d'objets
		 */
		inline uint32_t GetObjectCount()const
		{
			return uint32_t( this->m_elements.size() );
		}

	private:
		using Cache< Elem, Key, ProducerType >::SetRenderSystem;

	protected:
		//!\~english	The object attacher.
		//!\~french		L'attacheur d'objet.
		SceneGetter m_scene;
		//!\~english	The object attacher.
		//!\~french		L'attacheur d'objet.
		Attacher m_attach;
		//!\~english	The object detacher.
		//!\~french		Le détacheur d'objet.
		Detacher m_detach;
		//!\~english	The objects collection merger.
		//!\~french		Le fusionneur de collection d'objets.
		Merger m_merge;
		//!\~english	The root node.
		//!\~french		Le noeud père de tous les noeuds de la scène.
		SceneNodeWPtr m_rootNode;
		//!\~english	The root node used only for cameras.
		//!\~french		Le noeud père de tous les noeuds de caméra.
		SceneNodeWPtr m_rootCameraNode;
		//!\~english	The root node for every object other than camera.
		//!\~french		Le noeud père de tous les noeuds d'objet.
		SceneNodeWPtr m_rootObjectNode;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/10/2015
	\version	0.8.0
	\~english
	\brief		Helper structure to enable moving elements from a manager to another.
	\remarks	Specialisation for detachable object types.
	\~french
	\brief		Structure permettant de déplacer les éléments d'un gestionnaire à l'autre.
	\remarks	Spécialisation pour les types d'objet détachables.
	*/
	template< typename Elem, typename Key >
	struct ElementMerger< Elem, Key, typename std::enable_if< is_detachable< Elem >::value >::type >
	{
		template< typename ProducerType >
		void operator()( ObjectCache< Elem, Key, ProducerType > const & p_source, Castor::Collection< Elem, Key > & p_destination, std::shared_ptr< Elem > p_element, SceneNodeSPtr p_cameraRootNode, SceneNodeSPtr p_objectRootNode )
		{
			//if ( p_element->GetParent()->GetName() == p_cameraRootNode->GetName() )
			//{
			//	p_element->Detach();
			//	p_element->AttachTo( p_cameraRootNode );
			//}
			//else if ( p_element->GetParent()->GetName() == p_objectRootNode->GetName() )
			//{
			//	p_element->Detach();
			//	p_element->AttachTo( p_objectRootNode );
			//}

			//Castor::String l_name = p_element->GetName();

			//while ( p_destination.find( l_name ) != p_destination.end() )
			//{
			//	l_name = p_source.GetScene()->GetName() + cuT( "_" ) + l_name;
			//}

			//p_element->SetName( l_name );
			//p_destination.insert( l_name, p_element );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/10/2015
	\version	0.8.0
	\~english
	\brief		Helper structure to enable moving elements from a manager to another.
	\remarks	Specialisation for non detachable object types.
	\~french
	\brief		Structure permettant de déplacer les éléments d'un gestionnaire à l'autre.
	\remarks	Spécialisation pour les types d'objet non détachables.
	*/
	template< typename Elem, typename Key >
	struct ElementMerger < Elem, Key, typename std::enable_if < !is_detachable< Elem >::value >::type >
	{
		template< typename ProducerType >
		void operator()( ObjectCache< Elem, Key, ProducerType > const & p_source, Castor::Collection< Elem, Key > & p_destination, std::shared_ptr< Elem > p_element, SceneNodeSPtr p_cameraRootNode, SceneNodeSPtr p_objectRootNode )
		{
			//Castor::String l_name = p_element->GetName();

			//while ( p_destination.find( l_name ) != p_destination.end() )
			//{
			//	l_name = p_source.GetScene()->GetName() + cuT( "_" ) + l_name;
			//}

			//p_element->SetName( l_name );
			//p_destination.insert( l_name, p_element );
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
	template< typename Elem, typename Key, typename ProducerType >
	std::unique_ptr< ObjectCache< Elem, Key, ProducerType > >
	MakeObjectCache( SceneNodeSPtr p_rootNode , SceneNodeSPtr p_rootCameraNode , SceneNodeSPtr p_rootObjectNode, SceneGetter const & p_get, ProducerType const & p_produce )
	{
		return std::make_unique< ObjectCache< Elem, Key, ProducerType > >( p_get, p_produce, p_rootNode, p_rootCameraNode, p_rootObjectNode);
	}
}

#endif
