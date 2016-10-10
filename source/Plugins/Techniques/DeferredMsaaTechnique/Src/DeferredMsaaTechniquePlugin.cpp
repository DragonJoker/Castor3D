#include "DeferredMsaaRenderTechnique.hpp"

#include <Log/Logger.hpp>

#include <Engine.hpp>
#include <TechniqueCache.hpp>

#include <Miscellaneous/Version.hpp>

#ifndef _WIN32
#	define C3D_DeferredMsaaTechnique_API
#else
#	ifdef DeferredMsaaTechnique_EXPORTS
#		define C3D_DeferredMsaaTechnique_API __declspec(dllexport)
#	else
#		define C3D_DeferredMsaaTechnique_API __declspec(dllimport)
#	endif
#endif

using namespace DeferredMsaa;
static const Castor::String NAME = cuT( "deferred_msaa" );

C3D_DeferredMsaaTechnique_API void GetRequiredVersion( Castor3D::Version & p_version )
{
	p_version = Castor3D::Version();
}

C3D_DeferredMsaaTechnique_API Castor3D::PluginType GetType()
{
	return Castor3D::PluginType::Technique;
}

C3D_DeferredMsaaTechnique_API Castor::String GetName()
{
	return cuT( "Deferred Lighting MSAA Render Technique" );
}

C3D_DeferredMsaaTechnique_API void OnLoad( Castor3D::Engine * p_engine )
{
	p_engine->GetTechniqueFactory().Register( NAME, &RenderTechnique::CreateInstance );
}

C3D_DeferredMsaaTechnique_API void OnUnload( Castor3D::Engine * p_engine )
{
	p_engine->GetTechniqueFactory().Unregister( NAME );
}
