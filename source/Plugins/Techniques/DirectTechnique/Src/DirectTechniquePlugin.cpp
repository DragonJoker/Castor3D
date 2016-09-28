#include "DirectRenderTechnique.hpp"

#if defined( VLD_AVAILABLE )
#	include <vld.h>
#endif

#include <Log/Logger.hpp>

#include <Engine.hpp>
#include <TechniqueCache.hpp>

#include <Miscellaneous/Version.hpp>

#ifndef _WIN32
#	define C3D_DirectTechnique_API
#else
#	ifdef DirectTechnique_EXPORTS
#		define C3D_DirectTechnique_API __declspec(dllexport)
#	else
#		define C3D_DirectTechnique_API __declspec(dllimport)
#	endif
#endif

using namespace Direct;
static const Castor::String NAME = cuT( "direct" );

C3D_DirectTechnique_API void GetRequiredVersion( Castor3D::Version & p_version )
{
	p_version = Castor3D::Version();
}

C3D_DirectTechnique_API Castor3D::PluginType GetType()
{
	return Castor3D::PluginType::Technique;
}

C3D_DirectTechnique_API Castor::String GetName()
{
	return cuT( "Direct Render Technique" );
}

C3D_DirectTechnique_API void OnLoad( Castor3D::Engine * p_engine )
{
	p_engine->GetTechniqueFactory().Register( NAME, &RenderTechnique::CreateInstance );
}

C3D_DirectTechnique_API void OnUnload( Castor3D::Engine * p_engine )
{
	p_engine->GetTechniqueFactory().Unregister( NAME );
}
