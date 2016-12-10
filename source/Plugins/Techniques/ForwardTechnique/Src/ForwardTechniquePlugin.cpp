#include "ForwardRenderTechnique.hpp"

#if defined( VLD_AVAILABLE )
#	include <vld.h>
#endif

#include <Log/Logger.hpp>

#include <Engine.hpp>
#include <Cache/TechniqueCache.hpp>

#include <Miscellaneous/Version.hpp>

#ifndef _WIN32
#	define C3D_ForwardTechnique_API
#else
#	ifdef ForwardTechnique_EXPORTS
#		define C3D_ForwardTechnique_API __declspec(dllexport)
#	else
#		define C3D_ForwardTechnique_API __declspec(dllimport)
#	endif
#endif

C3D_ForwardTechnique_API void GetRequiredVersion( Castor3D::Version & p_version )
{
	p_version = Castor3D::Version();
}

C3D_ForwardTechnique_API Castor3D::PluginType GetType()
{
	return Castor3D::PluginType::eTechnique;
}

C3D_ForwardTechnique_API Castor::String GetName()
{
	return forward::RenderTechnique::Name;
}

C3D_ForwardTechnique_API void OnLoad( Castor3D::Engine * p_engine )
{
	p_engine->GetTechniqueFactory().Register( forward::RenderTechnique::Type, &forward::RenderTechnique::CreateInstance );
}

C3D_ForwardTechnique_API void OnUnload( Castor3D::Engine * p_engine )
{
	p_engine->GetTechniqueFactory().Unregister( forward::RenderTechnique::Type );
}
