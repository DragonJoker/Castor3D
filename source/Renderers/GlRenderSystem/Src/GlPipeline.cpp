#include "GlPipeline.hpp"
#include "GlShaderProgram.hpp"
#include "GlRenderSystem.hpp"

#include <Logger.hpp>
#include <Angle.hpp>

using namespace GlRender;
using namespace Castor3D;
using namespace Castor;

//*************************************************************************************************

float g_tmpMatrix[16];

void _printMatrixN( float const * p_mtx, uint32_t p_uiSize )
{
	std::ostringstream l_ostream;
	l_ostream.precision( 5 );
	l_ostream.fill( ' ' );

	for ( uint32_t i = 0; i < p_uiSize; i++ )
	{
		for ( uint32_t j = 0; j < p_uiSize; j++ )
		{
			l_ostream << p_mtx[i * p_uiSize + j] << " ";
		}

		l_ostream << std::endl;
	}

	l_ostream << std::endl;
	Logger::LogMessage( l_ostream.str() );
}

void _printMatrix4( float const * p_mtx )
{
	_printMatrixN( p_mtx, 4 );
}

void _printMatrix3( float const * p_mtx )
{
	_printMatrixN( p_mtx, 3 );
}

void _printMatrixN( double const * p_mtx, uint32_t p_uiSize )
{
	std::ostringstream l_ostream;
	l_ostream.precision( 5 );
	l_ostream.fill( ' ' );

	for ( uint32_t i = 0; i < p_uiSize; i++ )
	{
		for ( uint32_t j = 0; j < p_uiSize; j++ )
		{
			l_ostream << p_mtx[i * p_uiSize + j] << " ";
		}

		l_ostream << std::endl;
	}

	l_ostream << std::endl;
	Logger::LogMessage( l_ostream.str() );
}

void _printMatrix4( double const * p_mtx )
{
	_printMatrixN( p_mtx, 4 );
}

void _printMatrix3( double const * p_mtx )
{
	_printMatrixN( p_mtx, 3 );
}

eGL_MATRIX_TYPE g_matrixTypes[eMTXMODE_COUNT] =
{
	eGL_MATRIX_TYPE_PROJECTION
	,	eGL_MATRIX_TYPE_MODELVIEW
	,	eGL_MATRIX_TYPE_MODELVIEW
	,	eGL_MATRIX_TYPE_TEXTURE
	,	eGL_MATRIX_TYPE_TEXTURE
	,	eGL_MATRIX_TYPE_TEXTURE
	,	eGL_MATRIX_TYPE_TEXTURE
	,	eGL_MATRIX_TYPE_TEXTURE
	,	eGL_MATRIX_TYPE_TEXTURE
	,	eGL_MATRIX_TYPE_TEXTURE
	,	eGL_MATRIX_TYPE_TEXTURE
	,	eGL_MATRIX_TYPE_TEXTURE
	,	eGL_MATRIX_TYPE_TEXTURE
	,	eGL_MATRIX_TYPE_TEXTURE
	,	eGL_MATRIX_TYPE_TEXTURE
	,	eGL_MATRIX_TYPE_TEXTURE
	,	eGL_MATRIX_TYPE_TEXTURE
	,	eGL_MATRIX_TYPE_TEXTURE
	,	eGL_MATRIX_TYPE_TEXTURE
	,	eGL_MATRIX_TYPE_TEXTURE
	,	eGL_MATRIX_TYPE_TEXTURE
	,	eGL_MATRIX_TYPE_TEXTURE
	,	eGL_MATRIX_TYPE_TEXTURE
	,	eGL_MATRIX_TYPE_TEXTURE
	,	eGL_MATRIX_TYPE_TEXTURE
	,	eGL_MATRIX_TYPE_TEXTURE
	,	eGL_MATRIX_TYPE_TEXTURE
	,	eGL_MATRIX_TYPE_TEXTURE
	,	eGL_MATRIX_TYPE_TEXTURE
	,	eGL_MATRIX_TYPE_TEXTURE
	,	eGL_MATRIX_TYPE_TEXTURE
	,	eGL_MATRIX_TYPE_TEXTURE
	,	eGL_MATRIX_TYPE_TEXTURE
	,	eGL_MATRIX_TYPE_TEXTURE
	,	eGL_MATRIX_TYPE_TEXTURE
};

eGL_MATRIX g_matrixTypes2[eMTXMODE_COUNT] =
{
	eGL_MATRIX_PROJECTION
	,	eGL_MATRIX_MODELVIEW
	,	eGL_MATRIX_MODELVIEW
	,	eGL_MATRIX_TEXTURE
	,	eGL_MATRIX_TEXTURE
	,	eGL_MATRIX_TEXTURE
	,	eGL_MATRIX_TEXTURE
	,	eGL_MATRIX_TEXTURE
	,	eGL_MATRIX_TEXTURE
	,	eGL_MATRIX_TEXTURE
	,	eGL_MATRIX_TEXTURE
	,	eGL_MATRIX_TEXTURE
	,	eGL_MATRIX_TEXTURE
	,	eGL_MATRIX_TEXTURE
	,	eGL_MATRIX_TEXTURE
	,	eGL_MATRIX_TEXTURE
	,	eGL_MATRIX_TEXTURE
	,	eGL_MATRIX_TEXTURE
	,	eGL_MATRIX_TEXTURE
	,	eGL_MATRIX_TEXTURE
	,	eGL_MATRIX_TEXTURE
	,	eGL_MATRIX_TEXTURE
	,	eGL_MATRIX_TEXTURE
	,	eGL_MATRIX_TEXTURE
	,	eGL_MATRIX_TEXTURE
	,	eGL_MATRIX_TEXTURE
	,	eGL_MATRIX_TEXTURE
	,	eGL_MATRIX_TEXTURE
	,	eGL_MATRIX_TEXTURE
	,	eGL_MATRIX_TEXTURE
	,	eGL_MATRIX_TEXTURE
	,	eGL_MATRIX_TEXTURE
	,	eGL_MATRIX_TEXTURE
	,	eGL_MATRIX_TEXTURE
	,	eGL_MATRIX_TEXTURE
};

//*************************************************************************************************

GlPipelineImplNoShader::GlPipelineImplNoShader( OpenGl & p_gl, GlPipeline * p_pPipeline )
	:	IPipelineImpl( p_pPipeline	)
	,	m_gl( p_gl	)
{
}

GlPipelineImplNoShader::~GlPipelineImplNoShader()
{
}

void GlPipelineImplNoShader::ShowMatrices( String const & p_strText )
{
	Logger::LogMessage( p_strText );
	Logger::LogMessage( cuT( "OpenGL : " ) );
	GetMatrix( GetCurrentMode() );
	_printMatrix4( g_tmpMatrix );
	Logger::LogMessage( cuT( "Internal : " ) );
	_printMatrix4( GetCurrentMatrix().const_ptr() );
}

void GlPipelineImplNoShader::GetMatrix( eMTXMODE p_eMode )
{
	glGetFloatv( g_matrixTypes2[p_eMode], g_tmpMatrix );
	glCheckError( m_gl, cuT( "GlPipeline::DoGetMatrix - glGetFloatv" ) );
}

eMTXMODE GlPipelineImplNoShader::MatrixMode( eMTXMODE p_eMode )
{
	eMTXMODE l_eReturn = IPipelineImpl::MatrixMode( p_eMode );
	m_gl.MatrixMode( g_matrixTypes[p_eMode] );
	return l_eReturn;
}

bool GlPipelineImplNoShader::LoadIdentity()
{
	if ( GetCurrentMode() >= eMTXMODE_TEXTURE0 )
	{
		m_gl.ActiveTexture( eGL_TEXTURE_INDEX( eGL_TEXTURE_INDEX_0 + GetCurrentMode() - eMTXMODE_TEXTURE0 ) );
	}

//	ShowMatrices( cuT( "Before LoadIdentity" ) );
	IPipelineImpl::LoadIdentity();
	return m_gl.LoadIdentity();
//	ShowMatrices( cuT( "After LoadIdentity" ) );
}

bool GlPipelineImplNoShader::PushMatrix()
{
	if ( GetCurrentMode() >= eMTXMODE_TEXTURE0 )
	{
		m_gl.ActiveTexture( eGL_TEXTURE_INDEX( eGL_TEXTURE_INDEX_0 + GetCurrentMode() - eMTXMODE_TEXTURE0 ) );
	}

//	ShowMatrices( cuT( "Before Push" ) );
	IPipelineImpl::PushMatrix();
	return m_gl.PushMatrix();
//	ShowMatrices( cuT( "After Push" ) );
}

bool GlPipelineImplNoShader::PopMatrix()
{
	if ( GetCurrentMode() >= eMTXMODE_TEXTURE0 )
	{
		m_gl.ActiveTexture( eGL_TEXTURE_INDEX( eGL_TEXTURE_INDEX_0 + GetCurrentMode() - eMTXMODE_TEXTURE0 ) );
	}

	IPipelineImpl::PopMatrix();
	return m_gl.PopMatrix();
}

bool GlPipelineImplNoShader::Translate( Point3r const & p_translate )
{
//	ShowMatrices( cuT( "Before Translate" ) );
	IPipelineImpl::Translate( p_translate );
	m_gl.Translate( p_translate[0], p_translate[1], p_translate[2] );
//	ShowMatrices( cuT( "After Translate" ) );
	return true;
}

bool GlPipelineImplNoShader::Rotate( Quaternion const & p_rotate )
{
//	ShowMatrices( cuT( "Before Rotate" ) );
	IPipelineImpl::Rotate( p_rotate );
	Point3r l_axis;
	Angle l_angle;
	p_rotate.ToAxisAngle( l_axis, l_angle );
	m_gl.Rotate( l_angle.Degrees(), l_axis[0], l_axis[1], l_axis[2] );
//	ShowMatrices( cuT( "After Rotate" ) );
	return true;
}

bool GlPipelineImplNoShader::Scale( Point3r const & p_scale )
{
//	ShowMatrices( cuT( "Before Scale" ) );
	IPipelineImpl::Scale( p_scale );
	m_gl.Scale( p_scale[0], p_scale[1], p_scale[2] );
//	ShowMatrices( cuT( "After Scale" ) );
	return true;
}

bool GlPipelineImplNoShader::MultMatrix( Matrix4x4r const & p_matrix )
{
	if ( GetCurrentMode() >= eMTXMODE_TEXTURE0 )
	{
		m_gl.ActiveTexture( eGL_TEXTURE_INDEX( eGL_TEXTURE_INDEX_0 + GetCurrentMode() - eMTXMODE_TEXTURE0 ) );
	}

//	ShowMatrices( cuT( "Before MultMatrix (Mtx)" ) );
	IPipelineImpl::MultMatrix( p_matrix );
	m_gl.MultMatrix( p_matrix.const_ptr() );
//	ShowMatrices( cuT( "After MultMatrix (Mtx)" ) );
	return true;
}

bool GlPipelineImplNoShader::MultMatrix( real const * p_matrix )
{
	if ( GetCurrentMode() >= eMTXMODE_TEXTURE0 )
	{
		m_gl.ActiveTexture( eGL_TEXTURE_INDEX( eGL_TEXTURE_INDEX_0 + GetCurrentMode() - eMTXMODE_TEXTURE0 ) );
	}

//	ShowMatrices( cuT( "Before MultMatrix (Ptr)" ) );
	IPipelineImpl::MultMatrix( p_matrix );
	m_gl.MultMatrix( p_matrix );
//	ShowMatrices( cuT( "After MultMatrix (Ptr)" ) );
	return true;
}

bool GlPipelineImplNoShader::Perspective( Angle const & p_aFOVY, real p_rRatio, real p_rNear, real p_rFar )
{
	real l_rXMin, l_rXMax, l_rYMin, l_rYMax;
	l_rYMax = p_rNear * real( tan( p_aFOVY.Radians() / 2 ) );
	l_rYMin = -l_rYMax;
	l_rXMin = l_rYMin * p_rRatio;
	l_rXMax = l_rYMax * p_rRatio;
	Frustum( l_rXMin, l_rXMax, l_rYMin, l_rYMax, p_rNear, p_rFar );
	return true;
}

bool GlPipelineImplNoShader::Frustum( real p_rLeft, real p_rRight, real p_rBottom, real p_rTop, real p_rNear, real p_rFar )
{
	IPipelineImpl::Frustum( p_rLeft, p_rRight, p_rBottom, p_rTop, p_rNear, p_rFar );
	m_gl.Frustum( p_rLeft, p_rRight, p_rBottom, p_rTop, p_rNear, p_rFar );
//	ShowMatrices( cuT( "Frustum" ) );
	return true;
}

bool GlPipelineImplNoShader::Ortho( real p_rLeft, real p_rRight, real p_rBottom, real p_rTop, real p_rNear, real p_rFar )
{
	IPipelineImpl::Ortho( p_rLeft, p_rRight, p_rBottom, p_rTop, p_rNear, p_rFar );
	m_gl.Ortho( p_rLeft, p_rRight, p_rBottom, p_rTop, p_rNear, p_rFar );
//	ShowMatrices( cuT( "Ortho" ) );
	return true;
}

void GlPipelineImplNoShader::ApplyViewport( int p_iWindowWidth, int p_iWindowHeight )
{
	m_gl.Viewport( 0, 0, p_iWindowWidth, p_iWindowHeight );
}

//*************************************************************************************************

GlPipelineImplGlsl::GlPipelineImplGlsl( OpenGl & p_gl, GlPipeline * p_pPipeline )
	:	IPipelineImpl( p_pPipeline	)
	,	m_gl( p_gl	)
{
}

GlPipelineImplGlsl::~GlPipelineImplGlsl()
{
}

void GlPipelineImplGlsl::ApplyViewport( int p_iWindowWidth, int p_iWindowHeight )
{
	m_gl.Viewport( 0, 0, p_iWindowWidth, p_iWindowHeight );
}

//*************************************************************************************************

GlPipeline::GlPipeline( OpenGl & p_gl, GlRenderSystem * p_pRenderSystem )
	:	Pipeline( p_pRenderSystem	)
	,	m_pGlRenderSystem( p_pRenderSystem	)
{
	m_pPipelineNoShader	= new GlPipelineImplNoShader(	p_gl, this );
	m_pPipelineGlsl		= new GlPipelineImplGlsl(	p_gl, this );
}

GlPipeline::~GlPipeline()
{
	delete m_pPipelineNoShader;
	delete m_pPipelineGlsl;
}

void GlPipeline::Initialise()
{
	UpdateFunctions( NULL );
}

void GlPipeline::UpdateFunctions( ShaderProgramBase * p_pProgram )
{
	if ( m_pRenderSystem->ForceShaders() || ( m_pRenderSystem->UseShaders() && p_pProgram ) )
	{
		if ( m_pRenderSystem->ForceShaders() )
		{
			m_pPipelineImpl = m_pPipelineGlsl;
		}
		else if ( p_pProgram->GetLanguage() == eSHADER_LANGUAGE_GLSL )
		{
			m_pPipelineImpl = m_pPipelineGlsl;
		}

		if ( !m_pPipelineImpl )
		{
			if ( !m_pRenderSystem->ForceShaders() )
			{
				m_pPipelineImpl = m_pPipelineNoShader;
			}
			else
			{
				Logger::LogWarning( cuT( "GlPipeline::UpdateFunctions - NULL pipeline, trying to take GLSL pipeline, errors may occur" ) );
				m_pPipelineImpl = m_pPipelineGlsl;
			}
		}
	}
	else
	{
		m_pPipelineImpl = m_pPipelineNoShader;
	}
}

//*************************************************************************************************
