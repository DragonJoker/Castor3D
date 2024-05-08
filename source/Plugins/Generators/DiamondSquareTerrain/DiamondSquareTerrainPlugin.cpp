#include "DiamondSquareTerrain/DiamondSquareTerrain.hpp"

#include "DiamondSquareTerrain/DiamondSquare_Parsers.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Model/Mesh/MeshFactory.hpp>

#include <CastorUtils/FileParser/FileParser.hpp>

#ifndef CU_PlatformWindows
#	define C3D_DiamondSquareTerrain_API
#else
#	ifdef DiamondSquareTerrain_EXPORTS
#		define C3D_DiamondSquareTerrain_API __declspec( dllexport )
#	else
#		define C3D_DiamondSquareTerrain_API __declspec( dllimport )
#	endif
#endif

extern "C"
{
	C3D_DiamondSquareTerrain_API void getRequiredVersion( castor3d::Version * version );
	C3D_DiamondSquareTerrain_API void isDebug( int * value );
	C3D_DiamondSquareTerrain_API void getType( castor3d::PluginType * type );
	C3D_DiamondSquareTerrain_API void getName( char const ** name );
	C3D_DiamondSquareTerrain_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin );
	C3D_DiamondSquareTerrain_API void OnUnload( castor3d::Engine * engine );

	C3D_DiamondSquareTerrain_API void getRequiredVersion( castor3d::Version * version )
	{
		*version = castor3d::Version();
	}

	C3D_DiamondSquareTerrain_API void isDebug( int * value )
	{
		*value = castor::system::isDebug() ? 1 : 0;
	}

	C3D_DiamondSquareTerrain_API void getType( castor3d::PluginType * type )
	{
		*type = castor3d::PluginType::eGenerator;
	}

	C3D_DiamondSquareTerrain_API void getName( char const ** name )
	{
		*name = diamond_square_terrain::Generator::Name.c_str();
	}

	C3D_DiamondSquareTerrain_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin )
	{
		engine->registerParsers( diamond_square_terrain::Generator::Type
			, diamond_square_terrain::createParsers()
			, diamond_square_terrain::createSections()
			, nullptr );
		engine->getMeshFactory().registerType( diamond_square_terrain::Generator::Type
			, &diamond_square_terrain::Generator::create );
	}

	C3D_DiamondSquareTerrain_API void OnUnload( castor3d::Engine * engine )
	{
		engine->getMeshFactory().unregisterType( diamond_square_terrain::Generator::Type );
		engine->unregisterParsers( diamond_square_terrain::Generator::Type );
	}
}
