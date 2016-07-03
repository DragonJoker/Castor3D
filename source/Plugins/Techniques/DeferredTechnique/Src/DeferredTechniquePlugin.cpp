#include "DeferredRenderTechnique.hpp"

#include <Logger.hpp>

#include <Engine.hpp>
#include <TechniqueManager.hpp>

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

using namespace Deferred;
static const Castor::String NAME = cuT( "deferred" );

C3D_DeferredTechnique_API void GetRequiredVersion( Castor3D::Version & p_version )
{
	p_version = Castor3D::Version();
}

C3D_DeferredTechnique_API Castor3D::ePLUGIN_TYPE GetType()
{
	return Castor3D::ePLUGIN_TYPE_TECHNIQUE;
}

C3D_DeferredTechnique_API Castor::String GetName()
{
	return cuT( "Deferred Lighting Render Technique" );
}

C3D_DeferredTechnique_API void OnLoad( Castor3D::Engine * p_engine )
{
	p_engine->GetRenderTechniqueManager().GetFactory().Register( NAME, &RenderTechnique::CreateInstance );
}

C3D_DeferredTechnique_API void OnUnload( Castor3D::Engine * p_engine )
{
	p_engine->GetRenderTechniqueManager().GetFactory().Unregister( NAME );
}
