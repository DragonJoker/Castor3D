#include "WavesComponent.hpp"

#include <Castor3D/Engine.hpp>

#ifndef CU_PlatformWindows
#	define C3D_Waves_API
#else
#	ifdef WavesRendering_EXPORTS
#		define C3D_Waves_API __declspec( dllexport )
#	else
#		define C3D_Waves_API __declspec( dllimport )
#	endif
#endif

extern "C"
{
	C3D_Waves_API void getRequiredVersion( c3d::Version * version );
	C3D_Waves_API void getType( c3d::PluginType * type );
	C3D_Waves_API void isDebug( int * value );
	C3D_Waves_API void getName( char const ** name );
	C3D_Waves_API void onLoad( c3d::Engine * engine );
	C3D_Waves_API void onUnload( c3d::Engine * engine );

	C3D_Waves_API void getRequiredVersion( c3d::Version * version )
	{
		*version = c3d::Version();
	}

	C3D_Waves_API void isDebug( int * value )
	{
		*value = c3d::system::isDebug() ? 1 : 0;
	}

	C3D_Waves_API void getType( c3d::PluginType * type )
	{
		*type = c3d::PluginType::eGeneric;
	}

	C3D_Waves_API void getName( char const ** name )
	{
		*name = waves::WavesRenderComponent::FullName.c_str();
	}

	C3D_Waves_API void onLoad( c3d::Engine * engine )
	{
		engine->registerSubmeshComponent< waves::WavesRenderComponent >();
	}

	C3D_Waves_API void onUnload( c3d::Engine * engine )
	{
		engine->unregisterSubmeshComponent( waves::WavesRenderComponent::TypeName );
	}
}
