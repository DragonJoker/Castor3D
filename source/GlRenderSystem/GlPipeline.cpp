#include "GlRenderSystem/PrecompiledHeader.hpp"

#include "GlRenderSystem/GlPipeline.hpp"
#include "GlRenderSystem/GlShaderProgram.hpp"
#include "GlRenderSystem/GlFrameVariable.hpp"
#include "GlRenderSystem/CgGlShaderProgram.hpp"
#include "GlRenderSystem/CgGlFrameVariable.hpp"
#include "GlRenderSystem/GlRenderSystem.hpp"
#include "GlRenderSystem/OpenGl.hpp"

using namespace Castor3D;

GLfloat g_tmpMatrix[16];

void _printMatrix( float const * p_mtx)
{
	std::cout.precision( 5);
	for (int i = 0 ; i < 4 ; i++)
	{
		for (int j = 0 ; j < 4 ; j++)
		{
			std::cout << p_mtx[i * 4 + j] << " ";
		}

		std::cout << std::endl;
	}

	std::cout << std::endl;
}

GlPipeline::UIntUIntMap GlPipeline::sm_mapIdByProgram[eMODE_COUNT];
GlPipeline::UIntUIntMap GlPipeline::sm_normalMatrixIDsByProgram;
GlPipeline::UIntUIntMap GlPipeline::sm_pmvMatrixIDsByProgram;

GlPipeline::ParameterProgramMap GlPipeline::sm_mapIdByCgProgram[eMODE_COUNT];
GlPipeline::ParameterProgramMap GlPipeline::sm_normalMatrixIDsByCgProgram;
GlPipeline::ParameterProgramMap GlPipeline::sm_pmvMatrixIDsByCgProgram;

GLenum g_matrixTypes[Pipeline::eMODE_COUNT] =
{
	GL_PROJECTION,
	GL_MODELVIEW,
	GL_TEXTURE,
	GL_TEXTURE,
	GL_TEXTURE,
	GL_TEXTURE
};

GLenum g_matrixTypes2[Pipeline::eMODE_COUNT] =
{
	GL_PROJECTION_MATRIX,
	GL_MODELVIEW_MATRIX,
	GL_TEXTURE_MATRIX,
	GL_TEXTURE_MATRIX,
	GL_TEXTURE_MATRIX,
	GL_TEXTURE_MATRIX
};

GlPipeline :: GlPipeline()
{
}

GlPipeline :: ~GlPipeline()
{
}

void GlPipeline :: InitFunctions()
{
	ShaderProgramBase * l_pProgram = reinterpret_cast<ShaderProgramBase *>( GlRenderSystem::GetSingletonPtr()->GetCurrentShaderProgram());

	if (RenderSystem::ForceShaders() || (RenderSystem::UseShaders() && (l_pProgram)))
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
		else if (l_pProgram->GetType() == ShaderProgramBase::eSHADER_LANGUAGE_GLSL)
		{
			sm_pfnApplyProjection			= & GlPipeline::_applyProjection;
			sm_pfnApplyModelView			= & GlPipeline::_applyModelView;
			sm_pfnApplyModelNormal			= & GlPipeline::_applyModelNormal;
			sm_pfnApplyProjectionModelView	= & GlPipeline::_applyProjectionModelView;
		}
		else if (l_pProgram->GetType() == ShaderProgramBase::eSHADER_LANGUAGE_CG)
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

bool GlPipeline :: _matrixMode( eMODE p_eMode)
{
	CASTOR_TRACK;
	Pipeline::_matrixMode( p_eMode);
	return OpenGl::MatrixMode( g_matrixTypes[p_eMode]);
}

bool GlPipeline :: _loadIdentity()
{
	CASTOR_TRACK;
    if (sm_eCurrentMode >= eMODE_TEXTURE0)
	{
        glActiveTexture( GL_TEXTURE0 + sm_eCurrentMode - eMODE_TEXTURE0);
	}

	Pipeline::_loadIdentity();
	return OpenGl::LoadIdentity();
}

bool GlPipeline :: _pushMatrix()
{
	CASTOR_TRACK;
    if (sm_eCurrentMode >= eMODE_TEXTURE0)
	{
        glActiveTexture( GL_TEXTURE0 + sm_eCurrentMode - eMODE_TEXTURE0);
	}

	Pipeline::_pushMatrix();
	return OpenGl::PushMatrix();
}

bool GlPipeline :: _popMatrix()
{
	CASTOR_TRACK;
    if (sm_eCurrentMode >= eMODE_TEXTURE0)
	{
        glActiveTexture( GL_TEXTURE0 + sm_eCurrentMode - eMODE_TEXTURE0);
	}

	Pipeline::_popMatrix();
	return OpenGl::PopMatrix();
}

bool GlPipeline :: _translate( Point3r const & p_translate)
{
	return OpenGl::Translate( p_translate[0], p_translate[1], p_translate[2]);
}

bool GlPipeline :: _rotate( Quaternion const & p_rotate)
{
	return OpenGl::Rotate( p_rotate[3], p_rotate[0], p_rotate[1], p_rotate[2]);
}

bool GlPipeline :: _scale( Point3r const & p_scale)
{
	return OpenGl::Scale( p_scale[0], p_scale[1], p_scale[2]);
}

bool GlPipeline :: _multMatrixMtx( Matrix4x4r const & p_matrix)
{
	CASTOR_TRACK;
    if (sm_eCurrentMode >= eMODE_TEXTURE0)
	{
        glActiveTexture( GL_TEXTURE0 + sm_eCurrentMode - eMODE_TEXTURE0);
	}

	Pipeline::_multMatrixMtx( p_matrix);
	return OpenGl::MultMatrix( p_matrix.const_ptr());
/* Ok	*
	std::cout << "MultMatrix (Matrix)\nOpenGL : " << std::endl;
    CheckGlError( glGetFloatv( g_matrixTypes2[sm_eCurrentMode], g_tmpMatrix), cuT( "GlPipeline :: _multMatrixMtx - glGetFloatv"));;
	_printMatrix( g_tmpMatrix);
	std::cout << "Internal : " << std::endl;
	_printMatrix( sm_matrix[sm_eCurrentMode].top().const_ptr());
/**/
	return true;
}

bool GlPipeline :: _multMatrixPtr( real const * p_matrix)
{
	CASTOR_TRACK;
    if (sm_eCurrentMode >= eMODE_TEXTURE0)
	{
        glActiveTexture( GL_TEXTURE0 + sm_eCurrentMode - eMODE_TEXTURE0);
	}

	Pipeline::_multMatrixPtr( p_matrix);
	return OpenGl::MultMatrix( p_matrix);
/* Ok	*
	std::cout << "MultMatrix (Pointer)\nOpenGL : " << std::endl;
    CheckGlError( glGetFloatv( g_matrixTypes2[sm_eCurrentMode], g_tmpMatrix), cuT( "GlPipeline :: _multMatrixPtr - glGetFloatv"));;
	_printMatrix( g_tmpMatrix);
	std::cout << "Internal : " << std::endl;
	_printMatrix( sm_matrix[sm_eCurrentMode].top().const_ptr());
/**/
	return true;
}

bool GlPipeline :: _perspective( Angle const & p_aFOVY, real p_rRatio, real p_rNear, real p_rFar)
{
	CASTOR_TRACK;
	real l_rXMin, l_rXMax, l_rYMin, l_rYMax;
	l_rYMax = p_rNear * real( tan( p_aFOVY.Radians() / 2));
	l_rYMin = -l_rYMax;
	l_rXMin = l_rYMin * p_rRatio;
	l_rXMax = l_rYMax * p_rRatio;
	return _frustum( l_rXMin, l_rXMax, l_rYMin, l_rYMax, p_rNear, p_rFar);
}

bool GlPipeline :: _frustum( real p_rLeft, real p_rRight, real p_rBottom, real p_rTop, real p_rNear, real p_rFar)
{
	CASTOR_TRACK;
	Pipeline::_frustum( p_rLeft, p_rRight, p_rBottom, p_rTop, p_rNear, p_rFar);
//	CheckGlError( glFrustum( -1.0, 1.0, -1.0, 1.0, 0.0, 2.0), cuT( "GlPipeline :: glFrustum"));
	return OpenGl::Frustum( p_rLeft, p_rRight, p_rBottom, p_rTop, p_rNear, p_rFar);
/* Ok	*
	std::cout << "Frustum\nOpenGL : " << std::endl;
    CheckGlError( glGetFloatv( g_matrixTypes2[eMatrixProjection], g_tmpMatrix), cuT( "GlPipeline :: _frustum - glGetFloatv"));;
	_printMatrix( g_tmpMatrix);
	std::cout << "Internal : " << std::endl;
	_printMatrix( sm_matrix[sm_eCurrentMode].top().const_ptr());
/**/
	return true;
}

bool GlPipeline :: _ortho( real p_rLeft, real p_rRight, real p_rBottom, real p_rTop, real p_rNear, real p_rFar)
{
	CASTOR_TRACK;
	Pipeline::_ortho( p_rLeft, p_rRight, p_rBottom, p_rTop, p_rNear, p_rFar);
	return OpenGl::Ortho( p_rLeft, p_rRight, p_rBottom, p_rTop, p_rNear, p_rFar);
	return true;
}

bool GlPipeline :: _project( Point3r const & p_ptObj, Point4r const & p_ptViewport, Point3r & p_ptResult)
{
	Pipeline::_project( p_ptObj, p_ptViewport, p_ptResult);
	return true;
}

bool GlPipeline :: _unProject( const Point3i & p_ptWin, Point4r const & p_ptViewport, Point3r & p_ptResult)
{
	Pipeline::_unProject( p_ptWin, p_ptViewport, p_ptResult);
	return true;
}

void GlPipeline :: _applyModelView()
{
	CASTOR_TRACK;
	GlShaderProgram * l_pProgram = GlRenderSystem::GetSingletonPtr()->GetCurrentShaderProgram();

	if (l_pProgram->IsLinked())
	{
		_applyMatrix( eMODE_MODELVIEW, "ModelViewMatrix", l_pProgram);
	}
}

void GlPipeline :: _applyProjection()
{
	CASTOR_TRACK;
	GlShaderProgram * l_pProgram = GlRenderSystem::GetSingletonPtr()->GetCurrentShaderProgram();

	if (l_pProgram->IsLinked())
	{
		_applyMatrix( eMODE_PROJECTION, "ProjectionMatrix", l_pProgram);
	}
}

void GlPipeline :: _applyProjectionModelView()
{
	CASTOR_TRACK;
	GlShaderProgram * l_pProgram = GlRenderSystem::GetSingletonPtr()->GetCurrentShaderProgram();

	if (l_pProgram->IsLinked())
	{
		sm_matrixProjectionModelView = sm_matrix[eMODE_PROJECTION].top() * sm_matrix[eMODE_MODELVIEW].top();

		size_t l_uiID = l_pProgram->GetProgramObject();
		UIntUIntMap::iterator l_it = sm_pmvMatrixIDsByProgram.find( l_uiID);

		if (l_it == sm_pmvMatrixIDsByProgram.end())
		{
			sm_pmvMatrixIDsByProgram[l_uiID] = OpenGl::GetUniformLocation( l_uiID, "ProjectionModelViewMatrix");
			l_it = sm_pmvMatrixIDsByProgram.find( l_uiID);
		}

		OpenGl::UniformMatrix4fv( l_it->second, 1, false, sm_matrixProjectionModelView.ptr());
	}
}

void GlPipeline :: _applyModelNormal()
{
	CASTOR_TRACK;
	GlShaderProgram * l_pProgram = GlRenderSystem::GetSingletonPtr()->GetCurrentShaderProgram();

	if (l_pProgram->IsLinked())
	{
		sm_matrixNormal = sm_matrix[eMODE_MODELVIEW].top().GetMinor( 3, 3).GetInverse().GetTransposed();

		size_t l_uiID = l_pProgram->GetProgramObject();
		UIntUIntMap::iterator l_it = sm_normalMatrixIDsByProgram.find( l_uiID);

		if (l_it == sm_normalMatrixIDsByProgram.end())
		{
			sm_normalMatrixIDsByProgram[l_uiID] = OpenGl::GetUniformLocation( l_uiID, "NormalMatrix");
			l_it = sm_normalMatrixIDsByProgram.find( l_uiID);
		}

		OpenGl::UniformMatrix3fv( l_it->second, 1, false, sm_matrixNormal.ptr());
	}
}

void GlPipeline :: _applyViewport(int p_iWindowWidth, int p_iWindowHeight)
{
	CASTOR_TRACK;
	OpenGl::Viewport( 0, 0, p_iWindowWidth, p_iWindowHeight);
// #ifndef NDEBUG
// 	glBegin( GL_TRIANGLES);
// 	glColor3d( 1.0, 0.0, 0.0);
// 	glVertex3d( 0.0, 0.0, 1.0);
// 	glColor3d( 0.0, 1.0, 0.0);
// 	glVertex3d( 1.0, 1.0, 1.0);
// 	glColor3d( 0.0, 0.0, 1.0);
// 	glVertex3d( 1.0, 0.0, 1.0);
// 	glEnd();
// #endif
}

void GlPipeline :: _applyMatrix( eMODE p_eMatrix, char const * p_szName, GlShaderProgram * p_pProgram)
{
	CASTOR_TRACK;
	size_t l_uiID = p_pProgram->GetProgramObject();
	UIntUIntMap::iterator l_it = sm_mapIdByProgram[p_eMatrix].find( l_uiID);

	if (l_it == sm_mapIdByProgram[p_eMatrix].end())
	{
		sm_mapIdByProgram[p_eMatrix][l_uiID] = OpenGl::GetUniformLocation( l_uiID, p_szName);
		l_it = sm_mapIdByProgram[p_eMatrix].find( l_uiID);
	}

	OpenGl::UniformMatrix4fv( l_it->second, 1, false, sm_matrix[p_eMatrix].top().ptr());
}

void GlPipeline :: _cgApplyProjection()
{
	CgShaderProgram * l_pProgram = reinterpret_cast<CgShaderProgram *>( GlRenderSystem::GetSingletonPtr()->GetCurrentShaderProgram());

	if (l_pProgram->IsLinked())
	{
		_cgApplyMatrix( eMODE_MODELVIEW, "ModelViewMatrix", l_pProgram);
	}
}

void GlPipeline :: _cgApplyModelView()
{
	CgShaderProgram * l_pProgram = reinterpret_cast<CgShaderProgram *>( GlRenderSystem::GetSingletonPtr()->GetCurrentShaderProgram());

	if (l_pProgram->IsLinked())
	{
		_cgApplyMatrix( eMODE_PROJECTION, "ProjectionMatrix", l_pProgram);
	}
}

void GlPipeline :: _cgApplyProjectionModelView()
{
	CgShaderProgram * l_pProgram = reinterpret_cast<CgShaderProgram *>( GlRenderSystem::GetSingletonPtr()->GetCurrentShaderProgram());

	if (l_pProgram->IsLinked())
	{
		sm_matrixProjectionModelView = sm_matrix[eMODE_PROJECTION].top() * sm_matrix[eMODE_MODELVIEW].top();

		CGprogram l_cgProgram = static_pointer_cast<CgShaderObject>( l_pProgram->GetProgram( eSHADER_TYPE_VERTEX))->GetProgram();
		ParameterProgramMap::iterator l_it = sm_pmvMatrixIDsByCgProgram.find( l_cgProgram);

		if (l_it == sm_pmvMatrixIDsByCgProgram.end())
		{
			cgCheckError( sm_pmvMatrixIDsByCgProgram[l_cgProgram] = cgGetNamedParameter( l_cgProgram, "ProjectionModelViewMatrix"), cuT( "GlPipeline :: _cgApplyProjectionModelView - cgGetNamedParameter"));
			l_it = sm_pmvMatrixIDsByCgProgram.find( l_cgProgram);
		}

		if (l_it->second)
		{
			cgCheckError( cgSetMatrixParameterfr( l_it->second, sm_matrixProjectionModelView.ptr()), cuT( "GlPipeline :: _cgApplyProjectionModelView - cgSetMatrixParameterfr"));
		}
	}
}

void GlPipeline :: _cgApplyModelNormal()
{
	CgShaderProgram * l_pProgram = reinterpret_cast<CgShaderProgram *>( GlRenderSystem::GetSingletonPtr()->GetCurrentShaderProgram());

	if (l_pProgram->IsLinked())
	{
		sm_matrixNormal = sm_matrix[eMODE_MODELVIEW].top().GetMinor( 3, 3).GetInverse().GetTransposed();

		CGprogram l_cgProgram = static_pointer_cast<CgShaderObject>( l_pProgram->GetProgram( eSHADER_TYPE_VERTEX))->GetProgram();
		ParameterProgramMap::iterator l_it = sm_normalMatrixIDsByCgProgram.find( l_cgProgram);

		if (l_it == sm_normalMatrixIDsByCgProgram.end())
		{
			cgCheckError( sm_normalMatrixIDsByCgProgram[l_cgProgram] = cgGetNamedParameter( l_cgProgram, "NormalMatrix"), cuT( "GlPipeline :: _cgApplyModelNormal - cgGetNamedParameter"));
			l_it = sm_normalMatrixIDsByCgProgram.find( l_cgProgram);
		}

		if (l_it->second)
		{
			cgCheckError( cgSetMatrixParameterfr( l_it->second, sm_matrixNormal.ptr()), cuT( "GlPipeline :: _cgApplyModelNormal - cgSetMatrixParameterfr"));
		}
	}
}

void GlPipeline :: _cgApplyMatrix( eMODE p_eMatrix, char const * p_szName, CgShaderProgram * p_pProgram)
{
	CGprogram l_cgProgram = static_pointer_cast<CgShaderObject>( p_pProgram->GetProgram( eSHADER_TYPE_VERTEX))->GetProgram();
	ParameterProgramMap::iterator l_it = sm_mapIdByCgProgram[p_eMatrix].find( l_cgProgram);

	if (l_it == sm_mapIdByCgProgram[p_eMatrix].end())
	{
		cgCheckError( sm_mapIdByCgProgram[p_eMatrix][l_cgProgram] = cgGetNamedParameter( l_cgProgram, p_szName), cuT( "GlPipeline :: _cgApplyMatrix - cgGetNamedParameter"));
		l_it = sm_mapIdByCgProgram[p_eMatrix].find( l_cgProgram);
	}

	if (l_it->second)
	{
		cgCheckError( cgSetMatrixParameterfr( l_it->second, sm_matrix[p_eMatrix].top().ptr()), cuT( "GlPipeline :: _cgApplyMatrix - cgSetMatrixParameterfr"));
	}
}
