#include "Dx9RenderSystem/PrecompiledHeader.hpp"

#include "Dx9RenderSystem/CgDx9ShaderObject.hpp"
#include "Dx9RenderSystem/CgDx9ShaderProgram.hpp"
#include "Dx9RenderSystem/Dx9RenderSystem.hpp"

using namespace Castor3D;

//*************************************************************************************************

CgDx9ShaderObject :: CgDx9ShaderObject( CgDx9ShaderProgram * p_pParent, eSHADER_TYPE p_eType)
	:	CgShaderObject( p_pParent, p_eType)
{
}

CgDx9ShaderObject :: ~CgDx9ShaderObject()
{
	if (m_cgProgram && cgD3D9IsProgramLoaded( m_cgProgram))
	{
		CheckCgError( "CgDx9ShaderObject :: ~Dx9ShaderObject - cgGLUnloadProgram");
		m_cgProgram = nullptr;
	}

	DestroyProgram();
}

void CgDx9ShaderObject :: DestroyProgram()
{
	if (m_cgProfile != CG_PROFILE_UNKNOWN)
	{
		cgD3D9SetDevice( Dx9RenderSystem::GetDevice());
		CheckCgError( "CgDx9ShaderObject :: DestroyProgram - cgD3D9SetDevice");
		cgD3D9UnloadProgram( m_cgProgram);
		CheckCgError( "CgDx9ShaderObject :: DestroyProgram - cgD3D9UnloadProgram");
	}
}

bool CgDx9ShaderObject :: Compile()
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

					size_t l_iLength = m_strSource.size();
					char const * l_pTmp = m_strSource.char_str();

					if (m_cgProgram && cgD3D9IsProgramLoaded( m_cgProgram))
					{
						cgD3D9UnloadProgram( m_cgProgram);
						CheckCgError( "CgDx9ShaderObject :: Compile - cgGLUnloadProgram");
						m_cgProgram = nullptr;
					}

					m_cgProgram = cgCreateProgram( RenderSystem::GetCgContext(), CG_SOURCE, m_strSource.char_str(), m_cgProfile, m_strEntryPoint.char_str(), nullptr);
					CheckCgError( "CgDx9ShaderObject :: Compile - cgCreateProgram");

					if (m_cgProgram)
					{
						cgD3D9SetDevice( Dx9RenderSystem::GetDevice());
						cgD3D9LoadProgram( m_cgProgram, false, 0);
						CheckCgError( "CgDx9ShaderObject :: Compile - cgGLLoadProgram");

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

void CgDx9ShaderObject :: Bind()
{
	if (m_cgProgram && cgD3D9IsProgramLoaded( m_cgProgram) && m_cgProfile != CG_PROFILE_UNKNOWN)
	{
		cgD3D9BindProgram( m_cgProgram);
		CheckCgError( "CgDx9ShaderObject :: Compile - cgD3D9BindProgram");
	}
}

void CgDx9ShaderObject :: Unbind()
{
}

void CgDx9ShaderObject :: CreateProgram()
{
	if (m_cgProfile == CG_PROFILE_UNKNOWN)
	{
		m_cgProfile = RenderSystem::GetCgProfile( m_eType);
	}
}

//*************************************************************************************************
