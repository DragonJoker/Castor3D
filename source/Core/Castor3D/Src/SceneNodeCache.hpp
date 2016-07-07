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

#include "Scene/SceneNode.hpp"
#include "Cache/ObjectCache.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		04/02/2016
	\version	0.8.0
	\~english
	\brief		Helper structure to specialise a scene cache behaviour.
	\remarks	Specialisation for SceneNode.
	\~french
	\brief		Structure permettant de spécialiser le comportement d'un cache de scène.
	\remarks	Spécialisation pour SceneNode.
	*/
	template< typename KeyType >
	struct ObjectCacheTraits< SceneNode, KeyType >
	{
		C3D_API static const Castor::String Name;
		using Producer = std::function< std::shared_ptr< SceneNode >( KeyType const & ) >;
		using Merger = std::function< void( ObjectCacheBase< SceneNode, KeyType > const &
											, Castor::Collection< SceneNode, KeyType > &
											, std::shared_ptr< SceneNode >
											, SceneNodeSPtr
											, SceneNodeSPtr ) >;
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
	class ObjectCache< SceneNode, Castor::String >
		: public ObjectCacheBase< SceneNode, Castor::String >
	{
	public:
		using MyObjectCacheType = ObjectCacheBase< SceneNode, Castor::String >;
		using MyObjectCacheTraits = typename MyObjectCacheType::MyObjectCacheTraits;
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
		C3D_API ObjectCache( Engine & p_engine
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
			: MyObjectCacheType{ p_engine
								 , p_scene
								 , p_rootNode
								 , p_rootCameraNode
								 , p_rootObjectNode
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
				l_return = this->m_produce( p_name );
				this->m_initialise( l_return );
				this->m_elements.insert( p_name, l_return );
				m_attach( l_return, p_parent, m_rootNode.lock(), m_rootCameraNode.lock(), m_rootObjectNode.lock() );
				Castor::Logger::LogInfo( Castor::StringStream() << INFO_CACHE_CREATED_OBJECT << this->GetObjectTypeName() << cuT( ": " ) << p_name );
				this->m_onChanged();
			}
			else
			{
				l_return = this->m_elements.find( p_name );
				Castor::Logger::LogWarning( Castor::StringStream() << WARNING_CACHE_DUPLICATE_OBJECT << this->GetObjectTypeName() << cuT( ": " ) << p_name );
			}

			return l_return;
		}
	};
	using SceneNodeCache = ObjectCache< SceneNode, Castor::String >;
	DECLARE_SMART_PTR( SceneNodeCache );
}

#endif
