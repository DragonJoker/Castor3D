#include "Castor3D/Material/Pass/PassFactory.hpp"

#include "Castor3D/Material/Pass/Phong/PhongPass.hpp"
#include "Castor3D/Material/Pass/PBR/MetallicRoughnessPbrPass.hpp"
#include "Castor3D/Material/Pass/PBR/SpecularGlossinessPbrPass.hpp"

CU_ImplementCUSmartPtr( castor3d, PassFactory );

namespace castor3d
{
	PassFactory::PassFactory()
		: castor::Factory< Pass, castor::String, PassSPtr, std::function< PassSPtr( Material & ) > >{}
	{
		registerType( cuT( "phong" ), PhongPass::create );
		registerType( cuT( "metallic_roughness" ), MetallicRoughnessPbrPass::create );
		registerType( cuT( "specular_glossiness" ), SpecularGlossinessPbrPass::create );
	}

	PassFactory::~PassFactory()
	{
	}
}
