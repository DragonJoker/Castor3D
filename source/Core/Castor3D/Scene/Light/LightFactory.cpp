#include "LightFactory.hpp"

#include "DirectionalLight.hpp"
#include "PointLight.hpp"
#include "SpotLight.hpp"

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

	LightFactory::~LightFactory()
	{
	}
}
