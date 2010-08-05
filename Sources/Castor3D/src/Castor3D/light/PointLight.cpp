#include "PrecompiledHeader.h"

#include "light/Module_Light.h"

#include "light/PointLight.h"

#include "scene/SceneNode.h"
#include "render_system/RenderSystem.h"
#include "render_system/LightRenderer.h"
#include "main/Root.h"

using namespace Castor3D;

PointLight :: PointLight( LightRenderer * p_renderer, const String & p_name)
	:	Light( p_renderer, p_name),
		m_attenuation( 1.0f, 0.2f, 0.0f)
{
	p_renderer->SetTarget( this);
}

PointLight :: ~PointLight()
{
}

void PointLight :: _initialise()
{
	Light::_initialise();
	m_renderer->ApplyConstantAtt( m_attenuation.x);
	m_renderer->ApplyLinearAtt( m_attenuation.y);
	m_renderer->ApplyQuadraticAtt( m_attenuation.z);
}

void PointLight :: SetAttenuation( float * p_attenuation)
{
	m_attenuation.x = p_attenuation[0];
	m_attenuation.y = p_attenuation[1];
	m_attenuation.z = p_attenuation[2];
	m_renderer->ApplyConstantAtt( m_attenuation.x);
	m_renderer->ApplyLinearAtt( m_attenuation.y);
	m_renderer->ApplyQuadraticAtt( m_attenuation.z);
}

void PointLight :: SetAttenuation( float p_const, float p_linear,
								   float p_quadratic)
{
	m_attenuation.x = p_const;
	m_attenuation.y = p_linear;
	m_attenuation.z = p_quadratic;
	m_renderer->ApplyConstantAtt( m_attenuation.x);
	m_renderer->ApplyLinearAtt( m_attenuation.y);
	m_renderer->ApplyQuadraticAtt( m_attenuation.z);
}

void PointLight :: SetAttenuation( const Vector3f & p_attenuation)
{
	m_attenuation.x = p_attenuation.x;
	m_attenuation.y = p_attenuation.y;
	m_attenuation.z = p_attenuation.z;
	m_renderer->ApplyConstantAtt( m_attenuation.x);
	m_renderer->ApplyLinearAtt( m_attenuation.y);
	m_renderer->ApplyQuadraticAtt( m_attenuation.z);
}

bool PointLight :: Write( General::Utils::File & p_file)const
{
	bool l_bReturn = Light::Write( p_file);

	if (l_bReturn)
	{
		l_bReturn = (p_file.WriteArray<float>( m_attenuation.const_ptr(), 3) == sizeof( float) * 3);
	}

	return l_bReturn;
}

bool PointLight :: Read( General::Utils::File & p_file, Scene * p_scene)
{
	bool l_bReturn = Light::Read( p_file, p_scene);

	if (l_bReturn)
	{
		l_bReturn = (p_file.ReadArray<float>( m_attenuation.ptr(), 3) == sizeof( float) * 3);
	}

	SetEnabled( m_enabled);

	return l_bReturn;
}