/*
See LICENSE file in root folder
*/
#ifndef ___C3D_CacheModule_H___
#define ___C3D_CacheModule_H___

#include "Castor3D/Event/Frame/FrameEventModule.hpp"
#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Model/Mesh/MeshModule.hpp"
#include "Castor3D/Overlay/OverlayModule.hpp"
#include "Castor3D/Plugin/PluginModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Scene/SceneModule.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"
#include "Castor3D/Scene/Animation/AnimationModule.hpp"
#include "Castor3D/Scene/ParticleSystem/ParticleModule.hpp"
#include "Castor3D/Render/Technique/TechniqueModule.hpp"
#include "Castor3D/Render/Ssao/SsaoModule.hpp"

namespace castor3d
{
	/**@name Cache */
	//@{

	static const xchar * InfoCacheCreatedObject = cuT( "Cache::create - Created " );
	static const xchar * WarningCacheDuplicateObject = cuT( "Cache::create - Duplicate " );
	static const xchar * WarningCacheNullObject = cuT( "Cache::Insert - nullptr " );
	/**
	*\~english
	*\brief
	*	Base class for an element cache.
	*\~french
	*\brief
	*	Classe de base pour un cache d'éléments.
	*/
	template< typename ElementType, typename KeyType >
	class CacheBase;
	/**
	*\~english
	*\brief
	*	Base class for an element cache.
	*\~french
	*\brief
	*	Classe de base pour un cache d'éléments.
	*/
	template< typename ElementType, typename KeyType >
	class Cache;
	/**
	*\~english
	*\brief
	*	Helper structure to specialise a cache behaviour.
	*\remarks
	*	Must hold:
	*	<ul>
	*	<li>Name: The element type name.</li>
	*	<li>Producer: The element creation function prototype.</li>
	*	<li>Merger: The prototype of the function use to merge a cache element into another cache.</li>
	*	</ul>
	*\~french
	*\brief
	*	Structure permettant de spécialiser le comportement d'un cache.
	*\remarks
	*	Doit contenir:
	*	<ul>
	*	<li>Name: Le nom du type d'élément.</li>
	*	<li>Producer: Le prototype de la fonction de création d'un élément.</li>
	*	<li>Merger: Le prototype de la fonction pour fusionner un élément d'un cache dans un autre cache.</li>
	*	</ul>
	*/
	template< typename ElementType, typename KeyType >
	struct CacheTraits;
	/**
	*\~english
	*\brief
	*	Helper structure to specialise a cache behaviour.
	*\remarks
	*	Specialisation for AnimatedObjectGroup.
	*\~french
	*\brief
	*	Structure permettant de spécialiser le comportement d'un cache.
	*\remarks
	*	Spécialisation pour AnimatedObjectGroup.
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
	*\~english
	*\brief
	*	Helper structure to specialise a cache behaviour.
	*\remarks
	*	Specialisation for FrameListener.
	*\~french
	*\brief
	*	Structure permettant de spécialiser le comportement d'un cache.
	*\remarks
	*	Spécialisation pour FrameListener.
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
	*\~english
	*\brief
	*	Helper structure to specialise a cache behaviour.
	*\remarks
	*	Specialisation for Material.
	*\~french
	*\brief
	*	Structure permettant de spécialiser le comportement d'un cache.
	*\remarks
	*	Spécialisation pour Material.
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
	*\~english
	*\brief
	*	Helper structure to specialise a cache behaviour.
	*\remarks
	*	Specialisation for Mesh.
	*\~french
	*\brief
	*	Structure permettant de spécialiser le comportement d'un cache.
	*\remarks
	*	Spécialisation pour Mesh.
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
	*\~english
	*\brief
	*	Helper structure to specialise a cache behaviour.
	*\remarks
	*	Specialisation for Overlay.
	*\~french
	*\brief
	*	Structure permettant de spécialiser le comportement d'un cache.
	*\remarks
	*	Spécialisation pour Overlay.
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
	*\~english
	*\brief
	*	Helper structure to specialise a cache behaviour.
	*\remarks
	*	Specialisation for Plugin.
	*\~french
	*\brief
	*	Structure permettant de spécialiser le comportement d'un cache.
	*\remarks
	*	Spécialisation pour Plugin.
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
	*\~english
	*\brief
	*	Helper structure to specialise a cache behaviour.
	*\remarks
	*	Specialisation for RenderTechnique.
	*\~french
	*\brief
	*	Structure permettant de spécialiser le comportement d'un cache.
	*\remarks
	*	Spécialisation pour RenderTechnique.
	*/
	template< typename KeyType >
	struct CacheTraits< RenderTechnique, KeyType >
	{
		C3D_API static const castor::String Name;
		using Producer = std::function < std::shared_ptr< RenderTechnique >( KeyType const &
			, castor::String const &
			, RenderTarget &
			, Parameters const &
			, SsaoConfig const & ) >;
		using Merger = std::function< void( CacheBase< RenderTechnique, KeyType > const &
			, castor::Collection< RenderTechnique, KeyType > &
			, std::shared_ptr< RenderTechnique > ) >;
	};
	/**
	*\~english
	*\brief
	*	Helper structure to specialise a cache behaviour.
	*\remarks
	*	Specialisation for RenderWindow.
	*\~french
	*\brief
	*	Structure permettant de spécialiser le comportement d'un cache.
	*\remarks
	*	Spécialisation pour RenderWindow.
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
	/**
	*\~english
	*\brief
	*	Helper structure to specialise a cache behaviour.
	*\remarks
	*	Specialisation for Sampler.
	*\~french
	*\brief
	*	Structure permettant de spécialiser le comportement d'un cache.
	*\remarks
	*	Spécialisation pour Sampler.
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
	*\~english
	*\brief
	*	Helper structure to specialise a cache behaviour.
	*\remarks
	*	Specialisation for Scene.
	*\~french
	*\brief
	*	Structure permettant de spécialiser le comportement d'un cache.
	*\remarks
	*	Spécialisation pour Scene.
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
	*\~english
	*\brief
	*	View on a resource cache.
	*\remarks
	*	Allows deletion of elements created through the view, and only those.
	*\~french
	*\brief
	*	Vue sur un cache de ressources.
	*\remarks
	*	Permet de supprimer tous les éléments créés via la vue et uniquement ceux là.
	*/
	template< typename ResourceType, typename CacheType, EventType EventType >
	class CacheView;
	/**
	*\~english
	*\brief
	*	Base class for a scene element cache.
	*\~french
	*\brief
	*	Classe de base pour un cache d'éléments de scène.
	*/
	template< typename ElementType, typename KeyType >
	class ObjectCacheBase;
	/**
	*\~english
	*\brief
	*	Base class for a scene element cache.
	*\~french
	*\brief
	*	Classe de base pour un cache d'éléments de scène.
	*/
	template< typename ElementType, typename KeyType >
	class ObjectCache;
	/**
	*\~english
	*\brief
	*	Helper structure to specialise a scene objects cache behaviour.
	*\remarks
	*	Must hold:
	*	<ul>
	*	<li>Name: The element type name.</li>
	*	<li>Producer: The element creation function prototype.</li>
	*	<li>Merger: The prototype of the function use to merge a cache element into another cache.</li>
	*	</ul>
	*\~french
	*\brief
	*	Structure permettant de spécialiser le comportement d'un cache d'objets de scène.
	*\remarks
	*	Doit contenir:
	*	<ul>
	*	<li>Name: Le nom du type d'élément.</li>
	*	<li>Producer: Le prototype de la fonction de création d'un élément.</li>
	*	<li>Merger: Le prototype de la fonction pour fusionner un élément d'un cache dans un autre cache.</li>
	*	</ul>
	*/
	template< typename ElementType, typename KeyType >
	struct ObjectCacheTraits;
	/**
	*\~english
	*\brief
	*	Helper structure to specialise a scene objects cache behaviour.
	*\remarks
	*	Specialisation for BillboardList.
	*\~french
	*\brief
	*	Structure permettant de spécialiser le comportement d'un cache d'objets de scène.
	*\remarks
	*	Spécialisation pour BillboardList.
	*/
	template< typename KeyType >
	struct ObjectCacheTraits< BillboardList, KeyType >
	{
		C3D_API static const castor::String Name;
		using Producer = std::function< std::shared_ptr< BillboardList >( KeyType const &, SceneNode & ) >;
		using Merger = std::function< void( ObjectCacheBase< BillboardList, KeyType > const &
			, castor::Collection< BillboardList, KeyType > &
			, std::shared_ptr< BillboardList >
			, SceneNodeSPtr
			, SceneNodeSPtr ) >;
	};
	/**
	*\~english
	*\brief
	*	Helper structure to specialise a scene objects cache behaviour.
	*\remarks
	*	Specialisation for Camera.
	*\~french
	*\brief
	*	Structure permettant de spécialiser le comportement d'un cache d'objets de scène.
	*\remarks
	*	Spécialisation pour Camera.
	*/
	template< typename KeyType >
	struct ObjectCacheTraits< Camera, KeyType >
	{
		C3D_API static const castor::String Name;
		using Producer = std::function< std::shared_ptr< Camera >( KeyType const &, SceneNode &, Viewport && ) >;
		using Merger = std::function< void( ObjectCacheBase< Camera, KeyType > const &
			, castor::Collection< Camera, KeyType > &
			, std::shared_ptr< Camera >
			, SceneNodeSPtr
			, SceneNodeSPtr ) >;
	};
	/**
	*\~english
	*\brief
	*	Helper structure to specialise a scene objects cache behaviour.
	*\remarks
	*	Specialisation for Geometry.
	*\~french
	*\brief
	*	Structure permettant de spécialiser le comportement d'un cache d'objets de scène.
	*\remarks
	*	Spécialisation pour Geometry.
	*/
	template< typename KeyType >
	struct ObjectCacheTraits< Geometry, KeyType >
	{
		C3D_API static const castor::String Name;
		using Producer = std::function< std::shared_ptr< Geometry >( KeyType const &, SceneNode &, MeshSPtr ) >;
		using Merger = std::function< void( ObjectCacheBase< Geometry, KeyType > const &
			, castor::Collection< Geometry, KeyType > &
			, std::shared_ptr< Geometry >
			, SceneNodeSPtr
			, SceneNodeSPtr ) >;
	};
	/**
	*\~english
	*\brief
	*	Helper structure to specialise a scene objects cache behaviour.
	*\remarks
	*	Specialisation for Light.
	*\~french
	*\brief
	*	Structure permettant de spécialiser le comportement d'un cache d'objets de scène.
	*\remarks
	*	Spécialisation pour Light.
	*/
	template< typename KeyType >
	struct ObjectCacheTraits< Light, KeyType >
	{
		C3D_API static const castor::String Name;
		using Producer = std::function< std::shared_ptr< Light >( KeyType const &, SceneNode &, LightType ) >;
		using Merger = std::function< void( ObjectCacheBase< Light, KeyType > const &
			, castor::Collection< Light, KeyType > &
			, std::shared_ptr< Light >
			, SceneNodeSPtr
			, SceneNodeSPtr ) >;
	};
	/**
	*\~english
	*\brief
	*	Helper structure to specialise a scene objects cache behaviour.
	*\remarks
	*	Specialisation for ParticleSystem.
	*\~french
	*\brief
	*	Structure permettant de spécialiser le comportement d'un cache d'objets de scène.
	*\remarks
	*	Spécialisation pour ParticleSystem.
	*/
	template< typename KeyType >
	struct ObjectCacheTraits< ParticleSystem, KeyType >
	{
		C3D_API static const castor::String Name;
		using Producer = std::function< std::shared_ptr< ParticleSystem >( KeyType const &, SceneNode &, uint32_t ) >;
		using Merger = std::function< void( ObjectCacheBase< ParticleSystem, KeyType > const &
			, castor::Collection< ParticleSystem, KeyType > &
			, std::shared_ptr< ParticleSystem >
			, SceneNodeSPtr
			, SceneNodeSPtr ) >;
	};
	/**
	*\~english
	*\brief
	*	Helper structure to specialise a scene objects cache behaviour.
	*\remarks
	*	Specialisation for SceneNode.
	*\~french
	*\brief
	*	Structure permettant de spécialiser le comportement d'un cache d'objets de scène.
	*\remarks
	*	Spécialisation pour SceneNode.
	*/
	template< typename KeyType >
	struct ObjectCacheTraits< SceneNode, KeyType >
	{
		C3D_API static const castor::String Name;
		using Producer = std::function< std::shared_ptr< SceneNode >( KeyType const &, SceneNode & ) >;
		using Merger = std::function< void( ObjectCacheBase< SceneNode, KeyType > const &
			, castor::Collection< SceneNode, KeyType > &
			, std::shared_ptr< SceneNode >
			, SceneNodeSPtr
			, SceneNodeSPtr ) >;
	};

	template< typename ElementType >
	using ElementInitialiser = std::function< void( std::shared_ptr< ElementType > ) >;

	template< typename ElementType >
	using ElementCleaner = std::function< void( std::shared_ptr< ElementType > ) >;

	template< typename ElementType, typename KeyType >
	using ElementMerger = std::function< void( CacheBase< ElementType, KeyType > const &, castor::Collection< ElementType, KeyType > &, std::shared_ptr< ElementType > ) >;

	template< typename ElementType >
	using ElementAttacher = std::function< void( std::shared_ptr< ElementType >, SceneNode &, SceneNodeSPtr, SceneNodeSPtr, SceneNodeSPtr ) >;

	template< typename ElementType >
	using ElementDetacher = std::function< void( std::shared_ptr< ElementType > ) >;

	using OnCacheChangedFunction = std::function< void() >;
	using OnCacheChanged = castor::Signal < OnCacheChangedFunction >;


	using AnimatedObjectGroupCache = Cache< AnimatedObjectGroup, castor::String >;
	using BillboardListCache = ObjectCache< BillboardList, castor::String >;
	using CameraCache = ObjectCache< Camera, castor::String >;
	using GeometryCache = ObjectCache< Geometry, castor::String >;
	using LightCache = ObjectCache< Light, castor::String >;
	using FrameListenerCache = Cache< FrameListener, castor::String >;
	class MaterialCache;
	using MeshCache = Cache< Mesh, castor::String >;
	using OverlayCache = Cache< Overlay, castor::String >;
	using ParticleSystemCache = ObjectCache< ParticleSystem, castor::String >;
	using PluginCache = Cache< Plugin, castor::String >;
	using SamplerCache = Cache< Sampler, castor::String >;
	using SceneCache = Cache< Scene, castor::String >;
	using SceneNodeCache = ObjectCache< SceneNode, castor::String >;
	class RenderTargetCache;
	using RenderTechniqueCache = Cache< RenderTechnique, castor::String >;
	using RenderWindowCache = Cache< RenderWindow, castor::String >;
	class ShaderProgramCache;

	CU_DeclareSmartPtr( AnimatedObjectGroupCache );
	CU_DeclareSmartPtr( BillboardListCache );
	CU_DeclareSmartPtr( CameraCache );
	CU_DeclareSmartPtr( GeometryCache );
	CU_DeclareSmartPtr( LightCache );
	CU_DeclareSmartPtr( FrameListenerCache );
	CU_DeclareSmartPtr( MaterialCache );
	CU_DeclareSmartPtr( MeshCache );
	CU_DeclareSmartPtr( OverlayCache );
	CU_DeclareSmartPtr( ParticleSystemCache );
	CU_DeclareSmartPtr( PluginCache );
	CU_DeclareSmartPtr( SamplerCache );
	CU_DeclareSmartPtr( RenderTargetCache );
	CU_DeclareSmartPtr( RenderTechniqueCache );
	CU_DeclareSmartPtr( RenderWindowCache );
	CU_DeclareSmartPtr( SceneCache );
	CU_DeclareSmartPtr( SceneNodeCache );
	CU_DeclareSmartPtr( ShaderProgramCache );

	//@}
}

#define DECLARE_CACHE_MEMBER( memberName, className )\
	public:\
		inline className##Cache & get##className##Cache()\
		{\
			return *m_##memberName##Cache;\
		}\
		inline className##Cache const & get##className##Cache()const\
		{\
			return *m_##memberName##Cache;\
		}\
	private:\
		className##Cache##SPtr m_##memberName##Cache

#define DECLARE_OBJECT_CACHE_MEMBER( memberName, className )\
	public:\
		inline className##Cache & get##className##Cache()\
		{\
			return *m_##memberName##Cache;\
		}\
		inline className##Cache const & get##className##Cache()const\
		{\
			return *m_##memberName##Cache;\
		}\
	private:\
		castor::Connection< OnCacheChanged > m_on##className##Changed;\
		className##Cache##SPtr m_##memberName##Cache

#define DECLARE_CACHE_VIEW_MEMBER( memberName, className, eventType )\
	public:\
		inline castor3d::CacheView< className, className##Cache, eventType > & get##className##View()\
		{\
			return *m_##memberName##CacheView;\
		}\
		inline castor3d::CacheView< className, className##Cache, eventType > const & get##className##View()const\
		{\
			return *m_##memberName##CacheView;\
		}\
	private:\
		std::shared_ptr< castor3d::CacheView< className, className##Cache, eventType > > m_##memberName##CacheView

#define DECLARE_CU_CACHE_VIEW_MEMBER( memberName, className, eventType )\
	public:\
		inline castor3d::CacheView< castor::className, castor::className##Cache, eventType > & get##className##View()\
		{\
			return *m_##memberName##CacheView;\
		}\
		inline castor3d::CacheView< castor::className, castor::className##Cache, eventType > const & get##className##View()const\
		{\
			return *m_##memberName##CacheView;\
		}\
	private:\
		std::shared_ptr< castor3d::CacheView< castor::className, castor::className##Cache, eventType > > m_##memberName##CacheView

#endif
