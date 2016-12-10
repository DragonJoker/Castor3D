#include "DeferredRenderTechnique.hpp"

#if defined( VLD_AVAILABLE )
#	include <vld.h>
#endif

#include <Log/Logger.hpp>

#include <Engine.hpp>
#include <Cache/TechniqueCache.hpp>

#include <Miscellaneous/Version.hpp>

#ifndef _WIN32
#	define C3D_DeferredTechnique_API
#else
#	ifdef DeferredTechnique_EXPORTS
#		define C3D_DeferredTechnique_API __declspec(dllexport)
#	else
#		define C3D_DeferredTechnique_API __declspec(dllimport)
#	endif
#endif

C3D_DeferredTechnique_API void GetRequiredVersion( Castor3D::Version & p_version )
{
	p_version = Castor3D::Version();
}

C3D_DeferredTechnique_API Castor3D::PluginType GetType()
{
	return Castor3D::PluginType::eTechnique;
}

C3D_DeferredTechnique_API Castor::String GetName()
{
	return deferred::RenderTechnique::Name;
}

C3D_DeferredTechnique_API void OnLoad( Castor3D::Engine * p_engine )
{
	p_engine->GetTechniqueFactory().Register( deferred::RenderTechnique::Type, &deferred::RenderTechnique::CreateInstance );
}

C3D_DeferredTechnique_API void OnUnload( Castor3D::Engine * p_engine )
{
	p_engine->GetTechniqueFactory().Unregister( deferred::RenderTechnique::Type );
}
