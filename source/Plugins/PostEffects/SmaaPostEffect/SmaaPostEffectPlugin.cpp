#include "SmaaPostEffect/SmaaPostEffect.hpp"
#include "SmaaPostEffect/Smaa_Parsers.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/TargetCache.hpp>
#include <Castor3D/Plugin/PostFxPlugin.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>

#include <CastorUtils/FileParser/ParserParameter.hpp>
#include <CastorUtils/Log/Logger.hpp>
#include <CastorUtils/Math/Range.hpp>

#ifndef CU_PlatformWindows
#	define C3D_Smaa_API
#else
#	ifdef SmaaPostEffect_EXPORTS
#		define C3D_Smaa_API __declspec( dllexport )
#	else
#		define C3D_Smaa_API __declspec( dllimport )
#	endif
#endif

extern "C"
{
	C3D_Smaa_API void getRequiredVersion( castor3d::Version * version );
	C3D_Smaa_API void getType( castor3d::PluginType * type );
	C3D_Smaa_API void getName( char const ** name );
	C3D_Smaa_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin );
	C3D_Smaa_API void OnUnload( castor3d::Engine * engine );

	C3D_Smaa_API void getRequiredVersion( castor3d::Version * version )
	{
		*version = castor3d::Version();
	}

	C3D_Smaa_API void getType( castor3d::PluginType * type )
	{
		*type = castor3d::PluginType::ePostEffect;
	}

	C3D_Smaa_API void getName( char const ** name )
	{
		*name = smaa::PostEffect::Name.c_str();
	}

	C3D_Smaa_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin )
	{
		engine->getPostEffectFactory().registerType( smaa::PostEffect::Type
			, &smaa::PostEffect::create );
		engine->registerParsers( smaa::PostEffect::Type
			, smaa::createParsers()
			, smaa::createSections()
			, nullptr );
	}

	C3D_Smaa_API void OnUnload( castor3d::Engine * engine )
	{
		engine->unregisterParsers( smaa::PostEffect::Type );
		engine->getPostEffectFactory().unregisterType( smaa::PostEffect::Type );
	}
}
