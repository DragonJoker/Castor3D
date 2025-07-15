#include "Castor3D/Scene/Light/LightFactory.hpp"

#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"

CU_ImplementSmartPtr( c3d, LightFactory )

namespace c3d
{
	LightFactory::LightFactory()
		: Factory< LightCategory, LightType, LightCategoryUPtr, Function< LightCategoryUPtr( bool &, Function< void() > const & ) > >()
	{
		registerType( LightType::eDirectional, DirectionalLight::create );
		registerType( LightType::ePoint, PointLight::create );
		registerType( LightType::eSpot, SpotLight::create );
	}
}
