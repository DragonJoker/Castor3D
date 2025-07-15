#include "WaterMaterial/WaterComponent.hpp"
#include "WaterMaterial/WaterFoamMapComponent.hpp"
#include "WaterMaterial/WaterNormal1MapComponent.hpp"
#include "WaterMaterial/WaterNormal2MapComponent.hpp"
#include "WaterMaterial/WaterNoiseMapComponent.hpp"
#include "WaterMaterial/WaterReflRefrComponent.hpp"
#include "WaterMaterial/Shaders/GlslWaterProfile.hpp"
#include "WaterMaterial/Shaders/GlslWaterLighting.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/MaterialCache.hpp>
#include <Castor3D/Material/Pass/PassFactory.hpp>
#include <Castor3D/Material/Pass/PbrPass.hpp>
#include <Castor3D/Material/Pass/PhongPass.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Shader/ShaderBuffers/PassBuffer.hpp>

extern "C"
{
	C3D_WaterMaterial_API void getRequiredVersion( c3d::Version * version );
	C3D_WaterMaterial_API void getType( c3d::PluginType * type );
	C3D_WaterMaterial_API void isDebug( int * value );
	C3D_WaterMaterial_API void getName( char const ** name );
	C3D_WaterMaterial_API void onLoad( c3d::Engine * engine, c3d::Plugin * plugin );
	C3D_WaterMaterial_API void onUnload( c3d::Engine * engine );

	C3D_WaterMaterial_API void getRequiredVersion( c3d::Version * version )
	{
		*version = c3d::Version();
	}

	C3D_WaterMaterial_API void isDebug( int * value )
	{
		*value = c3d::system::isDebug() ? 1 : 0;
	}

	C3D_WaterMaterial_API void getType( c3d::PluginType * type )
	{
		*type = c3d::PluginType::eGeneric;
	}

	C3D_WaterMaterial_API void getName( char const ** name )
	{
		*name = "Water Material";
	}

	C3D_WaterMaterial_API void onLoad( c3d::Engine * engine, c3d::Plugin * plugin )
	{
		engine->registerPassComponent< water::WaterNormal1MapComponent >();
		engine->registerPassComponent< water::WaterNormal2MapComponent >();
		engine->registerPassComponent< water::WaterNoiseMapComponent >();
		engine->registerPassComponent< water::WaterFoamMapComponent >();
		engine->registerPassComponent< water::WaterComponent >();
		engine->registerPassComponent< water::WaterReflRefrComponent >();
		engine->registerLightingModel( c3d::String{ water::shader::WaterLightingModel::getName() }
			, { c3d::PbrPass::DefaultDiffuseBrdf
				, c3d::PbrPass::DefaultSpecularBrdf
				, c3d::PbrPass::DefaultSheenBrdf
				, c3d::PbrPass::DefaultClearcoatBrdf
				, c3d::PbrPass::DefaultScatteringModel }
			, water::shader::WaterLightingModel::create );
		engine->registerSpecificsBuffer( c3d::String{ water::shader::WaterProfile::getName() }
			, { &water::shader::WaterProfiles::create
				, &water::shader::WaterProfiles::update
				, &water::shader::WaterProfiles::declare } );
	}

	C3D_WaterMaterial_API void onUnload( c3d::Engine * engine )
	{
		engine->unregisterSpecificsBuffer( c3d::String{ water::shader::WaterProfile::getName() } );
		engine->unregisterLightingModel( c3d::String{ water::shader::WaterLightingModel::getName() } );
		engine->unregisterPassComponent( water::WaterReflRefrComponent::TypeName );
		engine->unregisterPassComponent( water::WaterComponent::TypeName );
		engine->unregisterPassComponent( water::WaterFoamMapComponent::TypeName );
		engine->unregisterPassComponent( water::WaterNoiseMapComponent::TypeName );
		engine->unregisterPassComponent( water::WaterNormal2MapComponent::TypeName );
		engine->unregisterPassComponent( water::WaterNormal1MapComponent::TypeName );
	}
}
