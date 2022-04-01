/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SceneModule_H___
#define ___C3D_SceneModule_H___

#include "Castor3D/Event/Frame/FrameEventModule.hpp"
#include "Castor3D/Material/MaterialModule.hpp"
#include "Castor3D/Material/Texture/TextureModule.hpp"
#include "Castor3D/Model/Mesh/Submesh/SubmeshModule.hpp"

#include <CastorUtils/Design/Signal.hpp>
#include <CastorUtils/Graphics/GraphicsModule.hpp>

namespace castor3d
{
	class Viewport;

	/**@name Scene */
	//@{

	struct BillboardVertex
	{
		castor::Point3f position;
		castor::Point2f uv;
	};
	/**
	*\~english
	*\brief
	*	Movable object types enumerator
	*\remarks
	*	There are four movable object types : camera, geometry, light and billboard.
	*\~french
	*\brief
	*	Enumération des types de MovableObject
	*\remarks
	*	Il y a quatre types d'objets déplaçables : caméra, géométrie, lumière et billboard
	*/
	enum class MovableType
		: uint8_t
	{
		eCamera = 0,
		eGeometry = 1,
		eLight = 2,
		eBillboard = 3,
		eParticleEmitter = 4,
		CU_ScopedEnumBounds( eCamera, eParticleEmitter )
	};
	C3D_API castor::String getName( MovableType value );
	/**
	*\~english
	*\brief
	*	Shadow filter types enumeration.
	*\~french
	*\brief
	*	Enumération des types de filtrage des ombres.
	*/
	enum class ShadowType
		: uint32_t
	{
		//!\~english	No shadows at all.
		//!\~french		Pas d'ombres du tout.
		eNone = 0,
		//!\~english	No filtering.
		//!\~french		Pas de filtrage.
		eRaw = 1,
		//!\~english	PCF filtering.
		//!\~french		Filtrage PCF.
		ePCF = 2,
		//!\~english	Variance shadow mapping.
		//!\~french		Filtrage par variance.
		eVariance = 3,
		CU_ScopedEnumBounds( eNone, eVariance )
	};
	C3D_API castor::String getName( ShadowType value );
	/**
	*\~english
	*\brief
	*	Fog types enumeration.
	*\~french
	*\brief
	*	Enumération des types de brouillard.
	*/
	enum class FogType
	{
		//!\~english	No fog.
		//!\~french		Pas de brouillard
		eDisabled = 0,
		//!\~english	Fog intensity increases linearly with distance to camera.
		//!\~french		L'intensité du brouillard augmente lin�airement avec la distance à la caméra.
		eLinear = 1,
		//!\~english	Fog intensity increases exponentially with distance to camera.
		//!\~french		L'intensité du brouillard augmente exponentiellement avec la distance à la caméra.
		eExponential = 2,
		//!\~english	Fog intensity increases even more with distance to camera.
		//!\~french		L'intensité du brouillard augmente encore plus avec la distance à la caméra.
		eSquaredExponential = 3,
		CU_ScopedEnumBounds( eDisabled, eSquaredExponential )
	};
	C3D_API castor::String getName( FogType value );
	/**
	*\~english
	*\brief
	*	Scene flags enumeration.
	*\~french
	*\brief
	*	Enumération des indicateurs de scène.
	*/
	enum class SceneFlag
		: uint16_t
	{
		//!\~english	No flag.
		//!\~french		Aucun indicateur.
		eNone = 0x0000,
		//!\~english	Linear fog.
		//!\~french		Brouillard linéaire.
		eFogLinear = 0x0001 << 0,
		//!\~english	Exponential fog.
		//!\~french		Brouillard exponentiel.
		eFogExponential = 0x0001 << 1,
		//!\~english	Squared exponential fog.
		//!\~french		Brouillard exponentiel au carré.
		eFogSquaredExponential = 0x0001 << 2,
		//!\~english	Shadow enabling, by light source type.
		//!\~french		Activation des ombres, par type de source lumineuse.
		eShadowBegin = 0x0001 << 3,
		eShadowDirectional = eShadowBegin << 0,
		eShadowPoint = eShadowBegin << 1,
		eShadowSpot = eShadowBegin << 2,
		eShadowEnd = eShadowSpot,
		eShadowAny = eShadowSpot | eShadowPoint | eShadowDirectional,
		//!\~english	Reflective Shadow Maps enabling.
		//!\~french		Activation des Reflective Shadow Maps.
		eRsmGI = 0x0001 << 6,
		//!\~english	Light Propagation Volumes enabling.
		//!\~french		Activation des Light Propagation Volumes.
		eLpvGI = 0x0001 << 7,
		eLayeredLpvGI = 0x0001 << 8,
		//!\~english	Voxel Cone Tracing enabling.
		//!\~french		Activation du Voxel Cone Tracing.
		eVoxelConeTracing = 0x0001 << 9,
		eGIAny = eRsmGI | eLpvGI | eLayeredLpvGI | eVoxelConeTracing,
		//!\~english	All flags.
		//!\~french		Tous les indicateurs.
		eAll = 0x03FF,
	};
	CU_ImplementFlags( SceneFlag )
	/**
	*\~english
	*\brief
	*	Gives the fog type matching the given flags.
	*\param[in] flags
	*	The scene flags.
	*\return
	*	The fog type.
	*\~french
	*\brief
	*	Récupère le type de brouillard correspondant aux indicateurs donnés.
	*\param[in] flags
	*	Les indicateurs de scène.
	*\return
	*	Le type de brouillard.
	*/
	C3D_API FogType getFogType( SceneFlags const & flags );
	/**
	*\~english
	*\brief
	*	Billboard rendering types enumeration.
	*\~french
	*\brief
	*	Enumération des types de rendu des billboards.
	*/
	enum class BillboardType
	{
		//!\~english	Billboards always face the camera.
		//!\~french		Les billboards font toujours face à la caméra.
		eSpherical = 0,
		//!\~english	Billboards rotate only on Y axis.
		//!\~french		Les billboards tournent uniquement sur l'axe Y.
		eCylindrical = 1,
		CU_ScopedEnumBounds( eSpherical, eCylindrical )
	};
	C3D_API castor::String getName( BillboardType value );
	/**
	*\~english
	*\brief
	*	Billboard sizing types enumeration.
	*\~french
	*\brief
	*	Enumération des types de dimensions des billboards.
	*/
	enum class BillboardSize
	{
		//!\~english	Billboards size changes with camera position.
		//!\~french		Les dimensions des billboards changent en fonction de la position de la caméra.
		eDynamic = 0,
		//!\~english	Billboards size doesn't change.
		//!\~french		Les dimensions des billboards ne changent pas.
		eFixed = 1,
		CU_ScopedEnumBounds( eDynamic, eFixed )
	};
	C3D_API castor::String getName( BillboardSize value );
	/**
	*\~english
	*\brief
	*	Billboards list
	*\remarks
	*	All billboards from this list shares the same texture
	*\~french
	*\brief
	*	Liste de billboards
	*\remarks
	*	Tous les billboards de cette liste ont la meme texture
	*/
	class BillboardBase;
	/**
	*\~english
	*\brief
	*	Billboards list
	*\remarks
	*	All billboards from this list shares the same texture
	*\~french
	*\brief
	*	Liste de billboards
	*\remarks
	*	Tous les billboards de cette liste ont la meme texture
	*/
	class BillboardList;
	/**
	*\~english
	*\brief
	*	Class which represents a Camera
	*\remarks
	*	Gives its position, orientation, viewport ...
	*\~french
	*\brief
	*	Classe de représentation de Camera
	*\remarks
	*	donne la position, orientation, viewport ...
	*/
	class Camera;
	/**
	*\~english
	*\brief
	*	Base class for external camera import.
	*\~french
	*\brief
	*	Classe de base pour l'import de caméras externes.
	*/
	class CameraImporter;
	/**
	*\~english
	*\brief
	*	Fog implementation.
	*\~french
	*\brief
	*	Implémentation de brouillard.
	*/
	class Fog;
	/**
	\~english
	\brief		Geometry handler class
	\~french
	\brief		Classe de gestion d'une géométrie affichable
	*/
	class Geometry;
	/**
	*\~english
	*\brief
	*	Movable object class
	*\~french
	*\brief
	*	Classe d'objet déplaçable
	*/
	class MovableObject;
	/**
	*\~english
	*\brief
	*	Handles object's visibility, and behavior with shadows.
	*\~french
	*\brief
	*	Gère la visibilité de l'objet, ainsi que son comportement avec les ombres.
	*/
	class RenderedObject;
	/**
	*\~english
	*\brief
	*	Scene handler class
	*\remarks
	*	A scene is a collection of lights, scene nodes and geometries.
	*	<br />It has at least one camera to render it.
	*\~french
	*\brief
	*	Classe de gestion d'un scène
	*\remarks
	*	Une scène est une collection de lumières, noeuds et géométries.
	*	<br />Elle a au moins une caméra permettant son rendu.
	*/
	class Scene;
	/**
	*\~english
	*\brief
	*	CSCN file parser.
	*\remarks
	*	Reads CSCN files and extracts all 3D data from it.
	*\~french
	*\brief
	*	Analyseur de fichiers CSCN.
	*\remarks
	*	Lit les fichiers CSCN et en extrait toutes les données 3D.
	*/
	class SceneFileParser;
	/**
	*\~english
	*\brief
	*	Base class for external file import
	*\~french
	*\brief
	*	Classe de base pour l'import de fichiers externes
	*/
	class SceneImporter;
	/**
	*\~english
	*\brief
	*	The scene node handler class
	*\remarks
	*	A scene node is a parent for nearly every object in a scene : geometry, camera, ...
	*\~french
	*\brief
	*	La classe de gestion de noeud de scène
	*\remarks
	*	Un noeud de scène est un parent pour à peu près tous les objets d'une scène : géométrie, caméra, ...
	*/
	class SceneNode;
	/**
	*\~english
	*\brief
	*	Base class for external file import
	*\~french
	*\brief
	*	Classe de base pour l'import de fichiers externes
	*/
	class SceneNodeImporter;
	/**
	*\~english
	*\brief
	*	Shadows configuration class.
	*\~french
	*\brief
	*	Classe de configuration des ombres.
	*/
	struct ShadowConfig;

	CU_DeclareSmartPtr( castor3d, BillboardBase, C3D_API );
	CU_DeclareSmartPtr( castor3d, BillboardList, C3D_API );
	CU_DeclareSmartPtr( castor3d, Camera, C3D_API );
	CU_DeclareSmartPtr( castor3d, CameraImporter, C3D_API );
	CU_DeclareSmartPtr( castor3d, Geometry, C3D_API );
	CU_DeclareSmartPtr( castor3d, MovableObject, C3D_API );
	CU_DeclareSmartPtr( castor3d, SceneFileParser, C3D_API );
	CU_DeclareSmartPtr( castor3d, SceneNode, C3D_API );
	CU_DeclareSmartPtr( castor3d, Scene, C3D_API );
	CU_DeclareSmartPtr( castor3d, SceneImporter, C3D_API );
	CU_DeclareSmartPtr( castor3d, SceneNodeImporter, C3D_API );
	CU_DeclareSmartPtr( castor3d, ShadowConfig, C3D_API );

	//! SceneNode pointer array.
	CU_DeclareVector( SceneNodeRPtr, SceneNodePtr );
	//! Scene pointer map, sorted by name.
	CU_DeclareMap( castor::String, SceneRPtr, ScenePtrStr );

	using OnSceneChangedFunction = castor::Function< void( Scene const & ) >;
	using OnSceneChanged = castor::SignalT< OnSceneChangedFunction >;
	using OnSceneChangedConnection = OnSceneChanged::connection;

	using OnSceneUpdateFunction = castor::Function< void( Scene const & ) >;
	using OnSceneUpdate = castor::SignalT< OnSceneUpdateFunction >;
	using OnSceneUpdateConnection = OnSceneUpdate::connection;

	using OnCameraChangedFunction = castor::Function< void( Camera const & ) >;
	using OnCameraChanged = castor::SignalT< OnCameraChangedFunction >;
	using OnCameraChangedConnection = OnCameraChanged::connection;

	using OnSceneNodeChangedFunction = castor::Function< void( SceneNode const & ) >;
	using OnSceneNodeChanged = castor::SignalT< OnSceneNodeChangedFunction >;
	using OnSceneNodeChangedConnection = OnSceneNodeChanged::connection;

	using OnSceneNodeReparentFunction = castor::Function< void( SceneNode & ) >;
	using OnSceneNodeReparent = castor::SignalT< OnSceneNodeReparentFunction >;
	using OnSceneNodeReparentConnection = OnSceneNodeReparent::connection;

	using SubmeshMaterialMap = castor::Map< Submesh const *, MaterialObs >;

	C3D_API castor::LoggerInstance & getLogger( Scene const & scene );
	C3D_API Engine & getEngine( Scene const & scene );
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
	template<>
	struct PtrCacheTraitsT< Scene, castor::String >
		: PtrCacheTraitsBaseT< Scene, castor::String >
	{
		using ResT = Scene;
		using KeyT = castor::String;
		using Base = PtrCacheTraitsBaseT< ResT, KeyT >;
		using ElementT = typename Base::ElementT;
		using ElementPtrT = typename Base::ElementPtrT;

		C3D_API static const castor::String Name;
	};
	/**
	*\~english
	*	Helper structure to build a castor3d::ObjectCacheTraitsT.
	*\remarks
	*	Predefines:
	*	<ul>
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
	*	Hence, only remains to define:
	*	<ul>
	*	<li>Name: The element type name.</li>
	*	</ul>
	*\~french
	*	Structure d'aide à la création d'un castor3d::ObjectCacheTraitsT.
	*\remarks
	*	Prédéfinit:
	*	<ul>
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
	*	Il ne reste donc ainsi qu'à définir:
	*	<ul>
	*	<li>Name: Le nom du type d'élément.</li>
	*	</ul>
	*/
	template< typename ObjT, typename KeyT >
	struct ObjectCacheTraitsBaseT
	{
		using ElementT = ObjT;
		using ElementKeyT = KeyT;
		using ElementPtrT = castor::UniquePtr< ElementT >;
		using ElementObsT = ElementT *;
		using ElementContT = castor::Map< KeyT, ElementPtrT >;
		using ElementCacheT = ObjectCacheBaseT< ElementT, KeyT >;

		using ElementInitialiserT = castor::Function< void( ElementT & ) >;
		using ElementCleanerT = castor::Function< void( ElementT & ) >;
		using ElementMergerT = castor::Function< void( ElementCacheT const &
			, ElementContT &
			, ElementPtrT
			, SceneNodeRPtr
			, SceneNodeRPtr ) >;
		using ElementAttacherT = castor::Function< void( ElementT &
			, SceneNode &
			, SceneNodeRPtr
			, SceneNodeRPtr
			, SceneNodeRPtr ) >;
		using ElementDetacherT = castor::Function< void( ElementT & ) >;

		template< typename ... ParametersT >
		static ElementPtrT makeElement( ElementCacheT const & cache
			, ElementKeyT const & key
			, ParametersT && ... params )
		{
			return castor::makeUnique< ElementT >( key
				, castor::forward< ParametersT >( params )... );
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
	*	Helper structure to specialise a scene objects cache behaviour.
	*\remarks
	*	Specialisation for BillboardList.
	*\~french
	*	Structure permettant de spécialiser le comportement d'un cache d'objets de scène.
	*\remarks
	*	Spécialisation pour BillboardList.
	*/
	template<>
	struct ObjectCacheTraitsT< BillboardList, castor::String >
		: ObjectCacheTraitsBaseT< BillboardList, castor::String >
	{
		using KeyT = castor::String;
		using ElementT = BillboardList;
		using BaseT = ObjectCacheTraitsBaseT< ElementT, KeyT >;
		using ElementPtrT = typename BaseT::ElementPtrT;

		C3D_API static const castor::String Name;
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
	template<>
	struct ObjectCacheTraitsT< Camera, castor::String >
		: ObjectCacheTraitsBaseT< Camera, castor::String >
	{
		using KeyT = castor::String;
		using ElementT = Camera;
		using BaseT = ObjectCacheTraitsBaseT< ElementT, KeyT >;
		using ElementPtrT = typename BaseT::ElementPtrT;

		C3D_API static const castor::String Name;
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
	template<>
	struct ObjectCacheTraitsT< Geometry, castor::String >
		: ObjectCacheTraitsBaseT< Geometry, castor::String >
	{
		using KeyT = castor::String;
		using ElementT = Geometry;
		using BaseT = ObjectCacheTraitsBaseT< ElementT, KeyT >;
		using ElementPtrT = typename BaseT::ElementPtrT;

		C3D_API static const castor::String Name;
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
	template<>
	struct ObjectCacheTraitsT< SceneNode, castor::String >
		: ObjectCacheTraitsBaseT< SceneNode, castor::String >
	{
		using KeyT = castor::String;
		using ElementT = SceneNode;
		using BaseT = ObjectCacheTraitsBaseT< ElementT, KeyT >;
		using ElementPtrT = typename BaseT::ElementPtrT;

		C3D_API static const castor::String Name;
	};

	template<>
	class ObjectCacheT< BillboardList, castor::String >;
	template<>
	class ObjectCacheT< Geometry, castor::String >;

	using SceneCacheTraits = PtrCacheTraitsT< Scene, castor::String >;
	using BillboardListCacheTraits = ObjectCacheTraitsT< BillboardList, castor::String >;
	using CameraCacheTraits = ObjectCacheTraitsT< Camera, castor::String >;
	using GeometryCacheTraits = ObjectCacheTraitsT< Geometry, castor::String >;
	using SceneNodeCacheTraits = ObjectCacheTraitsT< SceneNode, castor::String >;

	using SceneCache = castor::ResourceCacheT< Scene
		, castor::String
		, SceneCacheTraits >;
	using BillboardListCache = ObjectCacheT< BillboardList
		, castor::String
		, BillboardListCacheTraits >;
	using CameraCache = ObjectCacheT< Camera
		, castor::String
		, CameraCacheTraits >;
	using GeometryCache = ObjectCacheT< Geometry
		, castor::String
		, GeometryCacheTraits >;
	using SceneNodeCache = ObjectCacheT< SceneNode
		, castor::String
		, SceneNodeCacheTraits >;

	using SceneRes = SceneCacheTraits::ElementPtrT;
	using SceneResPtr = SceneCacheTraits::ElementObsT;
	using BillboardListRes = BillboardListCacheTraits::ElementPtrT;
	using BillboardListResPtr = BillboardListCacheTraits::ElementObsT;
	using CameraRes = CameraCacheTraits::ElementPtrT;
	using CameraResPtr = CameraCacheTraits::ElementObsT;
	using GeometryRes = CameraCacheTraits::ElementPtrT;
	using GeometryResPtr = CameraCacheTraits::ElementObsT;
	using SceneNodeRes = CameraCacheTraits::ElementPtrT;
	using SceneNodeResPtr = CameraCacheTraits::ElementObsT;
	using MaterialCacheView = CacheViewT< MaterialCache, EventType( GpuEventType::ePreUpload ) >;
	using SamplerCacheView = CacheViewT< SamplerCache, EventType( GpuEventType::ePreUpload ) >;
	using FontCacheView = CacheViewT< castor::FontCache, EventType( CpuEventType::ePreGpuStep ) >;

	CU_DeclareSmartPtr( castor3d, SceneCache, C3D_API );
	CU_DeclareSmartPtr( castor3d, BillboardListCache, C3D_API );
	CU_DeclareSmartPtr( castor3d, CameraCache, C3D_API );
	CU_DeclareSmartPtr( castor3d, GeometryCache, C3D_API );
	CU_DeclareSmartPtr( castor3d, SceneNodeCache, C3D_API );
	CU_DeclareSmartPtr( castor3d, MaterialCacheView, C3D_API );
	CU_DeclareSmartPtr( castor3d, SamplerCacheView, C3D_API );
	CU_DeclareSmartPtr( castor3d, FontCacheView, C3D_API );

	//@}
}

CU_DeclareExportedOwnedBy( C3D_API, castor3d::Scene, Scene )

#endif
