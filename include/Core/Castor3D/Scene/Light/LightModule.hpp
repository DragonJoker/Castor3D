/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightModule_H___
#define ___C3D_LightModule_H___

#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Scene/SceneModule.hpp"
#include "Castor3D/Shader/ShaderBuffers/ShaderBuffersModule.hpp"

#include "Castor3D/Limits.hpp"

namespace castor3d
{
	/**@name Scene */
	//@{
	/**@name Light */
	//@{

	using LightingModelID = uint8_t;
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
	C3D_API castor::String getName( LightType value );
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

	CU_DeclareSmartPtr( castor3d, Light, C3D_API );
	CU_DeclareSmartPtr( castor3d, LightFactory, C3D_API );
	CU_DeclareSmartPtr( castor3d, LightCategory, C3D_API );
	CU_DeclareSmartPtr( castor3d, LightImporter, C3D_API );
	CU_DeclareSmartPtr( castor3d, DirectionalLight, C3D_API );
	CU_DeclareSmartPtr( castor3d, PointLight, C3D_API );
	CU_DeclareSmartPtr( castor3d, SpotLight, C3D_API );

	//! Array of lights
	CU_DeclareVector( LightRPtr, Lights );
	//! Array to non owning light pointers
	CU_DeclareVector( LightRPtr, LightsRef );
	//! Array of lights per light type.
	using LightsMap = castor::Array< LightsArray, size_t( LightType::eCount ) >;

	using OnLightChangedFunction = castor::Function< void( Light & ) >;
	using OnLightChanged = castor::SignalT< OnLightChangedFunction >;
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
	struct ObjectCacheTraitsT< Light, castor::String >
		: ObjectCacheTraitsBaseT< Light, castor::String >
	{
		using KeyT = castor::String;
		using ElementT = Light;
		using BaseT = ObjectCacheTraitsBaseT< ElementT, KeyT >;
		using ElementPtrT = typename BaseT::ElementPtrT;

		C3D_API static const castor::String Name;
	};

	template<>
	class ObjectCacheT< Light, castor::String >;

	using LightCacheTraits = ObjectCacheTraitsT< Light, castor::String >;
	using LightCache = ObjectCacheT< Light
		, castor::String
		, LightCacheTraits >;
	using LightRes = CameraCacheTraits::ElementPtrT;
	using LightResPtr = CameraCacheTraits::ElementObsT;

	CU_DeclareSmartPtr( castor3d, LightCache, C3D_API );

	struct BaseShadowData
	{
		u32 shadowType;
		u32 cascadeCount;
		f32 pcfFilterSize;
		u32 pcfSampleCount;
		castor::Point2f rawShadowsOffsets;
		castor::Point2f pcfShadowsOffsets;
		f32 vsmMinVariance;
		f32 vsmLightBleedingReduction;
		u32 volumetricSteps;
		f32 volumetricScattering;
	};

	struct DirectionalShadowData
		: BaseShadowData
	{
		using CascasdeFloatArray = castor::Array< f32, ashes::getAlignedSize( MaxDirectionalCascadesCount, 4u ) >;
		CascasdeFloatArray splitDepths;
		CascasdeFloatArray splitScales;
		castor::Array< castor::Matrix4x4f, MaxDirectionalCascadesCount > transforms;
	};

	struct SpotShadowData
		: BaseShadowData
	{
		castor::Matrix4x4f transform;
	};

	using PointShadowData = BaseShadowData;

	struct AllShadowData
	{
		DirectionalShadowData directional;
		castor::Array< PointShadowData, MaxPointShadowMapCount > point;
		castor::Array< SpotShadowData, MaxSpotShadowMapCount > spot;
	};
	/**
	*\~english
	*\brief
	*	Retrieve the maximum lit distance for given light and attenuation.
	*\param[in] light
	*	The light source.
	*\param[in] attenuation
	*	The attenuation values.
	*\return
	*	The value.
	*\~french
	*\brief
	*	Calcule la distance maximale d'éclairage d'une source lumineuse selon son atténuation.
	*\param[in] light
	*	La source lumineuse.
	*\param[in] attenuation
	*	Les valeurs d'atténuation.
	*\return
	*	La valeur.
	*/
	C3D_API float getMaxDistance( LightCategory const & light
		, castor::Point3f const & attenuation );
	/**
	*\~english
	*\brief
	*	Retrieve the maximum lit distance for given light and attenuation.
	*\param[in] light
	*	The light source.
	*\param[in] attenuation
	*	The attenuation values.
	*\param[in] max
	*	The viewer max value.
	*\return
	*	The value.
	*\~french
	*\brief
	*	Calcule la distance maximale d'éclairage d'une source lumineuse selon son atténuation.
	*\param[in] light
	*	La source lumineuse.
	*\param[in] attenuation
	*	Les valeurs d'atténuation.
	*\param[in] max
	*	La valeur maximale de l'observateur.
	*\return
	*	La valeur.
	*/
	C3D_API float getMaxDistance( LightCategory const & light
		, castor::Point3f const & attenuation
		, float max );

	C3D_API float computeRange( castor::Point2f const & intensity
		, float range );

	//@}
	//@}
}

#endif
