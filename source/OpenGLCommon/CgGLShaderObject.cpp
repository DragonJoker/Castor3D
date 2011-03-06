#include "OpenGlCommon/PrecompiledHeader.h"

#include "OpenGlCommon/CgGlShaderObject.h"
#include "OpenGlCommon/CgGlShaderProgram.h"
#include "OpenGlCommon/GlRenderSystem.h"

using namespace Castor3D;

//*************************************************************************************************

CgGlShaderObject :: CgGlShaderObject( eSHADER_PROGRAM_TYPE p_eType)
	:	CgShaderObject( p_eType)
{
}

CgGlShaderObject :: ~CgGlShaderObject()
{
	if (m_cgProgram != NULL && cgGLIsProgramLoaded( m_cgProgram))
	{
		CheckCgError( cgGLUnloadProgram( m_cgProgram), "CgGlShaderObject :: ~GlShaderObject - cgGLUnloadProgram");
		m_cgProgram = NULL;
	}

	DestroyProgram();
}

void CgGlShaderObject :: DestroyProgram()
{
	if (m_cgProfile != CG_PROFILE_UNKNOWN)
	{
		CheckCgError( cgGLDisableProfile( m_cgProfile), "CgGlShaderObject :: ~GlShaderObject - cgGLDisableProfile");
		m_cgProfile = CG_PROFILE_UNKNOWN;
	}
}

bool CgGlShaderObject :: Compile()
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
						CheckCgError( cgGLUnloadProgram( m_cgProgram), "CgGlShaderObject :: Compile - cgGLUnloadProgram");
						m_cgProgram = NULL;
					}

					CheckCgError( cgGLSetOptimalOptions( m_cgProfile), "CgGlShaderObject :: Compile - cgGLSetOptimalOptions");
					CheckCgError( m_cgProgram = cgCreateProgram( static_cast <CgShaderProgram *>(m_pParent)->GetContext(),
												   CG_SOURCE, m_shaderSource.c_str(), m_cgProfile, NULL, NULL),
								  "CgGlShaderObject :: Compile - cgCreateProgram");

					if (m_cgProgram != NULL)
					{
						CheckCgError( cgGLLoadProgram( m_cgProgram), "CgGlShaderObject :: Compile - cgGLLoadProgram");

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

void CgGlShaderObject :: Bind()
{
	if (m_cgProgram != NULL && cgGLIsProgramLoaded( m_cgProgram) && m_cgProfile != CG_PROFILE_UNKNOWN)
	{
		CheckCgError( cgGLBindProgram( m_cgProgram), "CgGlShaderObject :: Compile - cgGLBindProgram");
		CheckCgError( cgGLEnableProfile( m_cgProfile), "CgGlShaderObject :: Bind - cgGLEnableProfile");
	}
}

void CgGlShaderObject :: Unbind()
{
	if (m_cgProfile != CG_PROFILE_UNKNOWN)
	{
		CheckCgError( cgGLDisableProfile( m_cgProfile), "CgGlShaderObject :: Bind - cgGLEnableProfile");
	}
}

//*************************************************************************************************

CgGlVertexShader :: CgGlVertexShader()
	:	CgGlShaderObject( eVertexShader)
{
}

CgGlVertexShader :: ~CgGlVertexShader()
{
}

void CgGlVertexShader :: CreateProgram()
{
	if (RenderSystem::UseShaders() && m_cgProfile == CG_PROFILE_UNKNOWN)
	{
		CheckCgError( m_cgProfile = cgGLGetLatestProfile( CG_GL_VERTEX), CU_T( "CgGlVertexShader :: CreateProgram - cgGLGetLatestProfile"));
		CASTOR_ASSERT( m_cgProfile != CG_PROFILE_UNKNOWN);
	}
}

//*************************************************************************************************

CgGlFragmentShader :: CgGlFragmentShader()
	:	CgGlShaderObject( ePixelShader)
{
}

CgGlFragmentShader :: ~CgGlFragmentShader()
{
}

void CgGlFragmentShader :: CreateProgram()
{
	if (RenderSystem::UseShaders() && m_cgProfile == CG_PROFILE_UNKNOWN)
	{
		CheckCgError( m_cgProfile = cgGLGetLatestProfile( CG_GL_FRAGMENT), CU_T( "CgGlFragmentShader :: CreateProgram - cgGLGetLatestProfile"));
		CASTOR_ASSERT( m_cgProfile != CG_PROFILE_UNKNOWN);
	}
}

//*************************************************************************************************

CgGlGeometryShader :: CgGlGeometryShader()
	:	CgGlShaderObject( eGeometryShader)
{
}

CgGlGeometryShader :: ~CgGlGeometryShader()
{
}

void CgGlGeometryShader :: CreateProgram()
{
	if (RenderSystem::UseShaders() && RenderSystem::HasGeometryShader() && m_cgProfile == CG_PROFILE_UNKNOWN)
	{
		CheckCgError( m_cgProfile = cgGLGetLatestProfile( CG_GL_GEOMETRY), CU_T( "CgGlGeometryShader :: CreateProgram - cgGLGetLatestProfile"));
		CASTOR_ASSERT( m_cgProfile != CG_PROFILE_UNKNOWN);
	}
}

//*************************************************************************************************