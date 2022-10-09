#include "Castor3D/Material/Pass/Phong/BlinnPhongPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/RoughnessComponent.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/SpecularComponent.hpp"
#include "Castor3D/Material/Pass/Component/Other/ColourComponent.hpp"
#include "Castor3D/Material/Pass/Phong/Shaders/GlslPhongLighting.hpp"

namespace castor3d
{
	castor::String const BlinnPhongPass::Type = cuT( "blinn_phong" );
	castor::String const BlinnPhongPass::LightingModel = shader::BlinnPhongLightingModel::getName();

	PassSPtr BlinnPhongPass::create( Material & parent )
	{
		auto result = std::make_shared< Pass >( parent
			, parent.getEngine()->getPassFactory().getNameId( Type )
			, PassFlag::eNone );
		result->createComponent< ColourComponent >();
		result->createComponent< RoughnessComponent >( Pass::computeRoughnessFromShininess( 50.0f ) );
		result->createComponent< SpecularComponent >( SpecularComponent::DefaultPhong );
		return result;
	}
}
