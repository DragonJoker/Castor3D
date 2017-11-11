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
		CASTOR_SCOPED_ENUM_BOUNDS( eDirectional )
	};

	class Light;
	class LightCategory;
	class DirectionalLight;
	class PointLight;
	class SpotLight;

	DECLARE_SMART_PTR( Light );
	DECLARE_SMART_PTR( LightCategory );
	DECLARE_SMART_PTR( DirectionalLight );
	DECLARE_SMART_PTR( PointLight );
	DECLARE_SMART_PTR( SpotLight );

	class LightFactory;
	DECLARE_SMART_PTR( LightFactory );

	//! Array of lights
	DECLARE_VECTOR( LightSPtr, LightPtr );
	//! Map of lights, sorted by name
	DECLARE_MAP( castor::String, LightSPtr, LightPtrStr );
	//! Map of lights, sorted by index
	DECLARE_MAP( int, LightSPtr, LightPtrInt );

	using OnLightChangedFunction = std::function< void( Light & ) >;
	using OnLightChanged = castor::Signal< OnLightChangedFunction >;
	using OnLightChangedConnection = OnLightChanged::connection;

	//@}
}

#endif
