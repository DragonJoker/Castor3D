#include "PrecompiledHeader.h"

#include "GLMatrix.h"
#include "GLSLProgram.h"
#include "GLRenderSystem.h"

using namespace Castor3D;
using namespace glm;

Pipeline :: Pipeline()
	:	m_eCurrentMode( eModelView)
{
	for (int i = 0 ; i < eNbModes ; i++)
	{
		m_matrix[m_eCurrentMode - eModelView].push( mat4( 1.0f));
	}
}

Pipeline :: ~Pipeline()
{
}

void Pipeline :: MatrixMode( eMATRIX_MODE p_eMode)
{
	Pipeline & l_singleton = GetSingleton();
	l_singleton.m_eCurrentMode = p_eMode;
	CASTOR_ASSERT( l_singleton.m_eCurrentMode != eNone && l_singleton.m_eCurrentMode != eNbModes);

	if ( ! GLRenderSystem::UseShaders())
	{
		glMatrixMode( l_singleton.m_eCurrentMode);
	}
}

void Pipeline :: LoadIdentity()
{
	Pipeline & l_singleton = GetSingleton();
	CASTOR_ASSERT( l_singleton.m_eCurrentMode != eNone && l_singleton.m_eCurrentMode != eNbModes);

	if ( ! GLRenderSystem::UseShaders())
	{
		glLoadIdentity();
	}
	else
	{
		while ( ! l_singleton.m_matrix[l_singleton.m_eCurrentMode - eModelView].empty())
		{
			l_singleton.m_matrix[l_singleton.m_eCurrentMode - eModelView].pop();
		}

		l_singleton.m_matrix[l_singleton.m_eCurrentMode - eModelView].push( mat4( 1.0f));
	}
}

void Pipeline :: PushMatrix()
{
	Pipeline & l_singleton = GetSingleton();
	CASTOR_ASSERT( l_singleton.m_eCurrentMode != eNone && l_singleton.m_eCurrentMode != eNbModes);

	if ( ! GLRenderSystem::UseShaders())
	{
		glPushMatrix();
	}
	else
	{
		l_singleton.m_matrix[l_singleton.m_eCurrentMode - eModelView].push( l_singleton.m_matrix[l_singleton.m_eCurrentMode - eModelView].top());
	}
}

void Pipeline :: PopMatrix()
{
	Pipeline & l_singleton = GetSingleton();
	CASTOR_ASSERT( l_singleton.m_eCurrentMode != eNone && l_singleton.m_eCurrentMode != eNbModes);

	if ( ! GLRenderSystem::UseShaders())
	{
		glPopMatrix();
	}
	else
	{
		l_singleton.m_matrix[l_singleton.m_eCurrentMode - eModelView].pop();
	}
}

void Pipeline :: Translate( const Point3r & p_translate)
{
	Pipeline & l_singleton = GetSingleton();
	CASTOR_ASSERT( l_singleton.m_eCurrentMode != eNone && l_singleton.m_eCurrentMode != eNbModes);

	if ( ! GLRenderSystem::UseShaders())
	{
		glTranslate( p_translate[0], p_translate[1], p_translate[2]);
	}
	else
	{
		l_singleton.m_matrix[l_singleton.m_eCurrentMode - eModelView].top() *= translate( vec3( float( p_translate[0]), float( p_translate[1]), float( p_translate[2])));
	}
}

void Pipeline :: Rotate( const Quaternion & p_rotate)
{
	Pipeline & l_singleton = GetSingleton();
	CASTOR_ASSERT( l_singleton.m_eCurrentMode != eNone && l_singleton.m_eCurrentMode != eNbModes);

	if ( ! GLRenderSystem::UseShaders())
	{
		glRotate( p_rotate[3], p_rotate[0], p_rotate[1], p_rotate[2]);
	}
	else
	{
		l_singleton.m_matrix[l_singleton.m_eCurrentMode - eModelView].top() *= rotate( float( p_rotate[3]), vec3( float( p_rotate[0]), float( p_rotate[1]), float( p_rotate[2])));
	}
}

void Pipeline :: Scale( const Point3r & p_scale)
{
	Pipeline & l_singleton = GetSingleton();
	CASTOR_ASSERT( l_singleton.m_eCurrentMode != eNone && l_singleton.m_eCurrentMode != eNbModes);

	if ( ! GLRenderSystem::UseShaders())
	{
		glTranslate( p_scale[0], p_scale[1], p_scale[2]);
	}
	else
	{
		l_singleton.m_matrix[l_singleton.m_eCurrentMode - eModelView].top() *= scale( vec3( float( p_scale[0]), float( p_scale[1]), float( p_scale[2])));
	}
}

void Pipeline :: MultMatrix( const Matrix4x4r & p_matrix)
{
	Pipeline & l_singleton = GetSingleton();
	CASTOR_ASSERT( l_singleton.m_eCurrentMode != eNone && l_singleton.m_eCurrentMode != eNbModes);

	if ( ! GLRenderSystem::UseShaders())
	{
		glMultMatrix( p_matrix.const_ptr());
	}
	else
	{
		l_singleton.m_matrix[l_singleton.m_eCurrentMode - eModelView].top() *= p_matrix;
	}
}

void Pipeline :: MultMatrix( const real * p_matrix)
{
	Pipeline & l_singleton = GetSingleton();
	CASTOR_ASSERT( l_singleton.m_eCurrentMode != eNone && l_singleton.m_eCurrentMode != eNbModes);

	if ( ! GLRenderSystem::UseShaders())
	{
		glMultMatrix( p_matrix);
	}
	else
	{
		mat4 l_matrix(	p_matrix[0],	p_matrix[1],	p_matrix[2],	p_matrix[3],
						p_matrix[4],	p_matrix[5],	p_matrix[6],	p_matrix[7],
						p_matrix[8],	p_matrix[9],	p_matrix[10],	p_matrix[11],
						p_matrix[12],	p_matrix[13],	p_matrix[14],	p_matrix[15]);
		l_singleton.m_matrix[l_singleton.m_eCurrentMode - eModelView].top() *= l_matrix;
	}
}

void Pipeline :: Apply( GLShaderProgramPtr p_pProgram)
{
	Pipeline & l_singleton = GetSingleton();
	CASTOR_ASSERT( l_singleton.m_eCurrentMode != eNone && l_singleton.m_eCurrentMode != eNbModes);

	if (GLRenderSystem::UseShaders())
	{
		if (l_singleton.m_mapIdByProgram[l_singleton.m_eCurrentMode - eModelView].find( p_pProgram->GetProgramObject()) == l_singleton.m_mapIdByProgram[l_singleton.m_eCurrentMode - eModelView].end())
		{
			switch (l_singleton.m_eCurrentMode)
			{
			case eModelView:
				l_singleton.m_mapIdByProgram[l_singleton.m_eCurrentMode - eModelView][p_pProgram->GetProgramObject()] = glGetUniformLocation( p_pProgram->GetProgramObject(), "ModelViewMatrix");
				break;
				
			case eProjection:
				l_singleton.m_mapIdByProgram[l_singleton.m_eCurrentMode - eModelView][p_pProgram->GetProgramObject()] = glGetUniformLocation( p_pProgram->GetProgramObject(), "ProjectionMatrix");
				break;
			}
		}

		glUniformMatrix4fv( l_singleton.m_mapIdByProgram[l_singleton.m_eCurrentMode - eModelView][p_pProgram->GetProgramObject()], 1, false, address( l_singleton.m_matrix[l_singleton.m_eCurrentMode - eModelView].top()));
	}
}

void Pipeline :: Perspective( real p_rFOVY, real p_rRatio, real p_rNear, real p_rFar)
{
	Pipeline & l_singleton = GetSingleton();
	CASTOR_ASSERT( l_singleton.m_eCurrentMode != eNone && l_singleton.m_eCurrentMode != eNbModes);

	if ( ! GLRenderSystem::UseShaders())
	{
		gluPerspective( p_rFOVY, p_rRatio, p_rNear, p_rFar);
	}
	else
	{
		l_singleton.m_matrix[l_singleton.m_eCurrentMode - eModelView].top() *= glm::perspective( p_rFOVY, p_rRatio, p_rNear, p_rFar);
	}
}

void Pipeline :: Ortho( real p_rLeft, real p_rRight, real p_rBottom, real p_rTop, real p_rNear, real p_rFar)
{
	Pipeline & l_singleton = GetSingleton();
	CASTOR_ASSERT( l_singleton.m_eCurrentMode != eNone && l_singleton.m_eCurrentMode != eNbModes);

	if ( ! GLRenderSystem::UseShaders())
	{
		glOrtho( p_rLeft, p_rRight, p_rBottom, p_rTop, p_rNear, p_rFar);
	}
	else
	{
		l_singleton.m_matrix[l_singleton.m_eCurrentMode - eModelView].top() *= glm::ortho( p_rLeft, p_rRight, p_rBottom, p_rTop, p_rNear, p_rFar);
	}
}

void Castor3D :: operator *=( mat4 & p_matA, const Matrix4x4r & p_matB)
{
	mat4 l_matB(	p_matB[0],	p_matB[1],	p_matB[2],	p_matB[3],
					p_matB[4],	p_matB[5],	p_matB[6],	p_matB[7],
					p_matB[8],	p_matB[9],	p_matB[10],	p_matB[11],
					p_matB[12],	p_matB[13],	p_matB[14],	p_matB[15]);
	p_matA *= l_matB;
}