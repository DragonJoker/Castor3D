#include "Uncharted2ToneMapping/Uncharted2ToneMapping.hpp"

#include <CastorUtils/Log/Logger.hpp>

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/TargetCache.hpp>
#include <Castor3D/Miscellaneous/Version.hpp>

#include <CastorUtils/Log/Logger.hpp>

#ifndef CU_PlatformWindows
#	define C3D_Uncharted2ToneMapping_API
#else
#	ifdef Uncharted2ToneMapping_EXPORTS
#		define C3D_Uncharted2ToneMapping_API __declspec( dllexport )
#	else
#		define C3D_Uncharted2ToneMapping_API __declspec( dllimport )
#	endif
#endif

extern "C"
{
	C3D_Uncharted2ToneMapping_API void getRequiredVersion( castor3d::Version * version );
	C3D_Uncharted2ToneMapping_API void isDebug( int * value );
	C3D_Uncharted2ToneMapping_API void getType( castor3d::PluginType * type );
	C3D_Uncharted2ToneMapping_API void getName( char const ** name );
	C3D_Uncharted2ToneMapping_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin );
	C3D_Uncharted2ToneMapping_API void OnUnload( castor3d::Engine * engine );

	C3D_Uncharted2ToneMapping_API void getRequiredVersion( castor3d::Version * version )
	{
		*version = castor3d::Version();
	}

	C3D_Uncharted2ToneMapping_API void isDebug( int * value )
	{
		*value = castor::system::isDebug() ? 1 : 0;
	}

	C3D_Uncharted2ToneMapping_API void getType( castor3d::PluginType * type )
	{
		*type = castor3d::PluginType::eToneMapping;
	}

	C3D_Uncharted2ToneMapping_API void getName( char const ** name )
	{
		*name = Uncharted2::ToneMapping::Name.c_str();
	}

	C3D_Uncharted2ToneMapping_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin )
	{
		engine->getToneMappingFactory().registerType( Uncharted2::ToneMapping::Type
			, &Uncharted2::ToneMapping::create );
		engine->getRenderTargetCache().registerToneMappingName( Uncharted2::ToneMapping::Type
			, castor::makeString( Uncharted2::ToneMapping::Name ) );
	}

	C3D_Uncharted2ToneMapping_API void OnUnload( castor3d::Engine * engine )
	{
		engine->getToneMappingFactory().unregisterType( Uncharted2::ToneMapping::Type );
	}
}
