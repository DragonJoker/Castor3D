#include "LightFactory.hpp"

#include "DirectionalLight.hpp"
#include "PointLight.hpp"
#include "SpotLight.hpp"

using namespace Castor;

namespace Castor3D
{
	LightFactory::LightFactory()
		: Factory< LightCategory, LightType, LightCategoryUPtr, std::function< LightCategoryUPtr( Light & ) > >()
	{
		Register( LightType::eDirectional, DirectionalLight::Create );
		Register( LightType::ePoint, PointLight::Create );
		Register( LightType::eSpot, SpotLight::Create );
	}

	LightFactory::~LightFactory()
	{
	}
}
