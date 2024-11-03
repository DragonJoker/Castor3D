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
	C3D_DisneyBRDF_API void getRequiredVersion( castor3d::Version * version );
	C3D_DisneyBRDF_API void getType( castor3d::PluginType * type );
	C3D_DisneyBRDF_API void isDebug( int * value );
	C3D_DisneyBRDF_API void getName( char const ** name );
	C3D_DisneyBRDF_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin );
	C3D_DisneyBRDF_API void OnUnload( castor3d::Engine * engine );

	C3D_DisneyBRDF_API void getRequiredVersion( castor3d::Version * version )
	{
		*version = castor3d::Version();
	}

	C3D_DisneyBRDF_API void isDebug( int * value )
	{
		*value = castor::system::isDebug() ? 1 : 0;
	}

	C3D_DisneyBRDF_API void getType( castor3d::PluginType * type )
	{
		*type = castor3d::PluginType::eGeneric;
	}

	C3D_DisneyBRDF_API void getName( char const ** name )
	{
		*name = "Disney BRDF";
	}

	C3D_DisneyBRDF_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin )
	{
		engine->registerDiffuseBrdf( { castor::String{ disney::shader::DisneyDiffuseBRDF::Name }, disney::shader::DisneyDiffuseBRDF::create } );
		engine->registerSpecularBrdf( { castor::String{ disney::shader::DisneySpecularBRDF::Name }, disney::shader::DisneySpecularBRDF::create } );
		engine->registerSheenBrdf( { castor::String{ disney::shader::DisneySheenBRDF::Name }, disney::shader::DisneySheenBRDF::create } );
		engine->registerClearcoatBrdf( { castor::String{ disney::shader::DisneyClearcoatBRDF::Name }, disney::shader::DisneyClearcoatBRDF::create } );
	}

	C3D_DisneyBRDF_API void OnUnload( castor3d::Engine * engine )
	{
		engine->unregisterClearcoatBrdf( castor::String{ disney::shader::DisneyClearcoatBRDF::Name } );
		engine->unregisterSheenBrdf( castor::String{ disney::shader::DisneySheenBRDF::Name } );
		engine->unregisterSpecularBrdf( castor::String{ disney::shader::DisneySpecularBRDF::Name } );
		engine->unregisterDiffuseBrdf( castor::String{ disney::shader::DisneyDiffuseBRDF::Name } );
	}
}
