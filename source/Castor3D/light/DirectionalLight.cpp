#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/light/DirectionalLight.h"
#include "Castor3D/render_system/RenderSystem.h"
#include "Castor3D/main/Root.h"
#include "Castor3D/scene/SceneFileParser.h"

using namespace Castor3D;

DirectionalLight :: DirectionalLight( Scene * p_pScene, SceneNodePtr p_pNode, const String & p_name)
	:	Light( p_pScene, p_pNode, p_name, eDirectional)
{
	m_diffuse = Colour( 0.0f, 0.0f, 0.0f, 0.0f);
	m_ptPositionType[3] = 1;
}

DirectionalLight :: ~DirectionalLight()
{
}

void DirectionalLight :: _initialise()
{
	Light::_initialise();
}

bool DirectionalLight :: Write( File & p_file)const
{
	bool l_bReturn = Light::Write( p_file);

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteLine( "\n}\n") > 0;
	}

	return l_bReturn;
}

bool DirectionalLight :: Save( File & p_file)const
{
	bool l_bReturn = Light::Save( p_file);

	return l_bReturn;
}

bool DirectionalLight :: Load( File & p_file)
{
	bool l_bReturn = Light::Load( p_file);

	return l_bReturn;
}