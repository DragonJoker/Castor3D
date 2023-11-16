#include "WaterMaterial/WaterComponent.hpp"
#include "WaterMaterial/WaterFoamMapComponent.hpp"
#include "WaterMaterial/WaterNormal1MapComponent.hpp"
#include "WaterMaterial/WaterNormal2MapComponent.hpp"
#include "WaterMaterial/WaterNoiseMapComponent.hpp"
#include "WaterMaterial/Shaders/GlslWaterLighting.hpp"
#include "WaterMaterial/Shaders/GlslWaterProfile.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/MaterialCache.hpp>
#include <Castor3D/Material/Pass/PassFactory.hpp>
#include <Castor3D/Material/Pass/PbrPass.hpp>
#include <Castor3D/Material/Pass/PhongPass.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Shader/ShaderBuffers/PassBuffer.hpp>

namespace water
{
	struct WaterPbrPass
	{
		static castor3d::PassUPtr create( castor3d::LightingModelID lightingModelId
			, castor3d::Material & parent )
		{
			auto result = castor3d::PbrPass::create( lightingModelId, parent );
			result->createComponent< WaterComponent >();
			return result;
		}
	};

	struct WaterPhongPass
	{
		static castor3d::PassUPtr create( castor3d::LightingModelID lightingModelId
			, castor3d::Material & parent )
		{
			auto result = castor3d::PhongPass::create( lightingModelId, parent );
			result->createComponent< WaterComponent >();
			return result;
		}
	};
}

extern "C"
{
	C3D_WaterMaterial_API void getRequiredVersion( castor3d::Version * version );
	C3D_WaterMaterial_API void getType( castor3d::PluginType * type );
	C3D_WaterMaterial_API void getName( char const ** name );
	C3D_WaterMaterial_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin );
	C3D_WaterMaterial_API void OnUnload( castor3d::Engine * engine );

	C3D_WaterMaterial_API void getRequiredVersion( castor3d::Version * version )
	{
		*version = castor3d::Version();
	}

	C3D_WaterMaterial_API void getType( castor3d::PluginType * type )
	{
		*type = castor3d::PluginType::eGeneric;
	}

	C3D_WaterMaterial_API void getName( char const ** name )
	{
		*name = "Water Material";
	}

	C3D_WaterMaterial_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin )
	{
		engine->registerPassComponent< water::WaterNormal1MapComponent >();
		engine->registerPassComponent< water::WaterNormal2MapComponent >();
		engine->registerPassComponent< water::WaterNoiseMapComponent >();
		engine->registerPassComponent< water::WaterFoamMapComponent >();
		engine->registerPassComponent< water::WaterComponent >();
		engine->registerPassModels( { water::shader::WaterPhongLightingModel::getName()
			, water::WaterPhongPass::create
			, &water::shader::WaterPhongLightingModel::create
			, false } );
		engine->registerPassModels( { water::shader::WaterPbrLightingModel::getName()
			, water::WaterPbrPass::create
			, &water::shader::WaterPbrLightingModel::create
			, true } );
		engine->registerSpecificsBuffer( water::shader::WaterProfile::getName()
			, { &water::shader::WaterProfiles::create
				, &water::shader::WaterProfiles::update
				, &water::shader::WaterProfiles::declare } );
	}

	C3D_WaterMaterial_API void OnUnload( castor3d::Engine * engine )
	{
		engine->unregisterSpecificsBuffer( water::shader::WaterProfile::getName() );
		engine->unregisterPassModels( water::shader::WaterPbrLightingModel::getName() );
		engine->unregisterPassModels( water::shader::WaterPhongLightingModel::getName() );
		engine->unregisterPassComponent( water::WaterComponent::TypeName );
		engine->unregisterPassComponent( water::WaterFoamMapComponent::TypeName );
		engine->unregisterPassComponent( water::WaterNoiseMapComponent::TypeName );
		engine->unregisterPassComponent( water::WaterNormal2MapComponent::TypeName );
		engine->unregisterPassComponent( water::WaterNormal1MapComponent::TypeName );
	}
}
