#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/main/Pipeline.h"
#include "Castor3D/render_system/RenderSystem.h"

using namespace Castor3D;

Pipeline * Pipeline::sm_singleton = NULL;
Pipeline::matrix4x4 Pipeline::Identity( 1.0f);
Pipeline::MatrixStack Pipeline::sm_matrix[eNbModes];
Pipeline::eMATRIX_MODE Pipeline::sm_eCurrentMode = eModelView;
Pipeline::matrix3x3 Pipeline::sm_matrixNormal;
Pipeline::matrix4x4 Pipeline::sm_matrixProjectionModelView;

Pipeline::PMatrixModeFunc				Pipeline::sm_pfnMatrixMode					= & Pipeline::_matrixMode;
Pipeline::PLoadIdentityFunc				Pipeline::sm_pfnLoadIdentity				= & Pipeline::_loadIdentity;
Pipeline::PPushMatrixFunc				Pipeline::sm_pfnPushMatrix					= & Pipeline::_pushMatrix;
Pipeline::PPopMatrixFunc				Pipeline::sm_pfnPopMatrix					= & Pipeline::_popMatrix;
Pipeline::PTranslateFunc				Pipeline::sm_pfnTranslate					= & Pipeline::_translate;
Pipeline::PRotateFunc					Pipeline::sm_pfnRotate						= & Pipeline::_rotate;
Pipeline::PScaleFunc					Pipeline::sm_pfnScale						= & Pipeline::_scale;
Pipeline::PMultMatrixMFunc				Pipeline::sm_pfnMultMatrixMtx				= & Pipeline::_multMatrixMtx;
Pipeline::PMultMatrixPFunc				Pipeline::sm_pfnMultMatrixPtr				= & Pipeline::_multMatrixPtr;
Pipeline::PPerspectiveFunc				Pipeline::sm_pfnPerspective					= & Pipeline::_perspective;
Pipeline::POrthoFunc					Pipeline::sm_pfnOrtho						= & Pipeline::_ortho;
Pipeline::PProjectFunc					Pipeline::sm_pfnProject						= & Pipeline::_project;
Pipeline::PUnProjectFunc				Pipeline::sm_pfnUnProject					= & Pipeline::_unProject;
Pipeline::PApplyProjectionFunc			Pipeline::sm_pfnApplyProjection				= & Pipeline::_applyProjection;
Pipeline::PApplyModelViewFunc			Pipeline::sm_pfnApplyModelView				= & Pipeline::_applyModelView;
Pipeline::PApplyModelNormalFunc			Pipeline::sm_pfnApplyModelNormal			= & Pipeline::_applyModelNormal;
Pipeline::PApplyProjectionModelViewFunc	Pipeline::sm_pfnApplyProjectionModelView	= & Pipeline::_applyProjectionModelView;
Pipeline::PApplyViewportFunc			Pipeline::sm_pfnApplyViewport				= & Pipeline::_applyViewport;

Pipeline :: Pipeline()
{
	sm_singleton = this;

	for (int i = eModelView ; i < eNbModes ; i++)
	{
		sm_matrix[i].push( Identity);
	}
}

Pipeline :: ~Pipeline()
{
}

const Pipeline::matrix4x4 & Pipeline :: GetMatrix( eMATRIX_MODE p_eMode)
{
	return sm_matrix[p_eMode].top();
}

bool Pipeline :: MatrixMode( eMATRIX_MODE p_eMode)
{
	return sm_pfnMatrixMode( p_eMode);
}

bool Pipeline :: LoadIdentity()
{
	return sm_pfnLoadIdentity();
}

bool Pipeline :: PushMatrix()
{
	return sm_pfnPushMatrix();
}

bool Pipeline :: PopMatrix()
{
	return sm_pfnPopMatrix();
}

bool Pipeline :: Translate( const Point3r & p_translate)
{
	return sm_pfnTranslate( p_translate);
}

bool Pipeline :: Rotate( const Quaternion & p_rotate)
{
	return sm_pfnRotate( p_rotate);
}

bool Pipeline :: Scale( const Point3r & p_scale)
{
	return sm_pfnScale( p_scale);
}

bool Pipeline :: MultMatrix( const Matrix4x4r & p_matrix)
{
	return sm_pfnMultMatrixMtx( p_matrix);
}

bool Pipeline :: MultMatrix( const real * p_matrix)
{
	return sm_pfnMultMatrixPtr( p_matrix);
}

bool Pipeline :: Perspective( real p_rFOVY, real p_rRatio, real p_rNear, real p_rFar)
{
	return sm_pfnPerspective( p_rFOVY, p_rRatio, p_rNear, p_rFar);
}

bool Pipeline :: Ortho( real p_rLeft, real p_rRight, real p_rBottom, real p_rTop, real p_rNear, real p_rFar)
{
	return sm_pfnOrtho( p_rLeft, p_rRight, p_rBottom, p_rTop, p_rNear, p_rFar);
}

bool Pipeline :: Project( const Point3r & p_ptObj, const Point4r & p_ptViewport, Point3r & p_ptResult)
{
	return sm_pfnProject( p_ptObj, p_ptViewport, p_ptResult);
}

bool Pipeline :: UnProject( const Point3i & p_ptWin, const Point4r & p_ptViewport, Point3r & p_ptResult)
{
	return sm_pfnUnProject( p_ptWin, p_ptViewport, p_ptResult);
}

void Pipeline :: ApplyProjection()
{
	sm_pfnApplyProjection();
}

void Pipeline :: ApplyModelView()
{
	sm_pfnApplyModelView();
}

void Pipeline :: ApplyModelNormal()
{
	sm_pfnApplyModelNormal();
}

void Pipeline :: ApplyProjectionModelView()
{
	sm_pfnApplyProjectionModelView();
}

void Pipeline :: ApplyMatrices()
{
	sm_pfnApplyModelView();
//	sm_pfnApplyProjection();
	sm_pfnApplyModelNormal();
	sm_pfnApplyProjectionModelView();
}

void Pipeline :: ApplyViewport( int p_iWindowWidth, int p_iWindowHeight)
{
	sm_pfnApplyViewport( p_iWindowWidth, p_iWindowHeight);
}

bool Pipeline :: _matrixMode( eMATRIX_MODE p_eMode)
{
	sm_eCurrentMode = p_eMode;
	CASTOR_ASSERT( sm_eCurrentMode < eNbModes);
	return RenderSystem::UseShaders();
}

bool Pipeline :: _loadIdentity()
{
	bool l_bReturn = false;
	CASTOR_ASSERT( sm_eCurrentMode < eNbModes);

	if (RenderSystem::UseShaders())
	{
		sm_matrix[sm_eCurrentMode].top().Identity();
		l_bReturn = true;
	}

	return l_bReturn;
}

bool Pipeline :: _pushMatrix()
{
	bool l_bReturn = false;
	CASTOR_ASSERT( sm_eCurrentMode < eNbModes);

	if (RenderSystem::UseShaders())
	{
		sm_matrix[sm_eCurrentMode].push( sm_matrix[sm_eCurrentMode].top());
		l_bReturn = true;
	}

	return l_bReturn;
}

bool Pipeline :: _popMatrix()
{
	bool l_bReturn = false;
	CASTOR_ASSERT( sm_eCurrentMode < eNbModes);

	if (RenderSystem::UseShaders())
	{
		sm_matrix[sm_eCurrentMode].pop();
		l_bReturn = true;
	}

	return l_bReturn;
}

bool Pipeline :: _translate( const Point3r & p_translate)
{
	bool l_bReturn = false;
	CASTOR_ASSERT( sm_eCurrentMode < eNbModes);

	if (RenderSystem::UseShaders())
	{
		translate( sm_matrix[sm_eCurrentMode].top(), p_translate);
		l_bReturn = true;
	}

	return l_bReturn;
}

bool Pipeline :: _rotate( const Quaternion & p_rotate)
{
	bool l_bReturn = false;
	CASTOR_ASSERT( sm_eCurrentMode < eNbModes);

	if (RenderSystem::UseShaders())
	{
		rotate( sm_matrix[sm_eCurrentMode].top(), p_rotate);
		l_bReturn = true;
	}

	return l_bReturn;
}

bool Pipeline :: _scale( const Point3r & p_scale)
{
	bool l_bReturn = false;
	CASTOR_ASSERT( sm_eCurrentMode < eNbModes);

	if (RenderSystem::UseShaders())
	{
		scale( sm_matrix[sm_eCurrentMode].top(), p_scale);
		l_bReturn = true;
	}

	return l_bReturn;
}

bool Pipeline :: _multMatrixMtx( const Matrix4x4r & p_matrix)
{
	bool l_bReturn = false;
	CASTOR_ASSERT( sm_eCurrentMode < eNbModes);

	if (RenderSystem::UseShaders())
	{
		sm_matrix[sm_eCurrentMode].top() *= p_matrix;
		l_bReturn = true;
	}

	return l_bReturn;
}

bool Pipeline :: _multMatrixPtr( const real * p_matrix)
{
	return _multMatrixMtx( p_matrix);
}

bool Pipeline :: _perspective( real p_rFOVY, real p_rRatio, real p_rNear, real p_rFar)
{
	bool l_bReturn = false;
	CASTOR_ASSERT( sm_eCurrentMode < eNbModes);

	if (RenderSystem::UseShaders())
	{
		perspective( sm_matrix[sm_eCurrentMode].top(), p_rFOVY, p_rRatio, p_rNear, p_rFar);
		l_bReturn = true;
	}

	return l_bReturn;
}

bool Pipeline :: _ortho( real p_rLeft, real p_rRight, real p_rBottom, real p_rTop, real p_rNear, real p_rFar)
{
	bool l_bReturn = false;
	CASTOR_ASSERT( sm_eCurrentMode < eNbModes);

	if (RenderSystem::UseShaders())
	{
		ortho( sm_matrix[sm_eCurrentMode].top(), p_rLeft, p_rRight, p_rBottom, p_rTop, p_rNear, p_rFar);
		l_bReturn = true;
	}

	return l_bReturn;
}

bool Pipeline :: _project( const Point3r & p_ptObj, const Point4r & p_ptViewport, Point3r & p_ptResult)
{
	bool l_bReturn = false;

	if (RenderSystem::UseShaders())
	{
		Point4r l_ptTmp = Point4r( p_ptObj[0], p_ptObj[1], p_ptObj[2], real( 1));
		l_ptTmp = sm_matrix[eModelView].top() * l_ptTmp;
		l_ptTmp = sm_matrix[eProjection].top() * l_ptTmp;

		l_ptTmp /= l_ptTmp[3];
		l_ptTmp = l_ptTmp * real( 0.5) + real( 0.5);
		l_ptTmp[0] = l_ptTmp[0] * p_ptViewport[2] + p_ptViewport[0];
		l_ptTmp[1] = l_ptTmp[1] * p_ptViewport[3] + p_ptViewport[1];

		p_ptResult = l_ptTmp.const_ptr();
		l_bReturn = true;
	}

	return l_bReturn;
}

bool Pipeline :: _unProject( const Point3i & p_ptWin, const Point4r & p_ptViewport, Point3r & p_ptResult)
{
	bool l_bReturn = false;

	if (RenderSystem::UseShaders())
	{
		Matrix4x4r l_mInverse = (sm_matrix[eProjection].top() * sm_matrix[eModelView].top()).GetInverse();

		Point4r l_ptTmp( real( p_ptWin[0]), real( p_ptWin[1]), real( p_ptWin[2]), real( 1));
		l_ptTmp[0] = (l_ptTmp[0] - p_ptViewport[0]) / p_ptViewport[2];
		l_ptTmp[1] = (l_ptTmp[1] - p_ptViewport[1]) / p_ptViewport[3];
		l_ptTmp = l_ptTmp * real( 2) - real( 1);

		Point4r l_ptObj = l_mInverse * l_ptTmp;
		l_ptObj /= l_ptObj[3];

		p_ptResult = l_ptObj.const_ptr();
		l_bReturn = true;
	}

	return l_bReturn;
}