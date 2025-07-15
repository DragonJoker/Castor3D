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

namespace c3d
{
	/**@name Cache */
	//@{

	template< typename ResT, typename KeyT >
	struct PtrCacheTraitsBaseT
	{
		using ElementT = ResT;
		using ElementKeyT = KeyT;
		using ElementPtrT = UniquePtr< ElementT >;
		using ElementObsT = ElementT *;
		using ElementContT = HashMap< ElementKeyT, ElementPtrT >;
		using ElementCacheT = ResourceCacheBaseT< ElementT, ElementKeyT, PtrCacheTraitsT< ElementT, ElementKeyT > >;

		using ElementInitialiserT = Function< void( ElementT & ) >;
		using ElementCleanerT = Function< void( ElementT & ) >;
		using ElementMergerT = Function< void( ElementCacheT const &
			, ElementContT &
			, ElementPtrT ) >;

		template< typename ... ParametersT >
		static ElementPtrT makeElement( ElementCacheT const & /*cache*/
			, ElementKeyT const & key
			, ParametersT && ... params )
		{
			return makeUnique< ElementT >( key
				, c3d::forward< ParametersT >( params )... );
		}

		static ElementObsT makeElementObs( ElementPtrT const & element )
		{
			return element.get();
		}

		static bool areElementsEqual( ElementObsT const & lhs
			, ElementObsT const & rhs )
		{
			return lhs == rhs;
		}

		static bool areElementsEqual( ElementObsT const & lhs
			, ElementPtrT const & rhs )
		{
			return lhs == rhs.get();
		}

		static bool areElementsEqual( ElementPtrT const & lhs
			, ElementObsT const & rhs )
		{
			return lhs.get() == rhs;
		}

		static bool areElementsEqual( ElementPtrT const & lhs
			, ElementPtrT const & rhs )
		{
			return lhs == rhs;
		}

		static bool isElementObsNull( ElementObsT const & element )
		{
			return element == nullptr;
		}
	};
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
	using ObjectCachePtrT = UniquePtr< ObjectCacheT< ObjT, KeyT, TraitsT > >;

	class RenderTargetCache;
	class ShaderProgramCache;
	class TextureUnitCache;

	/** @cond !Doxygen */
	CU_DeclareSmartPtr( c3d, RenderTargetCache, C3D_API );
	CU_DeclareSmartPtr( c3d, ShaderProgramCache, C3D_API );
	CU_DeclareSmartPtr( c3d, TextureUnitCache, C3D_API );
	/** @endcond */
	//@}
}

#define DECLARE_CACHE_MEMBER_MIN( memberName, className )\
	public:\
		className##Cache & get##className##Cache()noexcept\
		{\
			return *m_##memberName##Cache;\
		}\
		className##Cache const & get##className##Cache()const noexcept\
		{\
			return *m_##memberName##Cache;\
		}\
	private:\
		className##Cache##UPtr m_##memberName##Cache

#define DECLARE_CACHE_MEMBER( memberName, className )\
	public:\
		template< typename ... ParametersT >\
		className##Cache::ElementPtrT create##className( className##Cache::ElementKeyT const & key\
			, ParametersT && ... parameters )const\
		{\
			return m_##memberName##Cache->create( key\
				, c3d::forward< ParametersT >( parameters )... );\
		}\
		template< typename ... ParametersT >\
		className##Cache::ElementObsT addNew##className( className##Cache::ElementKeyT const & key\
			, ParametersT && ... parameters )\
		{\
			return m_##memberName##Cache->addNew( key\
				, c3d::forward< ParametersT >( parameters )... );\
		}\
		className##Cache::ElementObsT add##className( className##Cache::ElementKeyT const & key\
			, className##Cache::ElementPtrT & element\
			, bool initialise = false )\
		{\
			return m_##memberName##Cache->add( key, element, initialise );\
		}\
		className##Cache::ElementPtrT remove##className( className##Cache::ElementKeyT const & key\
			, bool cleanup = false )noexcept\
		{\
			return m_##memberName##Cache->remove( key, cleanup );\
		}\
		className##Cache::ElementObsT find##className( className##Cache::ElementKeyT const & key )const\
		{\
			return m_##memberName##Cache->find( key );\
		}\
		bool has##className( className##Cache::ElementKeyT const & key )const noexcept\
		{\
			return m_##memberName##Cache->has( key );\
		}\
		className##Cache::ElementObsT tryFind##className( className##Cache::ElementKeyT const & key )const noexcept\
		{\
			return m_##memberName##Cache->tryFind( key );\
		}\
		uint32_t get##className##sCount()const noexcept\
		{\
			return m_##memberName##Cache->getObjectCount();\
		}\
		DECLARE_CACHE_MEMBER_MIN( memberName, className )

#define DECLARE_OBJECT_CACHE_MEMBER_MIN( memberName, className )\
	public:\
		className##Cache & get##className##Cache()noexcept\
		{\
			return *m_##memberName##Cache;\
		}\
		className##Cache const & get##className##Cache()const noexcept\
		{\
			return *m_##memberName##Cache;\
		}\
	public:\
		template< typename ... ParametersT >\
		className##Cache::ElementPtrT create##className( className##Cache::ElementKeyT const & key\
			, ParametersT && ... parameters )const\
		{\
			return m_##memberName##Cache->create( key\
				, c3d::forward< ParametersT >( parameters )... );\
		}\
		className##Cache::ElementPtrT remove##className( className##Cache::ElementKeyT const & key\
			, bool cleanup = false )noexcept\
		{\
			return m_##memberName##Cache->remove( key, cleanup );\
		}\
		className##Cache::ElementObsT find##className( className##Cache::ElementKeyT const & key )const\
		{\
			return m_##memberName##Cache->find( key );\
		}\
		bool has##className( className##Cache::ElementKeyT const & key )const noexcept\
		{\
			return m_##memberName##Cache->has( key );\
		}\
		className##Cache::ElementObsT tryFind##className( className##Cache::ElementKeyT const & key )const noexcept\
		{\
			return m_##memberName##Cache->tryFind( key );\
		}\
	private:\
		ConnectionT< OnCacheChanged > m_on##className##Changed;\
		className##Cache##UPtr m_##memberName##Cache

#define DECLARE_OBJECT_CACHE_MEMBER( memberName, className )\
	public:\
		template< typename ... ParametersT >\
		className##Cache::ElementObsT addNew##className( className##Cache::ElementKeyT const & key\
			, ParametersT && ... parameters )\
		{\
			return m_##memberName##Cache->addNew( key\
				, c3d::forward< ParametersT >( parameters )... );\
		}\
		className##Cache::ElementObsT add##className( className##Cache::ElementKeyT const & key\
			, className##Cache::ElementPtrT & element\
			, bool initialise = false )\
		{\
			return m_##memberName##Cache->add( key, element, initialise );\
		}\
		DECLARE_OBJECT_CACHE_MEMBER_MIN( memberName, className )

#define DECLARE_CACHE_VIEW_MEMBER( memberName, className )\
	public:\
		CacheViewT< className##Cache > & get##className##View()noexcept\
		{\
			return *m_##memberName##CacheView;\
		}\
		CacheViewT< className##Cache > const & get##className##View()const noexcept\
		{\
			return *m_##memberName##CacheView;\
		}\
		bool has##className( className##Cache::ElementKeyT const & key )const noexcept\
		{\
			return m_##memberName##CacheView->has( key );\
		}\
		template< typename ... ParametersT >\
		className##Cache::ElementObsT addNew##className( className##Cache::ElementKeyT const & key\
			, ParametersT && ... parameters )\
		{\
			return m_##memberName##CacheView->addNew( key\
				, c3d::forward< ParametersT >( parameters )... );\
		}\
		className##Cache::ElementObsT add##className( className##Cache::ElementKeyT const & key\
			, className##Cache::ElementPtrT & element\
			, bool initialise = false )\
		{\
			return m_##memberName##CacheView->add( key, element, initialise );\
		}\
		className##Cache::ElementPtrT remove##className( className##Cache::ElementKeyT const & key )noexcept\
		{\
			return m_##memberName##CacheView->remove( key );\
		}\
		className##Cache::ElementObsT find##className( className##Cache::ElementKeyT const & key )const\
		{\
			return m_##memberName##CacheView->find( key );\
		}\
		className##Cache::ElementObsT tryFind##className( className##Cache::ElementKeyT const & key )const noexcept\
		{\
			return m_##memberName##CacheView->tryFind( key );\
		}\
	private:\
		CacheViewPtrT< className##Cache > m_##memberName##CacheView

#endif
