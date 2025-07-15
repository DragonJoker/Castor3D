/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightModule_H___
#define ___C3D_LightModule_H___

#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Scene/SceneModule.hpp"
#include "Castor3D/Shader/ShaderBuffers/ShaderBuffersModule.hpp"

namespace c3d
{
	/**@name Scene */
	//@{
	/**@name Light */
	//@{

	using LightingModelID = uint16_t;
	using BrdfID = uint8_t;
	/**
	*\~english
	*\brief
	*	Light types enumeration
	*\~french
	*\brief
	*	Enumération des types de lumières
	*/
	enum class LightType
		: uint8_t
	{
		//!\~english Directional light type.
		//!\~french Lumière directionnelle.
		eDirectional = 0,
		//!\~english Point light type.
		//!\~french Lumière ponctuelle.
		ePoint = 1,
		//!\~english Spot light type.
		//!\~french Lumière projecteur.
		eSpot = 2,
		CU_ScopedEnumBounds( eDirectional, eSpot )
	};
	C3D_API String getName( LightType value );
	/**
	*\~english
	*\brief
	*	Light source implementation.
	*\~french
	*\brief
	*	Implémentation d'une source lumineuse.
	*\remark
	*/
	class Light;
	/**
	*\~english
	*\brief
	*	Light sources group implementation.
	*\~french
	*\brief
	*	Implémentation d'un groupe de sources lumineuses.
	*\remark
	*/
	class LightGroup;
	/**
	*\~english
	*\brief
	*	Base class for external file import
	*\~french
	*\brief
	*	Classe de base pour l'import de fichiers externes
	*/
	class LightImporter;
	/**
	*\~english
	*\brief
	*	The light source factory
	*\~french
	*\brief
	*	La fabrique de sources lumineuse
	*/
	class LightFactory;
	/**
	*\~english
	*\brief
	*	Holds light components
	*\remarks
	*	Derived into DirectionalLight, PointLight and SpotLight
	*\~french
	*\brief
	*	Contient les composantes d'une lumière
	*\remarks
	*	Dérivée en DirectionalLight, PointLight et SpotLight
	*/
	class LightCategory;
	/**
	*\~english
	*\brief
	*	Class which represents a Directional Light
	*\remarks
	*	A directional light is a light which enlights from an infinite point in a given direction
	*\~french
	*\brief
	*	Classe representant une lumière directionnelle
	*\remarks
	*	Une lumière directionnelle illumine à partir d'un point infini dans une direction donnée
	*/
	class DirectionalLight;
	/**
	*\~english
	*\brief
	*	Class which represents a PointLight
	*\remarks
	*	A point light is a light which enlights from a point in all directions.
	*\~french
	*\brief
	*	Classe de représentation d'une PointLight
	*\remarks
	*	Une point light est une source de lumière à un point donné qui illumine dans toutes les directions
	*/
	class PointLight;
	/**
	*\~english
	*\brief
	*	Class which represents a SpotLight
	*\remarks
	*	A spot light is a light which enlights from a point in a given direction with a conic shape
	*\~french
	*\brief
	*	Classe de représentation d'une PointLight
	*\remarks
	*	Une spot light est une source de lumière à un point donné qui illumine dans une direction donnée en suivant une forme cônique
	*/
	class SpotLight;
	/**
	*\~english
	*\brief
	*	Holds light components that derive from a LightCategory and a Light.
	*\~french
	*\brief
	*	Contient les composantes d'une lumière dérivant d'une LightCategory et d'une Light.
	*/
	class LightInstance;
	/**
	*\~english
	*\brief
	*	Class which represents a Directional light instance.
	*\~french
	*\brief
	*	Classe representant une instance de lumière directionnelle.
	*/
	class DirectionalLightInstance;
	/**
	*\~english
	*\brief
	*	Class which represents a Point light instance.
	*\~french
	*\brief
	*	Classe de représentation d'une instance de lumière ponctuelle.
	*/
	class PointLightInstance;
	/**
	*\~english
	*\brief
	*	Class which represents a Spot light instance.
	*\~french
	*\brief
	*	Classe de représentation d'une instance de lumière projecteur.
	*/
	class SpotLightInstance;

	struct LightCreateInfo
	{
		NotNullT< Scene > scene;
		NotNullT< SceneNode > parentNode;
		NotNullT< LightFactory > factory;
		LightType lightType;
	};

	struct LightGroupCreateInfo
	{
		NotNullT< Scene > scene;
		NotNullT< LightFactory > factory;
		LightType lightType;
	};

	using OnLightChangedFunction = Function< void( LightInstance & ) >;
	using OnLightChanged = SignalT< OnLightChangedFunction >;
	using OnLightChangedConnection = OnLightChanged::connection;
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
	template<>
	struct ObjectCacheTraitsT< Light, String >
		: ObjectCacheTraitsBaseT< Light, String >
	{
		using KeyT = String;
		using ElementT = Light;
		using BaseT = ObjectCacheTraitsBaseT< ElementT, KeyT >;
		using ElementPtrT = typename BaseT::ElementPtrT;

		C3D_API static const String Name;
	};

	template<>
	class ObjectCacheT< Light, String >;

	using LightCacheTraits = ObjectCacheTraitsT< Light, String >;
	using LightCache = ObjectCacheT< Light
		, String
		, LightCacheTraits >;
	using LightRes = CameraCacheTraits::ElementPtrT;
	using LightResPtr = CameraCacheTraits::ElementObsT;

	/**
	*\~english
	*	Helper structure to specialise a cache behaviour.
	*\remarks
	*	Specialisation for LightGroup.
	*\~french
	*	Structure permettant de spécialiser le comportement d'un cache.
	*\remarks
	*	Spécialisation pour LightGroup.
	*/
	template<>
	struct PtrCacheTraitsT< LightGroup, String >
		: PtrCacheTraitsBaseT< LightGroup, String >
	{
		using ResT = LightGroup;
		using KeyT = String;
		using Base = PtrCacheTraitsBaseT< ResT, KeyT >;
		using ElementT = typename Base::ElementT;
		using ElementPtrT = typename Base::ElementPtrT;

		C3D_API static const String Name;
	};

	using LightGroupCacheTraits = PtrCacheTraitsT< LightGroup, String >;
	using LightGroupCache = ResourceCacheT< LightGroup
		, String
		, LightGroupCacheTraits >;
	using LightGroupPtr = LightGroupCacheTraits::ElementPtrT;
	using LightGroupObs = LightGroupCacheTraits::ElementObsT;

	struct BaseShadowData
	{
		u32 shadowType;
		u32 cascadeCount;
		f32 pcfFilterSize;
		u32 pcfSampleCount;
		Point2f rawShadowsOffsets;
		Point2f pcfShadowsOffsets;
		f32 vsmMinVariance;
		f32 vsmLightBleedingReduction;
		u32 volumetricSteps;
		f32 volumetricScattering;
	};

	struct DirectionalShadowData
		: BaseShadowData
	{
		using CascasdeFloatArray = Array< f32, ashes::getAlignedSize( MaxDirectionalCascadesCount, 4u ) >;
		CascasdeFloatArray splitDepths;
		CascasdeFloatArray splitScales;
		Array< Matrix4x4f, MaxDirectionalCascadesCount > transforms;
	};

	struct PointShadowData
		: BaseShadowData
	{
		Point4f position;
	};

	struct SpotShadowData
		: BaseShadowData
	{
		Matrix4x4f transform;
	};

	struct AllShadowData
	{
		DirectionalShadowData directional;
		Array< PointShadowData, MaxPointShadowMapCount > point;
		Array< SpotShadowData, MaxSpotShadowMapCount > spot;
	};

	/** @cond !Doxygen */
	CU_DeclareSmartPtr( c3d, Light, C3D_API );
	CU_DeclareSmartPtr( c3d, LightCache, C3D_API );
	CU_DeclareSmartPtr( c3d, LightCategory, C3D_API );
	CU_DeclareSmartPtr( c3d, LightFactory, C3D_API );
	CU_DeclareSmartPtr( c3d, LightGroup, C3D_API );
	CU_DeclareSmartPtr( c3d, LightGroupCache, C3D_API );
	CU_DeclareSmartPtr( c3d, LightImporter, C3D_API );
	CU_DeclareSmartPtr( c3d, LightInstance, C3D_API );
	CU_DeclareSmartPtr( c3d, DirectionalLight, C3D_API );
	CU_DeclareSmartPtr( c3d, PointLight, C3D_API );
	CU_DeclareSmartPtr( c3d, SpotLight, C3D_API );

	//! Array of light instances
	CU_DeclareVector( LightInstanceRPtr, LightInstances );
	//! Array of lights
	CU_DeclareVector( LightRPtr, Lights );
	//! Array to non owning light pointers
	CU_DeclareVector( LightRPtr, LightsRef );
	//! Array of lights per light type.
	/** @endcond */

	/**
	*\~english
	*\brief
	*	Retrieve the maximum lit distance for given light and attenuation.
	*\param[in] colour
	*	The light source colour.
	*\param[in] intensity
	*	The light source intensity.
	*\param[in] attenuation
	*	The attenuation values.
	*\return
	*	The value.
	*\~french
	*\brief
	*	Calcule la distance maximale d'éclairage d'une source lumineuse selon son atténuation.
	*\param[in] colour
	*	La couleur de la source lumineuse.
	*\param[in] intensity
	*	L'intensité de la source lumineuse.
	*\param[in] attenuation
	*	Les valeurs d'atténuation.
	*\return
	*	La valeur.
	*/
	C3D_API float getMaxDistance( Point3f const & colour
		, LuminousIntensity const & intensity
		, Point3f const & attenuation );

	C3D_API float computeRange( LuminousIntensity const & intensity
		, float range );

	//@}
	//@}
}

#endif
