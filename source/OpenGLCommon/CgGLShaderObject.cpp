#include "OpenGLCommon/PrecompiledHeader.h"

#include "OpenGLCommon/CgGLShaderObject.h"
#include "OpenGLCommon/CgGLShaderProgram.h"
#include "OpenGLCommon/GLRenderSystem.h"

using namespace Castor3D;

//*************************************************************************************************

CgGLShaderObject :: CgGLShaderObject()
	:	CgShaderObject()
{
}

CgGLShaderObject :: ~CgGLShaderObject()
{
	if (m_cgProgram != NULL && cgGLIsProgramLoaded( m_cgProgram))
	{
		cgGLUnloadProgram( m_cgProgram);
		CheckCgError( "CgGLShaderObject :: ~GLShaderObject - cgGLUnloadProgram");
		m_cgProgram = NULL;
	}

	DestroyProgram();
}

void CgGLShaderObject :: DestroyProgram()
{
	if (m_cgProfile != CG_PROFILE_UNKNOWN)
	{
		cgGLDisableProfile( m_cgProfile);
		CheckCgError( "CgGLShaderObject :: ~GLShaderObject - cgGLDisableProfile");
		m_cgProfile = CG_PROFILE_UNKNOWN;
	}
}

bool CgGLShaderObject :: Compile()
{
	bool l_bReturn = false;

	if (RenderSystem::UseShaders())
	{
		if ( ! m_bError)
		{
			if ( ! m_shaderSource.empty())
			{
				if (m_cgProfile != CG_PROFILE_UNKNOWN)
				{
					m_isCompiled = false;

					GLint l_iCompiled = 0;
					GLint l_iLength = m_shaderSource.size();
					const char * l_pTmp = m_shaderSource.char_str();

					if (m_cgProgram != NULL && cgGLIsProgramLoaded( m_cgProgram))
					{
						cgGLUnloadProgram( m_cgProgram);
						CheckCgError( "CgGLShaderObject :: Compile - cgGLUnloadProgram");
						m_cgProgram = NULL;
					}

					cgGLSetOptimalOptions( m_cgProfile);
					CheckCgError( "CgGLShaderObject :: Compile - cgGLSetOptimalOptions");
					m_cgProgram = cgCreateProgram( static_cast <CgShaderProgram *>(m_pParent)->GetContext(),
												   CG_SOURCE, m_shaderSource.c_str(), m_cgProfile, NULL, NULL);
					CheckCgError( "CgGLShaderObject :: Compile - cgCreateProgram");

					if (m_cgProgram != NULL)
					{
						cgGLLoadProgram( m_cgProgram);
						CheckCgError( "CgGLShaderObject :: Compile - cgGLLoadProgram");

						CgShaderObject::Compile();

						m_isCompiled = true;
					}
					else
					{
						m_bError = true;
					}

					l_bReturn = m_isCompiled;
				}
			}
		}
	}

	return l_bReturn;
}

void CgGLShaderObject :: Bind()
{
	if (m_cgProgram != NULL && cgGLIsProgramLoaded( m_cgProgram) && m_cgProfile != CG_PROFILE_UNKNOWN)
	{
		cgGLBindProgram( m_cgProgram);
		CheckCgError( "CgGLShaderObject :: Compile - cgGLBindProgram");
		cgGLEnableProfile( m_cgProfile);
		CheckCgError( "CgGLShaderObject :: Bind - cgGLEnableProfile");
	}
}

void CgGLShaderObject :: Unbind()
{
	if (m_cgProfile != CG_PROFILE_UNKNOWN)
	{
		cgGLDisableProfile( m_cgProfile);
		CheckCgError( "CgGLShaderObject :: Bind - cgGLEnableProfile");
	}
}

//*************************************************************************************************

CgGLVertexShader :: CgGLVertexShader()
	:	CgGLShaderObject()
{
	m_programType = eVertexShader; 
}

CgGLVertexShader :: ~CgGLVertexShader()
{
}

void CgGLVertexShader :: CreateProgram()
{
	if (RenderSystem::UseShaders() && m_cgProfile == CG_PROFILE_UNKNOWN)
	{
		m_cgProfile = cgGLGetLatestProfile( CG_GL_VERTEX);
		CheckCgError( CU_T( "CgGLVertexShader :: CreateProgram - cgGLGetLatestProfile"));
		CASTOR_ASSERT( m_cgProfile != CG_PROFILE_UNKNOWN);
	}
}

//*************************************************************************************************

CgGLFragmentShader :: CgGLFragmentShader()
	:	CgGLShaderObject()
{
	m_programType = ePixelShader;
}

CgGLFragmentShader :: ~CgGLFragmentShader()
{
}

void CgGLFragmentShader :: CreateProgram()
{
	if (RenderSystem::UseShaders() && m_cgProfile == CG_PROFILE_UNKNOWN)
	{
		m_cgProfile = cgGLGetLatestProfile( CG_GL_FRAGMENT);
		CheckCgError( CU_T( "CgGLFragmentShader :: CreateProgram - cgGLGetLatestProfile"));
		CASTOR_ASSERT( m_cgProfile != CG_PROFILE_UNKNOWN);
	}
}

//*************************************************************************************************

CgGLGeometryShader :: CgGLGeometryShader()
	:	CgGLShaderObject()
{
	m_programType = eGeometryShader; 
}

CgGLGeometryShader :: ~CgGLGeometryShader()
{
}

void CgGLGeometryShader :: CreateProgram()
{
	if (RenderSystem::UseShaders() && RenderSystem::HasGeometryShader() && m_cgProfile == CG_PROFILE_UNKNOWN)
	{
		m_cgProfile = cgGLGetLatestProfile( CG_GL_GEOMETRY);
		CheckCgError( CU_T( "CgGLGeometryShader :: CreateProgram - cgGLGetLatestProfile"));
		CASTOR_ASSERT( m_cgProfile != CG_PROFILE_UNKNOWN);
	}
}

//*************************************************************************************************