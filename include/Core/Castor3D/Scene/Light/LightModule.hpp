/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightModule_H___
#define ___C3D_LightModule_H___

#include "Castor3D/Scene/SceneModule.hpp"
#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"

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
		eDirectional,
		//!\~english Point light type.
		//!\~french Lumière ponctuelle.
		ePoint,
		//!\~english Spot light type.
		//!\~french Lumière projecteur.
		eSpot,
		CU_ScopedEnumBounds( eDirectional )
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
	using LightsMap = std::array< LightsArray, size_t( LightType::eCount ) >;

	using OnLightChangedFunction = std::function< void( Light & ) >;
	using OnLightChanged = castor::SignalT< OnLightChangedFunction >;
	using OnLightChangedConnection = OnLightChanged::connection;

	struct RsmConfig
	{
		C3D_API void accept( castor::String const & name
			, PipelineVisitorBase & visitor );

		castor::ChangeTracked< float > intensity;
		castor::ChangeTracked< float > maxRadius;
		castor::ChangeTracked< castor::RangedValue< uint32_t > > sampleCount{ castor::makeRangedValue( 100u, 20u, MaxRsmRange ) };
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

	//@}
	//@}
}

#endif
