#include "OpenGLCommon/PrecompiledHeader.h"

#include "OpenGLCommon/GLPipeline.h"
#include "OpenGLCommon/GLShaderProgram.h"
#include "OpenGLCommon/GLFrameVariable.h"
#include "OpenGLCommon/CgGLShaderProgram.h"
#include "OpenGLCommon/CgGLFrameVariable.h"

using namespace Castor3D;

GLPipeline::UIntUIntMap GLPipeline::sm_mapIdByProgram[eNbModes];
GLPipeline::UIntUIntMap GLPipeline::sm_normalMatrixIDsByProgram;
GLPipeline::UIntUIntMap GLPipeline::sm_pmvMatrixIDsByProgram;

GLPipeline::ParameterProgramMap GLPipeline::sm_mapIdByCgProgram[eNbModes];
GLPipeline::ParameterProgramMap GLPipeline::sm_normalMatrixIDsByCgProgram;
GLPipeline::ParameterProgramMap GLPipeline::sm_pmvMatrixIDsByCgProgram;

GLPipeline :: GLPipeline()
{
}

GLPipeline :: ~GLPipeline()
{
}

void GLPipeline :: InitFunctions()
{
	ShaderProgram * l_pProgram = RenderSystem::GetSingletonPtr()->GetCurrentShaderProgram();

	if (RenderSystem::ForceShaders() || (RenderSystem::UseShaders() && (l_pProgram != NULL)))
	{
		sm_pfnMatrixMode				= & Pipeline::_matrixMode;
		sm_pfnLoadIdentity				= & Pipeline::_loadIdentity;
		sm_pfnPushMatrix				= & Pipeline::_pushMatrix;
		sm_pfnPopMatrix					= & Pipeline::_popMatrix;
		sm_pfnTranslate					= & Pipeline::_translate;
		sm_pfnRotate					= & Pipeline::_rotate;
		sm_pfnScale						= & Pipeline::_scale;
		sm_pfnMultMatrixMtx				= & Pipeline::_multMatrixMtx;
		sm_pfnMultMatrixPtr				= & Pipeline::_multMatrixPtr;
		sm_pfnPerspective				= & Pipeline::_perspective;
		sm_pfnOrtho						= & Pipeline::_ortho;
		sm_pfnProject					= & Pipeline::_project;
		sm_pfnUnProject					= & Pipeline::_unProject;

		if (l_pProgram == NULL)
		{
			sm_pfnApplyProjection			= & Pipeline::_applyProjection;
			sm_pfnApplyModelView			= & Pipeline::_applyModelView;
			sm_pfnApplyModelNormal			= & Pipeline::_applyModelNormal;
			sm_pfnApplyProjectionModelView	= & Pipeline::_applyProjectionModelView;
		}
		else if (l_pProgram->GetType() == ShaderProgram::eGlShader)
		{
			sm_pfnApplyProjection			= & GLPipeline::_applyProjection;
			sm_pfnApplyModelView			= & GLPipeline::_applyModelView;
			sm_pfnApplyModelNormal			= & GLPipeline::_applyModelNormal;
			sm_pfnApplyProjectionModelView	= & GLPipeline::_applyProjectionModelView;
		}
		else if (l_pProgram->GetType() == ShaderProgram::eCgShader)
		{
			sm_pfnApplyProjection			= & GLPipeline::_cgApplyProjection;
			sm_pfnApplyModelView			= & GLPipeline::_cgApplyModelView;
			sm_pfnApplyModelNormal			= & GLPipeline::_cgApplyModelNormal;
			sm_pfnApplyProjectionModelView	= & GLPipeline::_cgApplyProjectionModelView;
		}

		sm_pfnApplyViewport				= & GLPipeline::_applyViewport;
	}
	else
	{
		sm_pfnMatrixMode				= & GLPipeline::_matrixMode;
		sm_pfnLoadIdentity				= & GLPipeline::_loadIdentity;
		sm_pfnPushMatrix				= & GLPipeline::_pushMatrix;
		sm_pfnPopMatrix					= & GLPipeline::_popMatrix;
		sm_pfnTranslate					= & GLPipeline::_translate;
		sm_pfnRotate					= & GLPipeline::_rotate;
		sm_pfnScale						= & GLPipeline::_scale;
		sm_pfnMultMatrixMtx				= & GLPipeline::_multMatrixMtx;
		sm_pfnMultMatrixPtr				= & GLPipeline::_multMatrixPtr;
		sm_pfnPerspective				= & GLPipeline::_perspective;
		sm_pfnOrtho						= & GLPipeline::_ortho;
		sm_pfnProject					= & GLPipeline::_project;
		sm_pfnUnProject					= & GLPipeline::_unProject;

		sm_pfnApplyProjection			= & Pipeline::_applyProjection;
		sm_pfnApplyModelView			= & Pipeline::_applyModelView;
		sm_pfnApplyModelNormal			= & Pipeline::_applyModelNormal;
		sm_pfnApplyProjectionModelView	= & Pipeline::_applyProjectionModelView;
		sm_pfnApplyViewport				= & GLPipeline::_applyViewport;
	}
}

bool GLPipeline :: _matrixMode( eMATRIX_MODE p_eMode)
{
	Pipeline::_matrixMode( p_eMode);
	glMatrixMode( p_eMode + GL_MODELVIEW);
	CheckGLError( CU_T( "GLPipeline :: _matrixMode - glMatrixMode"));

	return true;
}

bool GLPipeline :: _loadIdentity()
{
	Pipeline::_loadIdentity();
	glLoadIdentity();
	CheckGLError( CU_T( "GLPipeline :: _loadIdentity - glLoadIdentity"));

	return true;
}

bool GLPipeline :: _pushMatrix()
{
	Pipeline::_pushMatrix();
	glPushMatrix();
	CheckGLError( CU_T( "GLPipeline :: _pushMatrix - glPushMatrix"));

	return true;
}

bool GLPipeline :: _popMatrix()
{
	Pipeline::_popMatrix();
	glPopMatrix();
	CheckGLError( CU_T( "GLPipeline :: _popMatrix - glPopMatrix"));

	return true;
}

bool GLPipeline :: _translate( const Point3r & p_translate)
{
	Pipeline::_translate( p_translate);
	glTranslate( p_translate[0], p_translate[1], p_translate[2]);
	CheckGLError( CU_T( "GLPipeline :: _translate - glTranslate"));

	return true;
}

bool GLPipeline :: _rotate( const Quaternion & p_rotate)
{
	Pipeline::_rotate( p_rotate);
	glRotate( p_rotate[3], p_rotate[0], p_rotate[1], p_rotate[2]);
	CheckGLError( CU_T( "GLPipeline :: _rotate - glRotate"));

	return true;
}

bool GLPipeline :: _scale( const Point3r & p_scale)
{
	Pipeline::_scale( p_scale);
	glScale( p_scale[0], p_scale[1], p_scale[2]);
	CheckGLError( CU_T( "GLPipeline :: _scale - glScale"));

	return true;
}

bool GLPipeline :: _multMatrixMtx( const Matrix4x4r & p_matrix)
{
	Pipeline::_multMatrixMtx( p_matrix);
	glMultMatrix( p_matrix.const_ptr());
	CheckGLError( CU_T( "GLPipeline :: _multMatrix - glMultMatrix"));

	return true;
}

bool GLPipeline :: _multMatrixPtr( const real * p_matrix)
{
	Pipeline::_multMatrixPtr( p_matrix);
	glMultMatrix( p_matrix);
	CheckGLError( CU_T( "GLPipeline :: _multMatrix - glMultMatrix"));

	return true;
}

bool GLPipeline :: _perspective( real p_rFOVY, real p_rRatio, real p_rNear, real p_rFar)
{
	Pipeline::_perspective( p_rFOVY, p_rRatio, p_rNear, p_rFar);
	gluPerspective( p_rFOVY, p_rRatio, p_rNear, p_rFar);
	CheckGLError( CU_T( "GLPipeline :: _perspective - gluPerspective"));

	return true;
}

bool GLPipeline :: _ortho( real p_rLeft, real p_rRight, real p_rBottom, real p_rTop, real p_rNear, real p_rFar)
{
	Pipeline::_ortho( p_rLeft, p_rRight, p_rBottom, p_rTop, p_rNear, p_rFar);
	glOrtho( p_rLeft, p_rRight, p_rBottom, p_rTop, p_rNear, p_rFar);
	CheckGLError( CU_T( "GLPipeline :: _ortho - glOrtho"));

	return true;
}

bool GLPipeline :: _project( const Point3r & p_ptObj, const Point4r & p_ptViewport, Point3r & p_ptResult)
{
	Pipeline::_project( p_ptObj, p_ptViewport, p_ptResult);
	return true;
}

bool GLPipeline :: _unProject( const Point3i & p_ptWin, const Point4r & p_ptViewport, Point3r & p_ptResult)
{
	Pipeline::_unProject( p_ptWin, p_ptViewport, p_ptResult);
	double l_modelMatrix[16];
	double l_projMatrix[16];
	real l_depth[1];
	int l_viewport[4] = { int( p_ptViewport[0]), int( p_ptViewport[1]), int( p_ptViewport[2]), int( p_ptViewport[3]) };

	glGetDoublev( GL_MODELVIEW_MATRIX, l_modelMatrix);
	CheckGLError( CU_T( "GLPipeline :: _unProject - Retrieving model matrix"));
	glGetDoublev( GL_PROJECTION_MATRIX, l_projMatrix);
	CheckGLError( CU_T( "GLPipeline :: _unProject - Retrieving projection matrix"));
	glReadPixels( p_ptWin[0], p_ptWin[1], 1, 1, GL_DEPTH_COMPONENT, GL_REAL, l_depth);
	CheckGLError( CU_T( "GLPipeline :: _unProject - Retrieving depth"));

	double l_x = 0, l_y = 0, l_z = 0;
	gluUnProject( p_ptWin[0], p_ptWin[1], l_depth[0], l_modelMatrix, l_projMatrix, l_viewport, & l_x, & l_y, & l_z);
	CheckGLError( CU_T( "GLPipeline :: _unProject - Retrieving coordinates"));
	p_ptResult = Point3r( -real( l_x), -real( l_y), -real( l_z));

	return true;
}

void GLPipeline :: _applyModelView()
{
	GLShaderProgram * l_pProgram = (GLShaderProgram *)( RenderSystem::GetSingletonPtr()->GetCurrentShaderProgram());

	if (l_pProgram->IsLinked())
	{
		_applyMatrix( eModelView, "ModelViewMatrix", l_pProgram);
	}
}

void GLPipeline :: _applyProjection()
{
	GLShaderProgram * l_pProgram = (GLShaderProgram *)( RenderSystem::GetSingletonPtr()->GetCurrentShaderProgram());

	if (l_pProgram->IsLinked())
	{
		_applyMatrix( eProjection, "ProjectionMatrix", l_pProgram);
	}
}

void GLPipeline :: _applyProjectionModelView()
{
	GLShaderProgram * l_pProgram = (GLShaderProgram *)( RenderSystem::GetSingletonPtr()->GetCurrentShaderProgram());

	if (l_pProgram->IsLinked())
	{
		sm_matrixProjectionModelView = sm_matrix[eProjection].top() * sm_matrix[eModelView].top();

		size_t l_uiID = l_pProgram->GetProgramObject();
		UIntUIntMap::iterator l_it = sm_pmvMatrixIDsByProgram.find( l_uiID);

		if (l_it == sm_pmvMatrixIDsByProgram.end())
		{
			sm_pmvMatrixIDsByProgram[l_uiID] = glGetUniformLocation( l_uiID, "ProjectionModelViewMatrix");
			CheckGLError( CU_T( "GLPipeline :: _applyProjectionModelView - glGetUniformLocation"));
			l_it = sm_pmvMatrixIDsByProgram.find( l_uiID);
		}

		glUniformMatrix4fv( l_it->second, 1, false, sm_matrixProjectionModelView.ptr());
		CheckGLError( CU_T( "GLPipeline :: _applyProjectionModelView - glUniformMatrix4fv"));
	}
}

void GLPipeline :: _applyModelNormal()
{
	GLShaderProgram * l_pProgram = (GLShaderProgram *)( RenderSystem::GetSingletonPtr()->GetCurrentShaderProgram());

	if (l_pProgram->IsLinked())
	{
		sm_matrixNormal = sm_matrix[eModelView].top().GetMinor( 3, 3).GetInverse().GetTransposed();

		size_t l_uiID = l_pProgram->GetProgramObject();
		UIntUIntMap::iterator l_it = sm_normalMatrixIDsByProgram.find( l_uiID);

		if (l_it == sm_normalMatrixIDsByProgram.end())
		{
			sm_normalMatrixIDsByProgram[l_uiID] = glGetUniformLocation( l_uiID, "NormalMatrix");
			CheckGLError( CU_T( "GLPipeline :: _applyModelNormal - glGetUniformLocation"));
			l_it = sm_normalMatrixIDsByProgram.find( l_uiID);
		}

		glUniformMatrix3fv( l_it->second, 1, false, sm_matrixNormal.ptr());
		CheckGLError( CU_T( "GLPipeline :: _applyModelNormal - glUniformMatrix4fv"));
	}
}

void GLPipeline :: _applyViewport(int p_iWindowWidth, int p_iWindowHeight)
{
	glViewport( 0, 0, p_iWindowWidth, p_iWindowHeight);
	CheckGLError( CU_T( "GLPipeline :: _applyViewport - glViewport"));
}

void GLPipeline :: _applyMatrix( eMATRIX_MODE p_eMatrix, const char * p_szName, GLShaderProgram * p_pProgram)
{
	size_t l_uiID = p_pProgram->GetProgramObject();
	UIntUIntMap::iterator l_it = sm_mapIdByProgram[p_eMatrix].find( l_uiID);

	if (l_it == sm_mapIdByProgram[p_eMatrix].end())
	{
		sm_mapIdByProgram[p_eMatrix][l_uiID] = glGetUniformLocation( l_uiID, p_szName);
		CheckGLError( CU_T( "GLPipeline :: _applyMatrix - glGetUniformLocation"));
		l_it = sm_mapIdByProgram[p_eMatrix].find( l_uiID);
	}

	glUniformMatrix4fv( l_it->second, 1, false, sm_matrix[p_eMatrix].top().ptr());
	CheckGLError( CU_T( "GLPipeline :: _applyProjection - glUniformMatrix4fv"));
}

void GLPipeline :: _cgApplyProjection()
{
	CgGLShaderProgram * l_pProgram = (CgGLShaderProgram *)( RenderSystem::GetSingletonPtr()->GetCurrentShaderProgram());

	if (l_pProgram->IsLinked())
	{
		_cgApplyMatrix( eModelView, "ModelViewMatrix", l_pProgram);
	}
}

void GLPipeline :: _cgApplyModelView()
{
	CgGLShaderProgram * l_pProgram = (CgGLShaderProgram *)( RenderSystem::GetSingletonPtr()->GetCurrentShaderProgram());

	if (l_pProgram->IsLinked())
	{
		_cgApplyMatrix( eProjection, "ProjectionMatrix", l_pProgram);
	}
}

void GLPipeline :: _cgApplyProjectionModelView()
{
	CgGLShaderProgram * l_pProgram = (CgGLShaderProgram *)( RenderSystem::GetSingletonPtr()->GetCurrentShaderProgram());

	if (l_pProgram->IsLinked())
	{
		sm_matrixProjectionModelView = sm_matrix[eProjection].top() * sm_matrix[eModelView].top();

		CGprogram l_cgProgram = l_pProgram->GetProgram( eVertexShader)->GetProgram();
		ParameterProgramMap::iterator l_it = sm_pmvMatrixIDsByCgProgram.find( l_cgProgram);

		if (l_it == sm_pmvMatrixIDsByCgProgram.end())
		{
			sm_pmvMatrixIDsByCgProgram[l_cgProgram] = cgGetNamedParameter( l_cgProgram, "ProjectionModelViewMatrix");
			CheckGLError( CU_T( "GLPipeline :: _cgApplyProjectionModelView - cgGetNamedParameter"));
			l_it = sm_pmvMatrixIDsByCgProgram.find( l_cgProgram);
		}

		cgSetMatrixParameterfr( l_it->second, sm_matrixProjectionModelView.ptr());
		CheckGLError( CU_T( "GLPipeline :: _cgApplyProjectionModelView - cgSetMatrixParameterfr"));
	}
}

void GLPipeline :: _cgApplyModelNormal()
{
	CgGLShaderProgram * l_pProgram = (CgGLShaderProgram *)( RenderSystem::GetSingletonPtr()->GetCurrentShaderProgram());

	if (l_pProgram->IsLinked())
	{
		sm_matrixNormal = sm_matrix[eModelView].top().GetMinor( 3, 3).GetInverse().GetTransposed();

		CGprogram l_cgProgram = l_pProgram->GetProgram( eVertexShader)->GetProgram();
		ParameterProgramMap::iterator l_it = sm_normalMatrixIDsByCgProgram.find( l_cgProgram);

		if (l_it == sm_normalMatrixIDsByCgProgram.end())
		{
			sm_normalMatrixIDsByCgProgram[l_cgProgram] = cgGetNamedParameter( l_cgProgram, "NormalMatrix");
			CheckGLError( CU_T( "GLPipeline :: _cgApplyModelNormal - cgGetNamedParameter"));
			l_it = sm_normalMatrixIDsByCgProgram.find( l_cgProgram);
		}

		cgSetMatrixParameterfr( l_it->second, sm_matrixNormal.ptr());
		CheckGLError( CU_T( "GLPipeline :: _cgApplyModelNormal - cgSetMatrixParameterfr"));
	}
}

void GLPipeline :: _cgApplyMatrix( eMATRIX_MODE p_eMatrix, const char * p_szName, CgGLShaderProgram * p_pProgram)
{
	CGprogram l_cgProgram = p_pProgram->GetProgram( eVertexShader)->GetProgram();
	ParameterProgramMap::iterator l_it = sm_mapIdByCgProgram[p_eMatrix].find( l_cgProgram);

	if (l_it == sm_mapIdByCgProgram[p_eMatrix].end())
	{
		sm_mapIdByCgProgram[p_eMatrix][l_cgProgram] = cgGetNamedParameter( l_cgProgram, p_szName);
		CheckGLError( CU_T( "GLPipeline :: _cgApplyMatrix - cgGetNamedParameter"));
		l_it = sm_mapIdByCgProgram[p_eMatrix].find( l_cgProgram);
	}

	cgSetMatrixParameterfr( l_it->second, sm_matrix[p_eMatrix].top().ptr());
	CheckGLError( CU_T( "GLPipeline :: _cgApplyMatrix - cgSetMatrixParameterfr"));
}