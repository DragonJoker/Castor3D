/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SceneCache_H___
#define ___C3D_SceneCache_H___

#include "CacheModule.hpp"
#include "Castor3D/Scene/Scene.hpp"

namespace castor3d
{
	/**
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
#include "Castor3D/Cache/CacheBase.hpp"

#endif
