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
	template< typename Elem, typename Enable = void > struct ElementAttacher;
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
		static void Attach( std::shared_ptr< Elem > p_element, SceneNodeSPtr p_parent, SceneNodeSPtr p_rootNode, SceneNodeSPtr p_rootCameraNode, SceneNodeSPtr p_rootObjectNode )
		{
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		29/01/2016
	\version	0.8.0
	\~english
	\brief		Helper structure to enable detaching if a type supports it.
	\~french
	\brief		Structure permettant de détacher les éléments qui le supportent.
	*/
	template< typename Elem, typename Enable = void > struct ElementDetacher;
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
		static void Detach( Elem & p_element )
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
		static void Detach( Elem & p_element )
		{
			p_element.Detach();
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/10/2015
	\version	0.8.0
	\~english
	\brief		Helper structure to enable moving elements from a manager to another.
	\~french
	\brief		Structure permettant de déplacer les éléments d'un gestionnaire à l'autre.
	*/
	template< typename Key, typename Elem, typename Enable = void > struct ElementMerger;
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/10/2015
	\version	0.8.0
	\~english
	\brief		Helper structure to retrieve an ObjectManager Engine instance.
	\~french
	\brief		Structure permettant de récupérer l'Engine d'un ObjectManager.
	*/
	struct ObjectManagerEngineGetter
	{
		template< typename Key, typename Elem >
		static Engine * Get( Manager< Key, Elem, Scene, ObjectManagerEngineGetter > const & p_this )
		{
			return p_this.GetScene()->GetEngine();
		}
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
	template< typename Key, typename Elem >
	class ObjectManager
		: public Manager< Key, Elem, Scene, ObjectManagerEngineGetter >
	{
	protected:
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
		ObjectManager( Scene & p_owner, SceneNodeSPtr p_rootNode, SceneNodeSPtr p_rootCameraNode, SceneNodeSPtr p_rootObjectNode )
			: Manager< Key, Elem, Scene, ObjectManagerEngineGetter >( p_owner )
			, m_rootNode( p_rootNode )
			, m_rootCameraNode( p_rootCameraNode )
			, m_rootObjectNode( p_rootObjectNode )
		{
			this->m_renderSystem = p_owner.GetEngine()->GetRenderSystem();
		}
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~ObjectManager()
		{
		}

	public:
		/**
		 *\~english
		 *\return		The Engine.
		 *\~french
		 *\return		L'Engine.
		 */
		inline Engine * GetEngine()const
		{
			return this->GetScene()->GetEngine();
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

			for ( auto && l_it : this->m_elements )
			{
				ElementDetacher< Elem >::Detach( *l_it.second );
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
				ElementDetacher< Elem >::Detach( *l_element );
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
		inline void MergeInto( ObjectManager< Key, Elem > & p_destination )
		{
			auto l_lock = Castor::make_unique_lock( this->m_elements );
			auto l_lockOther = Castor::make_unique_lock( p_destination.m_elements );

			for ( auto l_it : this->m_elements )
			{
				ElementMerger< Key, Elem >::Merge( *this, p_destination.m_elements, l_it.second, p_destination.m_rootCameraNode.lock(), p_destination.m_rootObjectNode.lock() );
			}

			Manager< Key, Elem, Scene, ObjectManagerEngineGetter >::Clear();
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
		inline std::shared_ptr< Elem > Create( Key const & p_name, SceneNodeSPtr p_parent = nullptr, Parameters && ... p_params )
		{
			auto l_lock = Castor::make_unique_lock( this->m_elements );
			std::shared_ptr< Elem > l_return;

			if ( !this->m_elements.has( p_name ) )
			{
				l_return = std::make_shared< Elem >( p_name, *this->GetScene(), p_parent, std::forward< Parameters >( p_params )... );
				this->m_elements.insert( p_name, l_return );
				ElementAttacher< Elem >::Attach( l_return, p_parent, m_rootNode.lock(), m_rootCameraNode.lock(), m_rootObjectNode.lock() );
				Castor::Logger::LogInfo( Castor::StringStream() << INFO_MANAGER_CREATED_OBJECT << this->GetObjectTypeName() << cuT( ": " ) << p_name );
				this->GetScene()->SetChanged();
			}
			else
			{
				l_return = this->m_elements.find( p_name );
				Castor::Logger::LogWarning( Castor::StringStream() << WARNING_MANAGER_DUPLICATE_OBJECT << this->GetObjectTypeName() << cuT( ": " ) << p_name );
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
		using Manager< Key, Elem, Scene, ObjectManagerEngineGetter >::SetRenderSystem;

	protected:
		//!\~english The root node	\~french Le noeud père de tous les noeuds de la scène
		SceneNodeWPtr m_rootNode;
		//!\~english The root node used only for cameras (used to ease the use of cameras)	\~french Le noeud père de tous les noeuds de caméra
		SceneNodeWPtr m_rootCameraNode;
		//!\~english The root node for every object other than camera (used to ease the use of cameras)	\~french Le noeud père de tous les noeuds d'objet
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
	template< typename Key, typename Elem >
	struct ElementMerger< Key, Elem, typename std::enable_if< is_detachable< Elem >::value >::type >
	{
		static void Merge( ObjectManager< Key, Elem > const & p_source, Castor::Collection< Elem, Key > & p_destination, std::shared_ptr< Elem > p_element, SceneNodeSPtr p_cameraRootNode, SceneNodeSPtr p_objectRootNode )
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
	template< typename Key, typename Elem >
	struct ElementMerger < Key, Elem, typename std::enable_if < !is_detachable< Elem >::value >::type >
	{
		static void Merge( ObjectManager< Key, Elem > const & p_source, Castor::Collection< Elem, Key > & p_destination, std::shared_ptr< Elem > p_element, SceneNodeSPtr p_cameraRootNode, SceneNodeSPtr p_objectRootNode )
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
}

#endif
