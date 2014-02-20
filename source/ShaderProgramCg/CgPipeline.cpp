#include "ShaderProgramCg/PrecompiledHeader.hpp"

#include "ShaderProgramCg/CgPipeline.hpp"

using namespace Castor3D;
using namespace CgShader;

//*************************************************************************************************

void _printMatrix( float const * p_mtx)
{
	std::cout.precision( 5);
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			std::cout << p_mtx[i * 4 + j] << " ";
		}

		std::cout << std::endl;
	}

	std::cout << std::endl;
}

//*************************************************************************************************

CgPipeline :: CgPipeline( Pipeline * p_pPipeline )
	:	IPipelineImpl	( p_pPipeline		)
	,	m_cgContext		( cgCreateContext()	)
{
	cgSetContextBehavior( m_cgContext, CG_BEHAVIOR_LATEST );
}

CgPipeline :: ~CgPipeline()
{
 	if (m_cgContext)
 	{
 		cgDestroyContext( m_cgContext);
 		m_cgContext = NULL;
 	}
}

void CgPipeline :: Initialise()
{
	DoCheckCgAvailableProfiles();
}

//*************************************************************************************************

#if defined( Cg_Direct3D11 )

#define USE_DX11_MATRIX 0
#define DEF_EXCLUSIVE 1

#include "ShaderProgramCg/CgDx9ShaderObject.hpp"
#include "ShaderProgramCg/CgDx9ShaderProgram.hpp"
#include <Dx11RenderSystem/DxRenderSystem.hpp>

using namespace Castor;

//*************************************************************************************************

CgD3D11Pipeline :: CgD3D11Pipeline( Dx11Render::DxPipeline * p_pPipeline )
	:	CgPipeline		( p_pPipeline	)
	,	m_pDxPipeline	( p_pPipeline	)
{
}

CgD3D11Pipeline :: ~CgD3D11Pipeline()
{
}

void CgD3D11Pipeline :: ApplyViewport( int p_iWindowWidth, int p_iWindowHeight )
{
	m_viewport.Width = FLOAT( p_iWindowWidth );
	m_viewport.Height = FLOAT( p_iWindowHeight );
	ID3D11DeviceContext * l_pDeviceContext;
	m_pDxPipeline->GetDxRenderSystem()->GetDevice()->GetImmediateContext( &l_pDeviceContext );
	l_pDeviceContext->RSSetViewports( 1, &m_viewport );
	l_pDeviceContext->Release();
}

void CgD3D11Pipeline :: DoIsCgProfileSupported( eSHADER_TYPE p_eType, char const * p_szName )
{
	CGprofile l_cgProfile = cgGetProfile( p_szName );
	bool l_bSupported = cgIsProfileSupported( l_cgProfile ) != 0;
	l_bSupported = l_bSupported && cgD3D11IsProfileSupported( l_cgProfile ) != 0;
	Logger::LogMessage( cuT( "CgDx11Pipeline :: CheckCgProfileSupport - Profile : " ) + str_utils::from_str( p_szName ) + str_utils::from_str( " - Support : " ) + (l_bSupported ? cuT( "true" ) : cuT( "false" )) );

	if( l_bSupported )
	{
		if( m_mapSupportedProfiles[p_eType] == CG_PROFILE_UNKNOWN )
		{
			m_mapSupportedProfiles[p_eType] = l_cgProfile;
		}
	}
}

void CgD3D11Pipeline :: DoCheckCgAvailableProfiles()
{
	ID3D11Device * l_pDevice = m_pDxPipeline->GetDxRenderSystem()->GetDevice();
	cgD3D11SetDevice( m_cgContext, l_pDevice );

	for( int i = 0; i < eSHADER_TYPE_COUNT; i++ )
	{
		m_mapSupportedProfiles[i] = CG_PROFILE_UNKNOWN;
	}

	DoIsCgProfileSupported( eSHADER_TYPE_DOMAIN,	"ds_5_0"	);
	DoIsCgProfileSupported( eSHADER_TYPE_HULL,		"hs_5_0"	);
	DoIsCgProfileSupported( eSHADER_TYPE_VERTEX,	"vs_5_0"	);
	DoIsCgProfileSupported( eSHADER_TYPE_GEOMETRY,	"gs_5_0"	);
	DoIsCgProfileSupported( eSHADER_TYPE_PIXEL,		"ps_5_0"	);
	DoIsCgProfileSupported( eSHADER_TYPE_VERTEX,	"vs_4_0"	);
	DoIsCgProfileSupported( eSHADER_TYPE_GEOMETRY,	"gs_4_0"	);
	DoIsCgProfileSupported( eSHADER_TYPE_PIXEL,		"ps_4_0"	);
	DoIsCgProfileSupported( eSHADER_TYPE_VERTEX,	"hlslv"		);
	DoIsCgProfileSupported( eSHADER_TYPE_PIXEL,		"hlslf"		);
	DoIsCgProfileSupported( eSHADER_TYPE_VERTEX,	"vs_3_0"	);
	DoIsCgProfileSupported( eSHADER_TYPE_PIXEL,		"ps_3_0"	);
	DoIsCgProfileSupported( eSHADER_TYPE_VERTEX,	"vs_2_x"	);
	DoIsCgProfileSupported( eSHADER_TYPE_PIXEL,		"ps_2_x"	);
	DoIsCgProfileSupported( eSHADER_TYPE_VERTEX,	"vs_2_sw"	);
	DoIsCgProfileSupported( eSHADER_TYPE_PIXEL,		"ps_2_sw"	);
	DoIsCgProfileSupported( eSHADER_TYPE_VERTEX,	"vs_2_0"	);
	DoIsCgProfileSupported( eSHADER_TYPE_PIXEL,		"ps_2_0"	);
	DoIsCgProfileSupported( eSHADER_TYPE_VERTEX,	"vs_1_1"	);
	DoIsCgProfileSupported( eSHADER_TYPE_PIXEL,		"ps_1_3"	);
	DoIsCgProfileSupported( eSHADER_TYPE_PIXEL,		"ps_1_2"	);
	DoIsCgProfileSupported( eSHADER_TYPE_PIXEL,		"ps_1_1"	);
}

#endif

//*************************************************************************************************

#if defined( Cg_Direct3D10 )

#define USE_DX10_MATRIX 0
#define DEF_EXCLUSIVE 1

#include "ShaderProgramCg/CgDx9ShaderObject.hpp"
#include "ShaderProgramCg/CgDx9ShaderProgram.hpp"
#include <Dx10RenderSystem/DxRenderSystem.hpp>

using namespace Castor;

//*************************************************************************************************

CgD3D10Pipeline :: CgD3D10Pipeline( Dx10Render::DxPipeline * p_pPipeline )
	:	CgPipeline		( p_pPipeline	)
	,	m_pDxPipeline	( p_pPipeline	)
{
}

CgD3D10Pipeline :: ~CgD3D10Pipeline()
{
}

void CgD3D10Pipeline :: ApplyViewport( int p_iWindowWidth, int p_iWindowHeight )
{
	m_viewport.Width = UINT( p_iWindowWidth );
	m_viewport.Height = UINT( p_iWindowHeight );
	m_pDxPipeline->GetDxRenderSystem()->GetDevice()->RSSetViewports( 1, &m_viewport );
}

void CgD3D10Pipeline :: DoIsCgProfileSupported( eSHADER_TYPE p_eType, char const * p_szName )
{
	CGprofile l_cgProfile = cgGetProfile( p_szName );
	bool l_bSupported = cgIsProfileSupported( l_cgProfile ) != 0;
	l_bSupported = l_bSupported && cgD3D10IsProfileSupported( l_cgProfile ) != 0;
	Logger::LogMessage( cuT( "CgDx10Pipeline :: CheckCgProfileSupport - Profile : " ) + str_utils::from_str( p_szName ) + str_utils::from_str( " - Support : " ) + (l_bSupported ? cuT( "true" ) : cuT( "false" )) );

	if( l_bSupported )
	{
		if( m_mapSupportedProfiles[p_eType] == CG_PROFILE_UNKNOWN )
		{
			m_mapSupportedProfiles[p_eType] = l_cgProfile;
		}
	}
}

void CgD3D10Pipeline :: DoCheckCgAvailableProfiles()
{
	ID3D10Device * l_pDevice = m_pDxPipeline->GetDxRenderSystem()->GetDevice();
	cgD3D10SetDevice( m_cgContext, l_pDevice );

	for( int i = 0; i < eSHADER_TYPE_COUNT; i++ )
	{
		m_mapSupportedProfiles[i] = CG_PROFILE_UNKNOWN;
	}

	DoIsCgProfileSupported( eSHADER_TYPE_DOMAIN,	"ds_5_0"	);
	DoIsCgProfileSupported( eSHADER_TYPE_HULL,		"hs_5_0"	);
	DoIsCgProfileSupported( eSHADER_TYPE_VERTEX,	"vs_5_0"	);
	DoIsCgProfileSupported( eSHADER_TYPE_GEOMETRY,	"gs_5_0"	);
	DoIsCgProfileSupported( eSHADER_TYPE_PIXEL,		"ps_5_0"	);
	DoIsCgProfileSupported( eSHADER_TYPE_VERTEX,	"vs_4_0"	);
	DoIsCgProfileSupported( eSHADER_TYPE_GEOMETRY,	"gs_4_0"	);
	DoIsCgProfileSupported( eSHADER_TYPE_PIXEL,		"ps_4_0"	);
	DoIsCgProfileSupported( eSHADER_TYPE_VERTEX,	"hlslv"		);
	DoIsCgProfileSupported( eSHADER_TYPE_PIXEL,		"hlslf"		);
	DoIsCgProfileSupported( eSHADER_TYPE_VERTEX,	"vs_3_0"	);
	DoIsCgProfileSupported( eSHADER_TYPE_PIXEL,		"ps_3_0"	);
	DoIsCgProfileSupported( eSHADER_TYPE_VERTEX,	"vs_2_x"	);
	DoIsCgProfileSupported( eSHADER_TYPE_PIXEL,		"ps_2_x"	);
	DoIsCgProfileSupported( eSHADER_TYPE_VERTEX,	"vs_2_sw"	);
	DoIsCgProfileSupported( eSHADER_TYPE_PIXEL,		"ps_2_sw"	);
	DoIsCgProfileSupported( eSHADER_TYPE_VERTEX,	"vs_2_0"	);
	DoIsCgProfileSupported( eSHADER_TYPE_PIXEL,		"ps_2_0"	);
	DoIsCgProfileSupported( eSHADER_TYPE_VERTEX,	"vs_1_1"	);
	DoIsCgProfileSupported( eSHADER_TYPE_PIXEL,		"ps_1_3"	);
	DoIsCgProfileSupported( eSHADER_TYPE_PIXEL,		"ps_1_2"	);
	DoIsCgProfileSupported( eSHADER_TYPE_PIXEL,		"ps_1_1"	);
}

#endif

//*************************************************************************************************

#if defined( Cg_Direct3D9 )

#define USE_DX9_MATRIX 0
#define DEF_EXCLUSIVE 1

#include "ShaderProgramCg/CgDx9ShaderObject.hpp"
#include "ShaderProgramCg/CgDx9ShaderProgram.hpp"
#include <Dx9RenderSystem/DxRenderSystem.hpp>

using namespace Castor;

//*************************************************************************************************

CgD3D9Pipeline :: CgD3D9Pipeline( Dx9Render::DxPipeline * p_pPipeline )
	:	CgPipeline		( p_pPipeline	)
	,	m_pDxPipeline	( p_pPipeline	)
{
}

CgD3D9Pipeline :: ~CgD3D9Pipeline()
{
}

void CgD3D9Pipeline :: ApplyViewport( int p_iWindowWidth, int p_iWindowHeight )
{
	m_viewport.Width = UINT( p_iWindowWidth );
	m_viewport.Height = UINT( p_iWindowHeight );
	m_pDxPipeline->GetDxRenderSystem()->GetDevice()->SetViewport( &m_viewport );
}

void CgD3D9Pipeline :: DoIsCgProfileSupported( eSHADER_TYPE p_eType, char const * p_szName )
{
	CGprofile l_cgProfile = cgGetProfile( p_szName );
	bool l_bSupported = cgIsProfileSupported( l_cgProfile ) != 0;
	l_bSupported = l_bSupported && cgD3D9IsProfileSupported( l_cgProfile ) != 0;
	Logger::LogMessage( cuT( "CgDxPipeline :: CheckCgProfileSupport - Profile : " ) + str_utils::from_str( p_szName ) + str_utils::from_str( " - Support : " ) + (l_bSupported ? cuT( "true" ) : cuT( "false" )) );

	if( l_bSupported )
	{
		if( m_mapSupportedProfiles[p_eType] == CG_PROFILE_UNKNOWN )
		{
			m_mapSupportedProfiles[p_eType] = l_cgProfile;
		}
	}
}

void CgD3D9Pipeline :: DoCheckCgAvailableProfiles()
{
	IDirect3DDevice9 * l_pDevice = m_pDxPipeline->GetDxRenderSystem()->GetDevice();
	cgD3D9SetDevice( l_pDevice );

	for( int i = 0; i < eSHADER_TYPE_COUNT; i++ )
	{
		m_mapSupportedProfiles[i] = CG_PROFILE_UNKNOWN;
	}

	DoIsCgProfileSupported( eSHADER_TYPE_DOMAIN,	"ds_5_0"	);
	DoIsCgProfileSupported( eSHADER_TYPE_HULL,		"hs_5_0"	);
	DoIsCgProfileSupported( eSHADER_TYPE_VERTEX,	"vs_5_0"	);
	DoIsCgProfileSupported( eSHADER_TYPE_GEOMETRY,	"gs_5_0"	);
	DoIsCgProfileSupported( eSHADER_TYPE_PIXEL,		"ps_5_0"	);
	DoIsCgProfileSupported( eSHADER_TYPE_VERTEX,	"vs_4_0"	);
	DoIsCgProfileSupported( eSHADER_TYPE_GEOMETRY,	"gs_4_0"	);
	DoIsCgProfileSupported( eSHADER_TYPE_PIXEL,		"ps_4_0"	);
	DoIsCgProfileSupported( eSHADER_TYPE_VERTEX,	"hlslv"		);
	DoIsCgProfileSupported( eSHADER_TYPE_PIXEL,		"hlslf"		);
	DoIsCgProfileSupported( eSHADER_TYPE_VERTEX,	"vs_3_0"	);
	DoIsCgProfileSupported( eSHADER_TYPE_PIXEL,		"ps_3_0"	);
	DoIsCgProfileSupported( eSHADER_TYPE_VERTEX,	"vs_2_x"	);
	DoIsCgProfileSupported( eSHADER_TYPE_PIXEL,		"ps_2_x"	);
	DoIsCgProfileSupported( eSHADER_TYPE_VERTEX,	"vs_2_sw"	);
	DoIsCgProfileSupported( eSHADER_TYPE_PIXEL,		"ps_2_sw"	);
	DoIsCgProfileSupported( eSHADER_TYPE_VERTEX,	"vs_2_0"	);
	DoIsCgProfileSupported( eSHADER_TYPE_PIXEL,		"ps_2_0"	);
	DoIsCgProfileSupported( eSHADER_TYPE_VERTEX,	"vs_1_1"	);
	DoIsCgProfileSupported( eSHADER_TYPE_PIXEL,		"ps_1_3"	);
	DoIsCgProfileSupported( eSHADER_TYPE_PIXEL,		"ps_1_2"	);
	DoIsCgProfileSupported( eSHADER_TYPE_PIXEL,		"ps_1_1"	);
}

#endif

//*************************************************************************************************

#if defined( Cg_OpenGL )
#include "ShaderProgramCg/CgGlShaderObject.hpp"
#include "ShaderProgramCg/CgGlShaderProgram.hpp"
#include <GlRenderSystem/GlRenderSystem.hpp>

using namespace Castor;

CgGlPipeline :: CgGlPipeline( GlRender::GlPipeline * p_pPipeline )
	:	CgPipeline		( p_pPipeline	)
	,	m_pGlPipeline	( p_pPipeline	)
{
}

CgGlPipeline :: ~CgGlPipeline()
{
}

void CgGlPipeline :: ApplyViewport( int p_iWindowWidth, int p_iWindowHeight)
{
//	OpenGl::Viewport( 0, 0, p_iWindowWidth, p_iWindowHeight );
}

void CgGlPipeline :: DoIsCgProfileSupported( eSHADER_TYPE p_eType, char const * p_szName )
{
	CGprofile l_cgProfile = cgGetProfile( p_szName );
	bool l_bSupported = cgIsProfileSupported( l_cgProfile ) != 0 && cgGLIsProfileSupported( l_cgProfile ) != 0;
	Logger::LogMessage( cuT( "CgGlPipeline :: CheckCgProfileSupport - Profile : " ) + str_utils::from_str( p_szName ) + str_utils::from_str( " - Support : " ) + (l_bSupported ? cuT( "true" ) : cuT( "false" )) );

	if( l_bSupported )
	{
		if( m_mapSupportedProfiles[p_eType] == CG_PROFILE_UNKNOWN )
		{
			m_mapSupportedProfiles[p_eType] = l_cgProfile;
		}
	}
}

void CgGlPipeline :: DoCheckCgAvailableProfiles()
{
	for( int i = 0; i < eSHADER_TYPE_COUNT; i++ )
	{
		m_mapSupportedProfiles[i] = CG_PROFILE_UNKNOWN;
	}

	DoIsCgProfileSupported( eSHADER_TYPE_HULL,		"gp5tcp"	);
	DoIsCgProfileSupported( eSHADER_TYPE_DOMAIN,	"gp5tep"	);
	DoIsCgProfileSupported( eSHADER_TYPE_VERTEX,	"gp5vp"		);
	DoIsCgProfileSupported( eSHADER_TYPE_GEOMETRY,	"gp5gp"		);
	DoIsCgProfileSupported( eSHADER_TYPE_PIXEL,		"gp5fp"		);
	DoIsCgProfileSupported( eSHADER_TYPE_VERTEX,	"gp4vp"		);
	DoIsCgProfileSupported( eSHADER_TYPE_GEOMETRY,	"gp4gp"		);
	DoIsCgProfileSupported( eSHADER_TYPE_PIXEL,		"gp4fp"		);
	DoIsCgProfileSupported( eSHADER_TYPE_VERTEX,	"glslv"		);
	DoIsCgProfileSupported( eSHADER_TYPE_GEOMETRY,	"glslg"		);
	DoIsCgProfileSupported( eSHADER_TYPE_PIXEL,		"glslf"		);
	DoIsCgProfileSupported( eSHADER_TYPE_PIXEL,		"arbfp1"	);
	DoIsCgProfileSupported( eSHADER_TYPE_VERTEX,	"arbvp1"	);
}
#endif
//*************************************************************************************************
