#include "Castor3D/Material/Pass/PbrPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/MetalnessComponent.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/RoughnessComponent.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/SpecularComponent.hpp"
#include "Castor3D/Material/Pass/Component/Map/MetalnessMapComponent.hpp"
#include "Castor3D/Material/Pass/Component/Map/RoughnessMapComponent.hpp"
#include "Castor3D/Material/Pass/Component/Map/SpecularFactorMapComponent.hpp"
#include "Castor3D/Material/Pass/Component/Map/SpecularMapComponent.hpp"
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
		result->createComponent< RoughnessComponent >();
		result->createComponent< SpecularComponent >();

		createDefaultTextureComponent< MetalnessMapComponent >( *result );
		createDefaultTextureComponent< RoughnessMapComponent >( *result );
		createDefaultTextureComponent< SpecularMapComponent >( *result );

		return result;
	}
}
