#include "OpenGlCommon/PrecompiledHeader.h"

#include "OpenGlCommon/GlPipeline.h"
#include "OpenGlCommon/GlShaderProgram.h"
#include "OpenGlCommon/GlFrameVariable.h"
#include "OpenGlCommon/CgGlShaderProgram.h"
#include "OpenGlCommon/CgGlFrameVariable.h"
#include "OpenGlCommon/GlRenderSystem.h"

using namespace Castor3D;

GlPipeline::UIntUIntMap GlPipeline::sm_mapIdByProgram[eNbMatrixModes];
GlPipeline::UIntUIntMap GlPipeline::sm_normalMatrixIDsByProgram;
GlPipeline::UIntUIntMap GlPipeline::sm_pmvMatrixIDsByProgram;

GlPipeline::ParameterProgramMap GlPipeline::sm_mapIdByCgProgram[eNbMatrixModes];
GlPipeline::ParameterProgramMap GlPipeline::sm_normalMatrixIDsByCgProgram;
GlPipeline::ParameterProgramMap GlPipeline::sm_pmvMatrixIDsByCgProgram;

GLenum g_matrixTypes[Pipeline::eNbMatrixModes] =
{
	GL_PROJECTION,
	GL_MODELVIEW,
	GL_TEXTURE,
	GL_TEXTURE,
	GL_TEXTURE,
	GL_TEXTURE
};

GlPipeline :: GlPipeline()
{
}

GlPipeline :: ~GlPipeline()
{
}

void GlPipeline :: InitFunctions()
{
	ShaderProgramBase * l_pProgram = GlRenderSystem::GetSingletonPtr()->GetCurrentShaderProgram();

	if (RenderSystem::ForceShaders() || (RenderSystem::UseShaders() && (l_pProgram != NULL)))
	{
		sm_pfnMatrixMode					= & Pipeline::_matrixMode;
		sm_pfnLoadIdentity					= & Pipeline::_loadIdentity;
		sm_pfnPushMatrix					= & Pipeline::_pushMatrix;
		sm_pfnPopMatrix						= & Pipeline::_popMatrix;
		sm_pfnTranslate						= & Pipeline::_translate;
		sm_pfnRotate						= & Pipeline::_rotate;
		sm_pfnScale							= & Pipeline::_scale;
		sm_pfnMultMatrixMtx					= & Pipeline::_multMatrixMtx;
		sm_pfnMultMatrixPtr					= & Pipeline::_multMatrixPtr;
		sm_pfnPerspective					= & Pipeline::_perspective;
		sm_pfnFrustum						= & Pipeline::_frustum;
		sm_pfnOrtho							= & Pipeline::_ortho;
		sm_pfnProject						= & Pipeline::_project;
		sm_pfnUnProject						= & Pipeline::_unProject;

		if (l_pProgram == NULL)
		{
			sm_pfnApplyProjection			= & Pipeline::_applyProjection;
			sm_pfnApplyModelView			= & Pipeline::_applyModelView;
			sm_pfnApplyModelNormal			= & Pipeline::_applyModelNormal;
			sm_pfnApplyProjectionModelView	= & Pipeline::_applyProjectionModelView;
		}
		else if (l_pProgram->GetType() == ShaderProgramBase::eGlslShader)
		{
			sm_pfnApplyProjection			= & GlPipeline::_applyProjection;
			sm_pfnApplyModelView			= & GlPipeline::_applyModelView;
			sm_pfnApplyModelNormal			= & GlPipeline::_applyModelNormal;
			sm_pfnApplyProjectionModelView	= & GlPipeline::_applyProjectionModelView;
		}
		else if (l_pProgram->GetType() == ShaderProgramBase::eCgShader)
		{
			sm_pfnApplyProjection			= & GlPipeline::_cgApplyProjection;
			sm_pfnApplyModelView			= & GlPipeline::_cgApplyModelView;
			sm_pfnApplyModelNormal			= & GlPipeline::_cgApplyModelNormal;
			sm_pfnApplyProjectionModelView	= & GlPipeline::_cgApplyProjectionModelView;
		}

		sm_pfnApplyViewport					= & GlPipeline::_applyViewport;
	}
	else
	{
		sm_pfnMatrixMode					= & GlPipeline::_matrixMode;
		sm_pfnLoadIdentity					= & GlPipeline::_loadIdentity;
		sm_pfnPushMatrix					= & GlPipeline::_pushMatrix;
		sm_pfnPopMatrix						= & GlPipeline::_popMatrix;
		sm_pfnTranslate						= & GlPipeline::_translate;
		sm_pfnRotate						= & GlPipeline::_rotate;
		sm_pfnScale							= & GlPipeline::_scale;
		sm_pfnMultMatrixMtx					= & GlPipeline::_multMatrixMtx;
		sm_pfnMultMatrixPtr					= & GlPipeline::_multMatrixPtr;
		sm_pfnPerspective					= & GlPipeline::_perspective;
		sm_pfnFrustum						= & GlPipeline::_frustum;
		sm_pfnOrtho							= & GlPipeline::_ortho;
		sm_pfnProject						= & GlPipeline::_project;
		sm_pfnUnProject						= & GlPipeline::_unProject;

		sm_pfnApplyProjection				= & Pipeline::_applyProjection;
		sm_pfnApplyModelView				= & Pipeline::_applyModelView;
		sm_pfnApplyModelNormal				= & Pipeline::_applyModelNormal;
		sm_pfnApplyProjectionModelView		= & Pipeline::_applyProjectionModelView;
		sm_pfnApplyViewport					= & GlPipeline::_applyViewport;
	}
}

bool GlPipeline :: _matrixMode( eMATRIX_MODE p_eMode)
{
	Pipeline::_matrixMode( p_eMode);
	CheckGlError( glMatrixMode( g_matrixTypes[p_eMode]), CU_T( "GlPipeline :: _matrixMode - glMatrixMode"));
	return true;
}

bool GlPipeline :: _loadIdentity()
{
	Pipeline::_loadIdentity();
	CheckGlError( glLoadIdentity(), CU_T( "GlPipeline :: _loadIdentity - glLoadIdentity"));
	return true;
}

bool GlPipeline :: _pushMatrix()
{
	Pipeline::_pushMatrix();
	CheckGlError( glPushMatrix(), CU_T( "GlPipeline :: _pushMatrix - glPushMatrix"));
	return true;
}

bool GlPipeline :: _popMatrix()
{
	Pipeline::_popMatrix();
	CheckGlError( glPopMatrix(), CU_T( "GlPipeline :: _popMatrix - glPopMatrix"));
	return true;
}

bool GlPipeline :: _translate( const Point3r & p_translate)
{
	Pipeline::_translate( p_translate);
	CheckGlError( glTranslate( p_translate[0], p_translate[1], p_translate[2]), CU_T( "GlPipeline :: _translate - glTranslate"));
	return true;
}

bool GlPipeline :: _rotate( const Quaternion & p_rotate)
{
	Pipeline::_rotate( p_rotate);
	CheckGlError( glRotate( p_rotate[3], p_rotate[0], p_rotate[1], p_rotate[2]), CU_T( "GlPipeline :: _rotate - glRotate"));
	return true;
}

bool GlPipeline :: _scale( const Point3r & p_scale)
{
	Pipeline::_scale( p_scale);
	CheckGlError( glScale( p_scale[0], p_scale[1], p_scale[2]), CU_T( "GlPipeline :: _scale - glScale"));
	return true;
}

bool GlPipeline :: _multMatrixMtx( const Matrix4x4r & p_matrix)
{
	Pipeline::_multMatrixMtx( p_matrix);
	CheckGlError( glMultMatrix( p_matrix.const_ptr()), CU_T( "GlPipeline :: _multMatrix - glMultMatrix"));
	return true;
}

bool GlPipeline :: _multMatrixPtr( const real * p_matrix)
{
	Pipeline::_multMatrixPtr( p_matrix);
	CheckGlError( glMultMatrix( p_matrix), CU_T( "GlPipeline :: _multMatrix - glMultMatrix"));
	return true;
}

bool GlPipeline :: _perspective( real p_rFOVY, real p_rRatio, real p_rNear, real p_rFar)
{
	real l_rXMin, l_rXMax, l_rYMin, l_rYMax;
	l_rYMax = p_rNear * real( tan( p_rFOVY * Angle::PiDouble / 360.0));
	l_rYMin = -l_rYMax;
	l_rXMin = l_rYMin * p_rRatio;
	l_rXMax = l_rYMax * p_rRatio;
	return _frustum( l_rXMin, l_rXMax, l_rYMin, l_rYMax, p_rNear, p_rFar);
}

bool GlPipeline :: _frustum( real p_rLeft, real p_rRight, real p_rBottom, real p_rTop, real p_rNear, real p_rFar)
{
	Pipeline::_frustum( p_rLeft, p_rRight, p_rBottom, p_rTop, p_rNear, p_rFar);
	CheckGlError( glFrustum( p_rLeft, p_rRight, p_rBottom, p_rTop, p_rNear, p_rFar), CU_T( "GlPipeline :: _frustum - glFrustum"));
	return true;
}

bool GlPipeline :: _ortho( real p_rLeft, real p_rRight, real p_rBottom, real p_rTop, real p_rNear, real p_rFar)
{
	Pipeline::_ortho( p_rLeft, p_rRight, p_rBottom, p_rTop, p_rNear, p_rFar);
	CheckGlError( glOrtho( p_rLeft, p_rRight, p_rBottom, p_rTop, p_rNear, p_rFar), CU_T( "GlPipeline :: _ortho - glOrtho"));
	return true;
}

bool GlPipeline :: _project( const Point3r & p_ptObj, const Point4r & p_ptViewport, Point3r & p_ptResult)
{
	Pipeline::_project( p_ptObj, p_ptViewport, p_ptResult);
	return true;
}

bool GlPipeline :: _unProject( const Point3i & p_ptWin, const Point4r & p_ptViewport, Point3r & p_ptResult)
{
	Pipeline::_unProject( p_ptWin, p_ptViewport, p_ptResult);
	return true;
}

void GlPipeline :: _applyModelView()
{
	GlShaderProgram * l_pProgram = GlRenderSystem::GetSingletonPtr()->GetCurrentShaderProgram();

	if (l_pProgram->IsLinked())
	{
		_applyMatrix( eMatrixModelView, "ModelViewMatrix", l_pProgram);
	}
}

void GlPipeline :: _applyProjection()
{
	GlShaderProgram * l_pProgram = GlRenderSystem::GetSingletonPtr()->GetCurrentShaderProgram();

	if (l_pProgram->IsLinked())
	{
		_applyMatrix( eMatrixProjection, "ProjectionMatrix", l_pProgram);
	}
}

void GlPipeline :: _applyProjectionModelView()
{
	GlShaderProgram * l_pProgram = GlRenderSystem::GetSingletonPtr()->GetCurrentShaderProgram();

	if (l_pProgram->IsLinked())
	{
		sm_matrixProjectionModelView = sm_matrix[eMatrixProjection].top() * sm_matrix[eMatrixModelView].top();

		size_t l_uiID = l_pProgram->GetProgramObject();
		UIntUIntMap::iterator l_it = sm_pmvMatrixIDsByProgram.find( l_uiID);

		if (l_it == sm_pmvMatrixIDsByProgram.end())
		{
			CheckGlError( sm_pmvMatrixIDsByProgram[l_uiID] = glGetUniformLocation( l_uiID, "ProjectionModelViewMatrix"), CU_T( "GlPipeline :: _applyProjectionModelView - glGetUniformLocation"));
			l_it = sm_pmvMatrixIDsByProgram.find( l_uiID);
		}

		CheckGlError( glUniformMatrix4fv( l_it->second, 1, false, sm_matrixProjectionModelView.ptr()), CU_T( "GlPipeline :: _applyProjectionModelView - glUniformMatrix4fv"));
	}
}

void GlPipeline :: _applyModelNormal()
{
	GlShaderProgram * l_pProgram = GlRenderSystem::GetSingletonPtr()->GetCurrentShaderProgram();

	if (l_pProgram->IsLinked())
	{
		sm_matrixNormal = sm_matrix[eMatrixModelView].top().GetMinor( 3, 3).GetInverse().GetTransposed();

		size_t l_uiID = l_pProgram->GetProgramObject();
		UIntUIntMap::iterator l_it = sm_normalMatrixIDsByProgram.find( l_uiID);

		if (l_it == sm_normalMatrixIDsByProgram.end())
		{
			CheckGlError( sm_normalMatrixIDsByProgram[l_uiID] = glGetUniformLocation( l_uiID, "NormalMatrix"), CU_T( "GlPipeline :: _applyModelNormal - glGetUniformLocation"));
			l_it = sm_normalMatrixIDsByProgram.find( l_uiID);
		}

		CheckGlError( glUniformMatrix3fv( l_it->second, 1, false, sm_matrixNormal.ptr()), CU_T( "GlPipeline :: _applyModelNormal - glUniformMatrix4fv"));
	}
}

void GlPipeline :: _applyViewport(int p_iWindowWidth, int p_iWindowHeight)
{
//	glViewport( 0, 0, p_iWindowWidth, p_iWindowHeight);
//	CheckGlError( CU_T( "GlPipeline :: _applyViewport - glViewport"));
}

void GlPipeline :: _applyMatrix( eMATRIX_MODE p_eMatrix, const char * p_szName, GlShaderProgram * p_pProgram)
{
	size_t l_uiID = p_pProgram->GetProgramObject();
	UIntUIntMap::iterator l_it = sm_mapIdByProgram[p_eMatrix].find( l_uiID);

	if (l_it == sm_mapIdByProgram[p_eMatrix].end())
	{
		CheckGlError( sm_mapIdByProgram[p_eMatrix][l_uiID] = glGetUniformLocation( l_uiID, p_szName), CU_T( "GlPipeline :: _applyMatrix - glGetUniformLocation"));
		l_it = sm_mapIdByProgram[p_eMatrix].find( l_uiID);
	}

	CheckGlError( glUniformMatrix4fv( l_it->second, 1, false, sm_matrix[p_eMatrix].top().ptr()), CU_T( "GlPipeline :: _applyProjection - glUniformMatrix4fv"));
}

void GlPipeline :: _cgApplyProjection()
{
	CgGlShaderProgram * l_pProgram = (CgGlShaderProgram *)( GlRenderSystem::GetSingletonPtr()->GetCurrentShaderProgram());

	if (l_pProgram->IsLinked())
	{
		_cgApplyMatrix( eMatrixModelView, "ModelViewMatrix", l_pProgram);
	}
}

void GlPipeline :: _cgApplyModelView()
{
	CgGlShaderProgram * l_pProgram = (CgGlShaderProgram *)( GlRenderSystem::GetSingletonPtr()->GetCurrentShaderProgram());

	if (l_pProgram->IsLinked())
	{
		_cgApplyMatrix( eMatrixProjection, "ProjectionMatrix", l_pProgram);
	}
}

void GlPipeline :: _cgApplyProjectionModelView()
{
	CgGlShaderProgram * l_pProgram = (CgGlShaderProgram *)( GlRenderSystem::GetSingletonPtr()->GetCurrentShaderProgram());

	if (l_pProgram->IsLinked())
	{
		sm_matrixProjectionModelView = sm_matrix[eMatrixProjection].top() * sm_matrix[eMatrixModelView].top();

		CGprogram l_cgProgram = l_pProgram->GetProgram( eVertexShader)->GetProgram();
		ParameterProgramMap::iterator l_it = sm_pmvMatrixIDsByCgProgram.find( l_cgProgram);

		if (l_it == sm_pmvMatrixIDsByCgProgram.end())
		{
			CheckGlError( sm_pmvMatrixIDsByCgProgram[l_cgProgram] = cgGetNamedParameter( l_cgProgram, "ProjectionModelViewMatrix"), CU_T( "GlPipeline :: _cgApplyProjectionModelView - cgGetNamedParameter"));
			l_it = sm_pmvMatrixIDsByCgProgram.find( l_cgProgram);
		}

		CheckGlError( cgSetMatrixParameterfr( l_it->second, sm_matrixProjectionModelView.ptr()), CU_T( "GlPipeline :: _cgApplyProjectionModelView - cgSetMatrixParameterfr"));
	}
}

void GlPipeline :: _cgApplyModelNormal()
{
	CgGlShaderProgram * l_pProgram = (CgGlShaderProgram *)( GlRenderSystem::GetSingletonPtr()->GetCurrentShaderProgram());

	if (l_pProgram->IsLinked())
	{
		sm_matrixNormal = sm_matrix[eMatrixModelView].top().GetMinor( 3, 3).GetInverse().GetTransposed();

		CGprogram l_cgProgram = l_pProgram->GetProgram( eVertexShader)->GetProgram();
		ParameterProgramMap::iterator l_it = sm_normalMatrixIDsByCgProgram.find( l_cgProgram);

		if (l_it == sm_normalMatrixIDsByCgProgram.end())
		{
			CheckGlError( sm_normalMatrixIDsByCgProgram[l_cgProgram] = cgGetNamedParameter( l_cgProgram, "NormalMatrix"), CU_T( "GlPipeline :: _cgApplyModelNormal - cgGetNamedParameter"));
			l_it = sm_normalMatrixIDsByCgProgram.find( l_cgProgram);
		}

		CheckGlError( cgSetMatrixParameterfr( l_it->second, sm_matrixNormal.ptr()), CU_T( "GlPipeline :: _cgApplyModelNormal - cgSetMatrixParameterfr"));
	}
}

void GlPipeline :: _cgApplyMatrix( eMATRIX_MODE p_eMatrix, const char * p_szName, CgGlShaderProgram * p_pProgram)
{
	CGprogram l_cgProgram = p_pProgram->GetProgram( eVertexShader)->GetProgram();
	ParameterProgramMap::iterator l_it = sm_mapIdByCgProgram[p_eMatrix].find( l_cgProgram);

	if (l_it == sm_mapIdByCgProgram[p_eMatrix].end())
	{
		CheckGlError( sm_mapIdByCgProgram[p_eMatrix][l_cgProgram] = cgGetNamedParameter( l_cgProgram, p_szName), CU_T( "GlPipeline :: _cgApplyMatrix - cgGetNamedParameter"));
		l_it = sm_mapIdByCgProgram[p_eMatrix].find( l_cgProgram);
	}

	CheckGlError( cgSetMatrixParameterfr( l_it->second, sm_matrix[p_eMatrix].top().ptr()), CU_T( "GlPipeline :: _cgApplyMatrix - cgSetMatrixParameterfr"));
}