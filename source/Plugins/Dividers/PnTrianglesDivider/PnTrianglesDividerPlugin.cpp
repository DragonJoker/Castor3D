#include "PnTrianglesDivider/PnTrianglesDivider.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Model/Mesh/MeshFactory.hpp>

using namespace castor;
using namespace PnTriangles;

#ifndef CU_PlatformWindows
#	define C3D_PnTriangles_API
#else
#	ifdef PnTrianglesDivider_EXPORTS
#		define C3D_PnTriangles_API __declspec(dllexport)
#	else
#		define C3D_PnTriangles_API __declspec(dllimport)
#	endif
#endif

extern "C"
{
	C3D_PnTriangles_API void getRequiredVersion( castor3d::Version * p_version );
	C3D_PnTriangles_API void getType( castor3d::PluginType * p_type );
	C3D_PnTriangles_API void getName( char const ** p_name );
	C3D_PnTriangles_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * p_plugin );
	C3D_PnTriangles_API void OnUnload( castor3d::Engine * engine );

	C3D_PnTriangles_API void getRequiredVersion( castor3d::Version * p_version )
	{
		*p_version = castor3d::Version();
	}

	C3D_PnTriangles_API void getType( castor3d::PluginType * p_type )
	{
		*p_type = castor3d::PluginType::eDivider;
	}

	C3D_PnTriangles_API void getName( char const ** p_name )
	{
		*p_name = Subdivider::Name.c_str();
	}

	C3D_PnTriangles_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * p_plugin )
	{
		engine->getSubdividerFactory().registerType( Subdivider::Type, &Subdivider::create );
	}

	C3D_PnTriangles_API void OnUnload( castor3d::Engine * engine )
	{
		engine->getSubdividerFactory().unregisterType( Subdivider::Type );
	}
}
