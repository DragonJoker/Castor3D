#include "GlPipeline.hpp"

#include <Angle.hpp>
#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlPipelineImpl::GlPipelineImpl( OpenGl & p_gl, Pipeline & p_pipeline )
		: IPipelineImpl( p_pipeline, true )
		, m_gl( p_gl )
	{
	}

	GlPipelineImpl::~GlPipelineImpl()
	{
	}

	void GlPipelineImpl::ApplyViewport( int p_iWindowWidth, int p_iWindowHeight )
	{
		m_gl.Viewport( 0, 0, p_iWindowWidth, p_iWindowHeight );
	}

	void GlPipelineImpl::Perspective( Matrix4x4r & p_result, Angle const & p_aFOVY, real p_aspect, real p_near, real p_far )
	{
		// OpenGL right handed (cf. https://www.opengl.org/sdk/docs/man2/xhtml/gluPerspective.xml)
		real l_range = real( ( 1 / tan( p_aFOVY.radians() * 0.5 ) ) );
		p_result.initialise();
		p_result[0][0] = real( l_range / p_aspect );
		p_result[1][1] = real( l_range );
		p_result[2][2] = real( ( p_far + p_near ) / ( p_near - p_far ) );
		p_result[2][3] = real( -1 );
		p_result[3][2] = real( 2 * p_far * p_near / ( p_near - p_far ) );
	}

	void GlPipelineImpl::Frustum( Matrix4x4r & p_result, real p_left, real p_right, real p_bottom, real p_top, real p_near, real p_far )
	{
		// OpenGL right handed (cf. https://www.opengl.org/sdk/docs/man2/xhtml/glFrustum.xml)
		p_result.initialise();
		p_result[0][0] = real( ( 2 * p_near ) / ( p_right - p_left ) );
		p_result[1][1] = real( ( 2 * p_near ) / ( p_top - p_bottom ) );
		p_result[2][0] = real( ( p_right + p_left ) / ( p_right - p_left ) );
		p_result[2][1] = real( ( p_top + p_bottom ) / ( p_top - p_bottom ) );
		p_result[2][2] = real( -( p_far + p_near ) / ( p_far - p_near ) );
		p_result[2][3] = real( -1 );
		p_result[3][2] = real( -( 2 * p_far * p_near ) / ( p_far - p_near ) );
	}

	void GlPipelineImpl::Ortho( Matrix4x4r & p_result, real p_left, real p_right, real p_bottom, real p_top, real p_near, real p_far )
	{
		// OpenGL right handed (cf. https://www.opengl.org/sdk/docs/man2/xhtml/glOrtho.xml)
		p_result.set_identity();
		p_result[0][0] = real( 2 / ( p_right - p_left ) );
		p_result[1][1] = real( 2 / ( p_top - p_bottom ) );
		p_result[2][2] = real( -2 / ( p_far - p_near ) );
		p_result[3][0] = real( -( p_right + p_left ) / ( p_right - p_left ) );
		p_result[3][1] = real( -( p_top + p_bottom ) / ( p_top - p_bottom ) );
		p_result[3][2] = real( -( p_far + p_near ) / ( p_far - p_near ) );
	}

	void GlPipelineImpl::LookAt( Matrix4x4r & p_result, Point3r const & p_eye, Point3r const & p_center, Point3r const & p_up )
	{
		// OpenGL right handed (cf. https://www.opengl.org/sdk/docs/man2/xhtml/gluLookAt.xml)
		Point3r l_f( point::get_normalised( p_center - p_eye ) );
		Point3r l_u( point::get_normalised( p_up ) );
		Point3r l_s( point::get_normalised( l_f ^ l_u ) );
		l_u = l_s ^ l_f;
		p_result.set_identity();
		p_result[0][0] =  l_s[0];
		p_result[0][1] =  l_u[0];
		p_result[0][2] = -l_f[0];
		p_result[1][0] =  l_s[1];
		p_result[1][1] =  l_u[1];
		p_result[1][2] = -l_f[1];
		p_result[2][0] =  l_s[2];
		p_result[2][1] =  l_u[2];
		p_result[2][2] = -l_f[2];
	}
}
