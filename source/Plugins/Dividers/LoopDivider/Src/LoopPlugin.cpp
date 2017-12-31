﻿#include "LoopDivider.hpp"

#include <Engine.hpp>
#include <Log/Logger.hpp>

using namespace castor;
using namespace Loop;

#ifndef CASTOR_PLATFORM_WINDOWS
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
	C3D_Loop_API void getRequiredVersion( castor3d::Version * p_version )
	{
		*p_version = castor3d::Version();
	}

	C3D_Loop_API void getType( castor3d::PluginType * p_type )
	{
		*p_type = castor3d::PluginType::eDivider;
	}

	C3D_Loop_API void getName( char const ** p_name )
	{
		*p_name = Subdivider::Name.c_str();
	}

	C3D_Loop_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * p_plugin )
	{
		engine->getSubdividerFactory().registerType( Subdivider::Type, &Subdivider::create );
	}

	C3D_Loop_API void OnUnload( castor3d::Engine * engine )
	{
		engine->getSubdividerFactory().unregisterType( Subdivider::Type );
	}
}
