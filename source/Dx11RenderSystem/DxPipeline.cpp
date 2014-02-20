#include "Dx11RenderSystem/PrecompiledHeader.hpp"

#include "Dx11RenderSystem/DxPipeline.hpp"
#include "Dx11RenderSystem/DxShaderProgram.hpp"
#include "Dx11RenderSystem/DxShaderObject.hpp"
#include "Dx11RenderSystem/DxFrameVariable.hpp"
#include "Dx11RenderSystem/DxContext.hpp"
#include "Dx11RenderSystem/DxRenderSystem.hpp"

using namespace Castor3D;
using namespace Castor;
using namespace Dx11Render;

#define C3D_DIRECTX_MTX	0

#if C3D_DIRECTX_MTX
#	include <d3dx10math.h>

typedef std::stack< D3DXMATRIX > D3dMatrixStack;
D3DXMATRIX		g_d3dMtx;
D3dMatrixStack	g_d3dMatrix[Castor3D::eMTXMODE_COUNT];
#endif

//*************************************************************************************************

namespace
{
	void PrintMatrix( std::string const & p_strHeader, float const * p_mtx )
	{
		std::stringstream l_stream;
		l_stream.precision( 5 );
		l_stream << p_strHeader << std::endl;

		for( int i = 0; i < 4; i++ )
		{
			for( int j = 0; j < 4; j++ )
			{
				l_stream << p_mtx[i * 4 + j] << " ";
			}

			l_stream << std::endl;
		}

		Logger::LogDebug( l_stream.str() );
	}
}

//*************************************************************************************************

DxPipelineImplHlsl :: DxPipelineImplHlsl( DxPipeline * p_pPipeline )
	:	IPipelineImpl	( p_pPipeline	)
	,	m_pDxPipeline	( p_pPipeline	)
	,	m_viewport		(				)
{
	m_viewport.MaxDepth = 1.0f;

#if C3D_DIRECTX_MTX
	D3DXMatrixIdentity( &g_d3dMtx );

	for (int i = 0; i < eMTXMODE_COUNT; i++)
	{
		g_d3dMatrix[i].push( g_d3dMtx );
	}
#endif
}

DxPipelineImplHlsl :: ~DxPipelineImplHlsl()
{
}

eMTXMODE DxPipelineImplHlsl :: MatrixMode( eMTXMODE p_eMode )
{
	return IPipelineImpl::MatrixMode( p_eMode );
}

bool DxPipelineImplHlsl :: LoadIdentity()
{
	GetCurrentMatrix().set_identity();
	DoApplyMatrix();

#if C3D_DIRECTX_MTX
	D3DXMatrixIdentity( &g_d3dMatrix[GetCurrentMode()].top() );
#endif
	return true;
}

bool DxPipelineImplHlsl :: PushMatrix()
{
	IPipelineImpl::PushMatrix();
	DoApplyMatrix();
#if C3D_DIRECTX_MTX
	g_d3dMatrix[GetCurrentMode()].push( g_d3dMatrix[GetCurrentMode()].top() );
#endif
	return true;
}

bool DxPipelineImplHlsl :: PopMatrix()
{
	IPipelineImpl::PopMatrix();
	DoApplyMatrix();
#if C3D_DIRECTX_MTX
	g_d3dMatrix[GetCurrentMode()].pop();
#endif
	return true;
}

bool DxPipelineImplHlsl :: Translate( Point3r const & p_translate )
{
	IPipelineImpl::Translate( p_translate );
	DoApplyMatrix();

#if C3D_DIRECTX_MTX
	D3DXMatrixTranslation( &g_d3dMtx, p_translate[0], p_translate[1], p_translate[2] );
	g_d3dMatrix[GetCurrentMode()].top() *= g_d3dMtx;
#endif
	return true;
}

bool DxPipelineImplHlsl :: Rotate( Quaternion const & p_rotate )
{
	Point3r l_ptAxis;
	Angle l_angle;
	p_rotate.ToAxisAngle( l_ptAxis, l_angle );
	IPipelineImpl::Rotate( p_rotate );
	DoApplyMatrix();

#if C3D_DIRECTX_MTX
	D3DXVECTOR3 l_d3dAxis( l_ptAxis.const_ptr() );
	D3DXMatrixRotationAxis( &g_d3dMtx, &l_d3dAxis, l_angle.Radians() );
	g_d3dMatrix[GetCurrentMode()].top() *= g_d3dMtx;
#endif
	return true;
}

bool DxPipelineImplHlsl :: Scale( Point3r const & p_scale )
{
	IPipelineImpl::Scale( p_scale);
	DoApplyMatrix();

#if C3D_DIRECTX_MTX
	D3DXMatrixScaling( &g_d3dMtx, p_scale[0], p_scale[1], p_scale[2] );
	g_d3dMatrix[GetCurrentMode()].top() *= g_d3dMtx;
#endif
	return true;
}

bool DxPipelineImplHlsl :: MultMatrix( Matrix4x4r const & p_matrix )
{
	IPipelineImpl::MultMatrix( p_matrix.get_transposed() );
	DoApplyMatrix();

#if C3D_DIRECTX_MTX
	g_d3dMtx = g_d3dMatrix[GetCurrentMode()].top();
	g_d3dMtx *= D3DXMATRIX( p_matrix.get_transposed().const_ptr() );
/*	Ok	*/
	PrintMatrix( "MultMatrix\nDirect3D : ",	&g_d3dMtx._11					);
	PrintMatrix( "Internal : ",				GetCurrentMatrix().const_ptr()	);
/**/
#endif
	return true;
}

bool DxPipelineImplHlsl :: MultMatrix( real const * p_matrix )
{
	return MultMatrix( Matrix4x4r( p_matrix ) );
}

bool DxPipelineImplHlsl :: Perspective( Angle const & p_aFOVY, real p_rRatio, real p_rNear, real p_rFar )
{
//	IPipelineImpl::Perspective( p_aFOVY, p_rRatio, p_rNear, p_rFar );
	MtxUtils::perspective_dx( m_pPipeline->GetMatrix( eMTXMODE_PROJECTION ), p_aFOVY, p_rRatio, p_rNear, p_rFar );
	DoApplyMatrix();

#if C3D_DIRECTX_MTX
	D3DXMatrixPerspectiveFovRH( &g_d3dMtx, p_aFOVY.Radians(), p_rRatio, p_rNear, p_rFar );
/*	Ok	*/
	PrintMatrix( "Perspective\nDirect3D : ",	&g_d3dMtx._11					);
	PrintMatrix( "Internal : ",					m_pPipeline->GetMatrix( eMTXMODE_PROJECTION ).const_ptr()	);
/**/
#endif
	return true;
}

bool DxPipelineImplHlsl :: Frustum( real p_rLeft, real p_rRight, real p_rBottom, real p_rTop, real p_rNear, real p_rFar )
{
	IPipelineImpl::Frustum( p_rLeft, p_rRight, p_rBottom, p_rTop, p_rNear, p_rFar);
	DoApplyMatrix();

#if C3D_DIRECTX_MTX
	D3DXMatrixPerspectiveOffCenterRH( &g_d3dMtx, p_rLeft, p_rRight, p_rBottom, p_rTop, p_rNear, p_rFar );
/*	Ok	*/
	PrintMatrix( "Frustum\nDirect3D : ",	&g_d3dMtx._11					);
	PrintMatrix( "Internal : ",				m_pPipeline->GetMatrix( eMTXMODE_PROJECTION ).const_ptr()	);
/**/
#endif
	return true;
}

bool DxPipelineImplHlsl :: Ortho( real p_rLeft, real p_rRight, real p_rBottom, real p_rTop, real p_rNear, real p_rFar )
{
	MtxUtils::ortho_dx( m_pPipeline->GetMatrix( eMTXMODE_PROJECTION ), p_rLeft, p_rRight, p_rBottom, p_rTop, p_rNear, p_rFar);
//	IPipelineImpl::Ortho( p_rLeft, p_rRight, p_rBottom, p_rTop, p_rNear, p_rFar );
	DoApplyMatrix();

#if C3D_DIRECTX_MTX
	D3DXMatrixOrthoOffCenterRH( &g_d3dMtx, p_rLeft, p_rRight, p_rBottom, p_rTop, p_rNear, p_rFar );
/*	Ok	*/
	PrintMatrix( "Ortho\nDirect3D : ",	&g_d3dMtx._11					);
	PrintMatrix( "Internal : ",			m_pPipeline->GetMatrix( eMTXMODE_PROJECTION ).const_ptr()	);
/**/
#endif
	return true;
}

void DxPipelineImplHlsl :: ApplyViewport( int p_iWindowWidth, int p_iWindowHeight )
{
	m_viewport.Width = FLOAT( p_iWindowWidth );
	m_viewport.Height = FLOAT( p_iWindowHeight );
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;
	ID3D11DeviceContext * l_pDeviceContext;
	m_pDxPipeline->GetDxRenderSystem()->GetDevice()->GetImmediateContext( &l_pDeviceContext );
	l_pDeviceContext->RSSetViewports( 1, &m_viewport );
	l_pDeviceContext->Release();
}

void DxPipelineImplHlsl :: DoApplyMatrix()
{
#if C3D_DIRECTX_MTX
#	if CASTOR_USE_DOUBLE
	Matrix4x4f l_mtx( GetCurrentMatrix().const_ptr() ); 
	g_d3dMtx = D3DXMATRIX( l_mtx.const_ptr() );
#	else
	g_d3dMtx = D3DXMATRIX( GetCurrentMatrix().const_ptr() );
#	endif
#endif
}

//*************************************************************************************************

DxPipeline :: DxPipeline( DxRenderSystem * p_pRenderSystem )
	:	Pipeline			( p_pRenderSystem	)
	,	m_pDxRenderSystem	( p_pRenderSystem	)
	,	m_pPipelineImplCg	( NULL				)
{
	m_pPipelineImplHlsl	= new DxPipelineImplHlsl(	this );
//	m_pPipelineImplCg = m_pDxRenderSystem->CreatePipeline( this, eSHADER_LANGUAGE_CG );
}

DxPipeline :: ~DxPipeline()
{
	m_pDxRenderSystem->DestroyPipeline( eSHADER_LANGUAGE_CG, m_pPipelineImplCg );
	delete m_pPipelineImplHlsl;
}

void DxPipeline :: Initialise()
{
	if( m_pPipelineImplCg )
	{
		m_pPipelineImplCg->Initialise();
	}

	UpdateFunctions( NULL );
}

void DxPipeline :: UpdateFunctions( ShaderProgramBase * p_pProgram )
{
	if( p_pProgram && p_pProgram->GetLanguage() == eSHADER_LANGUAGE_CG )
	{
		m_pPipelineImpl = m_pPipelineImplCg;
	}
	else
	{
		m_pPipelineImpl = m_pPipelineImplHlsl;
	}
}

//*************************************************************************************************
