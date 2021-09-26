#include "ToonMaterial/ToonPass.hpp"
#include "ToonMaterial/Shaders/GlslToonLighting.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Material/Pass/PassFactory.hpp>

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
		engine->getPassFactory().registerType( toon::ToonMetallicRoughnessPass::Type
			, { toon::shader::ToonPbrMRLightingModel::getName()
				, toon::ToonMetallicRoughnessPass::create
				, toon::ToonMetallicRoughnessPass::createParsers()
				, toon::ToonMetallicRoughnessPass::createSections()
				, &toon::shader::ToonPbrMRLightingModel::create
				, true } );
		engine->getPassFactory().registerType( toon::ToonSpecularGlossinessPass::Type
			, { toon::shader::ToonPbrSGLightingModel::getName()
				, toon::ToonSpecularGlossinessPass::create
				, toon::ToonSpecularGlossinessPass::createParsers()
				, toon::ToonSpecularGlossinessPass::createSections()
				, &toon::shader::ToonPbrSGLightingModel::create
				, true } );
	}

	C3D_ToonMaterial_API void OnUnload( castor3d::Engine * engine )
	{
		engine->getPassFactory().unregisterType( toon::ToonSpecularGlossinessPass::Type );
		engine->getPassFactory().unregisterType( toon::ToonMetallicRoughnessPass::Type );
		engine->getPassFactory().unregisterType( toon::ToonBlinnPhongPass::Type );
		engine->getPassFactory().unregisterType( toon::ToonPhongPass::Type );
	}
}
