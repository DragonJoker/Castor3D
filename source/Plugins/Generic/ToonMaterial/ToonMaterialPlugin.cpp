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
		static c3d::PassUPtr create( c3d::LightingModelID lightingModelId
			, c3d::Material & parent )
		{
			auto result = c3d::PbrPass::create( lightingModelId, parent );
			result->createComponent< EdgesComponent >();
			return result;
		}
	};

	struct ToonPhongPass
	{
		static c3d::PassUPtr create( c3d::LightingModelID lightingModelId
			, c3d::Material & parent )
		{
			auto result = c3d::PhongPass::create( lightingModelId, parent );
			result->createComponent< EdgesComponent >();
			return result;
		}
	};
}

extern "C"
{
	C3D_ToonMaterial_API void getRequiredVersion( c3d::Version * version );
	C3D_ToonMaterial_API void getType( c3d::PluginType * type );
	C3D_ToonMaterial_API void isDebug( int * value );
	C3D_ToonMaterial_API void getName( char const ** name );
	C3D_ToonMaterial_API void onLoad( c3d::Engine * engine, c3d::Plugin * plugin );
	C3D_ToonMaterial_API void onUnload( c3d::Engine * engine );

	C3D_ToonMaterial_API void getRequiredVersion( c3d::Version * version )
	{
		*version = c3d::Version();
	}

	C3D_ToonMaterial_API void isDebug( int * value )
	{
		*value = c3d::system::isDebug() ? 1 : 0;
	}

	C3D_ToonMaterial_API void getType( c3d::PluginType * type )
	{
		*type = c3d::PluginType::eGeneric;
	}

	C3D_ToonMaterial_API void getName( char const ** name )
	{
		*name = "Toon Material";
	}

	C3D_ToonMaterial_API void onLoad( c3d::Engine * engine, c3d::Plugin * plugin )
	{
		engine->registerPassComponent< toon::EdgesComponent >();
		engine->registerLightingModel( toon::shader::ToonPhongLightingModel::getName()
			, { c3d::PhongPass::DefaultDiffuseBrdf
				, c3d::PhongPass::DefaultSpecularBrdf
				, c3d::PhongPass::DefaultSheenBrdf
				, c3d::PhongPass::DefaultClearcoatBrdf
				, c3d::PhongPass::DefaultScatteringModel }
			, &toon::shader::ToonPhongLightingModel::create );
		engine->registerLightingModel( toon::shader::ToonPbrLightingModel::getName()
			, { c3d::PbrPass::DefaultDiffuseBrdf
				, c3d::PbrPass::DefaultSpecularBrdf
				, c3d::PbrPass::DefaultSheenBrdf
				, c3d::PbrPass::DefaultClearcoatBrdf
				, c3d::PhongPass::DefaultScatteringModel }
			, &toon::shader::ToonPbrLightingModel::create );
		engine->registerPassModel( { toon::shader::ToonPhongLightingModel::getName()
			, toon::ToonPhongPass::create } );
		engine->registerPassModel( { toon::shader::ToonPbrLightingModel::getName()
			, toon::ToonPbrPass::create } );
		engine->registerSpecificsBuffer( c3d::String{ toon::shader::ToonProfile::getName() }
			, { &toon::shader::ToonProfiles::create
				, &toon::shader::ToonProfiles::update
				, &toon::shader::ToonProfiles::declare } );
	}

	C3D_ToonMaterial_API void onUnload( c3d::Engine * engine )
	{
		engine->unregisterSpecificsBuffer( c3d::String{ toon::shader::ToonProfile::getName() } );
		engine->unregisterPassModel( toon::shader::ToonPbrLightingModel::getName() );
		engine->unregisterPassModel( toon::shader::ToonPhongLightingModel::getName() );
		engine->unregisterLightingModel( toon::shader::ToonPbrLightingModel::getName() );
		engine->unregisterLightingModel( toon::shader::ToonPhongLightingModel::getName() );
		engine->unregisterPassComponent( toon::EdgesComponent::TypeName );
	}
}
