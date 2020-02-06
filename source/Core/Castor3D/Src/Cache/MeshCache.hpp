/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MESH_CACHE_H___
#define ___C3D_MESH_CACHE_H___

#include "Castor3DPrerequisites.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		04/02/2016
	\version	0.8.0
	\~english
	\brief		Helper structure to specialise a cache behaviour.
	\remarks	Specialisation for Mesh.
	\~french
	\brief		Structure permettant de spécialiser le comportement d'un cache.
	\remarks	Spécialisation pour Mesh.
	*/
	template< typename KeyType >
	struct CacheTraits< Mesh, KeyType >
	{
		C3D_API static const castor::String Name;
		using Producer = std::function< std::shared_ptr< Mesh >( KeyType const & ) >;
		using Merger = std::function< void( CacheBase< Mesh, KeyType > const &
											, castor::Collection< Mesh, KeyType > &
											, std::shared_ptr< Mesh > ) >;
	};
}

// included after because it depends on CacheTraits
#include "Cache/Cache.hpp"

#endif
