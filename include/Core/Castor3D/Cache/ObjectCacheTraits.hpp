/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ObjectCacheTraits_H___
#define ___C3D_ObjectCacheTraits_H___

#include "Castor3D/Cache/CacheTraits.hpp"

#include "Castor3D/Scene/SceneModule.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"
#include "Castor3D/Scene/ParticleSystem/ParticleModule.hpp"

namespace castor3d
{
	/**@name Cache */
	//@{

	/**
	\~english
	\brief		Helper structure to specialise a scene cache behaviour.
	\remarks	Specialisation for BillboardList.
	\~french
	\brief		Structure permettant de spécialiser le comportement d'un cache de scène.
	\remarks	Spécialisation pour BillboardList.
	*/
	template< typename KeyType >
	struct ObjectCacheTraits< BillboardList, KeyType >
	{
		C3D_API static const castor::String Name;
		using Producer = std::function< std::shared_ptr< BillboardList >( KeyType const &, SceneNodeSPtr ) >;
		using Merger = std::function< void( ObjectCacheBase< BillboardList, KeyType > const &
			, castor::Collection< BillboardList, KeyType > &
			, std::shared_ptr< BillboardList >
			, SceneNodeSPtr
			, SceneNodeSPtr ) >;
	};
	/**
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
	/**
	\~english
	\brief		Helper structure to specialise a scene cache behaviour.
	\remarks	Specialisation for Geometry.
	\~french
	\brief		Structure permettant de spécialiser le comportement d'un cache de scène.
	\remarks	Spécialisation pour Geometry.
	*/
	template< typename KeyType >
	struct ObjectCacheTraits< Geometry, KeyType >
	{
		C3D_API static const castor::String Name;
		using Producer = std::function< std::shared_ptr< Geometry >( KeyType const &, SceneNodeSPtr, MeshSPtr ) >;
		using Merger = std::function< void( ObjectCacheBase< Geometry, KeyType > const &
			, castor::Collection< Geometry, KeyType > &
			, std::shared_ptr< Geometry >
			, SceneNodeSPtr
			, SceneNodeSPtr ) >;
	};
	/**
	\~english
	\brief		Helper structure to specialise a scene cache behaviour.
	\remarks	Specialisation for Light.
	\~french
	\brief		Structure permettant de spécialiser le comportement d'un cache de scène.
	\remarks	Spécialisation pour Light.
	*/
	template< typename KeyType >
	struct ObjectCacheTraits< Light, KeyType >
	{
		C3D_API static const castor::String Name;
		using Producer = std::function< std::shared_ptr< Light >( KeyType const &, SceneNodeSPtr, LightType ) >;
		using Merger = std::function< void( ObjectCacheBase< Light, KeyType > const &
			, castor::Collection< Light, KeyType > &
			, std::shared_ptr< Light >
			, SceneNodeSPtr
			, SceneNodeSPtr ) >;
	};
	/**
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
	/**
	\~english
	\brief		Helper structure to specialise a scene cache behaviour.
	\remarks	Specialisation for SceneNode.
	\~french
	\brief		Structure permettant de spécialiser le comportement d'un cache de scène.
	\remarks	Spécialisation pour SceneNode.
	*/
	template< typename KeyType >
	struct ObjectCacheTraits< SceneNode, KeyType >
	{
		C3D_API static const castor::String Name;
		using Producer = std::function< std::shared_ptr< SceneNode >( KeyType const & ) >;
		using Merger = std::function< void( ObjectCacheBase< SceneNode, KeyType > const &
			, castor::Collection< SceneNode, KeyType > &
			, std::shared_ptr< SceneNode >
			, SceneNodeSPtr
			, SceneNodeSPtr ) >;
	};

	//@}
}

#include "Castor3D/Cache/ObjectCache.hpp"

#endif
