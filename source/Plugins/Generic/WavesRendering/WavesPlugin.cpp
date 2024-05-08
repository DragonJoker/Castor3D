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
	C3D_Waves_API void getRequiredVersion( castor3d::Version * version );
	C3D_Waves_API void getType( castor3d::PluginType * type );
	C3D_Waves_API void isDebug( int * value );
	C3D_Waves_API void getName( char const ** name );
	C3D_Waves_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin );
	C3D_Waves_API void OnUnload( castor3d::Engine * engine );

	C3D_Waves_API void getRequiredVersion( castor3d::Version * version )
	{
		*version = castor3d::Version();
	}

	C3D_Waves_API void isDebug( int * value )
	{
		*value = castor::system::isDebug() ? 1 : 0;
	}

	C3D_Waves_API void getType( castor3d::PluginType * type )
	{
		*type = castor3d::PluginType::eGeneric;
	}

	C3D_Waves_API void getName( char const ** name )
	{
		*name = waves::WavesRenderComponent::FullName.c_str();
	}

	C3D_Waves_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin )
	{
		engine->registerSubmeshComponent< waves::WavesRenderComponent >();
	}

	C3D_Waves_API void OnUnload( castor3d::Engine * engine )
	{
		engine->unregisterSubmeshComponent( waves::WavesRenderComponent::TypeName );
	}
}
