#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/light/DirectionalLight.h"
#include "Castor3D/render_system/RenderSystem.h"
#include "Castor3D/main/Root.h"



using namespace Castor3D;

DirectionalLight :: DirectionalLight( SceneNodePtr p_pNode, const String & p_name)
	:	Light( p_pNode, p_name)
{
	m_diffuse = Colour( 0.0, 0.0, 0.0, 0.0);
	m_ptPositionType[3] = 1;
}

DirectionalLight :: ~DirectionalLight()
{
}

void DirectionalLight :: _initialise()
{
	Light::_initialise();
}

bool DirectionalLight :: Write( File & p_pFile)const
{
	Logger::LogMessage( CU_T( "Writing Light ") + m_name);
	bool l_bReturn = p_pFile.WriteLine( "light " + m_name + "\n{\n");

	if (l_bReturn)
	{
		l_bReturn = p_pFile.WriteLine( "\ttype directional\n");
	}

	if (l_bReturn)
	{
		l_bReturn = Light::Write( p_pFile);
	}

	if (l_bReturn)
	{
		l_bReturn = p_pFile.Print( 256, "\tambient %f %f %f %f\n", m_ambient[0], m_ambient[1], m_ambient[2], m_ambient[3]);
	}

	if (l_bReturn)
	{
		l_bReturn = p_pFile.WriteLine( "}\n");
	}

	return l_bReturn;
}