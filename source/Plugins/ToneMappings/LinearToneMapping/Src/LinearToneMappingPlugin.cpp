#include "LinearToneMapping.hpp"

#include <Log/Logger.hpp>

#include <Engine.hpp>
#include <Cache/TargetCache.hpp>

#include <Miscellaneous/Version.hpp>

using namespace Linear;

#ifndef CASTOR_PLATFORM_WINDOWS
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
	C3D_LinearToneMapping_API void getRequiredVersion( castor3d::Version * p_version )
	{
		*p_version = castor3d::Version();
	}

	C3D_LinearToneMapping_API void getType( castor3d::PluginType * p_type )
	{
		*p_type = castor3d::PluginType::eToneMapping;
	}

	C3D_LinearToneMapping_API void getName( char const ** p_name )
	{
		static castor::String const Name = cuT( "Linear Tone Mapping" );
		*p_name = Name.c_str();
	}

	C3D_LinearToneMapping_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * p_plugin )
	{
		engine->getRenderTargetCache().getToneMappingFactory().registerType( ToneMapping::Name
			, &ToneMapping::create );
	}

	C3D_LinearToneMapping_API void OnUnload( castor3d::Engine * engine )
	{
		engine->getRenderTargetCache().getToneMappingFactory().unregisterType( ToneMapping::Name );
	}
}
