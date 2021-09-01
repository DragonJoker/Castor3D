#include "Castor3D/Cache/CacheModule.hpp"

CU_ImplementCUSmartPtr( castor3d, AnimatedObjectGroupCache )
CU_ImplementCUSmartPtr( castor3d, BillboardListCache )
CU_ImplementCUSmartPtr( castor3d, CameraCache )
CU_ImplementCUSmartPtr( castor3d, GeometryCache )
CU_ImplementCUSmartPtr( castor3d, LightCache )
CU_ImplementCUSmartPtr( castor3d, FrameListenerCache )
CU_ImplementCUSmartPtr( castor3d, MaterialCache )
CU_ImplementCUSmartPtr( castor3d, MeshCache )
CU_ImplementCUSmartPtr( castor3d, OverlayCache )
CU_ImplementCUSmartPtr( castor3d, ParticleSystemCache )
CU_ImplementCUSmartPtr( castor3d, PluginCache )
CU_ImplementCUSmartPtr( castor3d, SamplerCache )
CU_ImplementCUSmartPtr( castor3d, RenderTargetCache )
CU_ImplementCUSmartPtr( castor3d, RenderTechniqueCache )
CU_ImplementCUSmartPtr( castor3d, SceneCache )
CU_ImplementCUSmartPtr( castor3d, SceneNodeCache )
CU_ImplementCUSmartPtr( castor3d, ShaderProgramCache )

namespace castor
{
	template<> const String ResourceCacheTraitsT< castor3d::AnimatedObjectGroup, String >::Name = cuT( "AnimatedObjectGroup" );
	template<> const String ResourceCacheTraitsT< castor3d::FrameListener, String >::Name = cuT( "FrameListener" );
	template<> const String ResourceCacheTraitsT< castor3d::Material, String >::Name = cuT( "Material" );
	template<> const String ResourceCacheTraitsT< castor3d::Mesh, String >::Name = cuT( "Mesh" );
	template<> const String ResourceCacheTraitsT< castor3d::Overlay, String >::Name = cuT( "Overlay" );
	template<> const String ResourceCacheTraitsT< castor3d::Plugin, String >::Name = cuT( "Plugin" );
	template<> const String ResourceCacheTraitsT< castor3d::Sampler, String >::Name = cuT( "Sampler" );
	template<> const String ResourceCacheTraitsT< castor3d::Scene, String >::Name = cuT( "Scene" );
}

namespace castor3d
{
	template<> const castor::String ObjectCacheTraitsT< BillboardList, castor::String >::Name = cuT( "BillboardList" );
	template<> const castor::String ObjectCacheTraitsT< Camera, castor::String >::Name = cuT( "Camera" );
	template<> const castor::String ObjectCacheTraitsT< Geometry, castor::String >::Name = cuT( "Geometry" );
	template<> const castor::String ObjectCacheTraitsT< Light, castor::String >::Name = cuT( "Light" );
	template<> const castor::String ObjectCacheTraitsT< ParticleSystem, castor::String >::Name = cuT( "ParticleSystem" );
	template<> const castor::String ObjectCacheTraitsT< SceneNode, castor::String >::Name = cuT( "SceneNode" );
}
