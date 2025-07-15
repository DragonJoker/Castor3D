#include "DisneyBRDF/Shaders/GlslDisneyBRDF.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Material/Pass/PassFactory.hpp>
#include <Castor3D/Material/Pass/PbrPass.hpp>
#include <Castor3D/Material/Pass/Shaders/GlslPbrLighting.hpp>

#ifndef CU_PlatformWindows
#	define C3D_DisneyBRDF_API
#else
#	ifdef DisneyBRDF_EXPORTS
#		define C3D_DisneyBRDF_API __declspec( dllexport )
#	else
#		define C3D_DisneyBRDF_API __declspec( dllimport )
#	endif
#endif

extern "C"
{
	C3D_DisneyBRDF_API void getRequiredVersion( c3d::Version * version );
	C3D_DisneyBRDF_API void getType( c3d::PluginType * type );
	C3D_DisneyBRDF_API void isDebug( int * value );
	C3D_DisneyBRDF_API void getName( char const ** name );
	C3D_DisneyBRDF_API void onLoad( c3d::Engine * engine, c3d::Plugin * plugin );
	C3D_DisneyBRDF_API void onUnload( c3d::Engine * engine );

	C3D_DisneyBRDF_API void getRequiredVersion( c3d::Version * version )
	{
		*version = c3d::Version();
	}

	C3D_DisneyBRDF_API void isDebug( int * value )
	{
		*value = c3d::system::isDebug() ? 1 : 0;
	}

	C3D_DisneyBRDF_API void getType( c3d::PluginType * type )
	{
		*type = c3d::PluginType::eGeneric;
	}

	C3D_DisneyBRDF_API void getName( char const ** name )
	{
		*name = "Disney BRDF";
	}

	C3D_DisneyBRDF_API void onLoad( c3d::Engine * engine, c3d::Plugin * plugin )
	{
		engine->registerDiffuseBrdf( { c3d::String{ disney::shader::DisneyDiffuseBRDF::Name }, disney::shader::DisneyDiffuseBRDF::create } );
		engine->registerSpecularBrdf( { c3d::String{ disney::shader::DisneySpecularBRDF::Name }, disney::shader::DisneySpecularBRDF::create } );
		engine->registerSheenBrdf( { c3d::String{ disney::shader::DisneySheenBRDF::Name }, disney::shader::DisneySheenBRDF::create } );
		engine->registerClearcoatBrdf( { c3d::String{ disney::shader::DisneyClearcoatBRDF::Name }, disney::shader::DisneyClearcoatBRDF::create } );
	}

	C3D_DisneyBRDF_API void onUnload( c3d::Engine * engine )
	{
		engine->unregisterClearcoatBrdf( c3d::String{ disney::shader::DisneyClearcoatBRDF::Name } );
		engine->unregisterSheenBrdf( c3d::String{ disney::shader::DisneySheenBRDF::Name } );
		engine->unregisterSpecularBrdf( c3d::String{ disney::shader::DisneySpecularBRDF::Name } );
		engine->unregisterDiffuseBrdf( c3d::String{ disney::shader::DisneyDiffuseBRDF::Name } );
	}
}
