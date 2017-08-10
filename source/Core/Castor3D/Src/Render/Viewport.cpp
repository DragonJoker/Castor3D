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

	IViewportImpl::IViewportImpl( RenderSystem & renderSystem, Viewport & p_viewport )
		: OwnedBy< RenderSystem >{ renderSystem }
		, m_viewport{ p_viewport }
	{
	}

	IViewportImpl::~IViewportImpl()
	{
	}

	//*************************************************************************************************

	const std::array< String, size_t( ViewportType::eCount ) > Viewport::string_type = { cuT( "ortho" ), cuT( "perspective" ), cuT( "frustum" ) };

	Viewport::Viewport( Engine & engine, ViewportType p_type, castor::Angle const & p_fovY, real p_aspect, real p_left, real p_right, real p_bottom, real p_top, real p_near, real p_far )
		: OwnedBy< Engine >{ engine }
		, m_type{ p_type }
		, m_size{}
		, m_fovY{ p_fovY }
		, m_ratio{ p_aspect }
		, m_left{ p_left }
		, m_right{ p_right }
		, m_bottom{ p_bottom }
		, m_top{ p_top }
		, m_near{ p_near }
		, m_far{ p_far }
		, m_modified{ true }
	{
		if ( m_type != ViewportType::eOrtho && !m_near )
		{
			m_near = real( 0.1 ); // not zero or we have a Z fight (due to depth buffer precision)
		}
	}

	Viewport::Viewport( Engine & engine )
		: Viewport{ engine, ViewportType::eOrtho, Angle{}, 1, 0, 1, 0, 1, 0, 1 }
	{
	}

	Viewport::~Viewport()
	{
	}

	Viewport::Viewport( Viewport const & p_rhs )
		: OwnedBy< Engine >{ *p_rhs.getEngine() }
		, m_type{ p_rhs.m_type }
		, m_size{ p_rhs.m_size }
		, m_fovY{ p_rhs.m_fovY }
		, m_ratio{ p_rhs.m_ratio }
		, m_left{ p_rhs.m_left }
		, m_right{ p_rhs.m_right }
		, m_bottom{ p_rhs.m_bottom }
		, m_top{ p_rhs.m_top }
		, m_near{ p_rhs.m_near }
		, m_far{ p_rhs.m_far }
		, m_modified{ p_rhs.m_modified }
		, m_projection{ p_rhs.m_projection }
	{
	}

	Viewport & Viewport::operator=( Viewport const & p_rhs )
	{
		m_type = p_rhs.m_type;
		m_size = p_rhs.m_size;
		m_fovY = p_rhs.m_fovY;
		m_ratio = p_rhs.m_ratio;
		m_left = p_rhs.m_left;
		m_right = p_rhs.m_right;
		m_bottom = p_rhs.m_bottom;
		m_top = p_rhs.m_top;
		m_near = p_rhs.m_near;
		m_far = p_rhs.m_far;
		m_modified = p_rhs.m_modified;
		m_projection = p_rhs.m_projection;
		return *this;
	}

	bool Viewport::initialise()
	{
		m_impl = getEngine()->getRenderSystem()->createViewport( *this );
		return m_impl != nullptr;
	}

	void Viewport::cleanup()
	{
		m_impl.reset();
	}

	bool Viewport::update()
	{
		bool result = false;

		if ( isModified() )
		{
			switch ( m_type )
			{
			case castor3d::ViewportType::eOrtho:
				doComputeOrtho( m_left, m_right, m_bottom, m_top, m_near, m_far );
				break;

			case castor3d::ViewportType::ePerspective:
				doComputePerspective( m_fovY, m_ratio, m_near, m_far );
				break;

			case castor3d::ViewportType::eFrustum:
				doComputeFrustum( m_left, m_right, m_bottom, m_top, m_near, m_far );
				break;

			default:
				break;
			}

			m_modified = false;
			result = true;
		}

		return result;
	}

	void Viewport::apply()const
	{
		m_impl->apply();
	}

	void Viewport::setPerspective( Angle const & p_fovY, real p_aspect, real p_near, real p_far )
	{
		m_type = ViewportType::ePerspective;
		m_fovY = p_fovY;
		m_ratio = p_aspect;
		m_left = 0;
		m_right = 1;
		m_bottom = 0;
		m_top = 1;
		m_near = p_near;
		m_far = p_far;
		m_modified = true;
	}

	void Viewport::setFrustum( real p_left, real p_right, real p_bottom, real p_top, real p_near, real p_far )
	{
		m_type = ViewportType::eFrustum;
		m_fovY = Angle{};
		m_ratio = 0;
		m_left = p_left;
		m_right = p_right;
		m_bottom = p_bottom;
		m_top = p_top;
		m_near = p_near;
		m_far = p_far;
		m_modified = true;
	}

	void Viewport::setOrtho( real p_left, real p_right, real p_bottom, real p_top, real p_near, real p_far )
	{
		m_type = ViewportType::eOrtho;
		m_fovY = Angle{};
		m_ratio = 0;
		m_left = p_left;
		m_right = p_right;
		m_bottom = p_bottom;
		m_top = p_top;
		m_near = p_near;
		m_far = p_far;
		m_modified = true;
	}

	void Viewport::doComputePerspective( Angle const & p_fovy, real p_aspect, real p_near, real p_far )
	{
		m_projection = matrix::perspective( p_fovy, p_aspect, p_near, p_far );
	}

	void Viewport::doComputeFrustum( real p_left, real p_right, real p_bottom, real p_top, real p_near, real p_far )
	{
		// OpenGL right handed (cf. https://www.opengl.org/sdk/docs/man2/xhtml/glFrustum.xml)
		m_projection.initialise();
		m_projection[0][0] = real( ( 2 * p_near ) / ( p_right - p_left ) );
		m_projection[1][1] = real( ( 2 * p_near ) / ( p_top - p_bottom ) );
		m_projection[2][0] = real( ( p_right + p_left ) / ( p_right - p_left ) );
		m_projection[2][1] = real( ( p_top + p_bottom ) / ( p_top - p_bottom ) );
		m_projection[2][2] = real( -( p_far + p_near ) / ( p_far - p_near ) );
		m_projection[2][3] = real( -1 );
		m_projection[3][2] = real( -( 2 * p_far * p_near ) / ( p_far - p_near ) );
	}

	void Viewport::doComputeOrtho( real p_left, real p_right, real p_bottom, real p_top, real p_near, real p_far )
	{
		// OpenGL right handed (cf. https://www.opengl.org/sdk/docs/man2/xhtml/glOrtho.xml)
		m_projection.setIdentity();
		m_projection[0][0] = real( 2 / ( p_right - p_left ) );
		m_projection[1][1] = real( 2 / ( p_top - p_bottom ) );
		m_projection[2][2] = real( -2 / ( p_far - p_near ) );
		m_projection[3][0] = real( -( p_right + p_left ) / ( p_right - p_left ) );
		m_projection[3][1] = real( -( p_top + p_bottom ) / ( p_top - p_bottom ) );
		m_projection[3][2] = real( -( p_far + p_near ) / ( p_far - p_near ) );
	}

	void Viewport::doComputeLookAt( Point3r const & p_eye, Point3r const & p_center, Point3r const & p_up )
	{
		// OpenGL right handed (cf. https://www.opengl.org/sdk/docs/man2/xhtml/gluLookAt.xml)
		Point3r f( point::getNormalised( p_center - p_eye ) );
		Point3r u( point::getNormalised( p_up ) );
		Point3r s( point::getNormalised( f ^ u ) );
		u = s ^ f;
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
