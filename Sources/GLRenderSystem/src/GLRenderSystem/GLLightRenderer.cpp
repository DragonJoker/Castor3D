//******************************************************************************
#include "PrecompiledHeader.h"

#include "../../include/GLRenderSystem/GLLightRenderer.h"
#include "../../include/GLRenderSystem/Module_GL.h"
#include "../../include/GLRenderSystem/GLRenderSystem.h"
#include "../../include/GLRenderSystem/GLSLProgram.h"
#include "../../include/GLRenderSystem/GLUniformVariable.h"
//******************************************************************************
using namespace Castor3D;

String GLLightRenderer::sm_vertexShader = 
"varying vec3 N;\
varying vec3 v;\
void main(void)\
{\
	v = vec3(gl_ModelViewMatrix * gl_Vertex);\
	N = normalize(gl_NormalMatrix * gl_Normal);\
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\
}";

String GLLightRenderer::sm_fragmentShader = 
"varying vec3 N;\
varying vec3 v;\
uniform vec3 vPosition;\
uniform vec4 vAmbient;\
uniform vec4 vDiffuse;\
uniform vec4 vSpecular;\
uniform float vExponent;\
void main (void)\
{\
   vec3 L = normalize( vPosition.xyz - v);\
   vec3 E = normalize( -v);\
   vec3 R = normalize( -reflect( L, N));\
   vec4 Iamb = vAmbient;\
   vec4 Idiff = vDiffuse * max( dot( N, L), 0.0);\
   vec4 Ispec = vSpecular * pow( max( dot( R, E), 0.0), 0.3 * vExponent);\
   gl_FragColor = Iamb + Idiff + Ispec;\
}";

std::vector <GLShaderProgram *> GLLightRenderer::sm_pLightShaders;
//******************************************************************************

void GLLightRenderer :: Enable()
{
	m_iIndex = ((GLRenderSystem *)GLRenderSystem::GetSingletonPtr())->LockLight();

	if (m_iIndex >= 0)
	{
//		sm_pLightShaders[m_iIndex - GL_LIGHT0]->Initialise();
//		sm_pLightShaders[m_iIndex - GL_LIGHT0]->Begin();

		glEnable( m_iIndex);
		CheckGLError( "GLLightRenderer :: Enable - glEnable");
	}
}

//******************************************************************************

void GLLightRenderer :: Disable()
{
	((GLRenderSystem *)GLRenderSystem::GetSingletonPtr())->UnlockLight( m_iIndex);
	if (m_iIndex >= 0)
	{
//		sm_pLightShaders[m_iIndex - GL_LIGHT0]->End();
		glDisable( m_iIndex);
		m_iIndex = -1;
		CheckGLError( "GLLightRenderer :: Disable - glDisable");
	}
}

//******************************************************************************

void GLLightRenderer :: ApplyAmbient( float * p_ambient)
{
	if (m_iIndex >= 0)
	{
		glLightfv( m_iIndex, GL_AMBIENT, p_ambient);
//		sm_pLightShaders[m_iIndex - GL_LIGHT0]->GetGLUniformVariables().find( "vAmbient")->second->Apply();
		CheckGLError( "GLLightRenderer :: ApplyAmbient - glLightfv");
	}
}

//******************************************************************************

void GLLightRenderer :: ApplyDiffuse( float * p_diffuse)
{
	if (m_iIndex >= 0)
	{
		glLightfv( m_iIndex, GL_DIFFUSE, p_diffuse);
//		sm_pLightShaders[m_iIndex - GL_LIGHT0]->GetGLUniformVariables().find( "vDiffuse")->second->Apply();
		CheckGLError( "GLLightRenderer :: ApplyDiffuse - glLightfv");
	}
}

//******************************************************************************

void GLLightRenderer :: ApplySpecular( float * p_specular)
{
	if (m_iIndex >= 0)
	{
		glLightfv( m_iIndex, GL_SPECULAR, p_specular);
//		sm_pLightShaders[m_iIndex - GL_LIGHT0]->GetGLUniformVariables().find( "vSpecular")->second->Apply();
		CheckGLError( "GLLightRenderer :: ApplySpecular - glLightfv");
	}
}

//******************************************************************************

void GLLightRenderer :: ApplyExponent( float p_exponent)
{
	if (m_iIndex >= 0)
	{
		glLightf( m_iIndex, GL_SPOT_EXPONENT, p_exponent);
//		sm_pLightShaders[m_iIndex - GL_LIGHT0]->GetGLUniformVariables().find( "vExponent")->second->Apply();
		CheckGLError( "GLLightRenderer :: ApplyExponent - glLightfv");
	}
}

//******************************************************************************

void GLLightRenderer :: ApplyCutOff( float p_cutOff)
{
	if (m_iIndex >= 0)
	{
		glLightf( m_iIndex, GL_SPOT_CUTOFF, p_cutOff);
//		CheckGLError( "GLLightRenderer :: ApplyCutOff - glLightf");
	}
}

//******************************************************************************

void GLLightRenderer :: ApplyPosition( float * p_position)
{
	if (m_iIndex >= 0)
	{
		glLightfv( m_iIndex, GL_POSITION, p_position);
//		sm_pLightShaders[m_iIndex - GL_LIGHT0]->GetGLUniformVariables().find( "vPosition")->second->Apply();
		CheckGLError( "GLLightRenderer :: ApplyPosition - glLightfv");
	}
}

//******************************************************************************

void GLLightRenderer :: ApplyOrientation( float * p_matrix)
{
	if (m_iIndex >= 0)
	{
		glPushMatrix();
		glMultMatrixf( p_matrix);
		glPopMatrix();
		CheckGLError( "GLLightRenderer :: ApplyOrientation");
	}
}

//******************************************************************************

void GLLightRenderer :: ApplyConstantAtt( float p_constant)
{
	if (m_iIndex >= 0)
	{
		glLightf( m_iIndex, GL_CONSTANT_ATTENUATION, p_constant);
		CheckGLError( "GLLightRenderer :: ApplyConstantAtt");
	}
}

//******************************************************************************

void GLLightRenderer :: ApplyLinearAtt( float p_linear)
{
	if (m_iIndex >= 0)
	{
		glLightf( m_iIndex, GL_LINEAR_ATTENUATION, p_linear);
		CheckGLError( "GLLightRenderer :: ApplyLinearAtt");
	}
}

//******************************************************************************

void GLLightRenderer :: ApplyQuadraticAtt( float p_quadratic)
{
	if (m_iIndex >= 0)
	{
		glLightf( m_iIndex, GL_QUADRATIC_ATTENUATION, p_quadratic);
		CheckGLError( "GLLightRenderer :: ApplyQuadraticAtt");
	}
}

//******************************************************************************