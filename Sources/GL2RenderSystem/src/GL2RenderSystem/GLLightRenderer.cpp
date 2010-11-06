#include "PrecompiledHeader.h"

#include "GLLightRenderer.h"
#include "Module_GL.h"
#include "GLRenderSystem.h"
#include "GLSLProgram.h"
#include "GLUniformVariable.h"
#include "GLPipeline.h"

using namespace Castor3D;

void GLLightRenderer :: Enable()
{
	m_iIndex = ((GLRenderSystem *)GLRenderSystem::GetSingletonPtr())->LockLight();

	if (m_iIndex >= 0)
	{
		glEnable( m_iIndex);
		CheckGLError( "GLLightRenderer :: Enable - glEnable");
	}
}

void GLLightRenderer :: Disable()
{
	((GLRenderSystem *)GLRenderSystem::GetSingletonPtr())->UnlockLight( m_iIndex);
	if (m_iIndex >= 0)
	{
		glDisable( m_iIndex);
		m_iIndex = -1;
		CheckGLError( "GLLightRenderer :: Disable - glDisable");
	}
}

void GLLightRenderer :: ApplyAmbient( float * p_ambient)
{
	if (m_iIndex >= 0)
	{
		glLightfv( m_iIndex, GL_AMBIENT, p_ambient);
		CheckGLError( "GLLightRenderer :: ApplyAmbient - glLightfv");
	}
}

void GLLightRenderer :: ApplyDiffuse( float * p_diffuse)
{
	if (m_iIndex >= 0)
	{
		glLightfv( m_iIndex, GL_DIFFUSE, p_diffuse);
		CheckGLError( "GLLightRenderer :: ApplyDiffuse - glLightfv");
	}
}

void GLLightRenderer :: ApplySpecular( float * p_specular)
{
	if (m_iIndex >= 0)
	{
		glLightfv( m_iIndex, GL_SPECULAR, p_specular);
		CheckGLError( "GLLightRenderer :: ApplySpecular - glLightfv");
	}
}

void GLLightRenderer :: ApplyExponent( float p_exponent)
{
	if (m_iIndex >= 0)
	{
		glLightf( m_iIndex, GL_SPOT_EXPONENT, p_exponent);
		CheckGLError( "GLLightRenderer :: ApplyExponent - glLightfv");
	}
}

void GLLightRenderer :: ApplyCutOff( float p_cutOff)
{
	if (m_iIndex >= 0)
	{
		glLightf( m_iIndex, GL_SPOT_CUTOFF, p_cutOff);
	}
}

void GLLightRenderer :: ApplyPosition( float * p_position)
{
	if (m_iIndex >= 0)
	{
		glLightfv( m_iIndex, GL_POSITION, p_position);
		CheckGLError( "GLLightRenderer :: ApplyPosition - glLightfv");
	}
}

void GLLightRenderer :: ApplyOrientation( real * p_matrix)
{
	if (m_iIndex >= 0)
	{
		Pipeline::PushMatrix();
		Pipeline::MultMatrix( p_matrix);
		Pipeline::PopMatrix();
		CheckGLError( "GLLightRenderer :: ApplyOrientation");
	}
}

void GLLightRenderer :: ApplyConstantAtt( float p_constant)
{
	if (m_iIndex >= 0)
	{
		glLightf( m_iIndex, GL_CONSTANT_ATTENUATION, p_constant);
		CheckGLError( "GLLightRenderer :: ApplyConstantAtt");
	}
}

void GLLightRenderer :: ApplyLinearAtt( float p_linear)
{
	if (m_iIndex >= 0)
	{
		glLightf( m_iIndex, GL_LINEAR_ATTENUATION, p_linear);
		CheckGLError( "GLLightRenderer :: ApplyLinearAtt");
	}
}

void GLLightRenderer :: ApplyQuadraticAtt( float p_quadratic)
{
	if (m_iIndex >= 0)
	{
		glLightf( m_iIndex, GL_QUADRATIC_ATTENUATION, p_quadratic);
		CheckGLError( "GLLightRenderer :: ApplyQuadraticAtt");
	}
}