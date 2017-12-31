/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SAMPLER_CACHE_H___
#define ___C3D_SAMPLER_CACHE_H___

#include "Castor3DPrerequisites.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		04/02/2016
	\version	0.8.0
	\~english
	\brief		Helper structure to specialise a cache behaviour.
	\remarks	Specialisation for Sampler.
	\~french
	\brief		Structure permettant de spécialiser le comportement d'un cache.
	\remarks	Spécialisation pour Sampler.
	*/
	template< typename KeyType >
	struct CacheTraits< Sampler, KeyType >
	{
		C3D_API static const castor::String Name;
		using Producer = std::function< std::shared_ptr< Sampler >( KeyType const & ) >;
		using Merger = std::function< void( CacheBase< Sampler, KeyType > const &
											, castor::Collection< Sampler, KeyType > &
											, std::shared_ptr< Sampler > ) >;
	};
}

// included after because it depends on CacheTraits
#include "Cache/Cache.hpp"

#endif
