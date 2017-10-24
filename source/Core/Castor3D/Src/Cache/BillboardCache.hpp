/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BILLBOARD_CACHE_H___
#define ___C3D_BILLBOARD_CACHE_H___

#include "Castor3DPrerequisites.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		04/02/2016
	\version	0.8.0
	\~english
	\brief		Helper structure to specialise a scene cache behaviour.
	\remarks	Specialisation for BillboardList.
	\~french
	\brief		Structure permettant de spécialiser le comportement d'un cache de scène.
	\remarks	Spécialisation pour BillboardList.
	*/
	template< typename KeyType >
	struct ObjectCacheTraits< BillboardList, KeyType >
	{
		C3D_API static const castor::String Name;
		using Producer = std::function< std::shared_ptr< BillboardList >( KeyType const &, SceneNodeSPtr ) >;
		using Merger = std::function< void( ObjectCacheBase< BillboardList, KeyType > const &
											, castor::Collection< BillboardList, KeyType > &
											, std::shared_ptr< BillboardList >
											, SceneNodeSPtr
											, SceneNodeSPtr ) >;
	};
}

// included after because it depends on CacheTraits
#include "Cache/ObjectCache.hpp"

#endif
