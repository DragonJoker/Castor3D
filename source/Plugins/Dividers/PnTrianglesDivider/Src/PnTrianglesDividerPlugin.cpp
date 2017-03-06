#include "PnTrianglesDivider.hpp"

#include <Engine.hpp>
#include <Log/Logger.hpp>

using namespace Castor;
using namespace PnTriangles;

#ifndef CASTOR_PLATFORM_WINDOWS
#	define C3D_PnTriangles_API
#else
#	ifdef PnTrianglesDivider_EXPORTS
#		define C3D_PnTriangles_API __declspec(dllexport)
#	else
#		define C3D_PnTriangles_API __declspec(dllimport)
#	endif
#endif

C3D_PnTriangles_API void GetRequiredVersion( Castor3D::Version & p_version )
{
	p_version = Castor3D::Version();
}

C3D_PnTriangles_API Castor3D::PluginType GetType()
{
	return Castor3D::PluginType::eDivider;
}

C3D_PnTriangles_API String GetName()
{
	return Subdivider::Name;
}

C3D_PnTriangles_API void OnLoad( Castor3D::Engine * p_engine )
{
	p_engine->GetSubdividerFactory().Register( Subdivider::Type, &Subdivider::Create );
}

C3D_PnTriangles_API void OnUnload( Castor3D::Engine * p_engine )
{
	p_engine->GetSubdividerFactory().Unregister( Subdivider::Type );
}
