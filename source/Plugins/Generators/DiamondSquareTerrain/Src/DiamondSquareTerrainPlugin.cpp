#include "DiamondSquareTerrain.hpp"

#include <Engine.hpp>
#include <Log/Logger.hpp>

#ifndef CASTOR_PLATFORM_WINDOWS
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
	C3D_DiamondSquareTerrain_API void GetRequiredVersion( Castor3D::Version * p_version )
	{
		*p_version = Castor3D::Version();
	}

	C3D_DiamondSquareTerrain_API void GetType( Castor3D::PluginType * p_type )
	{
		*p_type = Castor3D::PluginType::eGenerator;
	}

	C3D_DiamondSquareTerrain_API void GetName( char const ** p_name )
	{
		*p_name = diamond_square_terrain::Generator::Name.c_str();
	}

	C3D_DiamondSquareTerrain_API void OnLoad( Castor3D::Engine * p_engine, Castor3D::Plugin * p_plugin )
	{
		p_engine->GetMeshFactory().Register( diamond_square_terrain::Generator::Type
			, &diamond_square_terrain::Generator::Create );
	}

	C3D_DiamondSquareTerrain_API void OnUnload( Castor3D::Engine * p_engine )
	{
		p_engine->GetMeshFactory().Unregister( diamond_square_terrain::Generator::Type );
	}
}
