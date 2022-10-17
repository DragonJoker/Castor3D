#include "Castor3D/Material/Pass/Phong/PhongPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/RoughnessComponent.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/SpecularComponent.hpp"
#include "Castor3D/Material/Pass/Component/Other/ColourComponent.hpp"
#include "Castor3D/Material/Pass/Phong/Shaders/GlslPhongLighting.hpp"

namespace castor3d
{
	castor::String const PhongPass::Type = cuT( "phong" );
	castor::String const PhongPass::LightingModel = shader::PhongLightingModel::getName();

	PassSPtr PhongPass::create( Material & parent )
	{
		auto result = std::make_shared< Pass >( parent
			, parent.getEngine()->getPassFactory().getNameId( Type ) );
		result->createComponent< ColourComponent >();
		result->createComponent< RoughnessComponent >( Pass::computeRoughnessFromShininess( 50.0f ) );
		result->createComponent< SpecularComponent >( SpecularComponent::DefaultPhong );
		return result;
	}
}
