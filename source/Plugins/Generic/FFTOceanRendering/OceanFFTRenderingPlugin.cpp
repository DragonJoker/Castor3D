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
	C3D_OceanRendering_API void getRequiredVersion( c3d::Version * version );
	C3D_OceanRendering_API void getType( c3d::PluginType * type );
	C3D_OceanRendering_API void isDebug( int * value );
	C3D_OceanRendering_API void getName( char const ** name );
	C3D_OceanRendering_API void onLoad( c3d::Engine * engine, c3d::Plugin * plugin );
	C3D_OceanRendering_API void onUnload( c3d::Engine * engine );

	C3D_OceanRendering_API void getRequiredVersion( c3d::Version * version )
	{
		*version = c3d::Version();
	}

	C3D_OceanRendering_API void isDebug( int * value )
	{
		*value = c3d::system::isDebug() ? 1 : 0;
	}

	C3D_OceanRendering_API void getType( c3d::PluginType * type )
	{
		*type = c3d::PluginType::eGeneric;
	}

	C3D_OceanRendering_API void getName( char const ** name )
	{
		*name = ocean_fft::FFTWavesComponent::FullName.c_str();
	}

	C3D_OceanRendering_API void onLoad( c3d::Engine * engine, c3d::Plugin * plugin )
	{
		engine->registerPassComponent< ocean_fft::FFTWaterComponent >();
		engine->registerSubmeshComponent< ocean_fft::FFTWavesComponent >();
	}

	C3D_OceanRendering_API void onUnload( c3d::Engine * engine )
	{
		engine->unregisterSubmeshComponent( ocean_fft::FFTWavesComponent::TypeName );
		engine->unregisterPassComponent( ocean_fft::FFTWaterComponent::TypeName );
	}
}
