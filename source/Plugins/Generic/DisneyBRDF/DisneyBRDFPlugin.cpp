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
		engine->registerPassModels( { castor::String{ castor3d::PbrPass::LightingModel }
			, castor3d::PbrPass::create
			, castor3d::shader::PbrLightingModel::create
			, { { castor::String{ disney::shader::DisneyDiffuseBRDF::Name }, disney::shader::DisneyDiffuseBRDF::create } }
			, castor3d::PbrPass::DefaultDiffuseBrdf
			, { { castor::String{ disney::shader::DisneySpecularBRDF::Name }, disney::shader::DisneySpecularBRDF::create } }
			, castor3d::PbrPass::DefaultSpecularBrdf } );
	}

	C3D_DisneyBRDF_API void OnUnload( castor3d::Engine * engine )
	{
		engine->unregisterPassModels( castor::String{ castor3d::PbrPass::LightingModel }
			, { castor::String{ disney::shader::DisneyDiffuseBRDF::Name } }
			, { castor::String{ disney::shader::DisneySpecularBRDF::Name } } );
	}
}
