/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PREREQUISITES_SCENE_H___
#define ___C3D_PREREQUISITES_SCENE_H___

namespace castor3d
{
	/**@name Scene */
	//@{

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
		CASTOR_SCOPED_ENUM_BOUNDS( eNone )
	};
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
		//!\~french		
		eExponential,
		//!\~english	Fog intensity increases even more with distance to camera.
		//!\~french		L'intensité du brouillard augmente encore plus avec la distance à la caméra.
		eSquaredExponential,
		CASTOR_SCOPED_ENUM_BOUNDS( eDisabled )
	};
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
		: uint16_t
	{
		//!\~english	No flag.
		//!\~french		Aucun indicateur.
		eNone = 0x000,
		//!\~english	Linear fog.
		//!\~french		Brouillard linéaire.
		eFogLinear = 0x001,
		//!\~english	Exponential fog.
		//!\~french		Brouillard exponentiel.
		eFogExponential = 0x002,
		//!\~english	Squared exponential fog.
		//!\~french		Brouillard exponentiel au carré.
		eFogSquaredExponential = 0x004,
		CASTOR_SCOPED_ENUM_BOUNDS( eNone )
	};
	IMPLEMENT_FLAGS( SceneFlag )
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
		CASTOR_SCOPED_ENUM_BOUNDS( eColour )
	};
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
		CASTOR_SCOPED_ENUM_BOUNDS( eSpherical )
	};
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
		CASTOR_SCOPED_ENUM_BOUNDS( eDynamic )
	};

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

	DECLARE_SMART_PTR( SceneNode );
	DECLARE_SMART_PTR( Scene );
	DECLARE_SMART_PTR( SceneFileContext );
	DECLARE_SMART_PTR( SceneFileParser );
	DECLARE_SMART_PTR( SceneBackground );
	DECLARE_SMART_PTR( ParticleSystem );
	DECLARE_SMART_PTR( ParticleSystemImpl );
	DECLARE_SMART_PTR( CpuParticleSystem );
	DECLARE_SMART_PTR( TransformFeedbackParticleSystem );
	DECLARE_SMART_PTR( ComputeParticleSystem );
	DECLARE_SMART_PTR( AnimatedObject );
	DECLARE_SMART_PTR( AnimatedObjectGroup );
	DECLARE_SMART_PTR( AnimatedSkeleton );
	DECLARE_SMART_PTR( AnimatedMesh );
	DECLARE_SMART_PTR( AnimationInstance );
	DECLARE_SMART_PTR( SkeletonAnimationInstance );
	DECLARE_SMART_PTR( SkeletonAnimationInstanceObject );
	DECLARE_SMART_PTR( SkeletonAnimationInstanceNode );
	DECLARE_SMART_PTR( SkeletonAnimationInstanceBone );
	DECLARE_SMART_PTR( MeshAnimationInstance );
	DECLARE_SMART_PTR( BillboardBase );
	DECLARE_SMART_PTR( BillboardList );

	//! SceneNode pointer array.
	DECLARE_VECTOR( SceneNodeSPtr, SceneNodePtr );
	//! Scene pointer map, sorted by name.
	DECLARE_MAP( castor::String, SceneSPtr, ScenePtrStr );
	//! SceneNode pointer map, sorted by name.
	DECLARE_MAP( castor::String, SceneNodeSPtr, SceneNodePtrStr );
	//! AnimatedObjectGroup pointer map, sorted by name.
	DECLARE_MAP( castor::String, AnimatedObjectGroupSPtr, AnimatedObjectGroupPtrStr );
	//! AnimatedObject pointer map, sorted by name.
	DECLARE_MAP( castor::String, AnimatedObjectSPtr, AnimatedObjectPtrStr );
	//! MovingObject pointer map, sorted by name.
	DECLARE_MAP( castor::String, SkeletonAnimationInstanceObjectSPtr, SkeletonAnimationInstanceObjectPtrStr );
	//! SkeletonAnimationInstanceObject pointer array.
	DECLARE_VECTOR( SkeletonAnimationInstanceObjectSPtr, SkeletonAnimationInstanceObjectPtr );
	//! MeshAnimationInstanceSubmesh map, sorted by submesh ID.
	DECLARE_MAP( uint32_t, MeshAnimationInstanceSubmesh, MeshAnimationInstanceSubmesh );
	//! Animation pointer map, sorted by name.
	DECLARE_MAP( castor::String, AnimationInstanceUPtr, AnimationInstancePtrStr );
	//! Skeleton animation instance pointer array.
	DECLARE_VECTOR( std::reference_wrapper< SkeletonAnimationInstance >, SkeletonAnimationInstance );

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
