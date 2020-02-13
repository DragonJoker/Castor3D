#include "Castor3D/Scene/Light/LightFactory.hpp"

#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"

using namespace castor;

namespace castor3d
{
	LightFactory::LightFactory()
		: Factory< LightCategory, LightType, LightCategoryUPtr, std::function< LightCategoryUPtr( Light & ) > >()
	{
		registerType( LightType::eDirectional, DirectionalLight::create );
		registerType( LightType::ePoint, PointLight::create );
		registerType( LightType::eSpot, SpotLight::create );
	}
}
