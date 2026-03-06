#include "AtmosphereScattering/AtmosphereScatteringPrerequisites.hpp"

#include "AtmosphereScattering/AtmosphereBackgroundModel.hpp"
#include "AtmosphereScattering/AtmosphereModel.hpp"
#include "AtmosphereScattering/AtmosphereLightingModel.hpp"
#include "AtmosphereScattering/AtmosphereScattering_Parsers.hpp"
#include "AtmosphereScattering/CloudsVolumePlugin.hpp"

#include <Castor3D/Engine.hpp>

#ifndef CU_PlatformWindows
#	define C3D_AtmosphereScattering_API
#else
#	ifdef AtmosphereScattering_EXPORTS
#		define C3D_AtmosphereScattering_API __declspec( dllexport )
#	else
#		define C3D_AtmosphereScattering_API __declspec( dllimport )
#	endif
#endif

extern "C"
{
	C3D_AtmosphereScattering_API void getRequiredVersion( c3d::Version * version );
	C3D_AtmosphereScattering_API void getType( c3d::PluginType * type );
	C3D_AtmosphereScattering_API void isDebug( int * value );
	C3D_AtmosphereScattering_API void getName( char const ** name );
	C3D_AtmosphereScattering_API void onLoad( c3d::Engine * engine );
	C3D_AtmosphereScattering_API void onUnload( c3d::Engine * engine );

	C3D_AtmosphereScattering_API void getRequiredVersion( c3d::Version * version )
	{
		*version = c3d::Version();
	}

	C3D_AtmosphereScattering_API void isDebug( int * value )
	{
		*value = c3d::system::isDebug() ? 1 : 0;
	}

	C3D_AtmosphereScattering_API void getType( c3d::PluginType * type )
	{
		*type = c3d::PluginType::eGeneric;
	}

	C3D_AtmosphereScattering_API void getName( char const ** name )
	{
		*name = atmosphere_scattering::AtmosphereBackgroundModel::PluginName.c_str();
	}

	C3D_AtmosphereScattering_API void onLoad( c3d::Engine * engine )
	{
		engine->registerScatteringModel( { c3d::String{ atmosphere_scattering::AtmosphereScatteringModel::Name }
			, atmosphere_scattering::AtmosphereScatteringModel::create } );
		engine->registerBackgroundModel( atmosphere_scattering::AtmosphereBackgroundModel::Name
			, atmosphere_scattering::AtmosphereBackgroundModel::create );
		engine->registerParsers( atmosphere_scattering::AtmosphereBackgroundModel::PluginType
			, atmosphere_scattering::createParsers()
			, atmosphere_scattering::createSections()
			, nullptr );
		engine->registerVolumeComponent< atmosphere_scattering::CloudsVolumePlugin >();
	}

	C3D_AtmosphereScattering_API void onUnload( c3d::Engine * engine )
	{
		engine->unregisterVolumeComponent( atmosphere_scattering::CloudsVolumePlugin::TypeName );
		engine->unregisterParsers( atmosphere_scattering::AtmosphereBackgroundModel::PluginType );
		engine->unregisterBackgroundModel( atmosphere_scattering::AtmosphereBackgroundModel::Name );
		engine->unregisterScatteringModel( c3d::String{ atmosphere_scattering::AtmosphereScatteringModel::Name } );
	}
}
