#include "HaarmPieterDuikerToneMapping/HaarmPieterDuikerToneMapping.hpp"

#include <CastorUtils/Log/Logger.hpp>

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/TargetCache.hpp>
#include <Castor3D/Miscellaneous/Version.hpp>

#ifndef CU_PlatformWindows
#	define C3D_HaarmPieterDuikerToneMapping_API
#else
#	ifdef HaarmPieterDuikerToneMapping_EXPORTS
#		define C3D_HaarmPieterDuikerToneMapping_API __declspec( dllexport )
#	else
#		define C3D_HaarmPieterDuikerToneMapping_API __declspec( dllimport )
#	endif
#endif

extern "C"
{
	C3D_HaarmPieterDuikerToneMapping_API void getRequiredVersion( castor3d::Version * p_version );
	C3D_HaarmPieterDuikerToneMapping_API void getType( castor3d::PluginType * p_type );
	C3D_HaarmPieterDuikerToneMapping_API void getName( char const ** p_name );
	C3D_HaarmPieterDuikerToneMapping_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * p_plugin );
	C3D_HaarmPieterDuikerToneMapping_API void OnUnload( castor3d::Engine * engine );

	C3D_HaarmPieterDuikerToneMapping_API void getRequiredVersion( castor3d::Version * p_version )
	{
		*p_version = castor3d::Version();
	}

	C3D_HaarmPieterDuikerToneMapping_API void getType( castor3d::PluginType * p_type )
	{
		*p_type = castor3d::PluginType::eToneMapping;
	}

	C3D_HaarmPieterDuikerToneMapping_API void getName( char const ** p_name )
	{
		*p_name = HaarmPieterDuiker::ToneMapping::Name.c_str();
	}

	C3D_HaarmPieterDuikerToneMapping_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * p_plugin )
	{
		engine->getToneMappingFactory().registerType( HaarmPieterDuiker::ToneMapping::Type
			, &HaarmPieterDuiker::ToneMapping::create );
		engine->getRenderTargetCache().registerToneMappingName( HaarmPieterDuiker::ToneMapping::Type
			, HaarmPieterDuiker::ToneMapping::Name );
	}

	C3D_HaarmPieterDuikerToneMapping_API void OnUnload( castor3d::Engine * engine )
	{
		engine->getToneMappingFactory().unregisterType( HaarmPieterDuiker::ToneMapping::Type );
	}
}
