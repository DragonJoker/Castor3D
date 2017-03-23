/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___C3D_GEOMETRY_CACHE_H___
#define ___C3D_GEOMETRY_CACHE_H___

#include "Cache/ObjectCache.hpp"
#include "Render/RenderInfo.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		04/02/2016
	\version	0.8.0
	\~english
	\brief		Helper structure to specialise a scene cache behaviour.
	\remarks	Specialisation for Geometry.
	\~french
	\brief		Structure permettant de spécialiser le comportement d'un cache de scène.
	\remarks	Spécialisation pour Geometry.
	*/
	template< typename KeyType >
	struct ObjectCacheTraits< Geometry, KeyType >
	{
		C3D_API static const Castor::String Name;
		using Producer = std::function< std::shared_ptr< Geometry >( KeyType const &, SceneNodeSPtr, MeshSPtr ) >;
		using Merger = std::function< void( ObjectCacheBase< Geometry, KeyType > const &
			, Castor::Collection< Geometry, KeyType > &
			, std::shared_ptr< Geometry >
			, SceneNodeSPtr
			, SceneNodeSPtr ) >;
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
	class ObjectCache< Geometry, Castor::String >
		: public ObjectCacheBase< Geometry, Castor::String >
	{
	public:
		using MyObjectCache = ObjectCacheBase< Geometry, Castor::String >;
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
		 *\param[in]	p_engine			The engine.
		 *\param[in]	p_scene				The scene.
		 *\param[in]	p_rootNode			The root node.
		 *\param[in]	p_rootCameraNode	The cameras root node.
		 *\param[in]	p_rootObjectNode	The objects root node.
		 *\param[in]	p_produce			The element producer.
		 *\param[in]	p_initialise		The element initialiser.
		 *\param[in]	p_clean				The element cleaner.
		 *\param[in]	p_merge				The element collection merger.
		 *\param[in]	p_attach			The element attacher (to a scene node).
		 *\param[in]	p_detach			The element detacher (from a scene node).
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_engine			Le moteur.
		 *\param[in]	p_scene				La scène.
		 *\param[in]	p_rootNode			Le noeud racine.
		 *\param[in]	p_rootCameraNode	Le noeud racine des caméras.
		 *\param[in]	p_rootObjectNode	Le noeud racine des objets.
		 *\param[in]	p_produce			Le créateur d'objet.
		 *\param[in]	p_initialise		L'initialiseur d'objet.
		 *\param[in]	p_clean				Le nettoyeur d'objet.
		 *\param[in]	p_merge				Le fusionneur de collection d'objets.
		 *\param[in]	p_attach			L'attacheur d'objet (à un noeud de scène).
		 *\param[in]	p_detach			Le détacheur d'objet (d'un noeud de scène).
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
			, Detacher && p_detach = Detacher{} );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~ObjectCache();
		/**
		 *\~english
		 *\return		The objects count.
		 *\~french
		 *\return		Le nombre d'objets
		 */
		C3D_API void FillInfo( RenderInfo & p_info )const;
		/**
		 *\~english
		 *\brief		Creates an object.
		 *\param[in]	p_name		The object name.
		 *\param[in]	p_parent	The parent scene node.
		 *\param[in]	p_mesh		The mesh.
		 *\return		The created object.
		 *\~french
		 *\brief		Crée un objet.
		 *\param[in]	p_name		Le nom d'objet.
		 *\param[in]	p_parent	Le noeud de scène parent.
		 *\param[in]	p_mesh		Le maillage.
		 *\return		L'objet créé.
		 */
		inline ElementPtr Add( Key const & p_name, SceneNodeSPtr p_parent, MeshSPtr p_mesh )
		{
			return MyObjectCache::Add( p_name, p_parent, p_mesh );
		}

	private:
		//!\~english	The total faces count.
		//!\~french		Le nombre total de faces.
		uint32_t m_faceCount{ 0 };
		//!\~english	The total vertex count.
		//!\~french		Le nombre total de sommets.
		uint32_t m_vertexCount{ 0 };
	};
	using GeometryCache = ObjectCache< Geometry, Castor::String >;
	DECLARE_SMART_PTR( GeometryCache );
}

#endif
