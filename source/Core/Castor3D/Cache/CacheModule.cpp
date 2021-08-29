#include "Castor3D/Cache/CacheModule.hpp"

namespace castor3d
{
	template<> const castor::String CacheTraits< FrameListener, castor::String >::Name = cuT( "FrameListener" );
	template<> const castor::String CacheTraits< Material, castor::String >::Name = cuT( "Material" );
	template<> const castor::String CacheTraits< Mesh, castor::String >::Name = cuT( "Mesh" );
	template<> const castor::String CacheTraits< Overlay, castor::String >::Name = cuT( "Overlay" );
	template<> const castor::String CacheTraits< Plugin, castor::String >::Name = cuT( "Plugin" );
	template<> const castor::String CacheTraits< Sampler, castor::String >::Name = cuT( "Sampler" );
	template<> const castor::String CacheTraits< Scene, castor::String >::Name = cuT( "Scene" );

	template<> const castor::String CacheTraits< AnimatedObjectGroup, castor::String >::Name = cuT( "AnimatedObjectGroup" );
	template<> const castor::String ObjectCacheTraits< BillboardList, castor::String >::Name = cuT( "BillboardList" );
	template<> const castor::String ObjectCacheTraits< Camera, castor::String >::Name = cuT( "Camera" );
	template<> const castor::String ObjectCacheTraits< Geometry, castor::String >::Name = cuT( "Geometry" );
	template<> const castor::String ObjectCacheTraits< Light, castor::String >::Name = cuT( "Light" );
	template<> const castor::String ObjectCacheTraits< ParticleSystem, castor::String >::Name = cuT( "ParticleSystem" );
	template<> const castor::String ObjectCacheTraits< SceneNode, castor::String >::Name = cuT( "SceneNode" );
}
