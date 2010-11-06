#include "PrecompiledHeader.h"

#include "GLPipeline.h"
#include "GLSLProgram.h"
#include "GLRenderSystem.h"

using namespace Castor3D;

GLPipeline :: GLPipeline()
{
	sm_singleton = this;
}

GLPipeline :: ~GLPipeline()
{
}

void GLPipeline :: _matrixMode( eMATRIX_MODE p_eMode)
{
	Pipeline::_matrixMode( p_eMode);
	glMatrixMode( m_eCurrentMode + GL_MODELVIEW);
	CheckGLError( "GLPipeline :: _matrixMode - glMatrixMode");
}

bool GLPipeline :: _loadIdentity()
{
	Pipeline::_loadIdentity();
	glLoadIdentity();
	CheckGLError( "GLPipeline :: _loadIdentity - glLoadIdentity");
	return true;
}

bool GLPipeline :: _pushMatrix()
{
	Pipeline::_pushMatrix();
	glPushMatrix();
	CheckGLError( "GLPipeline :: _pushMatrix - glPushMatrix");
	return true;
}

bool GLPipeline :: _popMatrix()
{
	Pipeline::_popMatrix();
	glPopMatrix();
	CheckGLError( "GLPipeline :: _popMatrix - glPopMatrix");
	return true;
}

bool GLPipeline :: _translate( const Point3r & p_translate)
{
	Pipeline::_translate( p_translate);
	glTranslate( p_translate[0], p_translate[1], p_translate[2]);
	CheckGLError( "GLPipeline :: _translate - glTranslate");
	return true;
}

bool GLPipeline :: _rotate( const Quaternion & p_rotate)
{
	Pipeline::_rotate( p_rotate);
	glRotate( p_rotate[3], p_rotate[0], p_rotate[1], p_rotate[2]);
	CheckGLError( "GLPipeline :: _rotate - glRotate");
	return true;
}

bool GLPipeline :: _scale( const Point3r & p_scale)
{
	Pipeline::_scale( p_scale);
	glScale( p_scale[0], p_scale[1], p_scale[2]);
	CheckGLError( "GLPipeline :: _scale - glScale");
	return true;
}

bool GLPipeline :: _multMatrix( const Matrix4x4r & p_matrix)
{
	Pipeline::_multMatrix( p_matrix);
	glMultMatrix( p_matrix.const_ptr());
	CheckGLError( "GLPipeline :: _multMatrix - glMultMatrix");
	return true;
}

bool GLPipeline :: _multMatrix( const real * p_matrix)
{
	Pipeline::_multMatrix( p_matrix);
	glMultMatrix( p_matrix);
	CheckGLError( "GLPipeline :: _multMatrix - glMultMatrix");
	return true;
}

bool GLPipeline :: _perspective( real p_rFOVY, real p_rRatio, real p_rNear, real p_rFar)
{
	Pipeline::_perspective( p_rFOVY, p_rRatio, p_rNear, p_rFar);
	gluPerspective( p_rFOVY, p_rRatio, p_rNear, p_rFar);
	CheckGLError( "GLPipeline :: _perspective - gluPerspective");
	return true;
}

bool GLPipeline :: _ortho( real p_rLeft, real p_rRight, real p_rBottom, real p_rTop, real p_rNear, real p_rFar)
{
	Pipeline::_ortho( p_rLeft, p_rRight, p_rBottom, p_rTop, p_rNear, p_rFar);
	glOrtho( p_rLeft, p_rRight, p_rBottom, p_rTop, p_rNear, p_rFar);
	CheckGLError( "GLPipeline :: _ortho - glOrtho");
	return true;
}

void GLPipeline :: _apply( ShaderProgramPtr p_pProgram)
{
	if ( ! p_pProgram.null())
	{
		size_t l_uiID = GLShaderProgramPtr( p_pProgram)->GetProgramObject();

		if (RenderSystem::UseShaders())
		{
			if (m_mapIdByProgram[eModelView].find( l_uiID) == m_mapIdByProgram[eModelView].end())
			{
				m_mapIdByProgram[eModelView][l_uiID] = glGetUniformLocation( l_uiID, "ModelViewMatrix");
				CheckGLError( "GLPipeline :: _apply - glGetUniformLocation - ModelView");
			}

			if (m_mapIdByProgram[eProjection].find( l_uiID) == m_mapIdByProgram[eProjection].end())
			{
				m_mapIdByProgram[eProjection][l_uiID] = glGetUniformLocation( l_uiID, "ProjectionMatrix");
				CheckGLError( "GLPipeline :: _apply - glGetUniformLocation - Projection");
			}

			if (m_normalMatrixIDsByProgram.find( l_uiID) == m_normalMatrixIDsByProgram.end())
			{
				m_normalMatrixIDsByProgram[l_uiID] = glGetUniformLocation( l_uiID, "NormalMatrix");
				CheckGLError( "GLPipeline :: _apply - glGetUniformLocation - Normal");
			}

			m_matrixNormal = glm::mat3(	m_matrix[eModelView].top()[0][0], m_matrix[eModelView].top()[0][1], m_matrix[eModelView].top()[0][2],
										m_matrix[eModelView].top()[1][0], m_matrix[eModelView].top()[1][1], m_matrix[eModelView].top()[1][2],
										m_matrix[eModelView].top()[2][0], m_matrix[eModelView].top()[2][1], m_matrix[eModelView].top()[2][2]);
			m_matrixNormal = glm::transpose( glm::inverse( m_matrixNormal));

			glUniformMatrix4fv( m_mapIdByProgram[eModelView][l_uiID], 1, false, glm::address( m_matrix[eModelView].top()));
			CheckGLError( "GLPipeline :: _apply - glUniformMatrix4fv - ModelView");
			glUniformMatrix4fv( m_mapIdByProgram[eProjection][l_uiID], 1, false, glm::address( m_matrix[eProjection].top()));
			CheckGLError( "GLPipeline :: _apply - glUniformMatrix4fv - Projection");
			glUniformMatrix3fv( m_normalMatrixIDsByProgram[l_uiID], 1, false, glm::address( m_matrixNormal));
			CheckGLError( "GLPipeline :: _apply - glUniformMatrix4fv - Normal");
		}
	}
}
