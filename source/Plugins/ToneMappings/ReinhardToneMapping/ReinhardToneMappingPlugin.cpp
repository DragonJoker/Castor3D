#include "ReinhardToneMapping/ReinhardToneMapping.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/TargetCache.hpp>
#include <Castor3D/Miscellaneous/Version.hpp>

#include <CastorUtils/Log/Logger.hpp>

using namespace Reinhard;

#ifndef CU_PlatformWindows
#	define C3D_ReinhardToneMapping_API
#else
#	ifdef castor3dReinhardToneMapping_EXPORTS
#		define C3D_ReinhardToneMapping_API __declspec( dllexport )
#	else
#		define C3D_ReinhardToneMapping_API __declspec( dllimport )
#	endif
#endif

extern "C"
{
	C3D_ReinhardToneMapping_API void getRequiredVersion( castor3d::Version * p_version )
	{
		*p_version = castor3d::Version();
	}

	C3D_ReinhardToneMapping_API void getType( castor3d::PluginType * p_type )
	{
		*p_type = castor3d::PluginType::eToneMapping;
	}

	C3D_ReinhardToneMapping_API void getName( char const ** p_name )
	{
		*p_name = ToneMapping::Name.c_str();
	}

	C3D_ReinhardToneMapping_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * p_plugin )
	{
		engine->getRenderTargetCache().getToneMappingFactory().registerType( ToneMapping::Type
			, &ToneMapping::create );
	}

	C3D_ReinhardToneMapping_API void OnUnload( castor3d::Engine * engine )
	{
		engine->getRenderTargetCache().getToneMappingFactory().unregisterType( ToneMapping::Type );
	}
}
