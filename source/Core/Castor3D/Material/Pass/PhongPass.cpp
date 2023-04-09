#include "Castor3D/Material/Pass/PhongPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/RoughnessComponent.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/SpecularComponent.hpp"
#include "Castor3D/Material/Pass/Component/Other/ColourComponent.hpp"
#include "Castor3D/Material/Pass/Shaders/GlslPhongLighting.hpp"

namespace castor3d
{
	castor::String const PhongPass::Type = "blinn_phong";
	castor::String const PhongPass::LightingModel = shader::PhongLightingModel::getName();

	PassUPtr PhongPass::create( LightingModelID lightingModelId
		, Material & parent )
	{
		auto result = castor::makeUnique< Pass >( parent
			, lightingModelId );
		result->createComponent< ColourComponent >();
		result->createComponent< RoughnessComponent >( Pass::computeRoughnessFromShininess( 50.0f ) );
		result->createComponent< SpecularComponent >( SpecularComponent::DefaultPhong );
		return result;
	}
}
