/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PREREQUISITES_LIGHT_H___
#define ___C3D_PREREQUISITES_LIGHT_H___

#include <Design/Signal.hpp>

namespace castor3d
{
	/**@name Light */
	//@{

	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Light types enumeration
	\~french
	\brief		Enumération des types de lumières
	*/
	enum class LightType
		: uint8_t
	{
		//!\~english Directional light type	\~french Lumière directionnelle
		eDirectional,
		//!\~english Point light type	\~french Lumière ponctuelle
		ePoint,
		//!\~english Spot light type	\~french Lumière projecteur
		eSpot,
		CU_ScopedEnumBounds( eDirectional )
	};

	class Light;
	class LightCategory;
	class DirectionalLight;
	class PointLight;
	class SpotLight;

	CU_DeclareSmartPtr( Light );
	CU_DeclareSmartPtr( LightCategory );
	CU_DeclareSmartPtr( DirectionalLight );
	CU_DeclareSmartPtr( PointLight );
	CU_DeclareSmartPtr( SpotLight );

	class LightFactory;
	CU_DeclareSmartPtr( LightFactory );

	//! Array of lights
	CU_DeclareVector( LightSPtr, LightPtr );
	//! Map of lights, sorted by name
	CU_DeclareMap( castor::String, LightSPtr, LightPtrStr );
	//! Map of lights, sorted by index
	CU_DeclareMap( int, LightSPtr, LightPtrInt );

	using OnLightChangedFunction = std::function< void( Light & ) >;
	using OnLightChanged = castor::Signal< OnLightChangedFunction >;
	using OnLightChangedConnection = OnLightChanged::connection;

	//@}
}

#endif
