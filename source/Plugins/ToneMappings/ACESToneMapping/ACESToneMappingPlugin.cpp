#include "ACESToneMapping/ACESToneMapping.hpp"

#include <CastorUtils/Log/Logger.hpp>

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/TargetCache.hpp>
#include <Castor3D/Miscellaneous/Version.hpp>

#include <CastorUtils/Log/Logger.hpp>

#ifndef CU_PlatformWindows
#	define C3D_ACESToneMapping_API
#else
#	ifdef ACESToneMapping_EXPORTS
#		define C3D_ACESToneMapping_API __declspec( dllexport )
#	else
#		define C3D_ACESToneMapping_API __declspec( dllimport )
#	endif
#endif

extern "C"
{
	C3D_ACESToneMapping_API void getRequiredVersion( castor3d::Version * version );
	C3D_ACESToneMapping_API void getType( castor3d::PluginType * type );
	C3D_ACESToneMapping_API void getName( char const ** name );
	C3D_ACESToneMapping_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin );
	C3D_ACESToneMapping_API void OnUnload( castor3d::Engine * engine );

	C3D_ACESToneMapping_API void getRequiredVersion( castor3d::Version * version )
	{
		*version = castor3d::Version();
	}

	C3D_ACESToneMapping_API void getType( castor3d::PluginType * type )
	{
		*type = castor3d::PluginType::eToneMapping;
	}

	C3D_ACESToneMapping_API void getName( char const ** name )
	{
		*name = aces::ToneMapping::Name.c_str();
	}

	C3D_ACESToneMapping_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin )
	{
		engine->getToneMappingFactory().registerType( aces::ToneMapping::Type
			, &aces::ToneMapping::create );
		engine->getRenderTargetCache().registerToneMappingName( aces::ToneMapping::Type
			, castor::makeString( aces::ToneMapping::Name ) );
	}

	C3D_ACESToneMapping_API void OnUnload( castor3d::Engine * engine )
	{
		engine->getToneMappingFactory().unregisterType( aces::ToneMapping::Type );
	}
}
