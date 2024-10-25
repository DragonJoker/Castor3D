#include "Castor3D/Material/Pass/PbrPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/MetalnessComponent.hpp"
#include "Castor3D/Material/Pass/Component/Map/MetalnessMapComponent.hpp"
#include "Castor3D/Material/Pass/Shaders/GlslPbrLighting.hpp"

namespace castor3d
{
	castor::StringView const PbrPass::Type = cuT( "pbr" );
	castor::StringView const PbrPass::LightingModel = shader::PbrLightingModel::getName();

	PassUPtr PbrPass::create( LightingModelID lightingModelId
		, Material & parent )
	{
		auto result = castor::makeUnique< Pass >( parent
			, lightingModelId );

		result->createComponent< MetalnessComponent >();

		createDefaultTextureComponent< MetalnessMapComponent >( *result );

		return result;
	}
}
