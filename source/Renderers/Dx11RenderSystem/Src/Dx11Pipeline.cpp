#include "Dx11Pipeline.hpp"
#include "Dx11ShaderProgram.hpp"
#include "Dx11ShaderObject.hpp"
#include "Dx11Context.hpp"
#include "Dx11RenderSystem.hpp"

#include <Logger.hpp>
#include <Angle.hpp>
#include <TransformationMatrix.hpp>

using namespace Castor3D;
using namespace Castor;
using namespace Dx11Render;

#define C3D_DIRECTX_MTX 0

#if C3D_DIRECTX_MTX
#	include <d3dx10math.h>
#endif

//*************************************************************************************************

namespace
{
	void PrintMatrix( std::string const & p_strHeader, float const * p_mtx )
	{
		std::stringstream l_stream;
		l_stream.precision( 5 );
		l_stream << p_strHeader << std::endl;

		for ( int i = 0; i < 4; i++ )
		{
			for ( int j = 0; j < 4; j++ )
			{
				l_stream << p_mtx[i * 4 + j] << " ";
			}

			l_stream << std::endl;
		}

		Logger::LogDebug( l_stream.str() );
	}
}

//*************************************************************************************************

DxPipelineImplHlsl::DxPipelineImplHlsl( DxPipeline * p_pPipeline )
	: IPipelineImpl( p_pPipeline )
	, m_pDxPipeline( p_pPipeline )
	, m_viewport()
{
	m_viewport.MaxDepth = 1.0f;
}

DxPipelineImplHlsl::~DxPipelineImplHlsl()
{
}

eMTXMODE DxPipelineImplHlsl::MatrixMode( eMTXMODE p_eMode )
{
	return IPipelineImpl::MatrixMode( p_eMode );
}

bool DxPipelineImplHlsl::LoadIdentity()
{
	GetCurrentMatrix().set_identity();
#if C3D_DIRECTX_MTX
	D3DXMATRIX l_d3dMtx;
	D3DXMatrixIdentity( &l_d3dMtx );
	memcpy( GetCurrentMatrix().ptr(), &l_d3dMtx, 16 * sizeof( float ) );
#endif
	return true;
}

bool DxPipelineImplHlsl::PushMatrix()
{
	IPipelineImpl::PushMatrix();
	return true;
}

bool DxPipelineImplHlsl::PopMatrix()
{
	IPipelineImpl::PopMatrix();
	return true;
}

bool DxPipelineImplHlsl::MultMatrix( Matrix4x4r const & p_matrix )
{
	IPipelineImpl::MultMatrix( p_matrix.get_transposed() );
#if C3D_DIRECTX_MTX
	D3DXMATRIX l_d3dMtx;
	l_d3dMtx *= D3DXMATRIX( p_matrix.get_transposed().const_ptr() );
	memcpy( GetCurrentMatrix().ptr(), &l_d3dMtx, 16 * sizeof( float ) );
	/*	Ok	*/
	PrintMatrix( "MultMatrix\nDirect3D : ", &l_d3dMtx._11 );
	PrintMatrix( "Internal : ", GetCurrentMatrix().const_ptr() );
	/**/
#endif
	return true;
}

bool DxPipelineImplHlsl::MultMatrix( real const * p_matrix )
{
	return MultMatrix( Matrix4x4r( p_matrix ) );
}

bool DxPipelineImplHlsl::Perspective( Angle const & p_aFOVY, real p_rRatio, real p_rNear, real p_rFar )
{
	IPipelineImpl::Perspective( p_aFOVY, p_rRatio, p_rNear, p_rFar );
#if C3D_DIRECTX_MTX
	D3DXMATRIX l_d3dMtx;
	D3DXMatrixPerspectiveFovRH( &l_d3dMtx, p_aFOVY.Radians(), p_rRatio, p_rNear, p_rFar );
	PrintMatrix( "Perspective\nInternal : ", m_pPipeline->GetMatrix( eMTXMODE_PROJECTION ).const_ptr() );
	memcpy( m_pPipeline->GetMatrix( eMTXMODE_PROJECTION ).ptr(), &l_d3dMtx, 16 * sizeof( float ) );
	/*	Ok	*/
	PrintMatrix( "Direct3D : ", &l_d3dMtx._11 );
	PrintMatrix( "Internal : ", m_pPipeline->GetMatrix( eMTXMODE_PROJECTION ).const_ptr() );
	/**/
#endif
	return true;
}

bool DxPipelineImplHlsl::Frustum( real p_rLeft, real p_rRight, real p_rBottom, real p_rTop, real p_rNear, real p_rFar )
{
	IPipelineImpl::Frustum( p_rLeft, p_rRight, p_rBottom, p_rTop, p_rNear, p_rFar );
#if C3D_DIRECTX_MTX
	D3DXMATRIX l_d3dMtx;
	D3DXMatrixPerspectiveOffCenterRH( &l_d3dMtx, p_rLeft, p_rRight, p_rBottom, p_rTop, p_rNear, p_rFar );
	PrintMatrix( "Frustum\nInternal : ", m_pPipeline->GetMatrix( eMTXMODE_PROJECTION ).const_ptr() );
	memcpy( m_pPipeline->GetMatrix( eMTXMODE_PROJECTION ).ptr(), &l_d3dMtx, 16 * sizeof( float ) );
	/*	Ok	*/
	PrintMatrix( "Direct3D : ", &l_d3dMtx._11 );
	PrintMatrix( "Internal : ", m_pPipeline->GetMatrix( eMTXMODE_PROJECTION ).const_ptr() );
	/**/
#endif
	return true;
}

bool DxPipelineImplHlsl::Ortho( real p_rLeft, real p_rRight, real p_rBottom, real p_rTop, real p_rNear, real p_rFar )
{
	IPipelineImpl::Ortho( p_rLeft, p_rRight, p_rBottom, p_rTop, p_rNear, p_rFar );
#if C3D_DIRECTX_MTX
	D3DXMATRIX l_d3dMtx;
	D3DXMatrixOrthoOffCenterRH( &l_d3dMtx, p_rLeft, p_rRight, p_rBottom, p_rTop, p_rNear, p_rFar );
	PrintMatrix( "Ortho\nInternal : ", m_pPipeline->GetMatrix( eMTXMODE_PROJECTION ).const_ptr() );
	memcpy( m_pPipeline->GetMatrix( eMTXMODE_PROJECTION ).ptr(), &l_d3dMtx, 16 * sizeof( float ) );
	/*	Ok	*/
	PrintMatrix( "Direct3D : ", &l_d3dMtx._11 );
	PrintMatrix( "Internal : ", m_pPipeline->GetMatrix( eMTXMODE_PROJECTION ).const_ptr() );
	/**/
#endif
	return true;
}

void DxPipelineImplHlsl::ApplyViewport( int p_iWindowWidth, int p_iWindowHeight )
{
	m_viewport.Width = FLOAT( p_iWindowWidth );
	m_viewport.Height = FLOAT( p_iWindowHeight );
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;
	ID3D11DeviceContext * l_pDeviceContext = static_cast< DxContext * >( m_pDxPipeline->GetDxRenderSystem()->GetCurrentContext() )->GetDeviceContext();
	l_pDeviceContext->RSSetViewports( 1, &m_viewport );
}

void DxPipelineImplHlsl::DoApplyMatrix()
{
}

//*************************************************************************************************

DxPipeline::DxPipeline( DxRenderSystem * p_pRenderSystem )
	: Pipeline( p_pRenderSystem	)
	, m_pDxRenderSystem( p_pRenderSystem	)
{
	m_pPipelineImplHlsl	= new DxPipelineImplHlsl( this );
}

DxPipeline::~DxPipeline()
{
	delete m_pPipelineImplHlsl;
}

void DxPipeline::Initialise()
{
	UpdateFunctions( NULL );
}

void DxPipeline::UpdateFunctions( ShaderProgramBase * p_pProgram )
{
	m_pPipelineImpl = m_pPipelineImplHlsl;
}

//*************************************************************************************************
