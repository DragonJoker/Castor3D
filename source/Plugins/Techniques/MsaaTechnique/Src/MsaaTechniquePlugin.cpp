#include "MsaaRenderTechnique.hpp"

#include <Engine.hpp>
#include <TechniqueManager.hpp>
#include <Version.hpp>

#include <Logger.hpp>

using namespace Msaa;
static const Castor::String NAME = cuT( "msaa" );

C3D_MsaaTechnique_API void GetRequiredVersion( Castor3D::Version & p_version )
{
	p_version = Castor3D::Version();
}

C3D_MsaaTechnique_API Castor3D::ePLUGIN_TYPE GetType()
{
	return Castor3D::ePLUGIN_TYPE_TECHNIQUE;
}

C3D_MsaaTechnique_API Castor::String GetName()
{
	return cuT( "MSAA Render Technique" );
}

C3D_MsaaTechnique_API void OnLoad( Castor3D::Engine * p_engine )
{
	p_engine->GetRenderTechniqueManager().GetTechniqueFactory().Register( NAME, &RenderTechnique::CreateInstance );
}

C3D_MsaaTechnique_API void OnUnload( Castor3D::Engine * p_engine )
{
	p_engine->GetRenderTechniqueManager().GetTechniqueFactory().Unregister( NAME );
}
