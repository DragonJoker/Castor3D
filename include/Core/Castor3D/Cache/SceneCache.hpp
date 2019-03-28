/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SCENE_CACHE_H___
#define ___C3D_SCENE_CACHE_H___

#include "Castor3D/Castor3DPrerequisites.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		04/02/2016
	\version	0.8.0
	\~english
	\brief		Helper structure to specialise a cache behaviour.
	\remarks	Specialisation for Scene.
	\~french
	\brief		Structure permettant de spécialiser le comportement d'un cache.
	\remarks	Spécialisation pour Scene.
	*/
	template< typename KeyType >
	struct CacheTraits< Scene, KeyType >
	{
		C3D_API static const castor::String Name;
		using Producer = std::function< std::shared_ptr< Scene >( KeyType const & ) >;
		using Merger = std::function< void( CacheBase< Scene, KeyType > const &
											, castor::Collection< Scene, KeyType > &
											, std::shared_ptr< Scene > ) >;
	};
}

// included after because it depends on CacheTraits
#include "Castor3D/Cache/Cache.hpp"

#endif
