#include "PrecompiledHeader.h"

#include "GLLightRenderer.h"
#include "Module_GL.h"
#include "GLRenderSystem.h"
#include "GLSLProgram.h"
#include "GLUniformVariable.h"
#include "GLBuffer.h"

using namespace Castor3D;

size_t GLLightRenderer :: sm_uiNumLights = 0;

GLLightRenderer :: GLLightRenderer()
	:	m_iIndex( sm_uiNumLights++),
		m_bInitialised( false)
{
}

void GLLightRenderer :: SetTarget( target_ptr p_target)
{
	LightRenderer::SetTarget( p_target);
}

void GLLightRenderer :: Enable()
{
	m_iIndex = GLRenderSystem::GetSingletonPtr()->LockLight();
}

void GLLightRenderer :: Disable()
{
	GLRenderSystem::GetSingletonPtr()->UnlockLight( m_iIndex);
}

void GLLightRenderer :: ApplyAmbient( float * p_ambient)
{
	if (m_iIndex >= 0)
	{
		m_varAmbient = p_ambient;
	}
}

void GLLightRenderer :: ApplyDiffuse( float * p_diffuse)
{
	if (m_iIndex >= 0)
	{
		m_varDiffuse = p_diffuse;
	}
}

void GLLightRenderer :: ApplySpecular( float * p_specular)
{
	if (m_iIndex >= 0)
	{
		m_varSpecular = p_specular;
	}
}

void GLLightRenderer :: ApplyExponent( float p_exponent)
{
	if (m_iIndex >= 0)
	{
		m_varExponent = p_exponent;
	}
}

void GLLightRenderer :: ApplyCutOff( float p_cutOff)
{
	if (m_iIndex >= 0)
	{
		m_varCutOff = p_cutOff;
	}
}

void GLLightRenderer :: ApplyPosition( float * p_position)
{
	if (m_iIndex >= 0)
	{
		m_varPosition = p_position;
	}
}

void GLLightRenderer :: ApplyOrientation( real * p_matrix)
{
	if (m_iIndex >= 0)
	{
		m_varOrientation = Matrix4x4r( p_matrix);
	}
}

void GLLightRenderer :: ApplyConstantAtt( float p_constant)
{
	if (m_iIndex >= 0)
	{
		m_varAttenuation[0] = p_constant;
	}
}

void GLLightRenderer :: ApplyLinearAtt( float p_linear)
{
	if (m_iIndex >= 0)
	{
		m_varAttenuation[1] = p_linear;
	}
}

void GLLightRenderer :: ApplyQuadraticAtt( float p_quadratic)
{
	if (m_iIndex >= 0)
	{
		m_varAttenuation[2] = p_quadratic;
	}
}