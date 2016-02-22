#include "SsaaRenderTechnique.hpp"

#include <Engine.hpp>
#include <TechniqueManager.hpp>
#include <Version.hpp>

#include <Logger.hpp>

using namespace Ssaa;
static const Castor::String NAME = cuT( "ssaa" );

C3D_SsaaTechnique_API void GetRequiredVersion( Castor3D::Version & p_version )
{
	p_version = Castor3D::Version();
}

C3D_SsaaTechnique_API Castor3D::ePLUGIN_TYPE GetType()
{
	return Castor3D::ePLUGIN_TYPE_TECHNIQUE;
}

C3D_SsaaTechnique_API Castor::String GetName()
{
	return cuT( "SSAA Render Technique" );
}

C3D_SsaaTechnique_API void OnLoad( Castor3D::Engine * p_engine )
{
	p_engine->GetRenderTechniqueManager().GetTechniqueFactory().Register( NAME, &RenderTechnique::CreateInstance );
}

C3D_SsaaTechnique_API void OnUnload( Castor3D::Engine * p_engine )
{
	p_engine->GetRenderTechniqueManager().GetTechniqueFactory().Unregister( NAME );
}
