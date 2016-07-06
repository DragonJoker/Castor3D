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

#include "Scene/Scene.hpp"

namespace Castor3D
{
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
	template< typename ElementType >
	struct ElementAttacher < ElementType, typename std::enable_if < !is_detachable< ElementType >::value >::type >
	{
		using ElementPtr = std::shared_ptr< ElementType >;
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
		inline void operator()( ElementPtr p_element, SceneNodeSPtr p_parent, SceneNodeSPtr p_rootNode, SceneNodeSPtr p_rootCameraNode, SceneNodeSPtr p_rootObjectNode )
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
	template< typename ElementType >
	struct ElementDetacher < ElementType, typename std::enable_if < !is_detachable< ElementType >::value >::type >
	{
		using ElementPtr = std::shared_ptr< ElementType >;

		inline void operator()( ElementPtr p_element )
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
	template< typename ElementType >
	struct ElementDetacher< ElementType, typename std::enable_if< is_detachable< ElementType >::value >::type >
	{
		using ElementPtr = std::shared_ptr< ElementType >;

		inline void operator()( ElementPtr p_element )
		{
			p_element->Detach();
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/10/2015
	\version	0.8.0
	\~english
	\brief		Helper structure to enable moving elements from a cache to another.
	\remarks	Specialisation for detachable object types.
	\~french
	\brief		Structure permettant de déplacer les éléments d'un cache à l'autre.
	\remarks	Spécialisation pour les types d'objet détachables.
	*/
	template< typename ElementType, typename KeyType >
	struct ElementMerger< ElementType, KeyType, typename std::enable_if< is_detachable< ElementType >::value >::type >
	{
		using ElementPtr = std::shared_ptr< ElementType >;

		template< typename InitialiserType, typename CleanerType, typename MergerType, typename AttacherType, typename DetacherType >
		inline void operator()( ObjectCache< ElementType, KeyType, InitialiserType, CleanerType, MergerType, AttacherType, DetacherType > const & p_source, Castor::Collection< ElementType, KeyType > & p_destination, ElementPtr p_element, SceneNodeSPtr p_cameraRootNode, SceneNodeSPtr p_objectRootNode )
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
	\brief		Base class for a scene element cache.
	\~french
	\brief		Classe de base pour un cache d'éléments de scène.
	*/
	template< typename ElementType
		, typename KeyType
		, typename InitialiserType
		, typename CleanerType
		, typename MergerType
		, typename AttacherType
		, typename DetacherType >
	class ObjectCacheBase
		: public CacheBase< ElementType, KeyType, InitialiserType, CleanerType, MergerType >
	{
	protected:
		using MyCacheType = CacheBase< ElementType, KeyType, InitialiserType, CleanerType, MergerType >;
		using MyObjectCacheType = ObjectCacheBase< ElementType, KeyType, InitialiserType, CleanerType, MergerType, AttacherType, DetacherType >;
		using Element = typename MyCacheType::Element;
		using Key = typename MyCacheType::Key;
		using Collection = typename MyCacheType::Collection;
		using ElementPtr = typename MyCacheType::ElementPtr;
		using Producer = typename MyCacheType::Producer;
		using Initialiser = typename MyCacheType::Initialiser;
		using Cleaner = typename MyCacheType::Cleaner;
		using Merger = typename MyCacheType::Merger;
		using Attacher = AttacherType;
		using Detacher = DetacherType;

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
		inline ObjectCacheBase( SceneNodeSPtr p_rootNode
							   , SceneNodeSPtr p_rootCameraNode
							   , SceneNodeSPtr p_rootObjectNode
							   , Engine & p_engine
							   , SceneGetter && p_scene
							   , Producer && p_produce
							   , Initialiser && p_initialise = Initialiser{}
							   , Cleaner && p_clean = Cleaner{}
							   , Merger && p_merge = Merger{}
							   , Attacher && p_attach = Attacher{}
							   , Detacher && p_detach = Detacher{} )
			: MyCacheType( p_engine
						  , std::move( p_produce )
						  , std::move( p_initialise )
						  , std::move( p_clean )
						  , std::move( p_merge ) )
			, m_rootNode( p_rootNode )
			, m_rootCameraNode( p_rootCameraNode )
			, m_rootObjectNode( p_rootObjectNode )
			, m_scene( std::move( p_scene ) )
			, m_attach( std::move( p_attach ) )
			, m_detach( std::move( p_detach ) )
		{
			this->m_renderSystem = this->m_get()->GetRenderSystem();
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
				m_detach( l_it.second );
			}

			DoUpdateScene();
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
				m_detach( l_element );
				this->m_elements.erase( p_name );
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
			auto l_lock = Castor::make_unique_lock( this->m_elements );
			auto l_lockOther = Castor::make_unique_lock( p_destination.m_elements );

			for ( auto l_it : this->m_elements )
			{
				m_merge( *this, p_destination.m_elements, l_it.second, p_destination.m_rootCameraNode.lock(), p_destination.m_rootObjectNode.lock() );
			}

			MyCacheType::Clear();
			DoUpdateScene();
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
				auto l_lock = Castor::make_unique_lock( this->m_elements );

				if ( this->m_elements.has( p_name ) )
				{
					Castor::Logger::LogWarning( Castor::StringStream() << WARNING_CACHE_DUPLICATE_OBJECT << this->GetObjectTypeName() << cuT( ": " ) << p_name );
					l_return = this->m_elements.find( p_name );
				}
				else
				{
					this->m_elements.insert( p_name, p_element );
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
		inline ElementPtr Get( Key const & p_name )
		{
			auto l_lock = Castor::make_unique_lock( this->m_elements );
			ElementPtr l_return;

			if ( !this->m_elements.has( p_name ) )
			{
				l_return = this->m_produce( p_name );
				this->m_initialise( *this->GetEngine(), l_return );
				this->m_elements.insert( p_name, l_return );
				m_attach( l_return, p_parent, m_rootNode.lock(), m_rootCameraNode.lock(), m_rootObjectNode.lock() );
				Castor::Logger::LogInfo( Castor::StringStream() << INFO_CACHE_CREATED_OBJECT << this->GetObjectTypeName() << cuT( ": " ) << p_name );
				DoUpdateScene();
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
		using MyCacheType::SetRenderSystem;
		/**
		 *\~english
		 *\brief		Sets the scene as changed.
		 *\~french
		 *\brief		Dit que la scène a changé.
		 */
		C3D_API void DoUpdateScene();

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
	\brief		Base class for a scene element cache.
	\~french
	\brief		Classe de base pour un cache d'éléments de scène.
	*/
	template< typename ElementType
		, typename KeyType
		, typename ProducerType
		, typename InitialiserType
		, typename CleanerType
		, typename MergerType
		, typename AttacherType
		, typename DetacherType >
	class ObjectCache
		: public ObjectCacheBase< ElementType, KeyType, ProducerType, InitialiserType, CleanerType, MergerType, AttacherType, DetacherType >
	{
	protected:
		using MyObjectCacheType = ObjectCacheBase< ElementType, KeyType, ProducerType, InitialiserType, CleanerType, MergerType, AttacherType, DetacherType >;
		using MyCacheType = typename MyObjectCacheType::MyCacheType;
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
		inline ObjectCache( SceneNodeSPtr p_rootNode
						   , SceneNodeSPtr p_rootCameraNode
						   , SceneNodeSPtr p_rootObjectNode
						   , Engine & p_engine
						   , SceneGetter && p_scene
						   , Producer && p_produce
						   , Initialiser && p_initialise = Initialiser{}
						   , Cleaner && p_clean = Cleaner{}
						   , Merger && p_merge = Merger{}
						   , Attacher && p_attach = Attacher{}
						   , Detacher && p_detach = Detacher{} )
			: MyObjectCacheType( p_rootNode
								, p_rootCameraNode
								, p_rootObjectNode
								, p_engine
								, std::move( p_scene )
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
	template< typename ElementType
		, typename KeyType
		, typename ProducerType
		, typename AttacherType = ElementAttacher< ElementType >
		, typename DetacherType = ElementDetacher< ElementType >
		, typename InitialiserType = ElementInitialiser< ElementType >
		, typename CleanerType = ElementCleaner< ElementType >
		, typename MergerType = ElementMerger< ElementType, KeyType > >
	inline std::unique_ptr< ObjectCache< ElementType, KeyType, ProducerType, InitialiserType, CleanerType, MergerType, AttacherType, DetacherType > >
	MakeObjectCache( SceneNodeSPtr p_rootNode , SceneNodeSPtr p_rootCameraNode , SceneNodeSPtr p_rootObjectNode, Engine & p_engine, Scene & p_scene, ProducerType && p_produce )
	{
		return std::make_unique< ObjectCache< ElementType, KeyType, ProducerType, InitialiserType, CleanerType, MergerType, AttacherType, DetacherType > >( p_rootNode, p_rootCameraNode, p_rootObjectNode, p_engine, p_scene, std::move( p_produce ) );
	}
}

#endif
