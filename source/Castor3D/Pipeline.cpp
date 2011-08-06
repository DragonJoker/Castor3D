#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/Pipeline.hpp"
#include "Castor3D/RenderSystem.hpp"

using namespace Castor3D;

Pipeline *				Pipeline::sm_singleton = nullptr;
Pipeline::matrix4x4		Pipeline::Identity( 1.0f);
Pipeline::MatrixStack	Pipeline::sm_matrix[eMODE_COUNT];
Pipeline::eMODE	Pipeline::sm_eCurrentMode = eMODE_MODELVIEW;
Pipeline::matrix3x3		Pipeline::sm_matrixNormal;
Pipeline::matrix4x4		Pipeline::sm_matrixProjectionModelView;

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
Pipeline::PFrustumFunc					Pipeline::sm_pfnFrustum						= & Pipeline::_frustum;
Pipeline::POrthoFunc					Pipeline::sm_pfnOrtho						= & Pipeline::_ortho;
Pipeline::PProjectFunc					Pipeline::sm_pfnProject						= & Pipeline::_project;
Pipeline::PUnProjectFunc				Pipeline::sm_pfnUnProject					= & Pipeline::_unProject;
Pipeline::PPickMatrixFunc				Pipeline::sm_pfnPickMatrix					= & Pipeline::_pickMatrix;
Pipeline::PApplyProjectionFunc			Pipeline::sm_pfnApplyProjection				= & Pipeline::_applyProjection;
Pipeline::PApplyModelViewFunc			Pipeline::sm_pfnApplyModelView				= & Pipeline::_applyModelView;
Pipeline::PApplyModelNormalFunc			Pipeline::sm_pfnApplyModelNormal			= & Pipeline::_applyModelNormal;
Pipeline::PApplyProjectionModelViewFunc	Pipeline::sm_pfnApplyProjectionModelView	= & Pipeline::_applyProjectionModelView;
Pipeline::PApplyViewportFunc			Pipeline::sm_pfnApplyViewport				= & Pipeline::_applyViewport;
Pipeline::PApplyCurrentMatrixFunc		Pipeline::sm_pfnApplyCurrentMatrix			= & Pipeline::_applyCurrentMatrix;

Pipeline :: Pipeline()
{
	sm_singleton = this;

	for (int i = 0 ; i < eMODE_COUNT ; i++)
	{
		sm_matrix[i].push( Identity);
	}
}

Pipeline :: ~Pipeline()
{
}

const Pipeline::matrix4x4 & Pipeline :: GetMatrix( eMODE p_eMode)
{
	return sm_matrix[p_eMode].top();
}

bool Pipeline :: MatrixMode( eMODE p_eMode)
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

bool Pipeline :: Translate( Point3r const & p_translate)
{
	return sm_pfnTranslate( p_translate);
}

bool Pipeline :: Rotate( Quaternion const & p_rotate)
{
	return sm_pfnRotate( p_rotate);
}

bool Pipeline :: Scale( Point3r const & p_scale)
{
	return sm_pfnScale( p_scale);
}

bool Pipeline :: MultMatrix( Matrix4x4r const & p_matrix)
{
	return sm_pfnMultMatrixMtx( p_matrix);
}

bool Pipeline :: MultMatrix( real const * p_matrix)
{
	return sm_pfnMultMatrixPtr( p_matrix);
}

bool Pipeline :: Perspective( Angle const & p_aFOVY, real p_rRatio, real p_rNear, real p_rFar)
{
	return sm_pfnPerspective( p_aFOVY, p_rRatio, p_rNear, p_rFar);
}

bool Pipeline :: Frustum( real p_rLeft, real p_rRight, real p_rBottom, real p_rTop, real p_rNear, real p_rFar)
{
	return sm_pfnFrustum( p_rLeft, p_rRight, p_rBottom, p_rTop, p_rNear, p_rFar);
}

bool Pipeline :: Ortho( real p_rLeft, real p_rRight, real p_rBottom, real p_rTop, real p_rNear, real p_rFar)
{
	return sm_pfnOrtho( p_rLeft, p_rRight, p_rBottom, p_rTop, p_rNear, p_rFar);
}

bool Pipeline :: Project( Point3r const & p_ptObj, Point4r const & p_ptViewport, Point3r & p_ptResult)
{
	return sm_pfnProject( p_ptObj, p_ptViewport, p_ptResult);
}

bool Pipeline :: UnProject( const Point3i & p_ptWin, Point4r const & p_ptViewport, Point3r & p_ptResult)
{
	return sm_pfnUnProject( p_ptWin, p_ptViewport, p_ptResult);
}

bool Pipeline :: PickMatrix( real x, real y, real width, real height, int viewport[])
{
	return sm_pfnPickMatrix( x, y, width, height, viewport);
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

void Pipeline :: ApplyCurrentMatrix()
{
	sm_pfnApplyCurrentMatrix();
}

bool Pipeline :: _matrixMode( eMODE p_eMode)
{
	CASTOR_TRACK;
	sm_eCurrentMode = p_eMode;
	CASTOR_ASSERT( sm_eCurrentMode < eMODE_COUNT);
	return RenderSystem::UseShaders();
}

bool Pipeline :: _loadIdentity()
{
	CASTOR_TRACK;
	bool l_bReturn = false;
	CASTOR_ASSERT( sm_eCurrentMode < eMODE_COUNT);

	if (RenderSystem::UseShaders())
	{
		sm_matrix[sm_eCurrentMode].top().SetIdentity();
		l_bReturn = true;
	}

	return l_bReturn;
}

bool Pipeline :: _pushMatrix()
{
	CASTOR_TRACK;
	bool l_bReturn = false;
	CASTOR_ASSERT( sm_eCurrentMode < eMODE_COUNT);

	if (RenderSystem::UseShaders())
	{
		sm_matrix[sm_eCurrentMode].push( sm_matrix[sm_eCurrentMode].top());
		l_bReturn = true;
	}

	return l_bReturn;
}

bool Pipeline :: _popMatrix()
{
	CASTOR_TRACK;
	bool l_bReturn = false;
	CASTOR_ASSERT( sm_eCurrentMode < eMODE_COUNT);

	if (RenderSystem::UseShaders())
	{
		sm_matrix[sm_eCurrentMode].pop();
		l_bReturn = true;
	}

	return l_bReturn;
}

bool Pipeline :: _translate( Point3r const & p_translate)
{
	bool l_bReturn = false;
	CASTOR_ASSERT( sm_eCurrentMode < eMODE_COUNT);

	if (RenderSystem::UseShaders())
	{
		MtxUtils::translate( sm_matrix[sm_eCurrentMode].top(), p_translate);
		l_bReturn = true;
	}

	return l_bReturn;
}

bool Pipeline :: _rotate( Quaternion const & p_rotate)
{
	bool l_bReturn = false;
	CASTOR_ASSERT( sm_eCurrentMode < eMODE_COUNT);

	if (RenderSystem::UseShaders())
	{
		MtxUtils::rotate( sm_matrix[sm_eCurrentMode].top(), p_rotate);
		l_bReturn = true;
	}

	return l_bReturn;
}

bool Pipeline :: _scale( Point3r const & p_scale)
{
	bool l_bReturn = false;
	CASTOR_ASSERT( sm_eCurrentMode < eMODE_COUNT);

	if (RenderSystem::UseShaders())
	{
		MtxUtils::scale( sm_matrix[sm_eCurrentMode].top(), p_scale);
		l_bReturn = true;
	}

	return l_bReturn;
}

bool Pipeline :: _multMatrixMtx( Matrix4x4r const & p_matrix)
{
	CASTOR_TRACK;
	bool l_bReturn = false;
	CASTOR_ASSERT( sm_eCurrentMode < eMODE_COUNT);

	if (RenderSystem::UseShaders())
	{
		sm_matrix[sm_eCurrentMode].top() = p_matrix * sm_matrix[sm_eCurrentMode].top();
		l_bReturn = true;
	}

	return l_bReturn;
}

bool Pipeline :: _multMatrixPtr( real const * p_matrix)
{
	CASTOR_TRACK;
	return _multMatrixMtx( p_matrix);
}

bool Pipeline :: _perspective( Angle const & p_aFOVY, real p_rRatio, real p_rNear, real p_rFar)
{
	CASTOR_TRACK;
	bool l_bReturn = false;
	CASTOR_ASSERT( sm_eCurrentMode < eMODE_COUNT);

	if (RenderSystem::UseShaders())
	{
		MtxUtils::perspective( sm_matrix[eMODE_PROJECTION].top(), p_aFOVY, p_rRatio, p_rNear, p_rFar);
		l_bReturn = true;
	}

	return l_bReturn;
}

bool Pipeline :: _frustum( real p_rLeft, real p_rRight, real p_rBottom, real p_rTop, real p_rNear, real p_rFar)
{
	CASTOR_TRACK;
	bool l_bReturn = false;
	CASTOR_ASSERT( sm_eCurrentMode < eMODE_COUNT);

	if (RenderSystem::UseShaders())
	{
		MtxUtils::frustum( sm_matrix[eMODE_PROJECTION].top(), p_rLeft, p_rRight, p_rBottom, p_rTop, p_rNear, p_rFar);
		l_bReturn = true;
	}

	return l_bReturn;
}

bool Pipeline :: _ortho( real p_rLeft, real p_rRight, real p_rBottom, real p_rTop, real p_rNear, real p_rFar)
{
	CASTOR_TRACK;
	bool l_bReturn = false;
	CASTOR_ASSERT( sm_eCurrentMode < eMODE_COUNT);

	if (RenderSystem::UseShaders())
	{
		MtxUtils::ortho( sm_matrix[eMODE_PROJECTION].top(), p_rLeft, p_rRight, p_rBottom, p_rTop, p_rNear, p_rFar);
		l_bReturn = true;
	}

	return l_bReturn;
}

bool Pipeline :: _project( Point3r const & p_ptObj, Point4r const & p_ptViewport, Point3r & p_ptResult)
{
	bool l_bReturn = false;

	if (RenderSystem::UseShaders())
	{
		Point4r l_ptTmp( p_ptObj[0], p_ptObj[1], p_ptObj[2], real( 1));
		l_ptTmp.copy( sm_matrix[eMODE_MODELVIEW].top() * l_ptTmp);
		l_ptTmp.copy( sm_matrix[eMODE_PROJECTION].top() * l_ptTmp);

		l_ptTmp /= l_ptTmp[3];
		l_ptTmp.copy( l_ptTmp * real( 0.5) + real( 0.5));
		l_ptTmp[0] = l_ptTmp[0] * p_ptViewport[2] + p_ptViewport[0];
		l_ptTmp[1] = l_ptTmp[1] * p_ptViewport[3] + p_ptViewport[1];

		p_ptResult.copy( l_ptTmp.const_ptr());
		l_bReturn = true;
	}

	return l_bReturn;
}

bool Pipeline :: _unProject( const Point3i & p_ptWin, Point4r const & p_ptViewport, Point3r & p_ptResult)
{
	bool l_bReturn = false;

	if (RenderSystem::UseShaders())
	{
		Matrix4x4r l_mInverse = (sm_matrix[eMODE_PROJECTION].top() * sm_matrix[eMODE_MODELVIEW].top()).GetInverse();

		Point4r l_ptTmp( (real)p_ptWin[0], (real)p_ptWin[1], (real)p_ptWin[2], real( 1));
		l_ptTmp[0] = (l_ptTmp[0] - p_ptViewport[0]) / p_ptViewport[2];
		l_ptTmp[1] = (l_ptTmp[1] - p_ptViewport[1]) / p_ptViewport[3];
		l_ptTmp.copy( l_ptTmp * real( 2) - real( 1));

		Point4r l_ptObj;
		l_ptObj.copy( l_mInverse.GetInverse() * l_ptTmp);
		l_ptObj /= l_ptObj[3];

		p_ptResult.copy( l_ptObj.const_ptr());
		l_bReturn = true;
	}

	return l_bReturn;
}

bool Pipeline :: _pickMatrix( real x, real y, real width, real height, int viewport[])
{
	real m[16];
	real sx, sy;
	real tx, ty;

	sx = viewport[2] / width;
	sy = viewport[3] / height;
	tx = (viewport[2] + real( 2.0) * (viewport[0] - x)) / width;
	ty = (viewport[3] + real( 2.0) * (viewport[1] - y)) / height;

#define M( row, col) m[col * 4 + row]
	M(0, 0) = sx;
	M(0, 1) = real( 0.0);
	M(0, 2) = real( 0.0);
	M(0, 3) = tx;
	M(1, 0) = real( 0.0);
	M(1, 1) = sy;
	M(1, 2) = real( 0.0);
	M(1, 3) = ty;
	M(2, 0) = real( 0.0);
	M(2, 1) = real( 0.0);
	M(2, 2) = real( 1.0);
	M(2, 3) = real( 0.0);
	M(3, 0) = real( 0.0);
	M(3, 1) = real( 0.0);
	M(3, 2) = real( 0.0);
	M(3, 3) = real( 1.0);
#undef M

	return MultMatrix( m);
}
