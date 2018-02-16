#include "Viewport.hpp"

#include "Engine.hpp"

using namespace castor;

namespace castor3d
{
	Viewport::TextWriter::TextWriter( String const & p_tabs )
		: castor::TextWriter< Viewport >{ p_tabs }
	{
	}

	bool Viewport::TextWriter::operator()( Viewport const & p_viewport, TextFile & p_file )
	{
		bool result = p_file.writeText( cuT( "\n" ) + m_tabs + cuT( "viewport\n" ) ) > 0
						&& p_file.writeText( m_tabs + cuT( "{\n" ) ) > 0;

		if ( result )
		{
			result = p_file.writeText( m_tabs + cuT( "\ttype " ) + Viewport::string_type[size_t( p_viewport.getType() )] + cuT( "\n" ) ) > 0;
			castor::TextWriter< Viewport >::checkError( result, "Viewport type" );
		}

		if ( result )
		{
			if ( p_viewport.getType() == ViewportType::eOrtho || p_viewport.getType() == ViewportType::eFrustum )
			{
				result = p_file.writeText( m_tabs + cuT( "\tnear " ) + string::toString( p_viewport.getNear() ) + cuT( "\n" ) ) > 0
						   && p_file.writeText( m_tabs + cuT( "\tfar " ) + string::toString( p_viewport.getFar() ) + cuT( "\n" ) ) > 0
						   && p_file.writeText( m_tabs + cuT( "\tleft " ) + string::toString( p_viewport.getLeft() ) + cuT( "\n" ) ) > 0
						   && p_file.writeText( m_tabs + cuT( "\tright " ) + string::toString( p_viewport.getRight() ) + cuT( "\n" ) ) > 0
						   && p_file.writeText( m_tabs + cuT( "\ttop " ) + string::toString( p_viewport.getTop() ) + cuT( "\n" ) ) > 0
						   && p_file.writeText( m_tabs + cuT( "\tbottom " ) + string::toString( p_viewport.getBottom() ) + cuT( "\n" ) ) > 0;
			}
			else
			{
				result = p_file.writeText( m_tabs + cuT( "\tnear " ) + string::toString( p_viewport.getNear() ) + cuT( "\n" ) ) > 0
						   && p_file.writeText( m_tabs + cuT( "\taspect_ratio " ) + string::toString( p_viewport.getRatio() ) + cuT( "\n" ) ) > 0
						   && p_file.writeText( m_tabs + cuT( "\tfar " ) + string::toString( p_viewport.getFar() ) + cuT( "\n" ) ) > 0
						   && p_file.writeText( m_tabs + cuT( "\tfov_y " ) + string::toString( p_viewport.getFovY().degrees() ) + cuT( "\n" ) ) > 0;
			}

			castor::TextWriter< Viewport >::checkError( result, "Viewport values" );
		}

		if ( result )
		{
			result = p_file.writeText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return result;
	}

	//*************************************************************************************************

	const std::array< String, size_t( ViewportType::eCount ) > Viewport::string_type = { cuT( "ortho" ), cuT( "perspective" ), cuT( "frustum" ) };

	Viewport::Viewport( ViewportType type
		, castor::Angle const & fovY
		, real aspect
		, real left
		, real right
		, real bottom
		, real top
		, real near
		, real far )
		: m_type{ type }
		, m_size{}
		, m_modified{ true }
		, m_fovY{ m_modified, fovY }
		, m_ratio{ m_modified, aspect }
		, m_left{ m_modified, left }
		, m_right{ m_modified, right }
		, m_bottom{ m_modified, bottom }
		, m_top{ m_modified, top }
		, m_near{ m_modified, near }
		, m_far{ m_modified, far }
		, m_viewport{ 1u, 1u, 0, 0 }
		, m_scissor{ 0, 0, 1u, 1u }
	{
		if ( m_type != ViewportType::eOrtho && !m_near )
		{
			m_near = real( 0.1 ); // not zero or we have a Z fight (due to depth buffer precision)
		}
	}

	Viewport::Viewport()
		: Viewport{ ViewportType::eOrtho, Angle{}, 1, 0, 1, 0, 1, 0, 1 }
	{
	}

	Viewport::~Viewport()
	{
	}

	bool Viewport::update()
	{
		bool result = false;

		if ( isModified() )
		{
			switch ( m_type )
			{
			case castor3d::ViewportType::eOrtho:
				doComputeOrtho( m_left
					, m_right
					, m_bottom
					, m_top
					, m_near
					, m_far );
				break;

			case castor3d::ViewportType::ePerspective:
				doComputePerspective( m_fovY
					, m_ratio
					, m_near
					, m_far );
				break;

			case castor3d::ViewportType::eFrustum:
				doComputeFrustum( m_left
					, m_right
					, m_bottom
					, m_top
					, m_near
					, m_far );
				break;

			default:
				break;
			}

			m_modified = false;
			result = true;
		}

		return result;
	}

	void Viewport::setPerspective( Angle const & fovY
		, real aspect
		, real near
		, real far )
	{
		m_type = ViewportType::ePerspective;
		m_fovY = fovY;
		m_ratio = aspect;
		m_left = 0;
		m_right = 1;
		m_bottom = 0;
		m_top = 1;
		m_near = near;
		m_far = far;
		m_modified = true;
	}

	void Viewport::setFrustum( real left
		, real right
		, real bottom
		, real top
		, real near
		, real far )
	{
		m_type = ViewportType::eFrustum;
		m_fovY = Angle{};
		m_ratio = 0;
		m_left = left;
		m_right = right;
		m_bottom = bottom;
		m_top = top;
		m_near = near;
		m_far = far;
		m_modified = true;
	}

	void Viewport::setOrtho( real left
		, real right
		, real bottom
		, real top
		, real near
		, real far )
	{
		m_type = ViewportType::eOrtho;
		m_fovY = Angle{};
		m_ratio = 0;
		m_left = left;
		m_right = right;
		m_bottom = bottom;
		m_top = top;
		m_near = near;
		m_far = far;
		m_modified = true;
	}

	void Viewport::resize( const castor::Size & value )
	{
		m_size = value;
		m_viewport = renderer::Viewport{ m_size[0], m_size[1], 0, 0 };
		m_scissor = renderer::Scissor{ 0, 0, m_size[0], m_size[1] };
	}

	void Viewport::doComputePerspective( Angle const & fovy
		, real aspect
		, real near
		, real far )
	{
		m_projection = matrix::perspective( fovy, aspect, near, far );
	}

	void Viewport::doComputeFrustum( real left
		, real right
		, real bottom
		, real top
		, real near
		, real far )
	{
		// OpenGL right handed (cf. https://www.opengl.org/sdk/docs/man2/xhtml/glFrustum.xml)
		m_projection.initialise();
		m_projection[0][0] = real( ( 2 * near ) / ( right - left ) );
		m_projection[1][1] = real( ( 2 * near ) / ( top - bottom ) );
		m_projection[2][0] = real( ( right + left ) / ( right - left ) );
		m_projection[2][1] = real( ( top + bottom ) / ( top - bottom ) );
		m_projection[2][2] = real( -( far + near ) / ( far - near ) );
		m_projection[2][3] = real( -1 );
		m_projection[3][2] = real( -( 2 * far * near ) / ( far - near ) );
	}

	void Viewport::doComputeOrtho( real left
		, real right
		, real bottom
		, real top
		, real near
		, real far )
	{
		// OpenGL right handed (cf. https://www.opengl.org/sdk/docs/man2/xhtml/glOrtho.xml)
		m_projection.setIdentity();
		m_projection[0][0] = real( 2 / ( right - left ) );
		m_projection[1][1] = real( 2 / ( top - bottom ) );
		m_projection[2][2] = real( -2 / ( far - near ) );
		m_projection[3][0] = real( -( right + left ) / ( right - left ) );
		m_projection[3][1] = real( -( top + bottom ) / ( top - bottom ) );
		m_projection[3][2] = real( -( far + near ) / ( far - near ) );
	}

	void Viewport::doComputeLookAt( Point3r const & eye
		, Point3r const & center
		, Point3r const & up )
	{
		// OpenGL right handed (cf. https://www.opengl.org/sdk/docs/man2/xhtml/gluLookAt.xml)
		Point3r f{ point::getNormalised( center - eye ) };
		Point3r u{ point::getNormalised( up ) };
		Point3r s{ point::getNormalised( point::cross( f, u ) ) };
		u = point::cross( s, f );
		m_projection.setIdentity();
		m_projection[0][0] = s[0];
		m_projection[0][1] = u[0];
		m_projection[0][2] = -f[0];
		m_projection[1][0] = s[1];
		m_projection[1][1] = u[1];
		m_projection[1][2] = -f[1];
		m_projection[2][0] = s[2];
		m_projection[2][1] = u[2];
		m_projection[2][2] = -f[2];
	}
}
