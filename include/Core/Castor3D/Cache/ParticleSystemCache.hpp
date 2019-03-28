/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PARTICLE_SYSTEM_CACHE_H___
#define ___C3D_PARTICLE_SYSTEM_CACHE_H___

#include "Castor3D/Castor3DPrerequisites.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		04/02/2016
	\version	0.8.0
	\~english
	\brief		Helper structure to specialise a scene cache behaviour.
	\remarks	Specialisation for ParticleSystem.
	\~french
	\brief		Structure permettant de spécialiser le comportement d'un cache de scène.
	\remarks	Spécialisation pour ParticleSystem.
	*/
	template< typename KeyType >
	struct ObjectCacheTraits< ParticleSystem, KeyType >
	{
		C3D_API static const castor::String Name;
		using Producer = std::function< std::shared_ptr< ParticleSystem >( KeyType const &, SceneNodeSPtr, uint32_t ) >;
		using Merger = std::function< void( ObjectCacheBase< ParticleSystem, KeyType > const &
			, castor::Collection< ParticleSystem, KeyType > &
			, std::shared_ptr< ParticleSystem >
			, SceneNodeSPtr
			, SceneNodeSPtr ) >;
	};
}

// included after because it depends on CacheTraits
#include "Castor3D/Cache/ObjectCache.hpp"

#endif
