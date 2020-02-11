/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightModule_H___
#define ___C3D_LightModule_H___

#include "Castor3D/Scene/SceneModule.hpp"

namespace castor3d
{
	/**@name Scene */
	//@{
	/**@name Light */
	//@{

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
	castor::String getName( LightType value );
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

	CU_DeclareSmartPtr( Light );
	CU_DeclareSmartPtr( LightFactory );
	CU_DeclareSmartPtr( LightCategory );
	CU_DeclareSmartPtr( DirectionalLight );
	CU_DeclareSmartPtr( PointLight );
	CU_DeclareSmartPtr( SpotLight );

	class LightFactory;
	CU_DeclareSmartPtr( LightFactory );

	//! Array of lights
	CU_DeclareVector( LightSPtr, LightPtr );
	//! Array of lights
	CU_DeclareVector( LightSPtr, Lights );
	//! Array to non owning light pointers
	CU_DeclareVector( LightRPtr, LightsRef );
	//! Map of lights, sorted by name
	CU_DeclareMap( castor::String, LightSPtr, LightPtrStr );
	//! Map of lights, sorted by index
	CU_DeclareMap( int, LightSPtr, LightPtrInt );
	//! Array of lights per light type.
	using LightsMap = std::array< LightsArray, size_t( LightType::eCount ) >;

	using OnLightChangedFunction = std::function< void( Light & ) >;
	using OnLightChanged = castor::Signal< OnLightChangedFunction >;
	using OnLightChangedConnection = OnLightChanged::connection;

	//@}
	//@}
}

#endif
