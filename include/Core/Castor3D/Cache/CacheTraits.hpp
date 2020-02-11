/*
See LICENSE file in root folder
*/
#ifndef ___C3D_CacheTraits_H___
#define ___C3D_CacheTraits_H___

#include "CacheModule.hpp"

#include "Castor3D/Material/Texture/TextureModule.hpp"
#include "Castor3D/Overlay/OverlayModule.hpp"
#include "Castor3D/Plugin/PluginModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Shader/ShaderModule.hpp"
#include "Castor3D/Scene/SceneModule.hpp"
#include "Castor3D/Scene/Animation/AnimationModule.hpp"

namespace castor3d
{
	/**@name Cache */
	//@{

	/**
	\~english
	\brief		Helper structure to specialise a cache behaviour.
	\remarks	Specialisation for AnimatedObjectGroup.
	\~french
	\brief		Structure permettant de spécialiser le comportement d'un cache.
	\remarks	Spécialisation pour AnimatedObjectGroup.
	*/
	template< typename KeyType >
	struct CacheTraits< AnimatedObjectGroup, KeyType >
	{
		C3D_API static const castor::String Name;
		using Producer = std::function< std::shared_ptr< AnimatedObjectGroup >( KeyType const & ) >;
		using Merger = std::function< void( CacheBase< AnimatedObjectGroup, KeyType > const &
			, castor::Collection< AnimatedObjectGroup, KeyType > &
			, std::shared_ptr< AnimatedObjectGroup > ) >;
	};
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
	/**
	\~english
	\brief		Helper structure to specialise a cache behaviour.
	\remarks	Specialisation for Material.
	\~french
	\brief		Structure permettant de spécialiser le comportement d'un cache.
	\remarks	Spécialisation pour Material.
	*/
	template< typename KeyType >
	struct CacheTraits< Material, KeyType >
	{
		C3D_API static const castor::String Name;
		using Producer = std::function< std::shared_ptr< Material >( KeyType const &, MaterialType ) >;
		using Merger = std::function< void( CacheBase< Material, KeyType > const &
			, castor::Collection< Material, KeyType > &
			, std::shared_ptr< Material > ) >;
	};
	/**
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
	/**
	\~english
	\brief		Helper structure to specialise a cache behaviour.
	\remarks	Specialisation for Overlay.
	\~french
	\brief		Structure permettant de spécialiser le comportement d'un cache.
	\remarks	Spécialisation pour Overlay.
	*/
	template< typename KeyType >
	struct CacheTraits< Overlay, KeyType >
	{
		C3D_API static const castor::String Name;
		using Producer = std::function< std::shared_ptr< Overlay >( KeyType const &, OverlayType, SceneSPtr, OverlaySPtr ) >;
		using Merger = std::function< void( CacheBase< Overlay, KeyType > const &
			, castor::Collection< Overlay, KeyType > &
			, std::shared_ptr< Overlay > ) >;
	};
	/**
	\~english
	\brief		Helper structure to specialise a cache behaviour.
	\remarks	Specialisation for Plugin.
	\~french
	\brief		Structure permettant de spécialiser le comportement d'un cache.
	\remarks	Spécialisation pour Plugin.
	*/
	template< typename KeyType >
	struct CacheTraits< Plugin, KeyType >
	{
		C3D_API static const castor::String Name;
		using Producer = std::function< std::shared_ptr< Plugin >( KeyType const &, PluginType, castor::DynamicLibrarySPtr ) >;
		using Merger = std::function< void( CacheBase< Plugin, KeyType > const &
			, castor::Collection< Plugin, KeyType > &
			, std::shared_ptr< Plugin > ) >;
	};
	/**
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
	/**
	\~english
	\brief		Helper structure to specialise a cache behaviour.
	\remarks	Specialisation for RenderWindow.
	\~french
	\brief		Structure permettant de spécialiser le comportement d'un cache.
	\remarks	Spécialisation pour RenderWindow.
	*/
	template< typename KeyType >
	struct CacheTraits< RenderTechnique, KeyType >
	{
		C3D_API static const castor::String Name;
		using Producer = std::function< std::shared_ptr< RenderTechnique >( KeyType const & ) >;
		using Merger = std::function< void( CacheBase< RenderTechnique, KeyType > const &
			, castor::Collection< RenderTechnique, KeyType > &
			, std::shared_ptr< RenderTechnique > ) >;
	};
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

	//@}
}

#include "Castor3D/Cache/Cache.hpp"

#endif
