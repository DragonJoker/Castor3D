#include "Castor3D/Material/Pass/PBR/PbrPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/MetalnessComponent.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/RoughnessComponent.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/SpecularComponent.hpp"
#include "Castor3D/Material/Pass/Component/Other/ColourComponent.hpp"
#include "Castor3D/Material/Pass/PBR/Shaders/GlslPbrLighting.hpp"

namespace castor3d
{
	castor::String const PbrPass::Type = cuT( "pbr" );
	castor::String const PbrPass::LightingModel = shader::PbrLightingModel::getName();

	PassSPtr PbrPass::create( Material & parent )
	{
		auto result = std::make_shared< Pass >( parent
			, parent.getEngine()->getPassFactory().getNameId( Type ) );
		result->createComponent< ColourComponent >();
		result->createComponent< MetalnessComponent >();
		result->createComponent< RoughnessComponent >();
		result->createComponent< SpecularComponent >();
		return result;
	}
}
