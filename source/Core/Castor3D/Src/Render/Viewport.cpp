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
			l_return = p_file.WriteText( m_tabs + cuT( "\ttype " ) + Viewport::string_type[p_viewport.GetType()] + cuT( "\n" ) ) > 0;
			Castor::TextWriter< Viewport >::CheckError( l_return, "Viewport type" );
		}

		if ( l_return )
		{
			if ( p_viewport.GetType() == eVIEWPORT_TYPE_ORTHO || p_viewport.GetType() == eVIEWPORT_TYPE_FRUSTUM )
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

	const String Viewport::string_type[eVIEWPORT_TYPE_COUNT] = { cuT( "ortho" ), cuT( "perspective" ), cuT( "frustum" ) };

	Viewport::Viewport( Engine & p_engine, eVIEWPORT_TYPE p_type, Castor::Angle const & p_fovY, real p_aspect, real p_left, real p_right, real p_bottom, real p_top, real p_near, real p_far )
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
		if ( m_type != eVIEWPORT_TYPE_ORTHO && !m_near )
		{
			m_near = real( 0.1 ); // not zero or we have a Z fight (due to depth buffer precision)
		}
	}

	Viewport::Viewport( Engine & p_engine )
		: Viewport{ p_engine, eVIEWPORT_TYPE_ORTHO, Angle{}, 1, 0, 1, 0, 1, 0, 1 }
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
		, m_planes( p_rhs.m_planes )
		, m_projection{ p_rhs.m_projection }
	{
	}

	Viewport::Viewport( Viewport && p_rhs )
		: OwnedBy< Engine >{ *p_rhs.GetEngine() }
		, m_type{ std::move( p_rhs.m_type ) }
		, m_size{ std::move( p_rhs.m_size ) }
		, m_fovY{ std::move( p_rhs.m_fovY ) }
		, m_ratio{ std::move( p_rhs.m_ratio ) }
		, m_left{ std::move( p_rhs.m_left ) }
		, m_right{ std::move( p_rhs.m_right ) }
		, m_bottom{ std::move( p_rhs.m_bottom ) }
		, m_top{ std::move( p_rhs.m_top ) }
		, m_near{ std::move( p_rhs.m_near ) }
		, m_far{ std::move( p_rhs.m_far ) }
		, m_modified{ std::move( p_rhs.m_modified ) }
		, m_planes( std::move( p_rhs.m_planes ) )
		, m_projection{ std::move( p_rhs.m_projection ) }
		, m_impl{ std::move( p_rhs.m_impl ) }
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
		m_planes = p_rhs.m_planes;
		m_projection = p_rhs.m_projection;
		return *this;
	}

	Viewport & Viewport::operator=( Viewport && p_rhs )
	{
		if ( &p_rhs != this )
		{
			m_type = std::move( p_rhs.m_type );
			m_size = std::move( p_rhs.m_size );
			m_fovY = std::move( p_rhs.m_fovY );
			m_ratio = std::move( p_rhs.m_ratio );
			m_left = std::move( p_rhs.m_left );
			m_right = std::move( p_rhs.m_right );
			m_bottom = std::move( p_rhs.m_bottom );
			m_top = std::move( p_rhs.m_top );
			m_near = std::move( p_rhs.m_near );
			m_far = std::move( p_rhs.m_far );
			m_modified = std::move( p_rhs.m_modified );
			m_planes = std::move( p_rhs.m_planes );
			m_projection = std::move( p_rhs.m_projection );
			m_impl = std::move( p_rhs.m_impl );
		}

		return *this;
	}

	Viewport::~Viewport()
	{
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

	bool Viewport::Render( Pipeline & p_pipeline )
	{
		bool l_return = false;

		if ( IsModified() )
		{
			Point3r l_d( 0, 0, 1 );
			real l_farHeight = 0;
			real l_farWidth = 0;
			real l_nearHeight = 0;
			real l_nearWidth = 0;

			if ( GetType() == eVIEWPORT_TYPE_ORTHO )
			{
				l_nearHeight = GetBottom() - GetTop();
				l_nearWidth = GetRight() - GetLeft();
				l_farHeight = l_nearHeight;
				l_farWidth = l_nearWidth;
			}
			else
			{
				real l_tan = real( tan( GetFovY().radians() / 2 ) );
				l_nearHeight = 2 * l_tan * GetNear();
				l_nearWidth = l_nearHeight * GetRatio();
				l_farHeight = 2 * l_tan * GetFar();
				l_farWidth = l_farHeight * GetRatio();
			}

			// N => Near, F => Far, C => Center, T => Top, L => Left, R => Right, B => Bottom
			Point3r l_ptNC( l_d * GetNear() );
			Point3r l_ptFC( l_d * GetFar() );
			Point3r l_ptNTL( l_ptNC  + Point3r( real( 0 ), l_nearHeight / 2, real( 0 ) ) - Point3r( l_nearWidth / 2, real( 0 ), real( 0 ) ) );
			Point3r l_ptNBL( l_ptNTL - Point3r( real( 0 ), l_nearHeight, real( 0 ) ) );
			Point3r l_ptNTR( l_ptNTL + Point3r( l_nearWidth, real( 0 ), real( 0 ) ) );
			Point3r l_ptNBR( l_ptNBL + Point3r( l_nearWidth, real( 0 ), real( 0 ) ) );
			Point3r l_ptFTL( l_ptFC  + Point3r( real( 0 ), l_farHeight / 2, real( 0 ) ) - Point3r( l_farWidth / 2, real( 0 ), real( 0 ) ) );
			Point3r l_ptFBL( l_ptFTL - Point3r( real( 0 ), l_farHeight, real( 0 ) ) );
			Point3r l_ptFTR( l_ptFTL + Point3r( l_farWidth, real( 0 ), real( 0 ) ) );
			Point3r l_ptFBR( l_ptFBL + Point3r( l_farWidth, real( 0 ), real( 0 ) ) );
			m_planes[eFRUSTUM_PLANE_NEAR].Set( l_ptNBL, l_ptNTL, l_ptNTR );
			m_planes[eFRUSTUM_PLANE_FAR].Set( l_ptFBR, l_ptFTR, l_ptFTL );
			m_planes[eFRUSTUM_PLANE_LEFT].Set( l_ptFBL, l_ptFTL, l_ptNTL );
			m_planes[eFRUSTUM_PLANE_RIGHT].Set( l_ptNBR, l_ptNTR, l_ptFTR );
			m_planes[eFRUSTUM_PLANE_TOP].Set( l_ptNTL, l_ptFTL, l_ptFTR );
			m_planes[eFRUSTUM_PLANE_BOTTOM].Set( l_ptNBR, l_ptFBR, l_ptFBL );

			switch ( m_type )
			{
			case Castor3D::eVIEWPORT_TYPE_ORTHO:
				DoComputeOrtho( m_left, m_right, m_bottom, m_top, m_near, m_far );
				break;

			case Castor3D::eVIEWPORT_TYPE_PERSPECTIVE:
				DoComputePerspective( m_fovY, m_ratio, m_near, m_far );
				break;

			case Castor3D::eVIEWPORT_TYPE_FRUSTUM:
				DoComputeFrustum( m_left, m_right, m_bottom, m_top, m_near, m_far );
				break;

			default:
				break;
			}

			m_modified = false;
			l_return = true;
		}

		p_pipeline.SetProjectionMatrix( m_projection );
		m_impl->Apply();
		return l_return;
	}

	void Viewport::SetPerspective( Angle const & p_fovY, real p_aspect, real p_near, real p_far )
	{
		m_type = eVIEWPORT_TYPE_PERSPECTIVE;
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
		m_type = eVIEWPORT_TYPE_FRUSTUM;
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
		m_type = eVIEWPORT_TYPE_ORTHO;
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
