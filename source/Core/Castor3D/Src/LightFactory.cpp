//#include "Light.hpp"
#include "LightFactory.hpp"

#include "DirectionalLight.hpp"
#include "PointLight.hpp"
#include "SpotLight.hpp"

using namespace Castor;

namespace Castor3D
{
	LightFactory::LightFactory()
		:	Factory< LightCategory, eLIGHT_TYPE, std::function< std::shared_ptr< LightCategory >() > >()
	{
		Initialise();
	}

	LightFactory::~LightFactory()
	{
	}

	void LightFactory::Initialise()
	{
		Register( eLIGHT_TYPE_DIRECTIONAL, DirectionalLight::Create );
		Register( eLIGHT_TYPE_POINT, PointLight::Create );
		Register( eLIGHT_TYPE_SPOT, SpotLight::Create );
	}
}
