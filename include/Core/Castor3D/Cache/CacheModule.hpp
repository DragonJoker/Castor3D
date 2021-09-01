/*
See LICENSE file in root folder
*/
#ifndef ___C3D_CacheModule_H___
#define ___C3D_CacheModule_H___

#include "Castor3D/Event/Frame/FrameEventModule.hpp"
#include "Castor3D/Material/MaterialModule.hpp"
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

#include <CastorUtils/Design/ResourceCache.hpp>

#include <functional>

namespace castor
{
	/**
	*\~english
	*	Helper structure to specialise a cache behaviour.
	*\remarks
	*	Specialisation for AnimatedObjectGroup.
	*\~french
	*	Structure permettant de spécialiser le comportement d'un cache.
	*\remarks
	*	Spécialisation pour AnimatedObjectGroup.
	*/
	template< typename KeyT >
	struct ResourceCacheTraitsT< castor3d::AnimatedObjectGroup, KeyT >
		: ResourceCacheTraitsBaseT< castor3d::AnimatedObjectGroup, KeyT >
	{
		using Base = ResourceCacheTraitsBaseT< castor3d::AnimatedObjectGroup, KeyT >;
		using ElementT = typename Base::ElementT;
		using ElementPtrT = typename Base::ElementPtrT;

		C3D_API static const String Name;

		using ElementProducerT = std::function< ElementPtrT( KeyT const & ) >;
	};
	/**
	*\~english
	*	Helper structure to specialise a cache behaviour.
	*\remarks
	*	Specialisation for FrameListener.
	*\~french
	*	Structure permettant de spécialiser le comportement d'un cache.
	*\remarks
	*	Spécialisation pour FrameListener.
	*/
	template< typename KeyT >
	struct ResourceCacheTraitsT< castor3d::FrameListener, KeyT >
		: ResourceCacheTraitsBaseT< castor3d::FrameListener, KeyT >
	{
		using Base = ResourceCacheTraitsBaseT< castor3d::FrameListener, KeyT >;
		using ElementT = typename Base::ElementT;
		using ElementPtrT = typename Base::ElementPtrT;

		C3D_API static const String Name;

		using ElementProducerT = std::function< ElementPtrT( KeyT const & ) >;
	};
	/**
	*\~english
	*	Helper structure to specialise a cache behaviour.
	*\remarks
	*	Specialisation for Material.
	*\~french
	*	Structure permettant de spécialiser le comportement d'un cache.
	*\remarks
	*	Spécialisation pour Material.
	*/
	template< typename KeyT >
	struct ResourceCacheTraitsT< castor3d::Material, KeyT >
		: ResourceCacheTraitsBaseT< castor3d::Material, KeyT >
	{
		using Base = ResourceCacheTraitsBaseT< castor3d::Material, KeyT >;
		using ElementT = typename Base::ElementT;
		using ElementPtrT = typename Base::ElementPtrT;

		C3D_API static const String Name;

		using ElementProducerT = std::function< ElementPtrT( KeyT const &
			, castor3d::PassTypeID ) >;
	};
	/**
	*\~english
	*	Helper structure to specialise a cache behaviour.
	*\remarks
	*	Specialisation for Mesh.
	*\~french
	*	Structure permettant de spécialiser le comportement d'un cache.
	*\remarks
	*	Spécialisation pour Mesh.
	*/
	template< typename KeyT >
	struct ResourceCacheTraitsT< castor3d::Mesh, KeyT >
		: ResourceCacheTraitsBaseT< castor3d::Mesh, KeyT >
	{
		using Base = ResourceCacheTraitsBaseT< castor3d::Mesh, KeyT >;
		using ElementT = typename Base::ElementT;
		using ElementPtrT = typename Base::ElementPtrT;

		C3D_API static const String Name;

		using ElementProducerT = std::function< ElementPtrT( KeyT const & ) >;
	};
	/**
	*\~english
	*	Helper structure to specialise a cache behaviour.
	*\remarks
	*	Specialisation for Overlay.
	*\~french
	*	Structure permettant de spécialiser le comportement d'un cache.
	*\remarks
	*	Spécialisation pour Overlay.
	*/
	template< typename KeyT >
	struct ResourceCacheTraitsT< castor3d::Overlay, KeyT >
		: ResourceCacheTraitsBaseT< castor3d::Overlay, KeyT >
	{
		using Base = ResourceCacheTraitsBaseT< castor3d::Overlay, KeyT >;
		using ElementT = typename Base::ElementT;
		using ElementPtrT = typename Base::ElementPtrT;

		C3D_API static const String Name;

		using ElementProducerT = std::function< ElementPtrT( KeyT const &
			, castor3d::OverlayType
			, castor3d::SceneSPtr
			, castor3d::OverlaySPtr ) >;
	};
	/**
	*\~english
	*	Helper structure to specialise a cache behaviour.
	*\remarks
	*	Specialisation for Plugin.
	*\~french
	*	Structure permettant de spécialiser le comportement d'un cache.
	*\remarks
	*	Spécialisation pour Plugin.
	*/
	template< typename KeyT >
	struct ResourceCacheTraitsT< castor3d::Plugin, KeyT >
		: ResourceCacheTraitsBaseT< castor3d::Plugin, KeyT >
	{
		using Base = ResourceCacheTraitsBaseT< castor3d::Plugin, KeyT >;
		using ElementT = typename Base::ElementT;
		using ElementPtrT = typename Base::ElementPtrT;

		C3D_API static const String Name;

		using ElementProducerT = std::function< ElementPtrT( KeyT const &
			, castor3d::PluginType
			, DynamicLibrarySPtr ) >;
	};
	/**
	*\~english
	*	Helper structure to specialise a cache behaviour.
	*\remarks
	*	Specialisation for RenderTechnique.
	*\~french
	*	Structure permettant de spécialiser le comportement d'un cache.
	*\remarks
	*	Spécialisation pour RenderTechnique.
	*/
	template< typename KeyT >
	struct ResourceCacheTraitsT< castor3d::RenderTechnique, KeyT >
		: ResourceCacheTraitsBaseT< castor3d::RenderTechnique, KeyT >
	{
		using Base = ResourceCacheTraitsBaseT< castor3d::RenderTechnique, KeyT >;
		using ElementT = typename Base::ElementT;
		using ElementPtrT = typename Base::ElementPtrT;

		C3D_API static const String Name;

		using ElementProducerT = std::function< ElementPtrT( KeyT const &
			, String const &
			, castor3d::RenderTarget &
			, castor3d::RenderDevice const &
			, castor3d::QueueData const &
			, castor3d::Parameters const &
			, castor3d::SsaoConfig const &
			, castor3d::ProgressBar * ) >;
	};
	/**
	*\~english
	*	Helper structure to specialise a cache behaviour.
	*\remarks
	*	Specialisation for Sampler.
	*\~french
	*	Structure permettant de spécialiser le comportement d'un cache.
	*\remarks
	*	Spécialisation pour Sampler.
	*/
	template< typename KeyT >
	struct ResourceCacheTraitsT< castor3d::Sampler, KeyT >
		: ResourceCacheTraitsBaseT< castor3d::Sampler, KeyT >
	{
		using Base = ResourceCacheTraitsBaseT< castor3d::Sampler, KeyT >;
		using ElementT = typename Base::ElementT;
		using ElementPtrT = typename Base::ElementPtrT;

		C3D_API static const String Name;

		using ElementProducerT = std::function< ElementPtrT( KeyT const & ) >;
	};
	/**
	*\~english
	*	Helper structure to specialise a cache behaviour.
	*\remarks
	*	Specialisation for Scene.
	*\~french
	*	Structure permettant de spécialiser le comportement d'un cache.
	*\remarks
	*	Spécialisation pour Scene.
	*/
	template< typename KeyT >
	struct ResourceCacheTraitsT< castor3d::Scene, KeyT >
		: ResourceCacheTraitsBaseT< castor3d::Scene, KeyT >
	{
		using Base = ResourceCacheTraitsBaseT< castor3d::Scene, KeyT >;
		using ElementT = typename Base::ElementT;
		using ElementPtrT = typename Base::ElementPtrT;

		C3D_API static const String Name;

		using ElementProducerT = std::function< ElementPtrT( KeyT const & ) >;
	};

	template<>
	struct ResourceCacheT< castor3d::AnimatedObjectGroup, String >;
	template<>
	struct ResourceCacheT< castor3d::Material, String >;
	template<>
	struct ResourceCacheT< castor3d::Overlay, String >;
	template<>
	struct ResourceCacheT< castor3d::Plugin, String >;
}

namespace castor3d
{
	/**@name Cache */
	//@{

	/**
	*\~english
	*	View on a resource cache.
	*\remarks
	*	Allows deletion of elements created through the view, and only those.
	*\~french
	*	Vue sur un cache de ressources.
	*\remarks
	*	Permet de supprimer tous les éléments créés via la vue et uniquement ceux là.
	*/
	template< typename CacheT, EventType EventT >
	class CacheViewT;
	/**
	*\~english
	*\brief
	*	Helper structure to specialise a scene objects cache behaviour.
	*\remarks
	*	Must hold:
	*	<ul>
	*	<li>Name: The element type name.</li>
	*	<li>ElementT: The resource type.</li>
	*	<li>ElementPtrT: The resource pointer type.</li>
	*	<li>ElementContT: The resource container type.</li>
	*	<li>ElementCacheT: The resource base cache type.</li>
	*	<li>ElementProducerT: The element creation function prototype.</li>
	*	<li>ElementInitialiserT: The prototype of the function use to initialise a resource.</li>
	*	<li>ElementCleanerT: The prototype of the function use to cleanup a resource.</li>
	*	<li>ElementMergerT: The prototype of the function use to merge a cache element into another cache.</li>
	*	<li>ElementAttacherT: The prototype of the function use to attach a cache element to a scene node.</li>
	*	<li>ElementDetacherT: The prototype of the function use to detach a cache element from a scene node.</li>
	*	</ul>
	*\~french
	*\brief
	*	Structure permettant de spécialiser le comportement d'un cache d'objets de scène.
	*\remarks
	*	Doit contenir:
	*	<ul>
	*	<li>Name: Le nom du type d'élément.</li>
	*	<li>ElementT: Le type de ressource.</li>
	*	<li>ElementPtrT: Le type de pointeur sur une ressource.</li>
	*	<li>ElementContT: Le type de conteneur de ressources.</li>
	*	<li>ElementCacheT: Le type de base de cache de ressources.</li>
	*	<li>ElementProducerT: Le prototype de la fonction de création d'un élément.</li>
	*	<li>ElementInitialiserT: Le prototype de la fonction pour initialiser une ressource.</li>
	*	<li>ElementCleanerT: Le prototype de la fonction pour nettoyer une ressource.</li>
	*	<li>ElementMergerT: Le prototype de la fonction pour fusionner un élément d'un cache dans un autre cache.</li>
	*	<li>ElementAttacherT: Le prototype de la fonction pour attacher un élément d'un cache à un scene node.</li>
	*	<li>ElementDetacherT: Le prototype de la fonction pour détacher un élément d'un cache d'un scene node.</li>
	*	</ul>
	*/
	template< typename ElementT, typename KeyT >
	struct ObjectCacheTraitsT;
	/**
	*\~english
	*	Base class for a scene element cache.
	*\~french
	*	Classe de base pour un cache d'éléments de scène.
	*/
	template< typename ElementT
		, typename KeyT
		, typename TraitsT = ObjectCacheTraitsT< ElementT, KeyT > >
	class ObjectCacheBaseT;
	/**
	*\~english
	*	Base class for a scene element cache.
	*\~french
	*	Classe de base pour un cache d'éléments de scène.
	*/
	template< typename ElementT
		, typename KeyT
		, typename TraitsT = ObjectCacheTraitsT< ElementT, KeyT > >
	class ObjectCacheT;
	/**
	*\~english
	*	Helper structure to build a castor3d::ObjectCacheTraitsT.
	*\remarks
	*	Predefines:
	*	<ul>
	*	<li>ElementT: The resource type.</li>
	*	<li>ElementPtrT: The resource pointer type.</li>
	*	<li>ElementContT: The resource container type.</li>
	*	<li>ElementCacheT: The resource base cache type.</li>
	*	<li>ElementInitialiserT: The prototype of the function use to initialise a resource.</li>
	*	<li>ElementCleanerT: The prototype of the function use to cleanup a resource.</li>
	*	<li>ElementMergerT: The prototype of the function use to merge a cache element into another cache.</li>
	*	<li>ElementAttacherT: The prototype of the function use to attach a cache element to a scene node.</li>
	*	<li>ElementDetacherT: The prototype of the function use to detach a cache element from a scene node.</li>
	*	</ul>
	*	Hence, only remains to define:
	*	<ul>
	*	<li>Name: The element type name.</li>
	*	<li>ElementProducerT: The element creation function prototype.</li>
	*	</ul>
	*\~french
	*	Structure d'aide à la création d'un castor3d::ObjectCacheTraitsT.
	*\remarks
	*	Prédéfinit:
	*	<ul>
	*	<li>ElementT: Le type de ressource.</li>
	*	<li>ElementPtrT: Le type de pointeur sur une ressource.</li>
	*	<li>ElementContT: Le type de conteneur de ressources.</li>
	*	<li>ElementCacheT: Le type de base de cache de ressources.</li>
	*	<li>ElementInitialiserT: Le prototype de la fonction pour initialiser une ressource.</li>
	*	<li>ElementCleanerT: Le prototype de la fonction pour nettoyer une ressource.</li>
	*	<li>ElementMergerT: Le prototype de la fonction pour fusionner un élément d'un cache dans un autre cache.</li>
	*	<li>ElementAttacherT: Le prototype de la fonction pour attacher un élément d'un cache à un scene node.</li>
	*	<li>ElementDetacherT: Le prototype de la fonction pour détacher un élément d'un cache d'un scene node.</li>
	*	</ul>
	*	Il ne reste donc ainsi qu'à définir:
	*	<ul>
	*	<li>Name: Le nom du type d'élément.</li>
	*	<li>ElementProducerT: Le prototype de la fonction de création d'un élément.</li>
	*	</ul>
	*/
	template< typename ObjT, typename KeyT >
	struct ObjectCacheTraitsBaseT
	{
		using ElementT = ObjT;
		using ElementPtrT = std::shared_ptr< ElementT >;
		using ElementContT = std::unordered_map< KeyT, ElementPtrT >;
		using ElementCacheT = ObjectCacheBaseT< ElementT, KeyT >;

		using ElementInitialiserT = std::function< void( ElementPtrT ) >;
		using ElementCleanerT = std::function< void( ElementPtrT ) >;
		using ElementMergerT = std::function< void( ElementCacheT const &
			, ElementContT &
			, ElementPtrT
			, SceneNodeSPtr
			, SceneNodeSPtr ) >;
		using ElementAttacherT = std::function< void( ElementPtrT
			, SceneNode &
			, SceneNodeSPtr
			, SceneNodeSPtr
			, SceneNodeSPtr ) >;
		using ElementDetacherT = std::function< void( ElementPtrT ) >;
	};
	/**
	*\~english
	*	Helper structure to specialise a scene objects cache behaviour.
	*\remarks
	*	Specialisation for BillboardList.
	*\~french
	*	Structure permettant de spécialiser le comportement d'un cache d'objets de scène.
	*\remarks
	*	Spécialisation pour BillboardList.
	*/
	template< typename KeyT >
	struct ObjectCacheTraitsT< BillboardList, KeyT >
		: ObjectCacheTraitsBaseT< BillboardList, KeyT >
	{
		using ElementT = BillboardList;
		using BaseT = ObjectCacheTraitsBaseT< ElementT, KeyT >;
		using ElementPtrT = typename BaseT::ElementPtrT;

		C3D_API static const castor::String Name;

		using ElementProducerT = std::function< ElementPtrT( KeyT const &
			, SceneNode & ) >;
	};
	/**
	*\~english
	*	Helper structure to specialise a scene objects cache behaviour.
	*\remarks
	*	Specialisation for Camera.
	*\~french
	*	Structure permettant de spécialiser le comportement d'un cache d'objets de scène.
	*\remarks
	*	Spécialisation pour Camera.
	*/
	template< typename KeyT >
	struct ObjectCacheTraitsT< Camera, KeyT >
		: ObjectCacheTraitsBaseT< Camera, KeyT >
	{
		using ElementT = Camera;
		using BaseT = ObjectCacheTraitsBaseT< ElementT, KeyT >;
		using ElementPtrT = typename BaseT::ElementPtrT;

		C3D_API static const castor::String Name;

		using ElementProducerT = std::function< ElementPtrT( KeyT const &
			, SceneNode &
			, Viewport ) >;
	};
	/**
	*\~english
	*	Helper structure to specialise a scene objects cache behaviour.
	*\remarks
	*	Specialisation for Geometry.
	*\~french
	*	Structure permettant de spécialiser le comportement d'un cache d'objets de scène.
	*\remarks
	*	Spécialisation pour Geometry.
	*/
	template< typename KeyT >
	struct ObjectCacheTraitsT< Geometry, KeyT >
		: ObjectCacheTraitsBaseT< Geometry, KeyT >
	{
		using ElementT = Geometry;
		using BaseT = ObjectCacheTraitsBaseT< ElementT, KeyT >;
		using ElementPtrT = typename BaseT::ElementPtrT;

		C3D_API static const castor::String Name;

		using ElementProducerT = std::function< ElementPtrT( KeyT const &
			, SceneNode &
			, MeshSPtr ) >;
	};
	/**
	*\~english
	*	Helper structure to specialise a scene objects cache behaviour.
	*\remarks
	*	Specialisation for Light.
	*\~french
	*	Structure permettant de spécialiser le comportement d'un cache d'objets de scène.
	*\remarks
	*	Spécialisation pour Light.
	*/
	template< typename KeyT >
	struct ObjectCacheTraitsT< Light, KeyT >
		: ObjectCacheTraitsBaseT< Light, KeyT >
	{
		using ElementT = Light;
		using BaseT = ObjectCacheTraitsBaseT< ElementT, KeyT >;
		using ElementPtrT = typename BaseT::ElementPtrT;

		C3D_API static const castor::String Name;

		using ElementProducerT = std::function< ElementPtrT( KeyT const &
			, SceneNode &
			, LightType ) >;
	};
	/**
	*\~english
	*	Helper structure to specialise a scene objects cache behaviour.
	*\remarks
	*	Specialisation for ParticleSystem.
	*\~french
	*	Structure permettant de spécialiser le comportement d'un cache d'objets de scène.
	*\remarks
	*	Spécialisation pour ParticleSystem.
	*/
	template< typename KeyT >
	struct ObjectCacheTraitsT< ParticleSystem, KeyT >
		: ObjectCacheTraitsBaseT< ParticleSystem, KeyT >
	{
		using ElementT = ParticleSystem;
		using BaseT = ObjectCacheTraitsBaseT< ElementT, KeyT >;
		using ElementPtrT = typename BaseT::ElementPtrT;

		C3D_API static const castor::String Name;

		using ElementProducerT = std::function< ElementPtrT( KeyT const &
			, SceneNode &
			, uint32_t ) >;
	};
	/**
	*\~english
	*	Helper structure to specialise a scene objects cache behaviour.
	*\remarks
	*	Specialisation for SceneNode.
	*\~french
	*	Structure permettant de spécialiser le comportement d'un cache d'objets de scène.
	*\remarks
	*	Spécialisation pour SceneNode.
	*/
	template< typename KeyT >
	struct ObjectCacheTraitsT< SceneNode, KeyT >
		: ObjectCacheTraitsBaseT< SceneNode, KeyT >
	{
		using ElementT = SceneNode;
		using BaseT = ObjectCacheTraitsBaseT< ElementT, KeyT >;
		using ElementPtrT = typename BaseT::ElementPtrT;

		C3D_API static const castor::String Name;

		using ElementProducerT = std::function< ElementPtrT( KeyT const &
			, SceneNode & ) >;
	};

	template<>
	class ObjectCacheT< BillboardList, castor::String >;
	template<>
	class ObjectCacheT< Light, castor::String >;
	template<>
	class ObjectCacheT< Geometry, castor::String >;

	template< typename ObjT, typename KeyT >
	using ObjectPtrT = typename ObjectCacheTraitsT< ObjT, KeyT >::ElementPtrT;
	template< typename ObjT, typename KeyT >
	using ObjectContT = typename ObjectCacheTraitsT< ObjT, KeyT >::ElementContT;
	template< typename ObjT, typename KeyT >
	using ObjectProducerT = typename ObjectCacheTraitsT< ObjT, KeyT >::ElementProducerT;
	template< typename ObjT, typename KeyT >
	using ObjectInitialiserT = typename ObjectCacheTraitsT< ObjT, KeyT >::ElementInitialiserT;
	template< typename ObjT, typename KeyT >
	using ObjectCleanerT = typename ObjectCacheTraitsT< ObjT, KeyT >::ElementCleanerT;
	template< typename ObjT, typename KeyT >
	using ObjectMergerT = typename ObjectCacheTraitsT< ObjT, KeyT >::ElementMergerT;
	template< typename ObjT, typename KeyT >
	using ObjectAttacherT = typename ObjectCacheTraitsT< ObjT, KeyT >::ElementAttacherT;
	template< typename ObjT, typename KeyT >
	using ObjectDetacherT = typename ObjectCacheTraitsT< ObjT, KeyT >::ElementDetacherT;

	template< typename ObjT, typename KeyT >
	using ObjectCachePtrT = std::shared_ptr< ObjectCacheT< ObjT, KeyT > >;
	template< typename CacheT, EventType EventT >
	using CacheViewPtrT = std::shared_ptr< CacheViewT< CacheT, EventT > >;

	class RenderTargetCache;
	class ShaderProgramCache;

	using AnimatedObjectGroupCache = castor::ResourceCacheT< AnimatedObjectGroup, castor::String >;
	using FrameListenerCache = castor::ResourceCacheT< FrameListener, castor::String >;
	using MaterialCache = castor::ResourceCacheT< Material, castor::String >;
	using MeshCache = castor::ResourceCacheT< Mesh, castor::String >;
	using OverlayCache = castor::ResourceCacheT< Overlay, castor::String >;
	using PluginCache = castor::ResourceCacheT< Plugin, castor::String >;
	using SamplerCache = castor::ResourceCacheT< Sampler, castor::String >;
	using SceneCache = castor::ResourceCacheT< Scene, castor::String >;
	using RenderTechniqueCache = castor::ResourceCacheT< RenderTechnique, castor::String >;

	using BillboardListCache = ObjectCacheT< BillboardList, castor::String >;
	using CameraCache = ObjectCacheT< Camera, castor::String >;
	using GeometryCache = ObjectCacheT< Geometry, castor::String >;
	using LightCache = ObjectCacheT< Light, castor::String >;
	using ParticleSystemCache = ObjectCacheT< ParticleSystem, castor::String >;
	using SceneNodeCache = ObjectCacheT< SceneNode, castor::String >;

	CU_DeclareCUSmartPtr( castor3d, AnimatedObjectGroupCache, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, BillboardListCache, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, CameraCache, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, GeometryCache, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, LightCache, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, FrameListenerCache, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, MaterialCache, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, MeshCache, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, OverlayCache, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, ParticleSystemCache, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, PluginCache, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, SamplerCache, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, RenderTargetCache, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, RenderTechniqueCache, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, SceneCache, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, SceneNodeCache, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, ShaderProgramCache, C3D_API );

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
		castor::Connection< castor::OnCacheChanged > m_on##className##Changed;\
		className##Cache##SPtr m_##memberName##Cache

#define DECLARE_CACHE_VIEW_MEMBER( memberName, className, eventType )\
	public:\
		inline castor3d::CacheViewT< className##Cache, eventType > & get##className##View()\
		{\
			return *m_##memberName##CacheView;\
		}\
		inline castor3d::CacheViewT< className##Cache, eventType > const & get##className##View()const\
		{\
			return *m_##memberName##CacheView;\
		}\
	private:\
		castor3d::CacheViewPtrT< className##Cache, eventType > m_##memberName##CacheView

#define DECLARE_CU_CACHE_VIEW_MEMBER( memberName, className, eventType )\
	public:\
		inline castor3d::CacheViewT< castor::className##Cache, eventType > & get##className##View()\
		{\
			return *m_##memberName##CacheView;\
		}\
		inline castor3d::CacheViewT< castor::className##Cache, eventType > const & get##className##View()const\
		{\
			return *m_##memberName##CacheView;\
		}\
	private:\
		castor3d::CacheViewPtrT< castor::className##Cache, eventType > m_##memberName##CacheView

#endif
