#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/DirectionalLight.hpp"
#include "Castor3D/RenderSystem.hpp"
#include "Castor3D/Root.hpp"
#include "Castor3D/SceneFileParser.hpp"

using namespace Castor3D;

//*************************************************************************************************

bool Loader<DirectionalLight> :: Write( const DirectionalLight & p_light, File & p_file)
{
	bool l_bReturn = Loader<LightCategory>::Write( p_light, p_file);

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteLine( "\n}\n") > 0;
	}

	return l_bReturn;
}

//*************************************************************************************************

DirectionalLight :: DirectionalLight()
	:	LightCategory( eLIGHT_TYPE_DIRECTIONAL)
{
	SetDiffuse( Colour::FromRGBA( 0));
	GetPositionType()[3] = 1;
}

DirectionalLight :: ~DirectionalLight()
{
}

LightCategoryPtr DirectionalLight :: Clone()
{
	return LightCategoryPtr( new DirectionalLight);
}

void DirectionalLight :: Render( LightRendererPtr p_pRenderer, ePRIMITIVE_TYPE p_displayMode)
{
	p_pRenderer->Enable();
	p_pRenderer->ApplyPosition();
	p_pRenderer->ApplyAmbient();
	p_pRenderer->ApplyDiffuse();
	p_pRenderer->ApplySpecular();
}

//*************************************************************************************************