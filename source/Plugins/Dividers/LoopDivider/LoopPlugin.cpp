#include "LoopDivider/LoopDivider.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Model/Mesh/MeshFactory.hpp>

#ifndef CU_PlatformWindows
#	define C3D_Loop_API
#else
#	ifdef LoopDivider_EXPORTS
#		define C3D_Loop_API __declspec(dllexport)
#	else
#		define C3D_Loop_API __declspec(dllimport)
#	endif
#endif

extern "C"
{
	C3D_Loop_API void getRequiredVersion( castor3d::Version * version );
	C3D_Loop_API void getType( castor3d::PluginType * type );
	C3D_Loop_API void getName( char const ** name );
	C3D_Loop_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin );
	C3D_Loop_API void OnUnload( castor3d::Engine * engine );

	C3D_Loop_API void getRequiredVersion( castor3d::Version * version )
	{
		*version = castor3d::Version();
	}

	C3D_Loop_API void getType( castor3d::PluginType * type )
	{
		*type = castor3d::PluginType::eDivider;
	}

	C3D_Loop_API void getName( char const ** name )
	{
		*name = Loop::Subdivider::Name.c_str();
	}

	C3D_Loop_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin )
	{
		engine->getSubdividerFactory().registerType( Loop::Subdivider::Type
			, &Loop::Subdivider::create );
	}

	C3D_Loop_API void OnUnload( castor3d::Engine * engine )
	{
		engine->getSubdividerFactory().unregisterType( Loop::Subdivider::Type );
	}
}
