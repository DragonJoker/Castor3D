#include "ToonMaterial/ToonPass.hpp"
#include "ToonMaterial/Shaders/GlslToonLighting.hpp"
#include "ToonMaterial/Shaders/GlslToonMaterial.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/MaterialCache.hpp>
#include <Castor3D/Material/Pass/PassFactory.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Shader/ShaderBuffers/PassBuffer.hpp>

extern "C"
{
	C3D_ToonMaterial_API void getRequiredVersion( castor3d::Version * p_version );
	C3D_ToonMaterial_API void getType( castor3d::PluginType * p_type );
	C3D_ToonMaterial_API void getName( char const ** p_name );
	C3D_ToonMaterial_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * p_plugin );
	C3D_ToonMaterial_API void OnUnload( castor3d::Engine * engine );

	C3D_ToonMaterial_API void getRequiredVersion( castor3d::Version * p_version )
	{
		*p_version = castor3d::Version();
	}

	C3D_ToonMaterial_API void getType( castor3d::PluginType * p_type )
	{
		*p_type = castor3d::PluginType::eGeneric;
	}

	C3D_ToonMaterial_API void getName( char const ** p_name )
	{
		*p_name = toon::ToonPassT< toon::ToonPhongPass >::Name.c_str();
	}

	C3D_ToonMaterial_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * p_plugin )
	{
		engine->getPassFactory().registerType( toon::ToonPhongPass::Type
			, { toon::shader::ToonPhongLightingModel::getName()
				, toon::ToonPhongPass::create
				, toon::ToonPhongPass::createParsers()
				, toon::ToonPhongPass::createSections()
				, &toon::shader::ToonPhongLightingModel::create
				, false } );
		engine->getPassFactory().registerType( toon::ToonBlinnPhongPass::Type
			, { toon::shader::ToonBlinnPhongLightingModel::getName()
				, toon::ToonBlinnPhongPass::create
				, toon::ToonBlinnPhongPass::createParsers()
				, toon::ToonBlinnPhongPass::createSections()
				, &toon::shader::ToonBlinnPhongLightingModel::create
				, false } );
		engine->getPassFactory().registerType( toon::ToonPbrPass::Type
			, { toon::shader::ToonPbrLightingModel::getName()
				, toon::ToonPbrPass::create
				, toon::ToonPbrPass::createParsers()
				, toon::ToonPbrPass::createSections()
				, &toon::shader::ToonPbrLightingModel::create
				, true } );
		engine->registerSpecificsBuffer( toon::shader::ToonProfile::getName()
			, { &toon::shader::ToonProfiles::create
				, &toon::shader::ToonProfiles::update
				, &toon::shader::ToonProfiles::declare } );
	}

	C3D_ToonMaterial_API void OnUnload( castor3d::Engine * engine )
	{
		engine->unregisterSpecificsBuffer( toon::shader::ToonProfile::getName() );
		engine->getPassFactory().unregisterType( toon::ToonPbrPass::Type );
		engine->getPassFactory().unregisterType( toon::ToonBlinnPhongPass::Type );
		engine->getPassFactory().unregisterType( toon::ToonPhongPass::Type );
	}
}
