/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ListenerCache_H___
#define ___C3D_ListenerCache_H___

#include "CacheModule.hpp"
#include "Castor3D/Event/Frame/FrameEventModule.hpp"

namespace castor3d
{
	/**
	\~english
	\brief		Helper structure to specialise a cache behaviour.
	\remarks	Specialisation for FrameListener.
	\~french
	\brief		Structure permettant de spécialiser le comportement d'un cache.
	\remarks	Spécialisation pour FrameListener.
	*/
	template< typename KeyType >
	struct CacheTraits< FrameListener, KeyType >
	{
		C3D_API static const castor::String Name;
		using Producer = std::function< std::shared_ptr< FrameListener >( KeyType const & ) >;
		using Merger = std::function< void( CacheBase< FrameListener, KeyType > const &
			, castor::Collection< FrameListener, KeyType > &
			, std::shared_ptr< FrameListener > ) >;
	};
}

// included after because it depends on CacheTraits
#include "Castor3D/Cache/Cache.hpp"

#endif
