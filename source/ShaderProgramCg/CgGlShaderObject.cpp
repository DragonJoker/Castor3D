#include "ShaderProgramCg/PrecompiledHeader.hpp"

#if defined( Cg_OpenGL )
#include "ShaderProgramCg/CgGlShaderObject.hpp"
#include "ShaderProgramCg/CgGlShaderProgram.hpp"
#include "ShaderProgramCg/CgPipeline.hpp"

using namespace CgShader;
using namespace Castor3D;
using namespace Castor;

//*************************************************************************************************

CgGlShaderObject :: CgGlShaderObject( CgGlShaderProgram * p_pParent, eSHADER_TYPE p_eType)
	:	CgShaderObject	( p_pParent, p_eType	)
{
}

CgGlShaderObject :: ~CgGlShaderObject()
{
	if (m_cgProgram && cgGLIsProgramLoaded( m_cgProgram))
	{
		CheckCgError( cgGLUnloadProgram( m_cgProgram), cuT( "CgGlShaderObject :: ~GlShaderObject - cgGLUnloadProgram"));
		m_cgProgram = NULL;
	}

	DestroyProgram();
}

void CgGlShaderObject :: DestroyProgram()
{
	if (m_cgProfile != CG_PROFILE_UNKNOWN)
	{
		CheckCgError( cgGLDisableProfile( m_cgProfile), cuT( "CgGlShaderObject :: DestroyProgram - cgGLDisableProfile"));
		m_cgProfile = CG_PROFILE_UNKNOWN;
	}
}

bool CgGlShaderObject :: Compile()
{
	bool l_bReturn = false;
	String l_strSource;

	for( int i = eSHADER_MODEL_5; i >= eSHADER_MODEL_1 && l_strSource.empty(); i-- )
	{
		if( m_pParent->GetRenderSystem()->CheckSupport( eSHADER_MODEL( i ) ) )
		{
			l_strSource = m_arraySources[i];
		}
	}

	if (m_pParent->GetRenderSystem()->UseShaders())
	{
		if ( m_eStatus != eSHADER_STATUS_ERROR )
		{
			if ( !l_strSource.empty() )
			{
				if (m_cgProfile != CG_PROFILE_UNKNOWN)
				{
					try
					{
						m_eStatus = eSHADER_STATUS_NOTCOMPILED;

						if (m_cgProgram && cgGLIsProgramLoaded( m_cgProgram))
						{
							CheckCgError( cgGLUnloadProgram( m_cgProgram), cuT( "CgGlShaderObject :: Compile - cgGLUnloadProgram"));
							m_cgProgram = NULL;
						}

						CheckCgError( cgGLSetOptimalOptions( m_cgProfile), cuT( "CgGlShaderObject :: Compile - cgGLSetOptimalOptions"));
						CgPipeline * l_pPipeline = reinterpret_cast< CgPipeline* >( m_pParent->GetRenderSystem()->GetPipeline()->GetImpl() );
						CheckCgError( m_cgProgram = cgCreateProgram( l_pPipeline->GetCgContext(), CG_SOURCE, str_utils::to_str( l_strSource ).c_str(), m_cgProfile, str_utils::to_str( m_strEntryPoint ).c_str(), nullptr), cuT( "CgGlShaderObject :: Compile - cgCreateProgram"));

						if (m_cgProgram)
						{
							CheckCgError( cgGLLoadProgram( m_cgProgram), cuT( "CgGlShaderObject :: Compile - cgGLLoadProgram"));
							CgShaderObject::Compile();
							m_eStatus = eSHADER_STATUS_COMPILED;
						}
						else
						{
							m_eStatus = eSHADER_STATUS_ERROR;
						}

						l_bReturn = m_eStatus == eSHADER_STATUS_COMPILED;
					}
					catch( ... )
					{
						m_eStatus = eSHADER_STATUS_ERROR;
					}
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
		CheckCgError( cgGLEnableProfile( m_cgProfile), cuT( "CgGlShaderObject :: Bind - cgGLEnableProfile"));
		CheckCgError( cgGLBindProgram( m_cgProgram), cuT( "CgGlShaderObject :: Bind - cgGLBindProgram"));
	}
}

void CgGlShaderObject :: Unbind()
{
	if (m_cgProfile != CG_PROFILE_UNKNOWN)
	{
		CheckCgError( cgGLDisableProfile( m_cgProfile), cuT( "CgGlShaderObject :: Unbind - cgGLDisableProfile"));
	}
}

void CgGlShaderObject :: CreateProgram()
{
	if (m_pParent->GetRenderSystem()->UseShaders() && m_cgProfile == CG_PROFILE_UNKNOWN)
	{
		CgPipeline * l_pPipeline = reinterpret_cast< CgPipeline* >( m_pParent->GetRenderSystem()->GetPipeline()->GetImpl() );
		m_cgProfile = l_pPipeline->GetCgProfile( m_eType );
		CASTOR_ASSERT( m_cgProfile != CG_PROFILE_UNKNOWN);
	}
}
#endif
//*************************************************************************************************
