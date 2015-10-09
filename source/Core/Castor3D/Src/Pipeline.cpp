#include "Pipeline.hpp"

#include "Camera.hpp"
#include "FrameVariableBuffer.hpp"
#include "MatrixFrameVariable.hpp"
#include "RenderSystem.hpp"
#include "SceneNode.hpp"
#include "Viewport.hpp"

#include <TransformationMatrix.hpp>

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	const String Pipeline::MtxProjection = cuT( "c3d_mtxProjection" );
	const String Pipeline::MtxModel = cuT( "c3d_mtxModel" );
	const String Pipeline::MtxView = cuT( "c3d_mtxView" );
	const String Pipeline::MtxModelView = cuT( "c3d_mtxModelView" );
	const String Pipeline::MtxProjectionView = cuT( "c3d_mtxProjectionView" );
	const String Pipeline::MtxProjectionModelView = cuT( "c3d_mtxProjectionModelView" );
	const String Pipeline::MtxNormal = cuT( "c3d_mtxNormal" );
	const String Pipeline::MtxBones = cuT( "c3d_mtxBones" );
	const String Pipeline::MtxTexture[C3D_MAX_TEXTURE_MATRICES] = 
	{
		cuT( "c3d_mtxTexture0" ),
		cuT( "c3d_mtxTexture1" ),
		cuT( "c3d_mtxTexture2" ),
		cuT( "c3d_mtxTexture3" ),
		cuT( "c3d_mtxTexture4" ),
		cuT( "c3d_mtxTexture5" ),
		cuT( "c3d_mtxTexture6" ),
		cuT( "c3d_mtxTexture7" ),
		cuT( "c3d_mtxTexture8" ),
		cuT( "c3d_mtxTexture9" ),
	};

	//*************************************************************************************************

	Pipeline::Pipeline( RenderSystem & p_renderSystem )
		: OwnedBy< RenderSystem >( p_renderSystem )
		, m_mtxIdentity( 1 )
		, m_mtxView( 1 )
		, m_mtxModel( 1 )
		, m_mtxProjection( 1 )
		, m_mtxProjectionView( 1 )
		, m_mtxModelView( 1 )
		, m_mtxNormal( 1 )
		, m_mtxProjectionModelView( 1 )
	{
	}

	Pipeline::~Pipeline()
	{
	}

	void Pipeline::Initialise()
	{
		UpdateImpl();
	}

	void Pipeline::UpdateImpl()
	{
		m_impl = GetOwner()->GetPipelineImpl();
	}

	bool Pipeline::Project( Point3r const & p_ptObj, Point4r const & p_ptViewport, Point3r & p_ptResult )
	{
		bool l_bReturn = false;

		if ( GetOwner()->UseShaders() )
		{
			Point4r l_ptTmp( p_ptObj[0], p_ptObj[1], p_ptObj[2], real( 1 ) );
			l_ptTmp = GetViewMatrix() * l_ptTmp;
			l_ptTmp = GetModelMatrix() * l_ptTmp;
			l_ptTmp = GetProjectionMatrix() * l_ptTmp;
			l_ptTmp /= Point4r( l_ptTmp[3], l_ptTmp[3], l_ptTmp[3], l_ptTmp[3] );
			Point4r l_ptHalf = Point4r( 0.5, 0.5, 0.5, 0.5 );
			l_ptTmp *= real( 0.5 );
			l_ptTmp += l_ptHalf;
			l_ptTmp[0] = l_ptTmp[0] * p_ptViewport[2] + p_ptViewport[0];
			l_ptTmp[1] = l_ptTmp[1] * p_ptViewport[3] + p_ptViewport[1];
			p_ptResult = Point3r( l_ptTmp.const_ptr() );
			l_bReturn = true;
		}

		return l_bReturn;
	}

	bool Pipeline::UnProject( Point3i const & p_ptWin, Point4r const & p_ptViewport, Point3r & p_ptResult )
	{
		bool l_bReturn = false;

		if ( GetOwner()->UseShaders() )
		{
			Matrix4x4r l_mInverse = ( GetProjectionMatrix() * GetModelMatrix() * GetViewMatrix() ).get_inverse();
			Point4r l_ptTmp( ( real )p_ptWin[0], ( real )p_ptWin[1], ( real )p_ptWin[2], real( 1 ) );
			Point4r l_ptUnit( 1, 1, 1, 1 );
			l_ptTmp[0] = ( l_ptTmp[0] - p_ptViewport[0] ) / p_ptViewport[2];
			l_ptTmp[1] = ( l_ptTmp[1] - p_ptViewport[1] ) / p_ptViewport[3];
			l_ptTmp *= real( 2.0 );
			l_ptTmp -= l_ptUnit;
			Point4r l_ptObj;
			l_ptObj = l_mInverse.get_inverse() * l_ptTmp;
			l_ptObj /= l_ptObj[3];
			p_ptResult = Point3r( l_ptObj.const_ptr() );
			l_bReturn = true;
		}

		return l_bReturn;
	}

	void Pipeline::ApplyProjection( FrameVariableBuffer & p_matrixBuffer )
	{
		DoApplyMatrix( m_mtxProjection, MtxProjection, p_matrixBuffer );
	}

	void Pipeline::ApplyModel( FrameVariableBuffer & p_matrixBuffer )
	{
		DoApplyMatrix( m_mtxModel, MtxModel, p_matrixBuffer );
	}

	void Pipeline::ApplyView( FrameVariableBuffer & p_matrixBuffer )
	{
		DoApplyMatrix( m_mtxView, MtxView, p_matrixBuffer );
	}

	void Pipeline::ApplyNormal( FrameVariableBuffer & p_matrixBuffer )
	{
		m_mtxNormal = m_mtxModelView.get_minor( 3, 3 ).invert().transpose();
		DoApplyMatrix( m_mtxNormal, MtxNormal, p_matrixBuffer );
	}

	void Pipeline::ApplyModelView( FrameVariableBuffer & p_matrixBuffer )
	{
		m_mtxModelView = m_mtxView * m_mtxModel;
		DoApplyMatrix( m_mtxModelView, MtxModelView, p_matrixBuffer );
	}

	void Pipeline::ApplyProjectionModelView( FrameVariableBuffer & p_matrixBuffer )
	{
		m_mtxProjectionModelView = m_mtxModelView * m_mtxProjection;
		DoApplyMatrix( m_mtxProjectionModelView, MtxProjectionModelView, p_matrixBuffer );
	}

	void Pipeline::ApplyProjectionView( FrameVariableBuffer & p_matrixBuffer )
	{
		m_mtxProjectionView = m_mtxView * m_mtxProjection;
		DoApplyMatrix( m_mtxProjectionView, MtxProjectionView, p_matrixBuffer );
	}

	void Pipeline::ApplyTexture( uint32_t p_index, FrameVariableBuffer & p_matrixBuffer )
	{
		CASTOR_ASSERT( p_index < C3D_MAX_TEXTURE_MATRICES );
		DoApplyMatrix( m_mtxTexture[p_index], MtxTexture[p_index], p_matrixBuffer );
	}

	void Pipeline::ApplyMatrices( FrameVariableBuffer & p_matrixBuffer, uint64_t p_matrices )
	{
		if ( p_matrices & MASK_MTXMODE_PROJECTION )
		{
			ApplyProjection( p_matrixBuffer );

			if ( ( p_matrices & MASK_MTXMODE_MODEL ) )
			{
				ApplyModel( p_matrixBuffer );
			}

			if ( ( p_matrices & MASK_MTXMODE_VIEW ) )
			{
				ApplyView( p_matrixBuffer );
				ApplyProjectionView( p_matrixBuffer );

				if ( ( p_matrices & MASK_MTXMODE_MODEL ) )
				{
					ApplyModelView( p_matrixBuffer );
					ApplyProjectionModelView( p_matrixBuffer );
					ApplyNormal( p_matrixBuffer );
				}
			}
		}
		else if ( p_matrices & MASK_MTXMODE_MODEL )
		{
			ApplyModel( p_matrixBuffer );

			if ( ( p_matrices & MASK_MTXMODE_VIEW ) )
			{
				ApplyModelView( p_matrixBuffer );
				ApplyNormal( p_matrixBuffer );
			}
		}
		else if ( p_matrices & MASK_MTXMODE_VIEW )
		{
			ApplyView( p_matrixBuffer );
		}

		for ( uint32_t i = 0; i < C3D_MAX_TEXTURE_MATRICES; ++i )
		{
			if ( p_matrices & ( uint64_t( 0x1 ) << ( eMTXMODE_TEXTURE0 + i ) ) )
			{
				ApplyTexture( i, p_matrixBuffer );
			}
		}
	}

	void Pipeline::ApplyViewport( int p_iWindowWidth, int p_iWindowHeight )
	{
		IPipelineImplSPtr l_impl = DoGetImpl();

		if ( l_impl )
		{
			l_impl->ApplyViewport( p_iWindowWidth, p_iWindowHeight );
		}
	}

	void Pipeline::Perspective( Angle const & p_aFOVY, real p_aspect, real p_near, real p_far )
	{
		IPipelineImplSPtr l_impl = DoGetImpl();

		if ( l_impl )
		{
			l_impl->Perspective( m_mtxProjection, p_aFOVY, p_aspect, p_near, p_far );
		}
	}

	void Pipeline::Frustum( real p_left, real p_right, real p_bottom, real p_top, real p_near, real p_far )
	{
		IPipelineImplSPtr l_impl = DoGetImpl();

		if ( l_impl )
		{
			l_impl->Frustum( m_mtxProjection, p_left, p_right, p_bottom, p_top, p_near, p_far );
		}
	}

	void Pipeline::Ortho( real p_left, real p_right, real p_bottom, real p_top, real p_near, real p_far )
	{
		IPipelineImplSPtr l_impl = DoGetImpl();

		if ( l_impl )
		{
			l_impl->Ortho( m_mtxProjection, p_left, p_right, p_bottom, p_top, p_near, p_far );
		}
	}

	void Pipeline::LookAt( Point3r const & p_eye, Point3r const & p_center, Point3r const & p_up )
	{
		IPipelineImplSPtr l_impl = DoGetImpl();

		if ( l_impl )
		{
			l_impl->LookAt( m_mtxProjection, p_eye, p_center, p_up );
		}
	}

	void Pipeline::DoApplyMatrix( Castor::Matrix4x4r const & p_mtxMatrix, String const & p_name, FrameVariableBuffer & p_matrixBuffer )
	{
		IPipelineImplSPtr l_impl = DoGetImpl();

		if ( l_impl )
		{
			l_impl->ApplyMatrix( p_mtxMatrix, p_name, p_matrixBuffer );
		}
	}

	//*************************************************************************************************

	IPipelineImpl::IPipelineImpl( Pipeline & p_pipeline, bool p_rightHanded )
		: m_pipeline( p_pipeline )
		, m_rightHanded( p_rightHanded )
	{
	}

	IPipelineImpl::~IPipelineImpl()
	{
	}

	void IPipelineImpl::ApplyMatrix( Castor::Matrix4x4r const & p_matrix, Castor::String const & p_name, FrameVariableBuffer & p_matrixBuffer )
	{
		Matrix4x4rFrameVariableSPtr l_pVariable;
		p_matrixBuffer.GetVariable( p_name, l_pVariable );

		if ( l_pVariable )
		{
			l_pVariable->SetValue( p_matrix );
		}
	}

	//*************************************************************************************************
}
