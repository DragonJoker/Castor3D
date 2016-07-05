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
#ifndef ___C3D_SCENE_NODE_CACHE_H___
#define ___C3D_SCENE_NODE_CACHE_H___

#include "Cache/ObjectCache.hpp"

#include "Scene/SceneNode.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		04/02/2016
	\version	0.8.0
	\~english
	\brief		Helper structure to get an object type name.
	\~french
	\brief		Structure permettant de récupérer le nom du type d'un objet.
	*/
	template<> struct CachedObjectNamer< SceneNode >
	{
		C3D_API static const Castor::String Name;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		29/01/2016
	\version	0.8.0
	\~english
	\brief		Helper structure to enable attaching if a type supports it.
	\remarks	Specialisation for SceneNode.
	\~french
	\brief		Structure permettant d'attacher les éléments qui le supportent.
	\remarks	Spécialisation pour SceneNode.
	*/
	template<>
	struct ElementAttacher< SceneNode >
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
		inline void operator()( std::shared_ptr< SceneNode > p_element, SceneNodeSPtr p_parent, SceneNodeSPtr p_rootNode, SceneNodeSPtr p_rootCameraNode, SceneNodeSPtr p_rootObjectNode )
		{
			if ( p_parent )
			{
				p_element->AttachTo( p_parent );
			}
			else
			{
				p_element->AttachTo( p_rootNode );
			}
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		29/01/2016
	\version	0.8.0
	\~english
	\brief		Light cache.
	\~french
	\brief		Cache de Light.
	*/
	template<>
	class ObjectCache< SceneNode, Castor::String, SceneNodeProducer >
		: public Cache< SceneNode, Castor::String, SceneNodeProducer >
	{
		using MyCacheType = Cache< SceneNode, Castor::String, SceneNodeProducer >;
		using MyObjectCacheType = ObjectCache< SceneNode, Castor::String, SceneNodeProducer >;
		using Element = typename MyCacheType::Element;
		using Key = typename MyCacheType::Key;
		using Producer = typename MyCacheType::Producer;
		using ElemPtr = typename MyCacheType::ElemPtr;
		using Initialiser = typename MyCacheType::Initialiser;
		using Cleaner = typename MyCacheType::Cleaner;
		using Merger = typename MyCacheType::Merger;
		using Attacher = ElementAttacher< Element >;
		using Detacher = ElementDetacher< Element >;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_rootNode			The root node.
		 *\param[in]	p_rootCameraNode	The cameras root node.
		 *\param[in]	p_rootObjectNode	The objects root node.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_rootNode			Le noeud racine.
		 *\param[in]	p_rootCameraNode	Le noeud racine des caméras.
		 *\param[in]	p_rootObjectNode	Le noeud racine des objets.
		 */
		C3D_API ObjectCache( SceneNodeSPtr p_rootNode
							, SceneNodeSPtr p_rootCameraNode
							, SceneNodeSPtr p_rootObjectNode
							, SceneGetter && p_get
							, Producer && p_produce
							, Initialiser && p_initialise = Initialiser{}
							, Cleaner && p_clean = Cleaner{}
							, Merger && p_merge = Merger{}
							, Attacher && p_attach = Attacher{}
							, Detacher && p_detach = Detacher{} )
			: MyCacheType( EngineGetter{ *p_get()->GetEngine() }, std::move( p_produce ), std::move( p_initialise ), std::move( p_clean ), std::move( p_merge ) )
			, m_rootNode( p_rootNode )
			, m_rootCameraNode( p_rootCameraNode )
			, m_rootObjectNode( p_rootObjectNode )
			, m_scene( std::move( p_get ) )
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
		inline ~ObjectCache()
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
				m_detach( l_element );
				this->m_elements.erase( p_name );
				this->GetScene()->SetChanged();
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
		inline ElemPtr Add( Key const & p_name, SceneNodeSPtr p_parent = nullptr )
		{
			auto l_lock = Castor::make_unique_lock( this->m_elements );
			ElemPtr l_return;

			if ( !this->m_elements.has( p_name ) )
			{
				l_return = this->m_produce( p_name, *this->GetScene() );
				this->m_initialise( *this->GetEngine(), l_return );
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
		using MyCacheType::SetRenderSystem;

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
	/**
	 *\~english
	 *\brief		Creates a SceneNode cache.
	 *\param[in]	p_rootNode			The root node.
	 *\param[in]	p_rootCameraNode	The cameras root node.
	 *\param[in]	p_rootObjectNode	The objects root node.
	 *\param[in]	p_get				The engine getter.
	 *\param[in]	p_produce			The element producer.
	 *\~french
	 *\brief		Crée un cache de SceneNode.
	 *\param[in]	p_rootNode			Le noeud racine.
	 *\param[in]	p_rootCameraNode	Le noeud racine des caméras.
	 *\param[in]	p_rootObjectNode	Le noeud racine des objets.
	 *\param[in]	p_get				Le récupérteur de moteur.
	 *\param[in]	p_produce			Le créateur d'objet.
	 */
	template<>
	inline std::unique_ptr< ObjectCache< SceneNode, Castor::String, SceneNodeProducer > >
	MakeObjectCache( SceneNodeSPtr p_rootNode, SceneNodeSPtr p_rootCameraNode , SceneNodeSPtr p_rootObjectNode, SceneGetter && p_get, SceneNodeProducer && p_produce )
	{
		return std::make_unique< ObjectCache< SceneNode, Castor::String, SceneNodeProducer > >( p_rootNode, p_rootCameraNode, p_rootObjectNode, std::move( p_get ), std::move( p_produce ) );
	}
}

#endif
