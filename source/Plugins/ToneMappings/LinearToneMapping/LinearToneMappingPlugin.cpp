#include "LinearToneMapping/LinearToneMapping.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/TargetCache.hpp>
#include <Castor3D/Miscellaneous/Version.hpp>

#include <CastorUtils/Log/Logger.hpp>

#ifndef CU_PlatformWindows
#	define C3D_LinearToneMapping_API
#else
#	ifdef LinearToneMapping_EXPORTS
#		define C3D_LinearToneMapping_API __declspec( dllexport )
#	else
#		define C3D_LinearToneMapping_API __declspec( dllimport )
#	endif
#endif

extern "C"
{
	C3D_LinearToneMapping_API void getRequiredVersion( castor3d::Version * version );
	C3D_LinearToneMapping_API void getType( castor3d::PluginType * type );
	C3D_LinearToneMapping_API void getName( char const ** name );
	C3D_LinearToneMapping_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin );
	C3D_LinearToneMapping_API void OnUnload( castor3d::Engine * engine );

	C3D_LinearToneMapping_API void getRequiredVersion( castor3d::Version * version )
	{
		*version = castor3d::Version();
	}

	C3D_LinearToneMapping_API void getType( castor3d::PluginType * type )
	{
		*type = castor3d::PluginType::eToneMapping;
	}

	C3D_LinearToneMapping_API void getName( char const ** name )
	{
		*name = Linear::ToneMapping::Name.c_str();
	}

	C3D_LinearToneMapping_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin )
	{
		engine->getToneMappingFactory().registerType( Linear::ToneMapping::Type
			, &Linear::ToneMapping::create );
		engine->getRenderTargetCache().registerToneMappingName( Linear::ToneMapping::Type
			, castor::makeString( Linear::ToneMapping::Name ) );
	}

	C3D_LinearToneMapping_API void OnUnload( castor3d::Engine * engine )
	{
		engine->getToneMappingFactory().unregisterType( Linear::ToneMapping::Type );
	}
}
