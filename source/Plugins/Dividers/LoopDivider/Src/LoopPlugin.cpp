#include "LoopDivider.hpp"

#include <Engine.hpp>
#include <Log/Logger.hpp>

using namespace Castor;
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
	C3D_Loop_API void GetRequiredVersion( Castor3D::Version * p_version )
	{
		*p_version = Castor3D::Version();
	}

	C3D_Loop_API void GetType( Castor3D::PluginType * p_type )
	{
		*p_type = Castor3D::PluginType::eDivider;
	}

	C3D_Loop_API void GetName( char const ** p_name )
	{
		*p_name = Subdivider::Name.c_str();
	}

	C3D_Loop_API void OnLoad( Castor3D::Engine * p_engine, Castor3D::Plugin * p_plugin )
	{
		p_engine->GetSubdividerFactory().Register( Subdivider::Type, &Subdivider::Create );
	}

	C3D_Loop_API void OnUnload( Castor3D::Engine * p_engine )
	{
		p_engine->GetSubdividerFactory().Unregister( Subdivider::Type );
	}
}
