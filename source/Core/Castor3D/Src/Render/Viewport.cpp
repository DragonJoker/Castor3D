#include "Viewport.hpp"

#include "Engine.hpp"
#include "Pipeline.hpp"
#include "RenderSystem.hpp"

#include <Math/TransformationMatrix.hpp>

using namespace Castor;

namespace Castor3D
{
	Viewport::TextWriter::TextWriter( String const & p_tabs )
		: Castor::TextWriter< Viewport >{ p_tabs }
	{
	}

	bool Viewport::TextWriter::operator()( Viewport const & p_viewport, TextFile & p_file )
	{
		bool l_return = p_file.WriteText( cuT( "\n" ) + m_tabs + cuT( "viewport\n" ) ) > 0
						&& p_file.WriteText( m_tabs + cuT( "{\n" ) ) > 0;

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "\ttype " ) + Viewport::string_type[size_t( p_viewport.GetType() )] + cuT( "\n" ) ) > 0;
			Castor::TextWriter< Viewport >::CheckError( l_return, "Viewport type" );
		}

		if ( l_return )
		{
			if ( p_viewport.GetType() == ViewportType::eOrtho || p_viewport.GetType() == ViewportType::eFrustum )
			{
				l_return = p_file.WriteText( m_tabs + cuT( "\tnear " ) + string::to_string( p_viewport.GetNear() ) + cuT( "\n" ) ) > 0
						   && p_file.WriteText( m_tabs + cuT( "\tfar " ) + string::to_string( p_viewport.GetFar() ) + cuT( "\n" ) ) > 0
						   && p_file.WriteText( m_tabs + cuT( "\tleft " ) + string::to_string( p_viewport.GetLeft() ) + cuT( "\n" ) ) > 0
						   && p_file.WriteText( m_tabs + cuT( "\tright " ) + string::to_string( p_viewport.GetRight() ) + cuT( "\n" ) ) > 0
						   && p_file.WriteText( m_tabs + cuT( "\ttop " ) + string::to_string( p_viewport.GetTop() ) + cuT( "\n" ) ) > 0
						   && p_file.WriteText( m_tabs + cuT( "\tbottom " ) + string::to_string( p_viewport.GetBottom() ) + cuT( "\n" ) ) > 0;
			}
			else
			{
				l_return = p_file.WriteText( m_tabs + cuT( "\tnear " ) + string::to_string( p_viewport.GetNear() ) + cuT( "\n" ) ) > 0
						   && p_file.WriteText( m_tabs + cuT( "\taspect_ratio " ) + string::to_string( p_viewport.GetRatio() ) + cuT( "\n" ) ) > 0
						   && p_file.WriteText( m_tabs + cuT( "\tfar " ) + string::to_string( p_viewport.GetFar() ) + cuT( "\n" ) ) > 0
						   && p_file.WriteText( m_tabs + cuT( "\tfov_y " ) + string::to_string( p_viewport.GetFovY().degrees() ) + cuT( "\n" ) ) > 0;
			}

			Castor::TextWriter< Viewport >::CheckError( l_return, "Viewport values" );
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return l_return;
	}

	//*************************************************************************************************

	IViewportImpl::IViewportImpl( RenderSystem & p_renderSystem, Viewport & p_viewport )
		: OwnedBy< RenderSystem >{ p_renderSystem }
		, m_viewport{ p_viewport }
	{
	}

	IViewportImpl::~IViewportImpl()
	{
	}

	//*************************************************************************************************

	const std::array< String, size_t( ViewportType::eCount ) > Viewport::string_type = { cuT( "ortho" ), cuT( "perspective" ), cuT( "frustum" ) };

	Viewport::Viewport( Engine & p_engine, ViewportType p_type, Castor::Angle const & p_fovY, real p_aspect, real p_left, real p_right, real p_bottom, real p_top, real p_near, real p_far )
		: OwnedBy< Engine >{ p_engine }
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

	Viewport::Viewport( Engine & p_engine )
		: Viewport{ p_engine, ViewportType::eOrtho, Angle{}, 1, 0, 1, 0, 1, 0, 1 }
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
		bool l_return = false;

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
			l_return = true;
		}

		return l_return;
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
		// OpenGL right handed (cf. https://www.opengl.org/sdk/docs/man2/xhtml/gluPerspective.xml)
		real l_range = real( ( 1 / tan( p_fovy.radians() * 0.5 ) ) );
		m_projection.initialise();
		m_projection[0][0] = real( l_range / p_aspect );
		m_projection[1][1] = real( l_range );
		m_projection[2][2] = real( ( p_far + p_near ) / ( p_near - p_far ) );
		m_projection[2][3] = real( -1 );
		m_projection[3][2] = real( 2 * p_far * p_near / ( p_near - p_far ) );
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
		Point3r l_f( point::get_normalised( p_center - p_eye ) );
		Point3r l_u( point::get_normalised( p_up ) );
		Point3r l_s( point::get_normalised( l_f ^ l_u ) );
		l_u = l_s ^ l_f;
		m_projection.set_identity();
		m_projection[0][0] = l_s[0];
		m_projection[0][1] = l_u[0];
		m_projection[0][2] = -l_f[0];
		m_projection[1][0] = l_s[1];
		m_projection[1][1] = l_u[1];
		m_projection[1][2] = -l_f[1];
		m_projection[2][0] = l_s[2];
		m_projection[2][1] = l_u[2];
		m_projection[2][2] = -l_f[2];
	}
}
