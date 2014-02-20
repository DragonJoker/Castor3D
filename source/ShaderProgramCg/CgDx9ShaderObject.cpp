#include "ShaderProgramCg/PrecompiledHeader.hpp"

#if defined( Cg_Direct3D9 )
#include "ShaderProgramCg/CgDx9ShaderObject.hpp"
#include "ShaderProgramCg/CgDx9ShaderProgram.hpp"
#include "ShaderProgramCg/CgPipeline.hpp"
#include <Dx9RenderSystem/DxRenderSystem.hpp>

using namespace Castor3D;
using namespace Castor;
using namespace CgShader;
using namespace Dx9Render;

//*************************************************************************************************

CgD3D9ShaderObject :: CgD3D9ShaderObject( CgD3D9ShaderProgram * p_pParent, eSHADER_TYPE p_eType)
	:	CgShaderObject( p_pParent, p_eType)
{
}

CgD3D9ShaderObject :: ~CgD3D9ShaderObject()
{
	if (m_cgProgram && cgD3D9IsProgramLoaded( m_cgProgram))
	{
		CheckCgError( true, cuT( "CgD3D9ShaderObject :: ~CgD3D9ShaderObject - cgD3D9UnloadProgram" ) );
		m_cgProgram = nullptr;
	}

	DestroyProgram();
}

void CgD3D9ShaderObject :: DestroyProgram()
{
	if (m_cgProfile != CG_PROFILE_UNKNOWN)
	{
		CgPipeline * l_pPipeline = reinterpret_cast< CgPipeline* >( m_pParent->GetRenderSystem()->GetPipeline()->GetImpl() );
		CheckCgError( cgD3D9SetDevice( static_cast< DxRenderSystem *>( m_pParent->GetRenderSystem() )->GetDevice()), cuT( "CgD3D9ShaderObject :: DestroyProgram - cgD3D9SetDevice" ) );
		CheckCgError( cgD3D9UnloadProgram( m_cgProgram), cuT( "CgD3D9ShaderObject :: DestroyProgram - cgD3D9UnloadProgram" ) );
	}
}

bool CgD3D9ShaderObject :: Compile()
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

					if (m_cgProgram && cgD3D9IsProgramLoaded( m_cgProgram))
					{
						CheckCgError( cgD3D9UnloadProgram( m_cgProgram), cuT( "CgD3D9ShaderObject :: Compile - cgD3D9UnloadProgram" ) );
						m_cgProgram = nullptr;
					}

					CgPipeline * l_pPipeline = reinterpret_cast< CgPipeline* >( m_pParent->GetRenderSystem()->GetPipeline()->GetImpl() );
					m_cgProgram = cgCreateProgram( l_pPipeline->GetCgContext(), CG_SOURCE, str_utils::to_str( l_strSource ).c_str(), m_cgProfile, str_utils::to_str( m_strEntryPoint ).c_str(), nullptr);
					CheckCgError( true, cuT( "CgD3D9ShaderObject :: Compile - cgCreateProgram" ) );

					if (m_cgProgram)
					{
						cgD3D9SetDevice( static_cast< DxRenderSystem *>( m_pParent->GetRenderSystem() )->GetDevice());
						cgD3D9LoadProgram( m_cgProgram, false, 0 );
						CheckCgError( true, cuT( "CgD3D9ShaderObject :: Compile - cgD3D9LoadProgram" ) );
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

void CgD3D9ShaderObject :: Bind()
{
	if (m_cgProgram && cgD3D9IsProgramLoaded( m_cgProgram) && m_cgProfile != CG_PROFILE_UNKNOWN)
	{
		cgD3D9BindProgram( m_cgProgram );
		CheckCgError( true, cuT( "CgD3D9ShaderObject :: Compile - cgD3D9BindProgram" ) );
	}
}

void CgD3D9ShaderObject :: Unbind()
{
}

void CgD3D9ShaderObject :: CreateProgram()
{
	if (m_cgProfile == CG_PROFILE_UNKNOWN)
	{
		CgPipeline * l_pPipeline = reinterpret_cast< CgPipeline* >( m_pParent->GetRenderSystem()->GetPipeline()->GetImpl() );
		m_cgProfile = l_pPipeline->GetCgProfile( m_eType );
	}
}

//*************************************************************************************************

#endif
