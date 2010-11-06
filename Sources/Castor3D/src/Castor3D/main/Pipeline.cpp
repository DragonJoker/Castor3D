#include "PrecompiledHeader.h"

#include "main/Pipeline.h"
#include "render_system/RenderSystem.h"

using namespace Castor3D;
using namespace glm;

Pipeline * Pipeline::sm_singleton = NULL;

Pipeline :: Pipeline()
	:	m_eCurrentMode( eModelView)
{
	for (int i = eModelView ; i < eNbModes ; i++)
	{
		m_matrix[m_eCurrentMode].push( mat4( 1.0f));
	}
}

Pipeline :: ~Pipeline()
{
}

void Pipeline :: MatrixMode( eMATRIX_MODE p_eMode)
{
	Pipeline * l_pSingleton = GetSingletonPtr<Pipeline>();
	l_pSingleton->_matrixMode( p_eMode);
}

bool Pipeline :: LoadIdentity()
{
	Pipeline * l_pSingleton = GetSingletonPtr<Pipeline>();
	return l_pSingleton->_loadIdentity();
}

bool Pipeline :: PushMatrix()
{
	Pipeline * l_pSingleton = GetSingletonPtr<Pipeline>();
	return l_pSingleton->_pushMatrix();
}

bool Pipeline :: PopMatrix()
{
	Pipeline * l_pSingleton = GetSingletonPtr<Pipeline>();
	return l_pSingleton->_popMatrix();
}

bool Pipeline :: Translate( const Point3r & p_translate)
{
	Pipeline * l_pSingleton = GetSingletonPtr<Pipeline>();
	return l_pSingleton->_translate( p_translate);
}

bool Pipeline :: Rotate( const Quaternion & p_rotate)
{
	Pipeline * l_pSingleton = GetSingletonPtr<Pipeline>();
	return l_pSingleton->_rotate( p_rotate);
}

bool Pipeline :: Scale( const Point3r & p_scale)
{
	Pipeline * l_pSingleton = GetSingletonPtr<Pipeline>();
	return l_pSingleton->_scale( p_scale);
}

bool Pipeline :: MultMatrix( const Matrix4x4r & p_matrix)
{
	Pipeline * l_pSingleton = GetSingletonPtr<Pipeline>();
	return l_pSingleton->_multMatrix( p_matrix);
}

bool Pipeline :: MultMatrix( const real * p_matrix)
{
	Pipeline * l_pSingleton = GetSingletonPtr<Pipeline>();
	return l_pSingleton->_multMatrix( p_matrix);
}

bool Pipeline :: Perspective( real p_rFOVY, real p_rRatio, real p_rNear, real p_rFar)
{
	Pipeline * l_pSingleton = GetSingletonPtr<Pipeline>();
	return l_pSingleton->_perspective( p_rFOVY, p_rRatio, p_rNear, p_rFar);
}

bool Pipeline :: Ortho( real p_rLeft, real p_rRight, real p_rBottom, real p_rTop, real p_rNear, real p_rFar)
{
	Pipeline * l_pSingleton = GetSingletonPtr<Pipeline>();
	return l_pSingleton->_ortho( p_rLeft, p_rRight, p_rBottom, p_rTop, p_rNear, p_rFar);
}

void Pipeline :: Apply( ShaderProgramPtr p_pProgram)
{
	Pipeline * l_pSingleton = GetSingletonPtr<Pipeline>();
	l_pSingleton->_apply( p_pProgram);
}

void Pipeline :: _matrixMode( eMATRIX_MODE p_eMode)
{
	m_eCurrentMode = p_eMode;
	CASTOR_ASSERT( m_eCurrentMode < eNbModes);
}

bool Pipeline :: _loadIdentity()
{
	bool l_bReturn = false;
	CASTOR_ASSERT( m_eCurrentMode < eNbModes);

	if (RenderSystem::UseShaders())
	{
		while ( ! m_matrix[m_eCurrentMode].empty())
		{
			m_matrix[m_eCurrentMode].pop();
		}

		m_matrix[m_eCurrentMode].push( mat4( 1.0f));
		l_bReturn = true;
	}

	return l_bReturn;
}

bool Pipeline :: _pushMatrix()
{
	bool l_bReturn = false;
	CASTOR_ASSERT( m_eCurrentMode < eNbModes);

	if (RenderSystem::UseShaders())
	{
		m_matrix[m_eCurrentMode].push( m_matrix[m_eCurrentMode].top());
		l_bReturn = true;
	}

	return l_bReturn;
}

bool Pipeline :: _popMatrix()
{
	bool l_bReturn = false;
	CASTOR_ASSERT( m_eCurrentMode < eNbModes);

	if (RenderSystem::UseShaders())
	{
		m_matrix[m_eCurrentMode].pop();
		l_bReturn = true;
	}

	return l_bReturn;
}

bool Pipeline :: _translate( const Point3r & p_translate)
{
	bool l_bReturn = false;
	CASTOR_ASSERT( m_eCurrentMode < eNbModes);

	if (RenderSystem::UseShaders())
	{
		m_matrix[m_eCurrentMode].top() *= translate( vec3( float( p_translate[0]), float( p_translate[1]), float( p_translate[2])));
		l_bReturn = true;
	}

	return l_bReturn;
}

bool Pipeline :: _rotate( const Quaternion & p_rotate)
{
	bool l_bReturn = false;
	CASTOR_ASSERT( m_eCurrentMode < eNbModes);

	if (RenderSystem::UseShaders())
	{
		m_matrix[m_eCurrentMode].top() *= rotate( float( p_rotate[3]), vec3( float( p_rotate[0]), float( p_rotate[1]), float( p_rotate[2])));
		l_bReturn = true;
	}

	return l_bReturn;
}

bool Pipeline :: _scale( const Point3r & p_scale)
{
	bool l_bReturn = false;
	CASTOR_ASSERT( m_eCurrentMode < eNbModes);

	if (RenderSystem::UseShaders())
	{
		m_matrix[m_eCurrentMode].top() *= scale( vec3( float( p_scale[0]), float( p_scale[1]), float( p_scale[2])));
		l_bReturn = true;
	}

	return l_bReturn;
}

bool Pipeline :: _multMatrix( const Matrix4x4r & p_matrix)
{
	bool l_bReturn = false;
	CASTOR_ASSERT( m_eCurrentMode < eNbModes);

	if (RenderSystem::UseShaders())
	{
		m_matrix[m_eCurrentMode].top() *= p_matrix;
		l_bReturn = true;
	}

	return l_bReturn;
}

bool Pipeline :: _multMatrix( const real * p_matrix)
{
	bool l_bReturn = false;
	CASTOR_ASSERT( m_eCurrentMode < eNbModes);

	if (RenderSystem::UseShaders())
	{
		mat4 l_matrix(	p_matrix[0],	p_matrix[1],	p_matrix[2],	p_matrix[3],
						p_matrix[4],	p_matrix[5],	p_matrix[6],	p_matrix[7],
						p_matrix[8],	p_matrix[9],	p_matrix[10],	p_matrix[11],
						p_matrix[12],	p_matrix[13],	p_matrix[14],	p_matrix[15]);
		m_matrix[m_eCurrentMode].top() *= l_matrix;
		l_bReturn = true;
	}

	return l_bReturn;
}

bool Pipeline :: _perspective( real p_rFOVY, real p_rRatio, real p_rNear, real p_rFar)
{
	bool l_bReturn = false;
	CASTOR_ASSERT( m_eCurrentMode < eNbModes);

	if (RenderSystem::UseShaders())
	{
		m_matrix[m_eCurrentMode].top() *= glm::perspective( p_rFOVY, p_rRatio, p_rNear, p_rFar);
		l_bReturn = true;
	}

	return l_bReturn;
}

bool Pipeline :: _ortho( real p_rLeft, real p_rRight, real p_rBottom, real p_rTop, real p_rNear, real p_rFar)
{
	bool l_bReturn = false;
	CASTOR_ASSERT( m_eCurrentMode < eNbModes);

	if (RenderSystem::UseShaders())
	{
		m_matrix[m_eCurrentMode].top() *= glm::ortho( p_rLeft, p_rRight, p_rBottom, p_rTop, p_rNear, p_rFar);
		l_bReturn = true;
	}

	return l_bReturn;
}

void Castor3D :: operator *=( mat4 & p_matA, const Matrix4x4r & p_matB)
{
	mat4 l_matB(	p_matB[0],	p_matB[1],	p_matB[2],	p_matB[3],
					p_matB[4],	p_matB[5],	p_matB[6],	p_matB[7],
					p_matB[8],	p_matB[9],	p_matB[10],	p_matB[11],
					p_matB[12],	p_matB[13],	p_matB[14],	p_matB[15]);
	p_matA *= l_matB;
}