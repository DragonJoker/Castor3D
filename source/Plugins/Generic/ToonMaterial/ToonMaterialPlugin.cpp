#include "ToonMaterial/EdgesComponent.hpp"
#include "ToonMaterial/Shaders/GlslToonLighting.hpp"
#include "ToonMaterial/Shaders/GlslToonProfile.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/MaterialCache.hpp>
#include <Castor3D/Material/Pass/PassFactory.hpp>
#include <Castor3D/Material/Pass/PbrPass.hpp>
#include <Castor3D/Material/Pass/PhongPass.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Shader/ShaderBuffers/PassBuffer.hpp>

namespace toon
{
	struct ToonPbrPass
	{
		static castor3d::PassUPtr create( castor3d::LightingModelID lightingModelId
			, castor3d::Material & parent )
		{
			auto result = castor3d::PbrPass::create( lightingModelId, parent );
			result->createComponent< EdgesComponent >();
			return result;
		}
	};

	struct ToonPhongPass
	{
		static castor3d::PassUPtr create( castor3d::LightingModelID lightingModelId
			, castor3d::Material & parent )
		{
			auto result = castor3d::PhongPass::create( lightingModelId, parent );
			result->createComponent< EdgesComponent >();
			return result;
		}
	};
}

extern "C"
{
	C3D_ToonMaterial_API void getRequiredVersion( castor3d::Version * version );
	C3D_ToonMaterial_API void getType( castor3d::PluginType * type );
	C3D_ToonMaterial_API void isDebug( int * value );
	C3D_ToonMaterial_API void getName( char const ** name );
	C3D_ToonMaterial_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin );
	C3D_ToonMaterial_API void OnUnload( castor3d::Engine * engine );

	C3D_ToonMaterial_API void getRequiredVersion( castor3d::Version * version )
	{
		*version = castor3d::Version();
	}

	C3D_ToonMaterial_API void isDebug( int * value )
	{
		*value = castor::system::isDebug() ? 1 : 0;
	}

	C3D_ToonMaterial_API void getType( castor3d::PluginType * type )
	{
		*type = castor3d::PluginType::eGeneric;
	}

	C3D_ToonMaterial_API void getName( char const ** name )
	{
		*name = "Toon Material";
	}

	C3D_ToonMaterial_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin )
	{
		engine->registerPassComponent< toon::EdgesComponent >();
		engine->registerLightingModel( toon::shader::ToonPhongLightingModel::getName()
			, castor3d::PhongPass::DefaultDiffuseBrdf
			, castor3d::PhongPass::DefaultSpecularBrdf
			, castor3d::PhongPass::DefaultSheenBrdf
			, castor3d::PhongPass::DefaultClearcoatBrdf
			, &toon::shader::ToonPhongLightingModel::create );
		engine->registerLightingModel( toon::shader::ToonPbrLightingModel::getName()
			, castor3d::PbrPass::DefaultDiffuseBrdf
			, castor3d::PbrPass::DefaultSpecularBrdf
			, castor3d::PbrPass::DefaultSheenBrdf
			, castor3d::PbrPass::DefaultClearcoatBrdf
			, &toon::shader::ToonPbrLightingModel::create );
		engine->registerPassModel( { toon::shader::ToonPhongLightingModel::getName()
			, toon::ToonPhongPass::create } );
		engine->registerPassModel( { toon::shader::ToonPbrLightingModel::getName()
			, toon::ToonPbrPass::create } );
		engine->registerSpecificsBuffer( castor::String{ toon::shader::ToonProfile::getName() }
			, { &toon::shader::ToonProfiles::create
				, &toon::shader::ToonProfiles::update
				, &toon::shader::ToonProfiles::declare } );
	}

	C3D_ToonMaterial_API void OnUnload( castor3d::Engine * engine )
	{
		engine->unregisterSpecificsBuffer( castor::String{ toon::shader::ToonProfile::getName() } );
		engine->unregisterPassModel( toon::shader::ToonPbrLightingModel::getName() );
		engine->unregisterPassModel( toon::shader::ToonPhongLightingModel::getName() );
		engine->unregisterLightingModel( toon::shader::ToonPbrLightingModel::getName() );
		engine->unregisterLightingModel( toon::shader::ToonPhongLightingModel::getName() );
		engine->unregisterPassComponent( toon::EdgesComponent::TypeName );
	}
}
