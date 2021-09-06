/*
See LICENSE file in root folder
*/
#ifndef ___C3D_CacheModule_H___
#define ___C3D_CacheModule_H___

#include "Castor3D/Castor3DModule.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>

#include <functional>
#include <memory>
#include <unordered_map>

namespace castor3d
{
	enum class EventType : uint8_t;

	/**@name Cache */
	//@{

	template< typename ResT, typename KeyT >
	struct PtrCacheTraitsBaseT
	{
		using ElementT = ResT;
		using ElementKeyT = KeyT;
		using ElementPtrT = std::shared_ptr< ElementT >;
		using ElementObsT = std::weak_ptr< ElementT >;
		using ElementContT = std::unordered_map< ElementKeyT, ElementPtrT >;
		using ElementCacheT = castor::ResourceCacheBaseT< ElementT, ElementKeyT, PtrCacheTraitsT< ElementT, ElementKeyT > >;

		using ElementInitialiserT = std::function< void( ElementT & ) >;
		using ElementCleanerT = std::function< void( ElementT & ) >;
		using ElementMergerT = std::function< void( ElementCacheT const &
			, ElementContT &
			, ElementPtrT ) >;

		template< typename ... ParametersT >
		static ElementPtrT makeElement( ElementCacheT const & cache
			, ElementKeyT const & key
			, ParametersT && ... params )
		{
			return std::make_shared< ElementT >( key
				, std::forward< ParametersT >( params )... );
		}
	};
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
	*	<li>ElementObjT: The resource observer type.</li>
	*	<li>ElementContT: The resource container type.</li>
	*	<li>ElementCacheT: The resource base cache type.</li>
	*	<li>ElementInitialiserT: The prototype of the function use to initialise a resource.</li>
	*	<li>ElementCleanerT: The prototype of the function use to cleanup a resource.</li>
	*	<li>ElementMergerT: The prototype of the function use to merge a cache element into another cache.</li>
	*	<li>ElementAttacherT: The prototype of the function use to attach a cache element to a scene node.</li>
	*	<li>ElementDetacherT: The prototype of the function use to detach a cache element from a scene node.</li>
	*	<li>static ElementPtrT makeElement( ElementKeyT const &, ParametersT && ... ): The element creation function.</li>
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
	*	<li>ElementObjT: Le type d'observateur sur une ressource.</li>
	*	<li>ElementContT: Le type de conteneur de ressources.</li>
	*	<li>ElementCacheT: Le type de base de cache de ressources.</li>
	*	<li>ElementInitialiserT: Le prototype de la fonction pour initialiser une ressource.</li>
	*	<li>ElementCleanerT: Le prototype de la fonction pour nettoyer une ressource.</li>
	*	<li>ElementMergerT: Le prototype de la fonction pour fusionner un élément d'un cache dans un autre cache.</li>
	*	<li>ElementAttacherT: Le prototype de la fonction pour attacher un élément d'un cache à un scene node.</li>
	*	<li>ElementDetacherT: Le prototype de la fonction pour détacher un élément d'un cache d'un scene node.</li>
	*	<li>static ElementPtrT makeElement( ElementKeyT const &, ParametersT && ... ): La fonction de création d'un élément.</li>
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

	template< typename ObjT, typename KeyT >
	using ObjectPtrT = typename ObjectCacheTraitsT< ObjT, KeyT >::ElementPtrT;
	template< typename ObjT, typename KeyT >
	using ObjectObsT = typename ObjectCacheTraitsT< ObjT, KeyT >::ElementObsT;
	template< typename ObjT, typename KeyT >
	using ObjectContT = typename ObjectCacheTraitsT< ObjT, KeyT >::ElementContT;
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

	template< typename ObjT, typename KeyT, typename TraitsT >
	using ObjectCachePtrT = std::shared_ptr< ObjectCacheT< ObjT, KeyT, TraitsT > >;
	template< typename CacheT, EventType EventT >
	using CacheViewPtrT = std::shared_ptr< CacheViewT< CacheT, EventT > >;

	class RenderTargetCache;
	class ShaderProgramCache;

	CU_DeclareCUSmartPtr( castor3d, RenderTargetCache, C3D_API );
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
