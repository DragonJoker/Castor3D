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
		using MyObjectCacheType = ObjectCacheBase< Geometry, Castor::String >;
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
		*\return		The faces count for all objects.
		*\~french
		*\return		Le nombre de faces de tous les objets.
		*/
		inline uint32_t GetFaceCount()const
		{
			return m_faceCount;
		}
		/**
		*\~english
		*\return		The vertex count for all objects.
		*\~french
		*\return		Le nombre de sommets de tous les objets.
		*/
		inline uint32_t GetVertexCount()const
		{
			return m_vertexCount;
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
