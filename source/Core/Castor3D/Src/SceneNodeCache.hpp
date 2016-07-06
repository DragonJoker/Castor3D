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
	template<>
	struct CachedObjectNamer< SceneNode >
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
	\brief		SceneNode cache.
	\~french
	\brief		Cache de SceneNode.
	*/
	template<>
	class ObjectCache< SceneNode, Castor::String, SceneNodeProducer, ElementInitialiser< SceneNode >, ElementCleaner< SceneNode >, ElementMerger< SceneNode, Castor::String >, ElementAttacher< SceneNode >, ElementDetacher< SceneNode > >
		: public ObjectCacheBase< SceneNode, Castor::String, SceneNodeProducer, ElementInitialiser< SceneNode >, ElementCleaner< SceneNode >, ElementMerger< SceneNode, Castor::String >, ElementAttacher< SceneNode >, ElementDetacher< SceneNode > >
	{
	public:
		using MyObjectCacheType = ObjectCacheBase< SceneNode, Castor::String, SceneNodeProducer, ElementInitialiser< SceneNode >, ElementCleaner< SceneNode >, ElementMerger< SceneNode, Castor::String >, ElementAttacher< SceneNode >, ElementDetacher< SceneNode > >;
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
							, Engine & p_engine
							, Scene & p_scene
							, Producer && p_produce
							, Initialiser && p_initialise = Initialiser{}
							, Cleaner && p_clean = Cleaner{}
							, Merger && p_merge = Merger{}
							, Attacher && p_attach = Attacher{}
							, Detacher && p_detach = Detacher{} )
			: MyObjectCacheType{ p_rootNode
								 , p_rootCameraNode
								 , p_rootObjectNode
								 , p_engine
								 , p_scene
								 , std::move( p_produce )
								 , std::move( p_initialise )
								 , std::move( p_clean )
								 , std::move( p_merge )
								 , std::move( p_attach )
								 , std::move( p_detach ) }
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
		inline ElementPtr Add( Key const & p_name, SceneNodeSPtr p_parent = nullptr )
		{
			auto l_lock = Castor::make_unique_lock( this->m_elements );
			ElementPtr l_return;

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
	};
	using SceneNodeCache = ObjectCache< SceneNode, Castor::String, SceneNodeProducer, ElementInitialiser< SceneNode >, ElementCleaner< SceneNode >, ElementMerger< SceneNode, Castor::String >, ElementAttacher< SceneNode >, ElementDetacher< SceneNode > >;
	DECLARE_SMART_PTR( SceneNodeCache );
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
	MakeObjectCache( SceneNodeSPtr p_rootNode, SceneNodeSPtr p_rootCameraNode , SceneNodeSPtr p_rootObjectNode, Engine & p_engine, Scene & p_scene, SceneNodeProducer && p_produce )
	{
		return std::make_unique< ObjectCache< SceneNode, Castor::String, SceneNodeProducer > >( p_rootNode, p_rootCameraNode, p_rootObjectNode, p_engine, p_scene, std::move( p_produce ) );
	}
}

#endif
