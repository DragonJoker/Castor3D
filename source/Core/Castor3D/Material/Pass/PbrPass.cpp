#include "Castor3D/Material/Pass/PbrPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/MetalnessComponent.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/RoughnessComponent.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/SpecularComponent.hpp"
#include "Castor3D/Material/Pass/Component/Other/ColourComponent.hpp"
#include "Castor3D/Material/Pass/Shaders/GlslPbrLighting.hpp"

namespace castor3d
{
	castor::StringView const PbrPass::Type = "pbr";
	castor::StringView const PbrPass::LightingModel = shader::PbrLightingModel::getName();

	PassUPtr PbrPass::create( LightingModelID lightingModelId
		, Material & parent )
	{
		auto result = castor::makeUnique< Pass >( parent
			, lightingModelId );
		result->createComponent< ColourComponent >();
		result->createComponent< MetalnessComponent >();
		result->createComponent< RoughnessComponent >();
		result->createComponent< SpecularComponent >();
		return result;
	}
}
