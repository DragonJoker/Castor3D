#include "Pipeline.hpp"
#include "RenderSystem.hpp"
#include "ShaderProgram.hpp"
#include "FrameVariableBuffer.hpp"
#include "MatrixFrameVariable.hpp"

#include <TransformationMatrix.hpp>

using namespace Castor3D;
using namespace Castor;

//*************************************************************************************************

const String Pipeline::MtxProjection			= cuT( "c3d_mtxProjection" );
const String Pipeline::MtxModel					= cuT( "c3d_mtxModel" );
const String Pipeline::MtxView					= cuT( "c3d_mtxView" );
const String Pipeline::MtxModelView				= cuT( "c3d_mtxModelView" );
const String Pipeline::MtxProjectionView		= cuT( "c3d_mtxProjectionView" );
const String Pipeline::MtxProjectionModelView	= cuT( "c3d_mtxProjectionModelView" );
const String Pipeline::MtxNormal				= cuT( "c3d_mtxNormal" );
const String Pipeline::MtxTexture0				= cuT( "c3d_mtxTexture0" );
const String Pipeline::MtxTexture1				= cuT( "c3d_mtxTexture1" );
const String Pipeline::MtxTexture2				= cuT( "c3d_mtxTexture2" );
const String Pipeline::MtxTexture3				= cuT( "c3d_mtxTexture3" );
const String Pipeline::MtxBones					= cuT( "c3d_mtxBones" );

//*************************************************************************************************

Pipeline::Pipeline( RenderSystem * p_pRenderSystem )
	:	m_pRenderSystem( p_pRenderSystem )
	,	m_eCurrentMode( eMTXMODE_COUNT )
	,	m_pPipelineImpl( NULL )
	,	m_mtxIdentity( 1.0 )
{
	for ( int i = 0; i < eMTXMODE_COUNT; i++ )
	{
		m_matrix[i].push( m_mtxIdentity );
	}
}

Pipeline::~Pipeline()
{
}

Pipeline::matrix4x4 const & Pipeline::GetMatrix( eMTXMODE p_eMode )const
{
	return m_matrix[p_eMode].top();
}

Pipeline::matrix4x4 & Pipeline::GetMatrix( eMTXMODE p_eMode )
{
	return m_matrix[p_eMode].top();
}

eMTXMODE Pipeline::MatrixMode( eMTXMODE p_eMode )
{
	return m_pPipelineImpl->MatrixMode( p_eMode );
}

bool Pipeline::LoadIdentity()
{
	return m_pPipelineImpl->LoadIdentity();
}

bool Pipeline::PushMatrix()
{
	return m_pPipelineImpl->PushMatrix();
}

bool Pipeline::PopMatrix()
{
	return m_pPipelineImpl->PopMatrix();
}

bool Pipeline::Translate( Point3r const & p_translate )
{
	return m_pPipelineImpl->Translate( p_translate );
}

bool Pipeline::Rotate( Quaternion const & p_rotate )
{
	return m_pPipelineImpl->Rotate( p_rotate );
}

bool Pipeline::Scale( Point3r const & p_scale )
{
	return m_pPipelineImpl->Scale( p_scale );
}

bool Pipeline::MultMatrix( Matrix4x4r const & p_matrix )
{
	return m_pPipelineImpl->MultMatrix( p_matrix );
}

bool Pipeline::MultMatrix( real const * p_matrix )
{
	return m_pPipelineImpl->MultMatrix( p_matrix );
}

bool Pipeline::Perspective( Angle const & p_aFOVY, real p_rRatio, real p_rNear, real p_rFar )
{
	return m_pPipelineImpl->Perspective( p_aFOVY, p_rRatio, p_rNear, p_rFar );
}

bool Pipeline::Frustum( real p_rLeft, real p_rRight, real p_rBottom, real p_rTop, real p_rNear, real p_rFar )
{
	return m_pPipelineImpl->Frustum( p_rLeft, p_rRight, p_rBottom, p_rTop, p_rNear, p_rFar );
}

bool Pipeline::Ortho( real p_rLeft, real p_rRight, real p_rBottom, real p_rTop, real p_rNear, real p_rFar )
{
	return m_pPipelineImpl->Ortho( p_rLeft, p_rRight, p_rBottom, p_rTop, p_rNear, p_rFar );
}

bool Pipeline::Project( Point3r const & p_ptObj, Point4r const & p_ptViewport, Point3r & p_ptResult )
{
	return m_pPipelineImpl->Project( p_ptObj, p_ptViewport, p_ptResult );
}

bool Pipeline::UnProject( Point3i const & p_ptWin, Point4r const & p_ptViewport, Point3r & p_ptResult )
{
	return m_pPipelineImpl->UnProject( p_ptWin, p_ptViewport, p_ptResult );
}

bool Pipeline::PickMatrix( real x, real y, real width, real height, int viewport[] )
{
	return m_pPipelineImpl->PickMatrix( x, y, width, height, viewport );
}

void Pipeline::ApplyProjection( ShaderProgramBase & p_program )
{
	DoApplyMatrix( eMTXMODE_PROJECTION, MtxProjection, p_program );
}

void Pipeline::ApplyModel( ShaderProgramBase & p_program )
{
	DoApplyMatrix( eMTXMODE_MODEL, MtxModel, p_program );
}

void Pipeline::ApplyView( ShaderProgramBase & p_program )
{
	DoApplyMatrix( eMTXMODE_VIEW, MtxView, p_program );
}

void Pipeline::ApplyNormal( ShaderProgramBase & p_program )
{
	m_mtxNormal = m_mtxModelView.get_minor( 3, 3 ).invert().transpose();
	DoApplyMatrix( m_mtxNormal, MtxNormal, p_program );
}

void Pipeline::ApplyModelView( ShaderProgramBase & p_program )
{
	// Model view must always be computed because normal and projection model view are computed from it
	m_mtxModelView = GetMatrix( eMTXMODE_VIEW ) * GetMatrix( eMTXMODE_MODEL );
	DoApplyMatrix( m_mtxModelView, MtxModelView, p_program );
}

void Pipeline::ApplyProjectionModelView( ShaderProgramBase & p_program )
{
	m_mtxProjectionModelView = GetMatrix( eMTXMODE_PROJECTION ) * m_mtxModelView;
	DoApplyMatrix( m_mtxProjectionModelView, MtxProjectionModelView, p_program );
}

void Pipeline::ApplyProjectionView( ShaderProgramBase & p_program )
{
	m_mtxProjectionView = GetMatrix( eMTXMODE_PROJECTION ) * GetMatrix( eMTXMODE_VIEW );
	DoApplyMatrix( m_mtxProjectionView, MtxProjectionView, p_program );
}

void Pipeline::ApplyTexture0( ShaderProgramBase & p_program )
{
	DoApplyMatrix( eMTXMODE_TEXTURE0, MtxTexture0, p_program );
}

void Pipeline::ApplyTexture1( ShaderProgramBase & p_program )
{
	DoApplyMatrix( eMTXMODE_TEXTURE1, MtxTexture1, p_program );
}

void Pipeline::ApplyTexture2( ShaderProgramBase & p_program )
{
	DoApplyMatrix( eMTXMODE_TEXTURE2, MtxTexture2, p_program );
}

void Pipeline::ApplyTexture3( ShaderProgramBase & p_program )
{
	DoApplyMatrix( eMTXMODE_TEXTURE3, MtxTexture3, p_program );
}

void Pipeline::ApplyMatrices( ShaderProgramBase & p_program )
{
	ApplyProjection(	p_program );
	ApplyModel(	p_program );
	ApplyView(	p_program );
	ApplyModelView(	p_program );
	ApplyProjectionView(	p_program );
	ApplyTexture0(	p_program );
	ApplyTexture1(	p_program );
	ApplyTexture2(	p_program );
	ApplyTexture3(	p_program );
	// Those 2 ones must be applied after model view because they're computed from it
	ApplyNormal(	p_program );
	ApplyProjectionModelView(	p_program );
}

void Pipeline::ApplyViewport( int p_iWindowWidth, int p_iWindowHeight )
{
	m_pPipelineImpl->ApplyViewport( p_iWindowWidth, p_iWindowHeight );
}

void Pipeline::DoApplyMatrix( eMTXMODE p_eMatrix, String const & p_strName, ShaderProgramBase & p_program )
{
	DoApplyMatrix( m_matrix[p_eMatrix].top(), p_strName, p_program );
}

void Pipeline::DoApplyMatrix( matrix4x4 const & p_mtxMatrix, String const & p_strName, ShaderProgramBase & p_program )
{
	Matrix4x4rFrameVariableSPtr l_pVariable;
	p_program.GetMatrixBuffer()->GetVariable( p_strName, l_pVariable );

	if ( l_pVariable )
	{
		if ( m_pRenderSystem->NeedsMatrixTransposition() )
		{
			l_pVariable->SetValue( p_mtxMatrix.get_transposed() );
		}
		else
		{
			l_pVariable->SetValue( p_mtxMatrix );
		}
	}
}

//*************************************************************************************************

IPipelineImpl::IPipelineImpl( Pipeline * p_pPipeline )
	:	m_pPipeline( p_pPipeline )
{
}

IPipelineImpl::~IPipelineImpl()
{
}

void IPipelineImpl::Initialise()
{
}

eMTXMODE IPipelineImpl::MatrixMode( eMTXMODE p_eMode )
{
	eMTXMODE l_eReturn = eMTXMODE_COUNT;

	if ( p_eMode < eMTXMODE_COUNT )
	{
		l_eReturn = m_pPipeline->m_eCurrentMode;
		m_pPipeline->m_eCurrentMode = p_eMode;
		l_eReturn = ( m_pPipeline->m_pRenderSystem->UseShaders() ? l_eReturn : eMTXMODE_COUNT );
	}
	else
	{
		throw std::range_error( "Pipeline::MatrixMode - Invalid pipeline mode given" );
	}

	return l_eReturn;
}

bool IPipelineImpl::LoadIdentity()
{
	bool l_bReturn = false;

	if ( m_pPipeline->m_eCurrentMode < eMTXMODE_COUNT )
	{
		if ( m_pPipeline->m_pRenderSystem->UseShaders() )
		{
			m_pPipeline->m_matrix[m_pPipeline->m_eCurrentMode].top().set_identity();
			l_bReturn = true;
		}
	}
	else
	{
		throw std::range_error( "Pipeline::LoadIdentity - Invalid pipeline current mode" );
	}

	return l_bReturn;
}

bool IPipelineImpl::PushMatrix()
{
	bool l_bReturn = false;

	if ( m_pPipeline->m_eCurrentMode < eMTXMODE_COUNT )
	{
		if ( m_pPipeline->m_pRenderSystem->UseShaders() )
		{
			m_pPipeline->m_matrix[m_pPipeline->m_eCurrentMode].push( m_pPipeline->m_matrix[m_pPipeline->m_eCurrentMode].top() );
			l_bReturn = true;
		}
	}
	else
	{
		throw std::range_error( "Pipeline::PushMatrix - Invalid pipeline current mode" );
	}

	return l_bReturn;
}

bool IPipelineImpl::PopMatrix()
{
	bool l_bReturn = false;

	if ( m_pPipeline->m_eCurrentMode < eMTXMODE_COUNT )
	{
		if ( m_pPipeline->m_pRenderSystem->UseShaders() )
		{
			m_pPipeline->m_matrix[m_pPipeline->m_eCurrentMode].pop();
			l_bReturn = true;
		}
	}
	else
	{
		throw std::range_error( "Pipeline::PopMatrix - Invalid pipeline current mode" );
	}

	return l_bReturn;
}

bool IPipelineImpl::Translate( Point3r const & p_translate )
{
	bool l_bReturn = false;

	if ( m_pPipeline->m_eCurrentMode < eMTXMODE_COUNT )
	{
		if ( m_pPipeline->m_pRenderSystem->UseShaders() )
		{
			MtxUtils::translate( m_pPipeline->m_matrix[m_pPipeline->m_eCurrentMode].top(), p_translate );
			l_bReturn = true;
		}
	}
	else
	{
		throw std::range_error( "Pipeline::Translate - Invalid pipeline current mode" );
	}

	return l_bReturn;
}

bool IPipelineImpl::Rotate( Quaternion const & p_rotate )
{
	bool l_bReturn = false;

	if ( m_pPipeline->m_eCurrentMode < eMTXMODE_COUNT )
	{
		if ( m_pPipeline->m_pRenderSystem->UseShaders() )
		{
			p_rotate.ToRotationMatrix( m_pPipeline->m_matTmp );
			m_pPipeline->m_matrix[m_pPipeline->m_eCurrentMode].top() = m_pPipeline->m_matrix[m_pPipeline->m_eCurrentMode].top() * m_pPipeline->m_matTmp;
			l_bReturn = true;
		}
	}
	else
	{
		throw std::range_error( "Pipeline::Rotate - Invalid pipeline current mode" );
	}

	return l_bReturn;
}

bool IPipelineImpl::Scale( Point3r const & p_scale )
{
	bool l_bReturn = false;

	if ( m_pPipeline->m_eCurrentMode < eMTXMODE_COUNT )
	{
		if ( m_pPipeline->m_pRenderSystem->UseShaders() )
		{
			MtxUtils::scale( m_pPipeline->m_matrix[m_pPipeline->m_eCurrentMode].top(), p_scale );
			l_bReturn = true;
		}
	}
	else
	{
		throw std::range_error( "Pipeline::Scale - Invalid pipeline current mode" );
	}

	return l_bReturn;
}

bool IPipelineImpl::MultMatrix( Matrix4x4r const & p_matrix )
{
	bool l_bReturn = false;

	if ( m_pPipeline->m_eCurrentMode < eMTXMODE_COUNT )
	{
		if ( m_pPipeline->m_pRenderSystem->UseShaders() )
		{
			m_pPipeline->m_matrix[m_pPipeline->m_eCurrentMode].top() = m_pPipeline->m_matrix[m_pPipeline->m_eCurrentMode].top() * p_matrix;
			l_bReturn = true;
		}
	}
	else
	{
		throw std::range_error( "Pipeline::MultMatrix - Invalid pipeline current mode" );
	}

	return l_bReturn;
}

bool IPipelineImpl::MultMatrix( real const * p_matrix )
{
	return MultMatrix( Matrix4x4r( p_matrix ) );
}

bool IPipelineImpl::Perspective( Angle const & p_aFOVY, real p_rRatio, real p_rNear, real p_rFar )
{
	bool l_bReturn = false;

	if ( m_pPipeline->m_pRenderSystem->UseShaders() )
	{
		MtxUtils::perspective( m_pPipeline->m_matrix[eMTXMODE_PROJECTION].top(), p_aFOVY, p_rRatio, p_rNear, p_rFar );
		l_bReturn = true;
	}

	return l_bReturn;
}

bool IPipelineImpl::Frustum( real p_rLeft, real p_rRight, real p_rBottom, real p_rTop, real p_rNear, real p_rFar )
{
	bool l_bReturn = false;

	if ( m_pPipeline->m_pRenderSystem->UseShaders() )
	{
		MtxUtils::frustum( m_pPipeline->m_matrix[eMTXMODE_PROJECTION].top(), p_rLeft, p_rRight, p_rBottom, p_rTop, p_rNear, p_rFar );
		l_bReturn = true;
	}

	return l_bReturn;
}

bool IPipelineImpl::Ortho( real p_rLeft, real p_rRight, real p_rBottom, real p_rTop, real p_rNear, real p_rFar )
{
	bool l_bReturn = false;

	if ( m_pPipeline->m_pRenderSystem->UseShaders() )
	{
		MtxUtils::ortho( m_pPipeline->m_matrix[eMTXMODE_PROJECTION].top(), p_rLeft, p_rRight, p_rBottom, p_rTop, p_rNear, p_rFar );
		l_bReturn = true;
	}

	return l_bReturn;
}

bool IPipelineImpl::Project( Point3r const & p_ptObj, Point4r const & p_ptViewport, Point3r & p_ptResult )
{
	bool l_bReturn = false;

	if ( m_pPipeline->m_pRenderSystem->UseShaders() )
	{
		Point4r l_ptTmp( p_ptObj[0], p_ptObj[1], p_ptObj[2], real( 1 ) );
		l_ptTmp = m_pPipeline->m_matrix[eMTXMODE_VIEW		].top()	* l_ptTmp;
		l_ptTmp = m_pPipeline->m_matrix[eMTXMODE_MODEL		].top()	* l_ptTmp;
		l_ptTmp = m_pPipeline->m_matrix[eMTXMODE_PROJECTION	].top()	* l_ptTmp;
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

bool IPipelineImpl::UnProject( Point3i const & p_ptWin, Point4r const & p_ptViewport, Point3r & p_ptResult )
{
	bool l_bReturn = false;

	if ( m_pPipeline->m_pRenderSystem->UseShaders() )
	{
		Matrix4x4r l_mInverse = ( m_pPipeline->m_matrix[eMTXMODE_PROJECTION].top() * m_pPipeline->m_matrix[eMTXMODE_MODEL].top() * m_pPipeline->m_matrix[eMTXMODE_VIEW].top() ).get_inverse();
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

bool IPipelineImpl::PickMatrix( real x, real y, real width, real height, int viewport[] )
{
	real m[16];
	real sx, sy;
	real tx, ty;
	sx = viewport[2] / width;
	sy = viewport[3] / height;
	tx = ( viewport[2] + real( 2.0 ) * ( viewport[0] - x ) ) / width;
	ty = ( viewport[3] + real( 2.0 ) * ( viewport[1] - y ) ) / height;
#define M( row, col) m[col * 4 + row]
	M( 0, 0 ) = sx;
	M( 0, 1 ) = real( 0.0 );
	M( 0, 2 ) = real( 0.0 );
	M( 0, 3 ) = tx;
	M( 1, 0 ) = real( 0.0 );
	M( 1, 1 ) = sy;
	M( 1, 2 ) = real( 0.0 );
	M( 1, 3 ) = ty;
	M( 2, 0 ) = real( 0.0 );
	M( 2, 1 ) = real( 0.0 );
	M( 2, 2 ) = real( 1.0 );
	M( 2, 3 ) = real( 0.0 );
	M( 3, 0 ) = real( 0.0 );
	M( 3, 1 ) = real( 0.0 );
	M( 3, 2 ) = real( 0.0 );
	M( 3, 3 ) = real( 1.0 );
#undef M
	return m_pPipeline->MultMatrix( m );
}

//*************************************************************************************************
