#include "Pipeline.hpp"

#include "RenderSystem.hpp"
#include "Viewport.hpp"

#include "Scene/Camera.hpp"
#include "Scene/SceneNode.hpp"
#include "Shader/FrameVariableBuffer.hpp"
#include "Shader/MatrixFrameVariable.hpp"

#include <TransformationMatrix.hpp>

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	const String Pipeline::MtxProjection = cuT( "c3d_mtxProjection" );
	const String Pipeline::MtxModel = cuT( "c3d_mtxModel" );
	const String Pipeline::MtxView = cuT( "c3d_mtxView" );
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

	Pipeline::Pipeline( Context & p_context )
		: OwnedBy< Context >( p_context )
		, m_mtxIdentity( 1 )
		, m_mtxView( 1 )
		, m_mtxModel( 1 )
		, m_mtxProjection( 1 )
		, m_mtxNormal( 1 )
	{
	}

	Pipeline::~Pipeline()
	{
	}

	bool Pipeline::Project( Point3r const & p_ptObj, Point4r const & p_ptViewport, Point3r & p_result )
	{
		Point4r l_ptTmp( p_ptObj[0], p_ptObj[1], p_ptObj[2], real( 1 ) );
		l_ptTmp = GetViewMatrix() * l_ptTmp;
		l_ptTmp = GetProjectionMatrix() * l_ptTmp;
		l_ptTmp /= Point4r( l_ptTmp[3], l_ptTmp[3], l_ptTmp[3], l_ptTmp[3] );
		Point4r l_ptHalf = Point4r( 0.5, 0.5, 0.5, 0.5 );
		l_ptTmp *= real( 0.5 );
		l_ptTmp += l_ptHalf;
		l_ptTmp[0] = l_ptTmp[0] * p_ptViewport[2] + p_ptViewport[0];
		l_ptTmp[1] = l_ptTmp[1] * p_ptViewport[3] + p_ptViewport[1];
		p_result = Point3r( l_ptTmp.const_ptr() );
		return true;
	}

	bool Pipeline::UnProject( Point3i const & p_ptWin, Point4r const & p_ptViewport, Point3r & p_result )
	{
		Matrix4x4r l_mInverse = ( GetProjectionMatrix() * GetViewMatrix() ).get_inverse();
		Point4r l_ptTmp( ( real )p_ptWin[0], ( real )p_ptWin[1], ( real )p_ptWin[2], real( 1 ) );
		Point4r l_ptUnit( 1, 1, 1, 1 );
		l_ptTmp[0] = ( l_ptTmp[0] - p_ptViewport[0] ) / p_ptViewport[2];
		l_ptTmp[1] = ( l_ptTmp[1] - p_ptViewport[1] ) / p_ptViewport[3];
		l_ptTmp *= real( 2.0 );
		l_ptTmp -= l_ptUnit;
		Point4r l_ptObj;
		l_ptObj = l_mInverse.get_inverse() * l_ptTmp;
		l_ptObj /= l_ptObj[3];
		p_result = Point3r( l_ptObj.const_ptr() );
		return true;
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
		m_mtxNormal = Matrix4x4r{ ( m_mtxModel * m_mtxView ).get_minor( 3, 3 ).invert().transpose() };
		DoApplyMatrix( m_mtxNormal, MtxNormal, p_matrixBuffer );
	}

	void Pipeline::ApplyTexture( uint32_t p_index, FrameVariableBuffer & p_matrixBuffer )
	{
		REQUIRE( p_index < C3D_MAX_TEXTURE_MATRICES );
		DoApplyMatrix( m_mtxTexture[p_index], MtxTexture[p_index], p_matrixBuffer );
	}

	void Pipeline::ApplyMatrices( FrameVariableBuffer & p_matrixBuffer, uint64_t p_matrices )
	{
		if ( p_matrices & MASK_MTXMODE_PROJECTION )
		{
			ApplyProjection( p_matrixBuffer );
		}

		if ( p_matrices & MASK_MTXMODE_MODEL )
		{
			ApplyModel( p_matrixBuffer );
		}

		if ( p_matrices & MASK_MTXMODE_VIEW )
		{
			ApplyView( p_matrixBuffer );

			if ( p_matrices & MASK_MTXMODE_MODEL )
			{
				ApplyNormal( p_matrixBuffer );
			}
		}

		for ( uint32_t i = 0; i < C3D_MAX_TEXTURE_MATRICES; ++i )
		{
			if ( p_matrices & ( uint64_t( 0x1 ) << ( eMTXMODE_TEXTURE0 + i ) ) )
			{
				ApplyTexture( i, p_matrixBuffer );
			}
		}
	}

	void Pipeline::DoApplyMatrix( Castor::Matrix4x4r const & p_matrix, String const & p_name, FrameVariableBuffer & p_matrixBuffer )
	{
		Matrix4x4rFrameVariableSPtr l_variable;
		p_matrixBuffer.GetVariable( p_name, l_variable );

		if ( l_variable )
		{
			l_variable->SetValue( p_matrix );
		}
	}

	//*************************************************************************************************
}
