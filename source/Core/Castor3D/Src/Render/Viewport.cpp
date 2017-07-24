#include "Viewport.hpp"

#include "Engine.hpp"

using namespace Castor;

namespace Castor3D
{
	Viewport::TextWriter::TextWriter( String const & p_tabs )
		: Castor::TextWriter< Viewport >{ p_tabs }
	{
	}

	bool Viewport::TextWriter::operator()( Viewport const & p_viewport, TextFile & p_file )
	{
		bool result = p_file.WriteText( cuT( "\n" ) + m_tabs + cuT( "viewport\n" ) ) > 0
						&& p_file.WriteText( m_tabs + cuT( "{\n" ) ) > 0;

		if ( result )
		{
			result = p_file.WriteText( m_tabs + cuT( "\ttype " ) + Viewport::string_type[size_t( p_viewport.GetType() )] + cuT( "\n" ) ) > 0;
			Castor::TextWriter< Viewport >::CheckError( result, "Viewport type" );
		}

		if ( result )
		{
			if ( p_viewport.GetType() == ViewportType::eOrtho || p_viewport.GetType() == ViewportType::eFrustum )
			{
				result = p_file.WriteText( m_tabs + cuT( "\tnear " ) + string::to_string( p_viewport.GetNear() ) + cuT( "\n" ) ) > 0
						   && p_file.WriteText( m_tabs + cuT( "\tfar " ) + string::to_string( p_viewport.GetFar() ) + cuT( "\n" ) ) > 0
						   && p_file.WriteText( m_tabs + cuT( "\tleft " ) + string::to_string( p_viewport.GetLeft() ) + cuT( "\n" ) ) > 0
						   && p_file.WriteText( m_tabs + cuT( "\tright " ) + string::to_string( p_viewport.GetRight() ) + cuT( "\n" ) ) > 0
						   && p_file.WriteText( m_tabs + cuT( "\ttop " ) + string::to_string( p_viewport.GetTop() ) + cuT( "\n" ) ) > 0
						   && p_file.WriteText( m_tabs + cuT( "\tbottom " ) + string::to_string( p_viewport.GetBottom() ) + cuT( "\n" ) ) > 0;
			}
			else
			{
				result = p_file.WriteText( m_tabs + cuT( "\tnear " ) + string::to_string( p_viewport.GetNear() ) + cuT( "\n" ) ) > 0
						   && p_file.WriteText( m_tabs + cuT( "\taspect_ratio " ) + string::to_string( p_viewport.GetRatio() ) + cuT( "\n" ) ) > 0
						   && p_file.WriteText( m_tabs + cuT( "\tfar " ) + string::to_string( p_viewport.GetFar() ) + cuT( "\n" ) ) > 0
						   && p_file.WriteText( m_tabs + cuT( "\tfov_y " ) + string::to_string( p_viewport.GetFovY().degrees() ) + cuT( "\n" ) ) > 0;
			}

			Castor::TextWriter< Viewport >::CheckError( result, "Viewport values" );
		}

		if ( result )
		{
			result = p_file.WriteText( m_tabs + cuT( "}\n" ) ) > 0;
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

	Viewport::Viewport( Engine & engine, ViewportType p_type, Castor::Angle const & p_fovY, real p_aspect, real p_left, real p_right, real p_bottom, real p_top, real p_near, real p_far )
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
		: OwnedBy< Engine >{ *p_rhs.GetEngine() }
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

	bool Viewport::Initialise()
	{
		m_impl = GetEngine()->GetRenderSystem()->CreateViewport( *this );
		return m_impl != nullptr;
	}

	void Viewport::Cleanup()
	{
		m_impl.reset();
	}

	bool Viewport::Update()
	{
		bool result = false;

		if ( IsModified() )
		{
			switch ( m_type )
			{
			case Castor3D::ViewportType::eOrtho:
				DoComputeOrtho( m_left, m_right, m_bottom, m_top, m_near, m_far );
				break;

			case Castor3D::ViewportType::ePerspective:
				DoComputePerspective( m_fovY, m_ratio, m_near, m_far );
				break;

			case Castor3D::ViewportType::eFrustum:
				DoComputeFrustum( m_left, m_right, m_bottom, m_top, m_near, m_far );
				break;

			default:
				break;
			}

			m_modified = false;
			result = true;
		}

		return result;
	}

	void Viewport::Apply()const
	{
		m_impl->Apply();
	}

	void Viewport::SetPerspective( Angle const & p_fovY, real p_aspect, real p_near, real p_far )
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

	void Viewport::SetFrustum( real p_left, real p_right, real p_bottom, real p_top, real p_near, real p_far )
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

	void Viewport::SetOrtho( real p_left, real p_right, real p_bottom, real p_top, real p_near, real p_far )
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

	void Viewport::DoComputePerspective( Angle const & p_fovy, real p_aspect, real p_near, real p_far )
	{
		m_projection = matrix::perspective( p_fovy, p_aspect, p_near, p_far );
	}

	void Viewport::DoComputeFrustum( real p_left, real p_right, real p_bottom, real p_top, real p_near, real p_far )
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

	void Viewport::DoComputeOrtho( real p_left, real p_right, real p_bottom, real p_top, real p_near, real p_far )
	{
		// OpenGL right handed (cf. https://www.opengl.org/sdk/docs/man2/xhtml/glOrtho.xml)
		m_projection.set_identity();
		m_projection[0][0] = real( 2 / ( p_right - p_left ) );
		m_projection[1][1] = real( 2 / ( p_top - p_bottom ) );
		m_projection[2][2] = real( -2 / ( p_far - p_near ) );
		m_projection[3][0] = real( -( p_right + p_left ) / ( p_right - p_left ) );
		m_projection[3][1] = real( -( p_top + p_bottom ) / ( p_top - p_bottom ) );
		m_projection[3][2] = real( -( p_far + p_near ) / ( p_far - p_near ) );
	}

	void Viewport::DoComputeLookAt( Point3r const & p_eye, Point3r const & p_center, Point3r const & p_up )
	{
		// OpenGL right handed (cf. https://www.opengl.org/sdk/docs/man2/xhtml/gluLookAt.xml)
		Point3r f( point::get_normalised( p_center - p_eye ) );
		Point3r u( point::get_normalised( p_up ) );
		Point3r s( point::get_normalised( f ^ u ) );
		u = s ^ f;
		m_projection.set_identity();
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
