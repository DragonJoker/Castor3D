/*
See LICENSE file in root folder
*/
#ifndef ___C3D_CAMERA_CACHE_H___
#define ___C3D_CAMERA_CACHE_H___

#include "Castor3DPrerequisites.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		04/02/2016
	\version	0.8.0
	\~english
	\brief		Helper structure to specialise a scene cache behaviour.
	\remarks	Specialisation for Camera.
	\~french
	\brief		Structure permettant de spécialiser le comportement d'un cache de scène.
	\remarks	Spécialisation pour Camera.
	*/
	template< typename KeyType >
	struct ObjectCacheTraits< Camera, KeyType >
	{
		C3D_API static const castor::String Name;
		using Producer = std::function< std::shared_ptr< Camera >( KeyType const &, SceneNodeSPtr, Viewport && ) >;
		using Merger = std::function< void( ObjectCacheBase< Camera, KeyType > const &
											, castor::Collection< Camera, KeyType > &
											, std::shared_ptr< Camera >
											, SceneNodeSPtr
											, SceneNodeSPtr ) >;
	};
}

// included after because it depends on CacheTraits
#include "Cache/ObjectCache.hpp"

#endif
