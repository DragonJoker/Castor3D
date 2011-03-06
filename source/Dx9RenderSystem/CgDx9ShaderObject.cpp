#include "Dx9RenderSystem/PrecompiledHeader.h"

#include "Dx9RenderSystem/CgDx9ShaderObject.h"
#include "Dx9RenderSystem/CgDx9ShaderProgram.h"
#include "Dx9RenderSystem/Dx9RenderSystem.h"

using namespace Castor3D;

//*************************************************************************************************

CgDx9ShaderObject :: CgDx9ShaderObject( eSHADER_PROGRAM_TYPE p_eType)
	:	CgShaderObject( p_eType)
{
}

CgDx9ShaderObject :: ~CgDx9ShaderObject()
{
	if (m_cgProgram != NULL && cgD3D9IsProgramLoaded( m_cgProgram))
	{
		cgD3D9UnloadProgram( m_cgProgram);
		CheckCgError( "CgDx9ShaderObject :: ~Dx9ShaderObject - cgGLUnloadProgram");
		m_cgProgram = NULL;
	}

	DestroyProgram();
}

void CgDx9ShaderObject :: DestroyProgram()
{
	if (m_cgProfile != CG_PROFILE_UNKNOWN)
	{
//		cgGLDisableProfile( m_cgProfile);
		CheckCgError( "CgDx9ShaderObject :: ~Dx9ShaderObject - cgGLDisableProfile");
		m_cgProfile = CG_PROFILE_UNKNOWN;
	}
}

bool CgDx9ShaderObject :: Compile()
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

					size_t l_iLength = m_shaderSource.size();
					const char * l_pTmp = m_shaderSource.char_str();

					if (m_cgProgram != NULL && cgD3D9IsProgramLoaded( m_cgProgram))
					{
						cgD3D9UnloadProgram( m_cgProgram);
						CheckCgError( "CgDx9ShaderObject :: Compile - cgGLUnloadProgram");
						m_cgProgram = NULL;
					}

//					cgGLSetOptimalOptions( m_cgProfile);
					CheckCgError( "CgDx9ShaderObject :: Compile - cgGLSetOptimalOptions");
					m_cgProgram = cgCreateProgram( static_cast <CgShaderProgram *>(m_pParent)->GetContext(),
												   CG_SOURCE, m_shaderSource.c_str(), m_cgProfile, NULL, NULL);
					CheckCgError( "CgDx9ShaderObject :: Compile - cgCreateProgram");

					if (m_cgProgram != NULL)
					{
						cgD3D9LoadProgram( m_cgProgram, false, 0);
						CheckCgError( "CgDx9ShaderObject :: Compile - cgGLLoadProgram");

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

void CgDx9ShaderObject :: Bind()
{
	if (m_cgProgram != NULL && cgD3D9IsProgramLoaded( m_cgProgram) && m_cgProfile != CG_PROFILE_UNKNOWN)
	{
		cgD3D9BindProgram( m_cgProgram);
		CheckCgError( "CgDx9ShaderObject :: Compile - cgGLBindProgram");
//		cgD3D9EnableProfile( m_cgProfile);
		CheckCgError( "CgDx9ShaderObject :: Bind - cgGLEnableProfile");
	}
}

void CgDx9ShaderObject :: Unbind()
{
	if (m_cgProfile != CG_PROFILE_UNKNOWN)
	{
//		cgD3D9DisableProfile( m_cgProfile);
		CheckCgError( "CgDx9ShaderObject :: Bind - cgGLEnableProfile");
	}
}

//*************************************************************************************************

CgDx9VertexShader :: CgDx9VertexShader()
	:	CgDx9ShaderObject( eVertexShader)
{
}

CgDx9VertexShader :: ~CgDx9VertexShader()
{
}

void CgDx9VertexShader :: CreateProgram()
{
	if (RenderSystem::UseShaders() && m_cgProfile == CG_PROFILE_UNKNOWN)
	{
//		m_cgProfile = cgGLGetLatestProfile( CG_GL_VERTEX);
		CheckCgError( CU_T( "CgDx9VertexShader :: CreateProgram - cgGLGetLatestProfile"));
		CASTOR_ASSERT( m_cgProfile != CG_PROFILE_UNKNOWN);
	}
}

//*************************************************************************************************

CgDx9FragmentShader :: CgDx9FragmentShader()
	:	CgDx9ShaderObject( ePixelShader)
{
}

CgDx9FragmentShader :: ~CgDx9FragmentShader()
{
}

void CgDx9FragmentShader :: CreateProgram()
{
	if (RenderSystem::UseShaders() && m_cgProfile == CG_PROFILE_UNKNOWN)
	{
//		m_cgProfile = cgGLGetLatestProfile( CG_GL_FRAGMENT);
		CheckCgError( CU_T( "CgDx9FragmentShader :: CreateProgram - cgGLGetLatestProfile"));
		CASTOR_ASSERT( m_cgProfile != CG_PROFILE_UNKNOWN);
	}
}

//*************************************************************************************************

CgDx9GeometryShader :: CgDx9GeometryShader()
	:	CgDx9ShaderObject( eGeometryShader)
{
}

CgDx9GeometryShader :: ~CgDx9GeometryShader()
{
}

void CgDx9GeometryShader :: CreateProgram()
{
	if (RenderSystem::UseShaders() && RenderSystem::HasGeometryShader() && m_cgProfile == CG_PROFILE_UNKNOWN)
	{
//		m_cgProfile = cgGLGetLatestProfile( CG_GL_GEOMETRY);
		CheckCgError( CU_T( "CgDx9GeometryShader :: CreateProgram - cgGLGetLatestProfile"));
		CASTOR_ASSERT( m_cgProfile != CG_PROFILE_UNKNOWN);
	}
}

//*************************************************************************************************