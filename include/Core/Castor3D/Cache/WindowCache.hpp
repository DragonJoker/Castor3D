/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderWindowCache_H___
#define ___C3D_RenderWindowCache_H___

#include "CacheModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"

namespace castor3d
{
	/**
	\~english
	\brief		Helper structure to specialise a cache behaviour.
	\remarks	Specialisation for RenderWindow.
	\~french
	\brief		Structure permettant de spécialiser le comportement d'un cache.
	\remarks	Spécialisation pour RenderWindow.
	*/
	template< typename KeyType >
	struct CacheTraits< RenderWindow, KeyType >
	{
		C3D_API static const castor::String Name;
		using Producer = std::function< std::shared_ptr< RenderWindow >( KeyType const & ) >;
		using Merger = std::function< void( CacheBase< RenderWindow, KeyType > const &
			, castor::Collection< RenderWindow, KeyType > &
			, std::shared_ptr< RenderWindow > ) >;
	};
}

// included after because it depends on CacheTraits
#include "Castor3D/Cache/Cache.hpp"

#endif
