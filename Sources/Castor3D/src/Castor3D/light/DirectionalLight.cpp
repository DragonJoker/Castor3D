#include "PrecompiledHeader.h"

#include "light/Module_Light.h"

#include "light/DirectionalLight.h"

#include "render_system/RenderSystem.h"
#include "render_system/LightRenderer.h"
#include "main/Root.h"

using namespace Castor3D;

DirectionalLight :: DirectionalLight( LightRenderer * p_renderer, const String & p_name)
	:	Light( p_renderer, p_name)
{
	p_renderer->SetTarget( this);
	m_diffuse = Point4D<float>( 0.0, 0.0, 0.0, 0.0);
	m_position = Point4D<float>( 0.0, 0.0, 1.0, 0.0);
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
		l_bReturn = (p_file.WriteArray<float>( m_ambient.const_ptr(), 4) > 0);
	}

	return l_bReturn;
}

bool DirectionalLight :: Read( File & p_file, Scene * p_scene)
{
	bool l_bReturn = Light::Read( p_file, p_scene);

	if (l_bReturn)
	{
		l_bReturn = (p_file.ReadArray<float>( m_ambient.ptr(), 4) > 0);
	}

	SetEnabled( m_enabled);

	return l_bReturn;
}