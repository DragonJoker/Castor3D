/*
See LICENSE file in root folder
*/
#ifndef ___C3D_CacheModule_H___
#define ___C3D_CacheModule_H___

#include "Castor3D/Event/Frame/FrameEventModule.hpp"
#include "Castor3D/Model/Mesh/MeshModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Scene/SceneModule.hpp"
#include "Castor3D/Scene/ParticleSystem/ParticleModule.hpp"
#include "Castor3D/Render/Technique/TechniqueModule.hpp"

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
	*	Holds the pools for billboard UBOs.
	*\~french
	*\brief
	*	Contient les pools pour les UBO des billboards.
	*/
	class BillboardUboPools;
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
	struct ObjectCacheTraits;

	/**
	*\~english
	*\brief
	*	Cache used to hold the shader programs. Holds it, destroys it during a rendering loop
	*\~french
	*\brief
	*	Cache utilisé pour garder les programmes de shaders. Il les garde et permet leur destruction au cours d'une boucle de rendu
	*/
	class ShaderProgramCache;
	/**
	*\~english
	*\brief
	*	RenderTarget cache.
	*\~french
	*\brief
	*	Cache de RenderTarget.
	*/
	class RenderTargetCache;

	template< typename ElementType >
	using ElementInitialiser = std::function< void( std::shared_ptr< ElementType > ) >;

	template< typename ElementType >
	using ElementCleaner = std::function< void( std::shared_ptr< ElementType > ) >;

	template< typename ElementType, typename KeyType >
	using ElementMerger = std::function< void( CacheBase< ElementType, KeyType > const &, castor::Collection< ElementType, KeyType > &, std::shared_ptr< ElementType > ) >;

	template< typename ElementType >
	using ElementAttacher = std::function< void( std::shared_ptr< ElementType >, SceneNodeSPtr, SceneNodeSPtr, SceneNodeSPtr, SceneNodeSPtr ) >;

	template< typename ElementType >
	using ElementDetacher = std::function< void( std::shared_ptr< ElementType > ) >;

	using ListenerCache = Cache< FrameListener, castor::String >;
	using MeshCache = Cache< Mesh, castor::String >;
	using SamplerCache = Cache< Sampler, castor::String >;
	using SceneCache = Cache< Scene, castor::String >;
	using RenderTechniqueCache = Cache< RenderTechnique, castor::String >;
	using RenderWindowCache = Cache< RenderWindow, castor::String >;

	CU_DeclareSmartPtr( ListenerCache );
	CU_DeclareSmartPtr( MeshCache );
	CU_DeclareSmartPtr( SceneCache );
	CU_DeclareSmartPtr( SamplerCache );
	CU_DeclareSmartPtr( ShaderProgramCache );
	CU_DeclareSmartPtr( RenderTargetCache );
	CU_DeclareSmartPtr( RenderTechniqueCache );
	CU_DeclareSmartPtr( RenderWindowCache );

	using CameraCache = ObjectCache< Camera, castor::String >;
	using ParticleSystemCache = ObjectCache< ParticleSystem, castor::String >;

	CU_DeclareSmartPtr( CameraCache );
	CU_DeclareSmartPtr( ParticleSystemCache );

	//@}
}

#define MAKE_CACHE_NAME( className )\
	castor3d::Cache< className, castor::String >

#define DECLARE_CACHE_MEMBER( memberName, className )\
	public:\
		inline MAKE_CACHE_NAME( className ) & get##className##Cache()\
		{\
			return *m_##memberName##Cache;\
		}\
		inline MAKE_CACHE_NAME( className ) const & get##className##Cache()const\
		{\
			return *m_##memberName##Cache;\
		}\
	private:\
		std::shared_ptr< MAKE_CACHE_NAME( className ) > m_##memberName##Cache

#define DECLARE_NAMED_CACHE_MEMBER( memberName, className )\
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
		std::shared_ptr< className##Cache > m_##memberName##Cache

#define MAKE_OBJECT_CACHE_NAME( className )\
	castor3d::ObjectCache< className, castor::String >

#define DECLARE_OBJECT_CACHE_MEMBER( memberName, className )\
	public:\
		inline MAKE_OBJECT_CACHE_NAME( className ) & get##className##Cache()\
		{\
			return *m_##memberName##Cache;\
		}\
		inline MAKE_OBJECT_CACHE_NAME( className ) const & get##className##Cache()const\
		{\
			return *m_##memberName##Cache;\
		}\
	private:\
		castor::Connection< MAKE_OBJECT_CACHE_NAME( className )::OnChanged > m_on##className##Changed;\
		std::shared_ptr< MAKE_OBJECT_CACHE_NAME( className ) > m_##memberName##Cache

#define DECLARE_CACHE_VIEW_MEMBER( memberName, className, eventType )\
	public:\
		inline castor3d::CacheView< className, MAKE_CACHE_NAME( className ), eventType > & get##className##View()\
		{\
			return *m_##memberName##CacheView;\
		}\
		inline castor3d::CacheView< className, MAKE_CACHE_NAME( className ), eventType > const & get##className##View()const\
		{\
			return *m_##memberName##CacheView;\
		}\
	private:\
		std::shared_ptr< castor3d::CacheView< className, MAKE_CACHE_NAME( className ), eventType > > m_##memberName##CacheView

#define DECLARE_CACHE_VIEW_MEMBER_CU( memberName, className, eventType )\
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

#define DECLARE_CACHE_VIEW_MEMBER_EX( memberName, mgrName, className, eventType )\
	public:\
		inline castor3d::CacheView< MAKE_CACHE_NAME( className ), mgrName##Cache, eventType > & get##className##View()\
		{\
			return *m_##memberName##CacheView;\
		}\
		inline castor3d::CacheView< MAKE_CACHE_NAME( className ), mgrName##Cache, eventType > const & get##className##View()const\
		{\
			return *m_##memberName##CacheView;\
		}\
	private:\
		std::shared_ptr< castor3d::CacheView< MAKE_CACHE_NAME( className ), mgrName##Cache, eventType > > m_##memberName##CacheView

#endif
