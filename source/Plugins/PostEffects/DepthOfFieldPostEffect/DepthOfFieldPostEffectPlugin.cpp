#include "DepthOfFieldPostEffect/DepthOfFieldPostEffect.hpp"

#include "DepthOfFieldPostEffect/DepthOfFieldUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Plugin/PostFxPlugin.hpp>

#ifndef CU_PlatformWindows
#	define C3D_DepthOfField_API
#else
#	ifdef DepthOfFieldPostEffect_EXPORTS
#		define C3D_DepthOfField_API __declspec( dllexport )
#	else
#		define C3D_DepthOfField_API __declspec( dllimport )
#	endif
#endif

extern "C"
{
	C3D_DepthOfField_API void getRequiredVersion( castor3d::Version * version );
	C3D_DepthOfField_API void getType( castor3d::PluginType * type );
	C3D_DepthOfField_API void getName( char const ** name );
	C3D_DepthOfField_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin );
	C3D_DepthOfField_API void OnUnload( castor3d::Engine * engine );

	C3D_DepthOfField_API void getRequiredVersion( castor3d::Version * version )
	{
		*version = castor3d::Version();
	}

	C3D_DepthOfField_API void getType( castor3d::PluginType * type )
	{
		*type = castor3d::PluginType::ePostEffect;
	}

	C3D_DepthOfField_API void getName( char const ** name )
	{
		*name = dof::PostEffect::Name.c_str();
	}

	C3D_DepthOfField_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin )
	{
		engine->getPostEffectFactory().registerType( dof::PostEffect::Type
			, &dof::PostEffect::create );
		engine->registerParsers( dof::PostEffect::Type
			, dof::DepthOfFieldUboConfiguration::createParsers()
			, dof::DepthOfFieldUboConfiguration::createSections()
			, nullptr );
	}

	C3D_DepthOfField_API void OnUnload( castor3d::Engine * engine )
	{
		engine->unregisterParsers( dof::PostEffect::Type );
		engine->getPostEffectFactory().unregisterType( dof::PostEffect::Type );
	}
}
