#include "Castor3D/Material/Pass/PassFactory.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Phong/PhongPass.hpp"
#include "Castor3D/Material/Pass/Phong/Shaders/GlslPhongLighting.hpp"
#include "Castor3D/Material/Pass/PBR/MetallicRoughnessPbrPass.hpp"
#include "Castor3D/Material/Pass/PBR/SpecularGlossinessPbrPass.hpp"
#include "Castor3D/Material/Pass/PBR/Shaders/GlslPbrLighting.hpp"

CU_ImplementCUSmartPtr( castor3d, PassFactory );

namespace castor3d
{
	PassFactory::PassFactory( Engine & engine )
		: castor::OwnedBy< Engine >{ engine }
		, castor::Factory< Pass, castor::String, PassSPtr, std::function< PassSPtr( Material & ) > >{}
	{
		registerType( PhongPass::Type, PhongPass::create );
		engine.registerParsers( PhongPass::Type, PhongPass::createParsers() );
		engine.registerSections( PhongPass::Type, PhongPass::createSections() );

		registerType( MetallicRoughnessPbrPass::Type, MetallicRoughnessPbrPass::create );
		engine.registerParsers( MetallicRoughnessPbrPass::Type, MetallicRoughnessPbrPass::createParsers() );
		engine.registerSections( MetallicRoughnessPbrPass::Type, MetallicRoughnessPbrPass::createSections() );

		registerType( SpecularGlossinessPbrPass::Type, SpecularGlossinessPbrPass::create );
		engine.registerParsers( SpecularGlossinessPbrPass::Type, SpecularGlossinessPbrPass::createParsers() );
		engine.registerSections( SpecularGlossinessPbrPass::Type, SpecularGlossinessPbrPass::createSections() );

		engine.registerLightingModel( "phong", &shader::PhongLightingModel::create );
		engine.registerLightingModel( "pbr", &shader::PbrLightingModel::create );
	}

	PassFactory::~PassFactory()
	{
	}
}
