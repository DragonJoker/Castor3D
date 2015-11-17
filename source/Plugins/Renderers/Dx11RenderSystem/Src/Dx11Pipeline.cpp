#include "Dx11Pipeline.hpp"

#include <FrameVariableBuffer.hpp>
#include <MatrixFrameVariable.hpp>
#include <TransformationMatrix.hpp>

#include "Dx11Context.hpp"
#include "Dx11RenderSystem.hpp"

using namespace Castor3D;
using namespace Castor;

namespace Dx11Render
{
	DxPipelineImpl::DxPipelineImpl( Pipeline & p_pipeline )
		: IPipelineImpl( p_pipeline, false )
		, m_viewport()
	{
	}

	DxPipelineImpl::~DxPipelineImpl()
	{
	}

	void DxPipelineImpl::ApplyMatrix( Castor::Matrix4x4r const & p_matrix, Castor::String const & p_name, FrameVariableBuffer & p_matrixBuffer )
	{
		Matrix4x4rFrameVariableSPtr l_pVariable;
		p_matrixBuffer.GetVariable( p_name, l_pVariable );

		if ( l_pVariable )
		{
			l_pVariable->SetValue( p_matrix );
		}
	}

	void DxPipelineImpl::ApplyViewport( int p_iWindowWidth, int p_iWindowHeight )
	{
		m_viewport.Width = FLOAT( p_iWindowWidth );
		m_viewport.Height = FLOAT( p_iWindowHeight );
		m_viewport.MinDepth = 0.0f;
		m_viewport.MaxDepth = 1.0f;
		m_viewport.TopLeftX = 0.0f;
		m_viewport.TopLeftY = 0.0f;
		ID3D11DeviceContext * l_deviceContext = static_cast< DxContext * >( m_pipeline.GetOwner()->GetCurrentContext() )->GetDeviceContext();
		l_deviceContext->RSSetViewports( 1, &m_viewport );
	}

	void DxPipelineImpl::Perspective( Matrix4x4r & p_result, Angle const & p_aFOVY, real p_aspect, real p_near, real p_far )
	{
		// DirectX left handed (cf. https://msdn.microsoft.com/en-us/library/windows/desktop/bb205350%28v=vs.85%29.aspx)
		real l_range = real( ( 1 / tan( p_aFOVY.radians() * 0.5 ) ) );
		p_result.initialise();
		p_result[0][0] = real( l_range / p_aspect );
		p_result[1][1] = real( l_range );
		p_result[2][2] = real( p_far / ( p_far - p_near ) );
		p_result[2][3] = real( -p_near * p_far / ( p_far - p_near ) );
		p_result[3][2] = real( 1 );
	}

	void DxPipelineImpl::Frustum( Matrix4x4r & p_result, real p_left, real p_right, real p_bottom, real p_top, real p_near, real p_far )
	{
		// DirectX left handed (cf. https://msdn.microsoft.com/en-us/library/windows/desktop/bb205353%28v=vs.85%29.aspx)
		p_result.initialise();
		p_result[0][0] = real( ( 2 * p_near ) / ( p_right - p_left ) );
		p_result[0][2] = real( ( p_right + p_left ) / ( p_left - p_right ) );
		p_result[1][1] = real( ( 2 * p_near ) / ( p_top - p_bottom ) );
		p_result[1][2] = real( ( p_top + p_bottom ) / ( p_bottom - p_top ) );
		p_result[2][2] = real( p_far / ( p_far - p_near ) );
		p_result[2][3] = real( ( p_far * p_near ) / ( p_near - p_far ) );
		p_result[3][2] = real( 1 );
	}

	void DxPipelineImpl::Ortho( Matrix4x4r & p_result, real p_left, real p_right, real p_bottom, real p_top, real p_near, real p_far )
	{
		// DirectX left handed (cf. https://msdn.microsoft.com/en-us/library/windows/desktop/bb205347%28v=vs.85%29.aspx)
		p_result.set_identity();
		p_result[0][0] = real( 2 / ( p_right - p_left ) );
		p_result[0][3] = real( ( p_left + p_right ) / ( p_left - p_right ) );
		p_result[1][1] = real( 2 / ( p_top - p_bottom ) );
		p_result[1][3] = real( ( p_bottom + p_top ) / ( p_bottom - p_top ) );
		p_result[2][2] = real( 1 / ( p_far - p_near ) );
		p_result[2][3] = real( p_near / ( p_near - p_far ) );
	}

	void DxPipelineImpl::LookAt( Matrix4x4r & p_result, Point3r const & p_eye, Point3r const & p_center, Point3r const & p_up )
	{
		// DirectX left handed (cf. https://msdn.microsoft.com/en-us/library/windows/desktop/bb205342%28v=vs.85%29.aspx)
		Point3r l_f( point::get_normalised( p_center - p_eye ) );
		Point3r l_u( point::get_normalised( p_up ) );
		Point3r l_s( point::get_normalised( l_f ^ l_u ) );
		l_u = l_s ^ l_f;
		p_result.set_identity();
		p_result[0][0] = l_s[0];
		p_result[0][1] = l_s[1];
		p_result[0][2] = l_s[2];
		p_result[0][3] = -point::dot( l_s, p_eye );
		p_result[1][0] = l_u[0];
		p_result[1][1] = l_u[1];
		p_result[1][2] = l_u[2];
		p_result[1][3] = -point::dot( l_u, p_eye );
		p_result[2][0] = l_f[0];
		p_result[2][1] = l_f[1];
		p_result[2][2] = l_f[2];
		p_result[2][3] = -point::dot( l_f, p_eye );
	}
}
