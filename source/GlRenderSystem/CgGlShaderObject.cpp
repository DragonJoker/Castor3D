#include "GlRenderSystem/PrecompiledHeader.hpp"

#include "GlRenderSystem/CgGlShaderObject.hpp"
#include "GlRenderSystem/CgGlShaderProgram.hpp"
#include "GlRenderSystem/GlRenderSystem.hpp"

using namespace Castor3D;

//*************************************************************************************************

CgGlShaderObject :: CgGlShaderObject( CgGlShaderProgram * p_pParent, eSHADER_TYPE p_eType)
	:	CgShaderObject( p_pParent, p_eType)
{
}

CgGlShaderObject :: ~CgGlShaderObject()
{
	if (m_cgProgram && cgGLIsProgramLoaded( m_cgProgram))
	{
		cgCheckError( cgGLUnloadProgram( m_cgProgram), cuT( "CgGlShaderObject :: ~GlShaderObject - cgGLUnloadProgram"));
		m_cgProgram = NULL;
	}

	DestroyProgram();
}

void CgGlShaderObject :: DestroyProgram()
{
	if (m_cgProfile != CG_PROFILE_UNKNOWN)
	{
		cgCheckError( cgGLDisableProfile( m_cgProfile), cuT( "CgGlShaderObject :: ~GlShaderObject - cgGLDisableProfile"));
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
			if ( ! m_strSource.empty())
			{
				if (m_cgProfile != CG_PROFILE_UNKNOWN)
				{
					m_bCompiled = false;

					GLint l_iCompiled = 0;
					GLint l_iLength = m_strSource.size();
					char const * l_pTmp = m_strSource.char_str();

					if (m_cgProgram && cgGLIsProgramLoaded( m_cgProgram))
					{
						cgCheckError( cgGLUnloadProgram( m_cgProgram), cuT( "CgGlShaderObject :: Compile - cgGLUnloadProgram"));
						m_cgProgram = NULL;
					}

					cgCheckError( cgGLSetOptimalOptions( m_cgProfile), cuT( "CgGlShaderObject :: Compile - cgGLSetOptimalOptions"));
					cgCheckError( m_cgProgram = cgCreateProgram( RenderSystem::GetCgContext(), CG_SOURCE, m_strSource.char_str(), m_cgProfile, m_strEntryPoint.char_str(), nullptr), cuT( "CgGlShaderObject :: Compile - cgCreateProgram"));

					if (m_cgProgram)
					{
						cgCheckError( cgGLLoadProgram( m_cgProgram), cuT( "CgGlShaderObject :: Compile - cgGLLoadProgram"));
						CgShaderObject::Compile();
						m_bCompiled = true;
					}
					else
					{
						m_bError = true;
					}

					l_bReturn = m_bCompiled;
				}
			}
		}
	}

	return l_bReturn;
}

void CgGlShaderObject :: Bind()
{
	if (m_cgProgram && cgGLIsProgramLoaded( m_cgProgram) && m_cgProfile != CG_PROFILE_UNKNOWN)
	{
		cgCheckError( cgGLEnableProfile( m_cgProfile), cuT( "CgGlShaderObject :: Bind - cgGLEnableProfile"));
		cgCheckError( cgGLBindProgram( m_cgProgram), cuT( "CgGlShaderObject :: Compile - cgGLBindProgram"));
	}
}

void CgGlShaderObject :: Unbind()
{
	if (m_cgProfile != CG_PROFILE_UNKNOWN)
	{
		cgCheckError( cgGLDisableProfile( m_cgProfile), cuT( "CgGlShaderObject :: Bind - cgGLEnableProfile"));
	}
}

void CgGlShaderObject :: CreateProgram()
{
	if (RenderSystem::UseShaders() && m_cgProfile == CG_PROFILE_UNKNOWN)
	{
		m_cgProfile = RenderSystem::GetCgProfile( m_eType);
		CASTOR_ASSERT( m_cgProfile != CG_PROFILE_UNKNOWN);
	}
}

//*************************************************************************************************
