//#include "Light.hpp"
#include "LightFactory.hpp"

#include "DirectionalLight.hpp"
#include "PointLight.hpp"
#include "SpotLight.hpp"

using namespace Castor;

namespace Castor3D
{
	LightFactory::LightFactory()
		: Factory< LightCategory, LightType, LightCategorySPtr, std::function< LightCategorySPtr( Viewport & ) > >()
	{
		Register( LightType::Directional, DirectionalLight::Create );
		Register( LightType::Point, PointLight::Create );
		Register( LightType::Spot, SpotLight::Create );
	}

	LightFactory::~LightFactory()
	{
	}
}
