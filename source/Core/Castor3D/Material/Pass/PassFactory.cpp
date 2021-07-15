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
	namespace
	{
		size_t computeID( castor::String const & passType )
		{
			return std::hash< castor::String >{}( passType );
		}
	}

	PassFactory::PassFactory( Engine & engine )
		: castor::OwnedBy< Engine >{ engine }
		, PassFactoryBase{}
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

	void PassFactory::registerType( castor::String const & passType
		, PassFactoryBase::Creator creator )
	{
		PassFactoryBase::registerType( getNameID( passType ), creator );
	}

	void PassFactory::unregisterType( castor::String const & passType )
	{
		PassFactoryBase::unregisterType( getNameID( passType ) );
	}

	PassSPtr PassFactory::create( castor::String const & passType
		, Material & parent )const
	{
		return create( getNameID( passType ), parent );
	}

	size_t PassFactory::getNameID( castor::String const & passType )const
	{
		return computeID( passType );
	}
}
