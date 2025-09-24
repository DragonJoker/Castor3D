#include "DepthOfFieldPostEffect/DepthOfFieldPostEffect.hpp"

#include "DepthOfFieldPostEffect/DepthOfFieldUbo.hpp"

#include <Castor3D/Engine.hpp>

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
	C3D_DepthOfField_API void getRequiredVersion( c3d::Version * version );
	C3D_DepthOfField_API void isDebug( int * value );
	C3D_DepthOfField_API void getType( c3d::PluginType * type );
	C3D_DepthOfField_API void getName( char const ** name );
	C3D_DepthOfField_API void onLoad( c3d::Engine * engine );
	C3D_DepthOfField_API void onUnload( c3d::Engine * engine );

	C3D_DepthOfField_API void getRequiredVersion( c3d::Version * version )
	{
		*version = c3d::Version();
	}

	C3D_DepthOfField_API void isDebug( int * value )
	{
		*value = c3d::system::isDebug() ? 1 : 0;
	}

	C3D_DepthOfField_API void getType( c3d::PluginType * type )
	{
		*type = c3d::PluginType::ePostEffect;
	}

	C3D_DepthOfField_API void getName( char const ** name )
	{
		*name = dof::PostEffect::Name.c_str();
	}

	C3D_DepthOfField_API void onLoad( c3d::Engine * engine )
	{
		engine->getPostEffectFactory().registerType( dof::PostEffect::Type
			, &dof::PostEffect::create );
		engine->registerParsers( dof::PostEffect::Type
			, dof::DepthOfFieldConfig::createParsers()
			, dof::DepthOfFieldConfig::createSections()
			, nullptr );
	}

	C3D_DepthOfField_API void onUnload( c3d::Engine * engine )
	{
		engine->unregisterParsers( dof::PostEffect::Type );
		engine->getPostEffectFactory().unregisterType( dof::PostEffect::Type );
	}
}
