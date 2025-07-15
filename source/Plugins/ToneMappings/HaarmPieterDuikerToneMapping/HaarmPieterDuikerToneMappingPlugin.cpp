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
	C3D_HaarmPieterDuikerToneMapping_API void getRequiredVersion( c3d::Version * version );
	C3D_HaarmPieterDuikerToneMapping_API void isDebug( int * value );
	C3D_HaarmPieterDuikerToneMapping_API void getType( c3d::PluginType * type );
	C3D_HaarmPieterDuikerToneMapping_API void getName( char const ** name );
	C3D_HaarmPieterDuikerToneMapping_API void onLoad( c3d::Engine * engine, c3d::Plugin * plugin );
	C3D_HaarmPieterDuikerToneMapping_API void onUnload( c3d::Engine * engine );

	C3D_HaarmPieterDuikerToneMapping_API void getRequiredVersion( c3d::Version * version )
	{
		*version = c3d::Version();
	}

	C3D_HaarmPieterDuikerToneMapping_API void isDebug( int * value )
	{
		*value = c3d::system::isDebug() ? 1 : 0;
	}

	C3D_HaarmPieterDuikerToneMapping_API void getType( c3d::PluginType * type )
	{
		*type = c3d::PluginType::eToneMapping;
	}

	C3D_HaarmPieterDuikerToneMapping_API void getName( char const ** name )
	{
		*name = HaarmPieterDuiker::ToneMapping::Name.c_str();
	}

	C3D_HaarmPieterDuikerToneMapping_API void onLoad( c3d::Engine * engine, c3d::Plugin * plugin )
	{
		engine->getToneMappingFactory().registerType( HaarmPieterDuiker::ToneMapping::Type
			, &HaarmPieterDuiker::ToneMapping::create );
		engine->getRenderTargetCache().registerToneMappingName( HaarmPieterDuiker::ToneMapping::Type
			, c3d::makeString( HaarmPieterDuiker::ToneMapping::Name ) );
	}

	C3D_HaarmPieterDuikerToneMapping_API void onUnload( c3d::Engine * engine )
	{
		engine->getToneMappingFactory().unregisterType( HaarmPieterDuiker::ToneMapping::Type );
	}
}
