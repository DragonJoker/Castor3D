#include "PrecompiledHeader.h"

#include "light/Module_Light.h"

#include "light/SpotLight.h"

#include "render_system/RenderSystem.h"
#include "render_system/LightRenderer.h"
#include "main/Root.h"

using namespace Castor3D;

SpotLight :: SpotLight( LightRenderer * p_renderer, const String & p_name)
	:	Light( p_renderer, p_name),
		m_matrix( new float[16]),
		m_attenuation( 1, 0, 0)
{
	p_renderer->SetTarget( this);

	m_exponent = 0.0;
	m_cutOff = 180.0;
}

SpotLight :: ~SpotLight()
{
	delete m_matrix;
}

void SpotLight :: Render()
{
	Light::Render();
	m_orientation.ToRotationMatrix( m_matrix);
	m_renderer->ApplyOrientation( m_matrix);
}

void SpotLight :: _initialise()
{
	Light::_initialise();
	m_renderer->ApplyConstantAtt( m_attenuation.x);
	m_renderer->ApplyLinearAtt( m_attenuation.y);
	m_renderer->ApplyQuadraticAtt( m_attenuation.z);
	m_renderer->ApplyExponent( m_exponent);
	m_renderer->ApplyCutOff( m_cutOff);
}

void SpotLight :: Yaw( float p_angle)
{
	Quaternion l_tmp( Vector3f( 0.0, 1.0, 0.0), p_angle);
	m_orientation *= l_tmp;
}

void SpotLight :: Pitch( float p_angle)
{
	Quaternion l_tmp( Vector3f( 1.0, 0.0, 0.0), p_angle);
	m_orientation *= l_tmp;
}

void SpotLight :: Roll( float p_angle)
{
	Quaternion l_tmp( Vector3f( 0.0, 0.0, 1.0), p_angle);
	m_orientation *= l_tmp;
}

void SpotLight :: Rotate( const Quaternion & p_quat)
{
	m_orientation *= p_quat;
}

void SpotLight :: SetAttenuation( float * p_attenuation)
{
	m_attenuation.x = p_attenuation[0];
	m_attenuation.y = p_attenuation[1];
	m_attenuation.z = p_attenuation[2];
	m_renderer->ApplyConstantAtt( m_attenuation.x);
	m_renderer->ApplyLinearAtt( m_attenuation.y);
	m_renderer->ApplyQuadraticAtt( m_attenuation.z);
}

void SpotLight :: SetAttenuation( float p_const, float p_linear,
								  float p_quadratic)
{
	m_attenuation.x = p_const;
	m_attenuation.y = p_linear;
	m_attenuation.z = p_quadratic;
	m_renderer->ApplyConstantAtt( m_attenuation.x);
	m_renderer->ApplyLinearAtt( m_attenuation.y);
	m_renderer->ApplyQuadraticAtt( m_attenuation.z);
}

void SpotLight :: SetAttenuation( const Vector3f & p_attenuation)
{
	m_attenuation.x = p_attenuation.x;
	m_attenuation.y = p_attenuation.y;
	m_attenuation.z = p_attenuation.z;
	m_renderer->ApplyConstantAtt( m_attenuation.x);
	m_renderer->ApplyLinearAtt( m_attenuation.y);
	m_renderer->ApplyQuadraticAtt( m_attenuation.z);
}

void SpotLight :: SetExponent( float p_exponent)
{
	m_exponent = p_exponent;
	m_renderer->ApplyExponent( m_exponent);
}

void SpotLight :: SetCutOff( float p_cutOff)
{
	m_cutOff = p_cutOff;
	m_renderer->ApplyCutOff( m_cutOff); 
}

float * SpotLight :: Get4x4RotationMatrix()
{
	m_orientation.ToRotationMatrix( m_matrix);
	return m_matrix;
}

bool SpotLight :: Write( General::Utils::File & p_file)const
{
	bool l_bReturn = Light::Write( p_file);

	if (l_bReturn)
	{
		l_bReturn = (p_file.WriteArray<float>( m_attenuation.const_ptr(), 3) == sizeof( float) * 3);
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.Write<float>( m_exponent) == sizeof( float));
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.Write<float>( m_cutOff) == sizeof( float));
	}

	if (l_bReturn)
	{
		l_bReturn = m_orientation.Write( p_file);
	}

	return l_bReturn;
}

bool SpotLight :: Read( General::Utils::File & p_file, Scene * p_scene)
{
	bool l_bReturn = Light::Read( p_file, p_scene);

	if (l_bReturn)
	{
		l_bReturn = (p_file.ReadArray<float>( m_attenuation.ptr(), 3) == sizeof( float) * 3);
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.Read<float>( m_exponent) == sizeof( float));
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.Read<float>( m_cutOff) == sizeof( float));
	}

	if (l_bReturn)
	{
		l_bReturn = m_orientation.Read( p_file);
	}

	SetEnabled( m_enabled);

	return l_bReturn;
}