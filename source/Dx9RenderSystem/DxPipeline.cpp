#include "Dx9RenderSystem/PrecompiledHeader.hpp"

#include "Dx9RenderSystem/DxPipeline.hpp"
#include "Dx9RenderSystem/DxShaderProgram.hpp"
#include "Dx9RenderSystem/DxShaderObject.hpp"
#include "Dx9RenderSystem/DxFrameVariable.hpp"
#include "Dx9RenderSystem/DxContext.hpp"
#include "Dx9RenderSystem/DxRenderSystem.hpp"

using namespace Castor3D;
using namespace Castor;
using namespace Dx9Render;

#define C3D_DIRECTX_MTX	0

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
	m_viewport.MaxZ = 1.0;
	m_matrixTypes[eMTXMODE_PROJECTION	] = D3DTS_PROJECTION;
	m_matrixTypes[eMTXMODE_MODEL		] = D3DTS_VIEW;
	m_matrixTypes[eMTXMODE_VIEW			] = D3DTS_VIEW;
	m_matrixTypes[eMTXMODE_TEXTURE0		] = D3DTS_TEXTURE0;
	m_matrixTypes[eMTXMODE_TEXTURE1		] = D3DTS_TEXTURE1;
	m_matrixTypes[eMTXMODE_TEXTURE2		] = D3DTS_TEXTURE2;
	m_matrixTypes[eMTXMODE_TEXTURE3		] = D3DTS_TEXTURE3;
	m_matrixTypes[eMTXMODE_TEXTURE4		] = D3DTS_TEXTURE4;
	m_matrixTypes[eMTXMODE_TEXTURE5		] = D3DTS_TEXTURE5;
	m_matrixTypes[eMTXMODE_TEXTURE6		] = D3DTS_TEXTURE6;
	m_matrixTypes[eMTXMODE_TEXTURE7		] = D3DTS_TEXTURE7;

#if C3D_DIRECTX_MTX
	D3DXMatrixIdentity( &m_d3dMtx );

	for (int i = 0; i < eMTXMODE_COUNT; i++)
	{
		m_d3dMatrix[i].push( m_d3dMtx );
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
	D3DXMatrixIdentity( &m_d3dMatrix[GetCurrentMode()].top() );
	m_pDxPipeline->GetDxRenderSystem()->GetDevice()->SetTransform( m_matrixTypes[GetCurrentMode()], &m_d3dMatrix[GetCurrentMode()].top() );
#endif
	return true;
}

bool DxPipelineImplHlsl :: PushMatrix()
{
	IPipelineImpl::PushMatrix();
	DoApplyMatrix();
#if C3D_DIRECTX_MTX
	m_d3dMatrix[GetCurrentMode()].push( m_d3dMatrix[GetCurrentMode()].top() );
#endif
	return true;
}

bool DxPipelineImplHlsl :: PopMatrix()
{
	IPipelineImpl::PopMatrix();
	DoApplyMatrix();
#if C3D_DIRECTX_MTX
	m_d3dMatrix[GetCurrentMode()].pop();
#endif
	return true;
}

bool DxPipelineImplHlsl :: Translate( Point3r const & p_translate )
{
	IPipelineImpl::Translate( p_translate );
	DoApplyMatrix();

#if C3D_DIRECTX_MTX
	D3DXMatrixTranslation( &m_d3dMtx, p_translate[0], p_translate[1], p_translate[2] );
	m_d3dMatrix[GetCurrentMode()].top() *= m_d3dMtx;
	m_pDxPipeline->GetDxRenderSystem()->GetDevice()->SetTransform( m_matrixTypes[GetCurrentMode()], &m_d3dMtx );
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
	D3DXMatrixRotationAxis( &m_d3dMtx, &l_d3dAxis, l_angle.Radians() );
	m_d3dMatrix[GetCurrentMode()].top() *= m_d3dMtx;
	m_pDxPipeline->GetDxRenderSystem()->GetDevice()->SetTransform( m_matrixTypes[GetCurrentMode()], &m_d3dMtx );
#endif
	return true;
}

bool DxPipelineImplHlsl :: Scale( Point3r const & p_scale )
{
	IPipelineImpl::Scale( p_scale);
	DoApplyMatrix();

#if C3D_DIRECTX_MTX
	D3DXMatrixScaling( &m_d3dMtx, p_scale[0], p_scale[1], p_scale[2] );
	m_d3dMatrix[GetCurrentMode()].top() *= m_d3dMtx;
	m_pDxPipeline->GetDxRenderSystem()->GetDevice()->SetTransform( m_matrixTypes[GetCurrentMode()], &m_d3dMtx );
#endif
	return true;
}

bool DxPipelineImplHlsl :: MultMatrix( Matrix4x4r const & p_matrix )
{
	IPipelineImpl::MultMatrix( p_matrix );
	DoApplyMatrix();

#if C3D_DIRECTX_MTX
	m_d3dMtx = m_d3dMatrix[GetCurrentMode()].top();
	m_d3dMtx *= D3DXMATRIX( p_matrix.const_ptr() );
	m_pDxPipeline->GetDxRenderSystem()->GetDevice()->SetTransform( m_matrixTypes[GetCurrentMode()], &m_d3dMtx );
/*	Ok	*/
	PrintMatrix( "MultMatrix\nDirect3D : ",	&m_d3dMtx._11					);
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
//	MtxUtils::perspective_dx( m_pPipeline->GetMatrix( eMTXMODE_PROJECTION ), p_aFOVY, p_rRatio, p_rNear, p_rFar );
	IPipelineImpl::Perspective( p_aFOVY, p_rRatio, p_rNear, p_rFar );
	DoApplyMatrix();

#if C3D_DIRECTX_MTX
	D3DXMatrixPerspectiveFovRH( &m_d3dMtx, p_aFOVY.Radians(), p_rRatio, p_rNear, p_rFar );
	m_pDxPipeline->GetDxRenderSystem()->GetDevice()->SetTransform( D3DTS_PROJECTION, &m_d3dMtx );
/*	Ok	*/
	PrintMatrix( "Perspective\nDirect3D : ",	&m_d3dMtx._11					);
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
	D3DXMatrixPerspectiveOffCenterRH( &m_d3dMtx, p_rLeft, p_rRight, p_rBottom, p_rTop, p_rNear, p_rFar );
	m_pDxPipeline->GetDxRenderSystem()->GetDevice()->SetTransform( D3DTS_PROJECTION, &m_d3dMtx );
/*	Ok	*/
	PrintMatrix( "Frustum\nDirect3D : ",	&m_d3dMtx._11					);
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
	D3DXMatrixOrthoOffCenterRH( &m_d3dMtx, p_rLeft, p_rRight, p_rBottom, p_rTop, p_rNear, p_rFar );
	m_pDxPipeline->GetDxRenderSystem()->GetDevice()->SetTransform( D3DTS_PROJECTION, &m_d3dMtx );
/*	Ok	*/
	PrintMatrix( "Ortho\nDirect3D : ",	&m_d3dMtx._11					);
	PrintMatrix( "Internal : ",			m_pPipeline->GetMatrix( eMTXMODE_PROJECTION ).const_ptr()	);
/**/
#endif
	return true;
}

void DxPipelineImplHlsl :: ApplyViewport( int p_iWindowWidth, int p_iWindowHeight )
{
	m_viewport.Width	= p_iWindowWidth;
	m_viewport.Height	= p_iWindowHeight;
// 	m_viewport.Width	= GetSystemMetrics(	SM_CXFULLSCREEN );
// 	m_viewport.Height	= GetSystemMetrics(	SM_CYFULLSCREEN );
	HRESULT l_hr = m_pDxPipeline->GetDxRenderSystem()->GetDevice()->SetViewport( &m_viewport );
	dxCheckError( l_hr, "ApplyViewport" );
}

void DxPipelineImplHlsl :: DoApplyMatrix()
{
#if CASTOR_USE_DOUBLE
	Matrix4x4f l_mtx( GetCurrentMatrix().const_ptr() ); 
	m_d3dMtx = D3DXMATRIX( l_mtx.const_ptr() );
#else
	m_d3dMtx = D3DXMATRIX( GetCurrentMatrix().const_ptr() );
#endif
	HRESULT l_hr = m_pDxPipeline->GetDxRenderSystem()->GetDevice()->SetTransform( m_matrixTypes[GetCurrentMode()], &m_d3dMtx );
	dxCheckError( l_hr, "ApplyMatrix" );
}

//*************************************************************************************************

DxPipeline :: DxPipeline( DxRenderSystem * p_pRenderSystem )
	:	Pipeline( p_pRenderSystem )
	,	m_pDxRenderSystem( p_pRenderSystem )
{
	m_pPipelineImplHlsl	= new DxPipelineImplHlsl(	this );
	m_pPipelineImplCg = m_pDxRenderSystem->CreatePipeline( this, eSHADER_LANGUAGE_CG );
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
