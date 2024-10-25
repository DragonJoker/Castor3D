#include "Castor3D/Material/Pass/PhongPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/RoughnessComponent.hpp"
#include "Castor3D/Material/Pass/Shaders/GlslPhongLighting.hpp"

namespace castor3d
{
	castor::StringView const PhongPass::Type = cuT( "blinn_phong" );
	castor::StringView const PhongPass::LightingModel = shader::PhongLightingModel::getName();

	PassUPtr PhongPass::create( LightingModelID lightingModelId
		, Material & parent )
	{
		auto result = castor::makeUnique< Pass >( parent
			, lightingModelId );

		result->createComponent< RoughnessComponent >()->setShininess( 50.0f );

		return result;
	}
}
