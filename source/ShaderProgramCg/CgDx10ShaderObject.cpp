#include "ShaderProgramCg/PrecompiledHeader.hpp"

#if defined( Cg_Direct3D10 )
#include "ShaderProgramCg/CgDx10ShaderObject.hpp"
#include "ShaderProgramCg/CgDx10ShaderProgram.hpp"
#include "ShaderProgramCg/CgPipeline.hpp"
#include <Dx10RenderSystem/DxRenderSystem.hpp>

using namespace Castor3D;
using namespace Castor;
using namespace CgShader;
using namespace Dx10Render;

//*************************************************************************************************

CgD3D10ShaderObject :: CgD3D10ShaderObject( CgD3D10ShaderProgram * p_pParent, eSHADER_TYPE p_eType)
	:	CgShaderObject( p_pParent, p_eType)
{
}

CgD3D10ShaderObject :: ~CgD3D10ShaderObject()
{
	if (m_cgProgram && cgD3D10IsProgramLoaded( m_cgProgram))
	{
		CheckCgError( true, cuT( "CgD3D10ShaderObject :: ~DxShaderObject - cgGLUnloadProgram" ) );
		m_cgProgram = nullptr;
	}

	DestroyProgram();
}

void CgD3D10ShaderObject :: DestroyProgram()
{
	if (m_cgProfile != CG_PROFILE_UNKNOWN)
	{
		CgPipeline * l_pPipeline = reinterpret_cast< CgPipeline* >( m_pParent->GetRenderSystem()->GetPipeline()->GetImpl() );
		CheckCgError( cgD3D10SetDevice( l_pPipeline->GetCgContext(), static_cast< DxRenderSystem *>( m_pParent->GetRenderSystem() )->GetDevice()), cuT( "CgD3D10ShaderObject :: DestroyProgram - cgD3D10SetDevice" ) );
		CheckCgError( cgD3D10UnloadProgram( m_cgProgram), cuT( "CgD3D10ShaderObject :: DestroyProgram - cgD3D10UnloadProgram" ) );
	}
}

bool CgD3D10ShaderObject :: Compile()
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
			if ( ! l_strSource.empty())
			{
				if (m_cgProfile != CG_PROFILE_UNKNOWN)
				{
					m_eStatus = eSHADER_STATUS_NOTCOMPILED;

					if (m_cgProgram && cgD3D10IsProgramLoaded( m_cgProgram))
					{
						CheckCgError( cgD3D10UnloadProgram( m_cgProgram), cuT( "CgD3D10ShaderObject :: Compile - cgD3D10UnloadProgram" ) );
						m_cgProgram = nullptr;
					}

					CgPipeline * l_pPipeline = reinterpret_cast< CgPipeline* >( m_pParent->GetRenderSystem()->GetPipeline()->GetImpl() );
					m_cgProgram = cgCreateProgram( l_pPipeline->GetCgContext(), CG_SOURCE, str_utils::to_str( l_strSource ).c_str(), m_cgProfile, str_utils::to_str( m_strEntryPoint ).c_str(), nullptr);
					CheckCgError( true, cuT( "CgD3D10ShaderObject :: Compile - cgCreateProgram" ) );

					if (m_cgProgram)
					{
						cgD3D10SetDevice( l_pPipeline->GetCgContext(), static_cast< DxRenderSystem *>( m_pParent->GetRenderSystem() )->GetDevice());
						cgD3D10LoadProgram( m_cgProgram, 0 );
						CheckCgError( true, cuT( "CgD3D10ShaderObject :: Compile - cgD3D10LoadProgram" ) );
						CgShaderObject::Compile();
						m_eStatus = eSHADER_STATUS_COMPILED;
					}
					else
					{
						m_eStatus = eSHADER_STATUS_ERROR;
					}

					l_bReturn = m_eStatus == eSHADER_STATUS_COMPILED;
				}
			}
		}
	}

	return l_bReturn;
}

void CgD3D10ShaderObject :: Bind()
{
	if (m_cgProgram && cgD3D10IsProgramLoaded( m_cgProgram) && m_cgProfile != CG_PROFILE_UNKNOWN)
	{
		cgD3D10BindProgram( m_cgProgram );
		CheckCgError( true, cuT( "CgD3D10ShaderObject :: Compile - cgD3D10BindProgram" ) );
	}
}

void CgD3D10ShaderObject :: Unbind()
{
}

void CgD3D10ShaderObject :: CreateProgram()
{
	if (m_cgProfile == CG_PROFILE_UNKNOWN)
	{
		CgPipeline * l_pPipeline = reinterpret_cast< CgPipeline* >( m_pParent->GetRenderSystem()->GetPipeline()->GetImpl() );
		m_cgProfile = l_pPipeline->GetCgProfile( m_eType );
	}
}

//*************************************************************************************************

#endif
