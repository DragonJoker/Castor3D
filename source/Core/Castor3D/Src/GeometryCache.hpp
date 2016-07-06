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
#ifndef ___C3D_GEOMETRY_CACHE_H___
#define ___C3D_GEOMETRY_CACHE_H___

#include "Cache/ObjectCache.hpp"

#include "Event/Frame/FunctorEvent.hpp"
#include "Scene/Geometry.hpp"

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
	struct CachedObjectNamer< Geometry >
	{
		C3D_API static const Castor::String Name;
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
	struct GeometryInitialiser
	{
		inline void operator()( Engine & p_engine, GeometrySPtr p_element )
		{
			p_engine.PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [p_element, this]()
			{
				p_element->CreateBuffers( m_faceCount, m_vertexCount );
			} ) );
		}

		uint32_t m_faceCount{ 0 };
		uint32_t m_vertexCount{ 0 };
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		29/01/2016
	\version	0.8.0
	\~english
	\brief		Geometry cache.
	\~french
	\brief		Cache de Geometry.
	*/
	template<>
	class ObjectCache< Geometry, Castor::String, GeometryProducer, GeometryInitialiser, ElementCleaner< Geometry >, ElementMerger< Geometry, Castor::String >, MovableAttacher, MovableDetacher >
		: public ObjectCacheBase< Geometry, Castor::String, GeometryProducer, GeometryInitialiser, ElementCleaner< Geometry >, ElementMerger< Geometry, Castor::String >, MovableAttacher, MovableDetacher >
	{
	public:
		using MyObjectCache = ObjectCacheBase< Geometry, Castor::String, GeometryProducer, GeometryInitialiser, ElementCleaner< Geometry >, ElementMerger< Geometry, Castor::String >, MovableAttacher, MovableDetacher >;
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
		inline ObjectCache( SceneNodeSPtr p_rootNode
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
			: MyObjectCache{ p_rootNode
							, p_rootCameraNode
							, p_rootCameraNode
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
		*\return		The faces count for all objects.
		*\~french
		*\return		Le nombre de faces de tous les objets.
		*/
		inline uint32_t GetFaceCount()const
		{
			return m_initialise.m_faceCount;
		}
		/**
		*\~english
		*\return		The vertex count for all objects.
		*\~french
		*\return		Le nombre de sommets de tous les objets.
		*/
		inline uint32_t GetVertexCount()const
		{
			return m_initialise.m_vertexCount;
		}
	};
	using GeometryCache = ObjectCache< Geometry, Castor::String, GeometryProducer, GeometryInitialiser, ElementCleaner< Geometry >, ElementMerger< Geometry, Castor::String >, ElementAttacher< MovableObject >, ElementDetacher< MovableObject > >;
	DECLARE_SMART_PTR (GeometryCache);
	/**
	 *\~english
	 *\brief		Creates a Geometry cache.
	 *\param[in]	p_rootNode			The root node.
	 *\param[in]	p_rootCameraNode	The cameras root node.
	 *\param[in]	p_rootObjectNode	The objects root node.
	 *\param[in]	p_get				The engine getter.
	 *\param[in]	p_produce			The element producer.
	 *\~french
	 *\brief		Crée un cache de Geometry.
	 *\param[in]	p_rootNode			Le noeud racine.
	 *\param[in]	p_rootCameraNode	Le noeud racine des caméras.
	 *\param[in]	p_rootObjectNode	Le noeud racine des objets.
	 *\param[in]	p_get				Le récupérteur de moteur.
	 *\param[in]	p_produce			Le créateur d'objet.
	 */
	template<>
	inline std::unique_ptr< GeometryCache >
	MakeObjectCache< Geometry, Castor::String, GeometryProducer, MovableAttacher, MovableDetacher, GeometryInitialiser, ElementCleaner< Geometry >, ElementMerger< Geometry, Castor::String > >
		( SceneNodeSPtr p_rootNode, SceneNodeSPtr p_rootCameraNode , SceneNodeSPtr p_rootObjectNode, Engine & p_engine, Scene & p_scene, GeometryProducer && p_produce )
	{
		return std::make_unique< GeometryCache >( p_rootNode, p_rootCameraNode, p_rootObjectNode, p_engine, p_scene, std::move( p_produce ) );
	}
}

#endif
