#include "FFTOceanRendering/FFTWaterComponent.hpp"
#include "FFTOceanRendering/FFTWavesComponent.hpp"

#include <Castor3D/Engine.hpp>

#ifndef CU_PlatformWindows
#	define C3D_OceanRendering_API
#else
#	ifdef FFTOceanRendering_EXPORTS
#		define C3D_OceanRendering_API __declspec( dllexport )
#	else
#		define C3D_OceanRendering_API __declspec( dllimport )
#	endif
#endif

extern "C"
{
	C3D_OceanRendering_API void getRequiredVersion( castor3d::Version * version );
	C3D_OceanRendering_API void getType( castor3d::PluginType * type );
	C3D_OceanRendering_API void isDebug( int * value );
	C3D_OceanRendering_API void getName( char const ** name );
	C3D_OceanRendering_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin );
	C3D_OceanRendering_API void OnUnload( castor3d::Engine * engine );

	C3D_OceanRendering_API void getRequiredVersion( castor3d::Version * version )
	{
		*version = castor3d::Version();
	}

	C3D_OceanRendering_API void isDebug( int * value )
	{
		*value = castor::system::isDebug() ? 1 : 0;
	}

	C3D_OceanRendering_API void getType( castor3d::PluginType * type )
	{
		*type = castor3d::PluginType::eGeneric;
	}

	C3D_OceanRendering_API void getName( char const ** name )
	{
		*name = ocean_fft::FFTWavesComponent::FullName.c_str();
	}

	C3D_OceanRendering_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin )
	{
		engine->registerPassComponent< ocean_fft::FFTWaterComponent >();
		engine->registerSubmeshComponent< ocean_fft::FFTWavesComponent >();
	}

	C3D_OceanRendering_API void OnUnload( castor3d::Engine * engine )
	{
		engine->unregisterSubmeshComponent( ocean_fft::FFTWavesComponent::TypeName );
		engine->unregisterPassComponent( ocean_fft::FFTWaterComponent::TypeName );
	}
}
