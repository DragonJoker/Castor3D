#include "ToonMaterial/EdgesComponent.hpp"
#include "ToonMaterial/Shaders/GlslToonLighting.hpp"
#include "ToonMaterial/Shaders/GlslToonProfile.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/MaterialCache.hpp>
#include <Castor3D/Material/Pass/PassFactory.hpp>
#include <Castor3D/Material/Pass/PBR/PbrPass.hpp>
#include <Castor3D/Material/Pass/Phong/BlinnPhongPass.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Shader/ShaderBuffers/PassBuffer.hpp>

namespace toon
{
	struct ToonPbrPass
	{
		static castor::String const Type;

		static castor3d::PassSPtr create( castor3d::Material & parent )
		{
			auto result = castor3d::PbrPass::create( parent );
			result->createComponent< EdgesComponent >();
			return result;
		}
	};
	castor::String const ToonPbrPass::Type = cuT( "toon_pbr" );

	struct ToonPhongPass
	{
		static castor::String const Type;

		static castor3d::PassSPtr create( castor3d::Material & parent )
		{
			auto result = castor3d::PhongPass::create( parent );
			result->createComponent< EdgesComponent >();
			return result;
		}
	};
	castor::String const ToonPhongPass::Type = cuT( "toon_phong" );

	struct ToonBlinnPhongPass
	{
		static castor::String const Type;

		static castor3d::PassSPtr create( castor3d::Material & parent )
		{
			auto result = castor3d::BlinnPhongPass::create( parent );
			result->createComponent< EdgesComponent >();
			return result;
		}
	};
	castor::String const ToonBlinnPhongPass::Type = cuT( "toon_blinn_phong" );
}

extern "C"
{
	C3D_ToonMaterial_API void getRequiredVersion( castor3d::Version * version );
	C3D_ToonMaterial_API void getType( castor3d::PluginType * type );
	C3D_ToonMaterial_API void getName( char const ** name );
	C3D_ToonMaterial_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin );
	C3D_ToonMaterial_API void OnUnload( castor3d::Engine * engine );

	C3D_ToonMaterial_API void getRequiredVersion( castor3d::Version * version )
	{
		*version = castor3d::Version();
	}

	C3D_ToonMaterial_API void getType( castor3d::PluginType * type )
	{
		*type = castor3d::PluginType::eGeneric;
	}

	C3D_ToonMaterial_API void getName( char const ** name )
	{
		*name = "toon";
	}

	C3D_ToonMaterial_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin )
	{
		engine->registerPassComponent< toon::EdgesComponent >();
		engine->getPassFactory().registerType( toon::ToonPhongPass::Type
			, { toon::shader::ToonPhongLightingModel::getName()
				, toon::ToonPhongPass::create
				, &toon::shader::ToonPhongLightingModel::create
				, false } );
		engine->getPassFactory().registerType( toon::ToonBlinnPhongPass::Type
			, { toon::shader::ToonBlinnPhongLightingModel::getName()
				, toon::ToonBlinnPhongPass::create
				, &toon::shader::ToonBlinnPhongLightingModel::create
				, false } );
		engine->getPassFactory().registerType( toon::ToonPbrPass::Type
			, { toon::shader::ToonPbrLightingModel::getName()
				, toon::ToonPbrPass::create
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
		engine->unregisterPassComponent( toon::EdgesComponent::TypeName );
	}
}
