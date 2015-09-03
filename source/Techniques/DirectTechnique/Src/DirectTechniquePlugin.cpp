#include "DirectRenderTechnique.hpp"

#include <Engine.hpp>
#include <TechniqueFactory.hpp>
#include <Version.hpp>

#include <Logger.hpp>

using namespace Direct;
static const Castor::String NAME = cuT( "direct" );

C3D_DirectTechnique_API void GetRequiredVersion( Castor3D::Version & p_version )
{
	p_version = Castor3D::Version();
}

C3D_DirectTechnique_API Castor3D::ePLUGIN_TYPE GetType()
{
	return Castor3D::ePLUGIN_TYPE_TECHNIQUE;
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
