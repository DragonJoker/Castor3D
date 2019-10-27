/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PREREQUISITES_SCENE_H___
#define ___C3D_PREREQUISITES_SCENE_H___

namespace castor3d
{
	/**@name Scene */
	//@{

	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Movable object types enumerator
	\remark		There are four movable object types : camera, geometry, light and billboard.
	\~french
	\brief		Enumération des types de MovableObject
	\remark		Il y a quatre types d'objets déplaçables : caméra, géométrie, lumière et billboard
	*/
	enum class MovableType
		: uint8_t
	{
		eCamera,
		eGeometry,
		eLight,
		eBillboard,
		eParticleEmitter,
		CU_ScopedEnumBounds( eCamera )
	};
	castor::String getName( MovableType value );
	/**
	*\version
	*	0.9.0
	*\~english
	*\brief
	*	Shadow filter types enumeration.
	*\~french
	*\brief
	*	Enumération des types de filtrage des ombres.
	*/
	enum class ShadowType
	{
		//!\~english	No shadows at all.
		//!\~french		Pas d'ombres du tout.
		eNone,
		//!\~english	No filtering.
		//!\~french		Pas de filtrage.
		eRaw,
		//!\~english	PCF filtering.
		//!\~french		Filtrage PCF.
		ePCF,
		//!\~english	Variance shadow mapping.
		//!\~french		Filtrage par variance.
		eVariance,
		CU_ScopedEnumBounds( eNone )
	};
	castor::String getName( ShadowType value );
	/**
	*\version
	*	0.9.0
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
		eDisabled,
		//!\~english	Fog intensity increases linearly with distance to camera.
		//!\~french		L'intensité du brouillard augmente lin�airement avec la distance à la caméra.
		eLinear,
		//!\~english	Fog intensity increases exponentially with distance to camera.
		//!\~french		L'intensité du brouillard augmente exponentiellement avec la distance à la caméra.
		eExponential,
		//!\~english	Fog intensity increases even more with distance to camera.
		//!\~french		L'intensité du brouillard augmente encore plus avec la distance à la caméra.
		eSquaredExponential,
		CU_ScopedEnumBounds( eDisabled )
	};
	castor::String getName( FogType value );
	/**
	*\version
	*	0.9.0
	*\~english
	*\brief
	*	Scene flags enumeration.
	*\~french
	*\brief
	*	Enumération des indicateurs de scène.
	*/
	enum class SceneFlag
		: uint8_t
	{
		//!\~english	No flag.
		//!\~french		Aucun indicateur.
		eNone = 0x00,
		//!\~english	Linear fog.
		//!\~french		Brouillard linéaire.
		eFogLinear = 0x01,
		//!\~english	Exponential fog.
		//!\~french		Brouillard exponentiel.
		eFogExponential = 0x02,
		//!\~english	Squared exponential fog.
		//!\~french		Brouillard exponentiel au carré.
		eFogSquaredExponential = 0x04,
		//!\~english	All flags.
		//!\~french		Tous les indicateurs.
		eAll = 0x07,
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
	*\version
	*	0.11.0
	*\~english
	*\brief
	*	Scene background types enumeration.
	*\~french
	*\brief
	*	Enumération des types de fond de scène.
	*/
	enum class BackgroundType
	{
		eColour,
		eImage,
		eSkybox,
		CU_ScopedEnumBounds( eColour )
	};
	castor::String getName( BackgroundType value );
	/**
	*\version
	*	0.9.0
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
		eSpherical,
		//!\~english	Billboards rotate only on Y axis.
		//!\~french		Les billboards tournent uniquement sur l'axe Y.
		eCylindrical,
		CU_ScopedEnumBounds( eSpherical )
	};
	castor::String getName( BillboardType value );
	/**
	*\version
	*	0.9.0
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
		eDynamic,
		//!\~english	Billboards size doesn't change.
		//!\~french		Les dimensions des billboards ne changent pas.
		eFixed,
		CU_ScopedEnumBounds( eDynamic )
	};
	castor::String getName( BillboardSize value );

	enum class ParticleFormat
	{
		eInt,
		eVec2i,
		eVec3i,
		eVec4i,
		eUInt,
		eVec2ui,
		eVec3ui,
		eVec4ui,
		eFloat,
		eVec2f,
		eVec3f,
		eVec4f,
		eMat2f,
		eMat3f,
		eMat4f,
	};
	castor::String getName( ParticleFormat value );

	class Scene;
	class SceneLoader;
	class SceneNode;
	class SceneFileContext;
	class SceneFileParser;
	class SceneBackground;
	class SkyboxBackground;
	class ColourBackground;
	class ImageBackground;
	class BackgroundVisitor;
	class ParticleSystemImpl;
	class ParticleSystem;
	class CpuParticleSystem;
	class ComputeParticleSystem;
	class TransformFeedbackParticleSystem;
	class Fog;
	class AnimatedObject;
	class AnimatedObjectGroup;
	class AnimatedSkeleton;
	class AnimatedMesh;
	class AnimationInstance;
	class SkeletonAnimationInstance;
	class SkeletonAnimationInstanceKeyFrame;
	class SkeletonAnimationInstanceObject;
	class SkeletonAnimationInstanceNode;
	class SkeletonAnimationInstanceBone;
	class MeshAnimationInstance;
	class MeshAnimationInstanceSubmesh;
	class BillboardBase;
	class BillboardList;

	CU_DeclareSmartPtr( SceneNode );
	CU_DeclareSmartPtr( Scene );
	CU_DeclareSmartPtr( SceneFileContext );
	CU_DeclareSmartPtr( SceneFileParser );
	CU_DeclareSmartPtr( SceneBackground );
	CU_DeclareSmartPtr( ParticleSystem );
	CU_DeclareSmartPtr( ParticleSystemImpl );
	CU_DeclareSmartPtr( CpuParticleSystem );
	CU_DeclareSmartPtr( TransformFeedbackParticleSystem );
	CU_DeclareSmartPtr( ComputeParticleSystem );
	CU_DeclareSmartPtr( AnimatedObject );
	CU_DeclareSmartPtr( AnimatedObjectGroup );
	CU_DeclareSmartPtr( AnimatedSkeleton );
	CU_DeclareSmartPtr( AnimatedMesh );
	CU_DeclareSmartPtr( AnimationInstance );
	CU_DeclareSmartPtr( SkeletonAnimationInstance );
	CU_DeclareSmartPtr( SkeletonAnimationInstanceObject );
	CU_DeclareSmartPtr( SkeletonAnimationInstanceNode );
	CU_DeclareSmartPtr( SkeletonAnimationInstanceBone );
	CU_DeclareSmartPtr( MeshAnimationInstance );
	CU_DeclareSmartPtr( BillboardBase );
	CU_DeclareSmartPtr( BillboardList );

	//! SceneNode pointer array.
	CU_DeclareVector( SceneNodeSPtr, SceneNodePtr );
	//! Scene pointer map, sorted by name.
	CU_DeclareMap( castor::String, SceneSPtr, ScenePtrStr );
	//! SceneNode pointer map, sorted by name.
	CU_DeclareMap( castor::String, SceneNodeSPtr, SceneNodePtrStr );
	//! AnimatedObjectGroup pointer map, sorted by name.
	CU_DeclareMap( castor::String, AnimatedObjectGroupSPtr, AnimatedObjectGroupPtrStr );
	//! AnimatedObject pointer map, sorted by name.
	CU_DeclareMap( castor::String, AnimatedObjectSPtr, AnimatedObjectPtrStr );
	//! MovingObject pointer map, sorted by name.
	CU_DeclareMap( castor::String, SkeletonAnimationInstanceObjectSPtr, SkeletonAnimationInstanceObjectPtrStr );
	//! SkeletonAnimationInstanceObject pointer array.
	CU_DeclareVector( SkeletonAnimationInstanceObjectSPtr, SkeletonAnimationInstanceObjectPtr );
	//! MeshAnimationInstanceSubmesh map, sorted by submesh ID.
	CU_DeclareMap( uint32_t, MeshAnimationInstanceSubmesh, MeshAnimationInstanceSubmesh );
	//! Animation pointer map, sorted by name.
	CU_DeclareMap( castor::String, AnimationInstanceUPtr, AnimationInstancePtrStr );
	//! Skeleton animation instance pointer array.
	CU_DeclareVector( std::reference_wrapper< SkeletonAnimationInstance >, SkeletonAnimationInstance );

	using OnSceneChangedFunction = std::function< void( Scene const & ) >;
	using OnSceneChanged = castor::Signal< OnSceneChangedFunction >;
	using OnSceneChangedConnection = OnSceneChanged::connection;

	using OnSceneUpdateFunction = std::function< void( Scene const & ) >;
	using OnSceneUpdate = castor::Signal< OnSceneUpdateFunction >;
	using OnSceneUpdateConnection = OnSceneUpdate::connection;

	using OnCameraChangedFunction = std::function< void( Camera const & ) >;
	using OnCameraChanged = castor::Signal< OnCameraChangedFunction >;
	using OnCameraChangedConnection = OnCameraChanged::connection;

	using OnSceneNodeChangedFunction = std::function< void( SceneNode const & ) >;
	using OnSceneNodeChanged = castor::Signal< OnSceneNodeChangedFunction >;
	using OnSceneNodeChangedConnection = OnSceneNodeChanged::connection;

	using SubmeshBoundingBoxList = std::vector< std::pair< Submesh const *, castor::BoundingBox > >;
	using SubmeshBoundingBoxMap = std::map< Submesh const *, castor::BoundingBox >;
	using SubmeshBoundingSphereMap = std::map< Submesh const *, castor::BoundingSphere >;
	using SubmeshMaterialMap = std::map< Submesh const *, MaterialWPtr >;

	using OnSubmeshMaterialChangedFunction = std::function< void( Geometry const &, Submesh const &, MaterialSPtr oldMaterial, MaterialSPtr newMaterial) >;
	using OnSubmeshMaterialChanged = castor::Signal< OnSubmeshMaterialChangedFunction >;
	using OnSubmeshMaterialChangedConnection = OnSubmeshMaterialChanged::connection;

	using OnBillboardMaterialChangedFunction = std::function< void( BillboardBase const &, MaterialSPtr oldMaterial, MaterialSPtr newMaterial ) >;
	using OnBillboardMaterialChanged = castor::Signal< OnBillboardMaterialChangedFunction >;
	using OnBillboardMaterialChangedConnection = OnBillboardMaterialChanged::connection;

	using OnAnimatedSkeletonChangeFunction = std::function< void( AnimatedObjectGroup const &, AnimatedSkeleton const & ) >;
	using OnAnimatedSkeletonChange = castor::Signal< OnAnimatedSkeletonChangeFunction >;
	using OnAnimatedSkeletonChangeConnection = OnAnimatedSkeletonChange::connection;

	using OnAnimatedMeshChangeFunction = std::function< void( AnimatedObjectGroup const &, AnimatedMesh const & ) >;
	using OnAnimatedMeshChange = castor::Signal< OnAnimatedMeshChangeFunction >;
	using OnAnimatedMeshChangeConnection = OnAnimatedMeshChange::connection;

	//@}
}

#endif
