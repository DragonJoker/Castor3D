#include "Viewport.hpp"

#include "Engine.hpp"
#include "Pipeline.hpp"
#include "RenderSystem.hpp"

#include <TransformationMatrix.hpp>

using namespace Castor;

namespace Castor3D
{
	Viewport::BinaryParser::BinaryParser( Path const & p_path )
		:	Castor3D::BinaryParser< Viewport >( p_path )
	{
	}

	bool Viewport::BinaryParser::Fill( Viewport const & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_return = true;
		BinaryChunk l_chunk( eCHUNK_TYPE_CAMERA );

		if ( l_return )
		{
			l_return = DoFillChunk( uint8_t( p_obj.GetType() ), eCHUNK_TYPE_VIEWPORT_TYPE, l_chunk );
		}

		if ( l_return )
		{
			l_return = DoFillChunk( double( p_obj.GetNear() ), eCHUNK_TYPE_VIEWPORT_NEAR, l_chunk );
		}

		if ( l_return )
		{
			l_return = DoFillChunk( double( p_obj.GetFar() ), eCHUNK_TYPE_VIEWPORT_FAR, l_chunk );
		}

		if ( p_obj.GetType() != eVIEWPORT_TYPE_PERSPECTIVE )
		{
			if ( l_return )
			{
				l_return = DoFillChunk( double( p_obj.GetLeft() ), eCHUNK_TYPE_VIEWPORT_LEFT, l_chunk );
			}

			if ( l_return )
			{
				l_return = DoFillChunk( double( p_obj.GetRight() ), eCHUNK_TYPE_VIEWPORT_RIGHT, l_chunk );
			}

			if ( l_return )
			{
				l_return = DoFillChunk( double( p_obj.GetTop() ), eCHUNK_TYPE_VIEWPORT_TOP, l_chunk );
			}

			if ( l_return )
			{
				l_return = DoFillChunk( double( p_obj.GetBottom() ), eCHUNK_TYPE_VIEWPORT_BOTTOM, l_chunk );
			}
		}
		else
		{
			if ( l_return )
			{
				l_return = DoFillChunk( double( p_obj.GetFovY().degrees() ), eCHUNK_TYPE_VIEWPORT_FOVY, l_chunk );
			}
		}

		return l_return;
	}

	bool Viewport::BinaryParser::Parse( Viewport & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_return = true;
		uint8_t l_type;
		String l_name;
		double l_value;

		while ( p_chunk.CheckAvailable( 1 ) )
		{
			BinaryChunk l_chunk;
			l_return = p_chunk.GetSubChunk( l_chunk );

			if ( l_return )
			{
				switch ( l_chunk.GetChunkType() )
				{
				case eCHUNK_TYPE_VIEWPORT_TYPE:
					l_return = DoParseChunk( l_type, l_chunk );

					if ( l_return )
					{
						p_obj.UpdateType( eVIEWPORT_TYPE( l_type ) );
					}

					break;

				case eCHUNK_TYPE_VIEWPORT_NEAR:
					l_return = DoParseChunk( l_value, l_chunk );

					if ( l_return )
					{
						p_obj.UpdateNear( real( l_value ) );
					}

					break;

				case eCHUNK_TYPE_VIEWPORT_FAR:
					l_return = DoParseChunk( l_value, l_chunk );

					if ( l_return )
					{
						p_obj.UpdateFar( real( l_value ) );
					}

					break;

				case eCHUNK_TYPE_VIEWPORT_LEFT:
					l_return = DoParseChunk( l_value, l_chunk );

					if ( l_return )
					{
						p_obj.UpdateLeft( real( l_value ) );
					}

					break;

				case eCHUNK_TYPE_VIEWPORT_RIGHT:
					l_return = DoParseChunk( l_value, l_chunk );

					if ( l_return )
					{
						p_obj.UpdateRight( real( l_value ) );
					}

					break;

				case eCHUNK_TYPE_VIEWPORT_TOP:
					l_return = DoParseChunk( l_value, l_chunk );

					if ( l_return )
					{
						p_obj.UpdateTop( real( l_value ) );
					}

					break;

				case eCHUNK_TYPE_VIEWPORT_BOTTOM:
					l_return = DoParseChunk( l_value, l_chunk );

					if ( l_return )
					{
						p_obj.UpdateBottom( real( l_value ) );
					}

					break;

				case eCHUNK_TYPE_VIEWPORT_FOVY:
					l_return = DoParseChunk( l_value, l_chunk );

					if ( l_return )
					{
						p_obj.UpdateFovY( Angle::from_degrees( l_value ) );
					}

					break;
				}
			}

			if ( !l_return )
			{
				p_chunk.EndParse();
			}
		}

		return l_return;
	}

	//*************************************************************************************************

	Viewport::TextLoader::TextLoader( File::eENCODING_MODE p_encodingMode )
		:	Loader< Viewport, eFILE_TYPE_TEXT, TextFile >( File::eOPEN_MODE_DUMMY, p_encodingMode )
	{
	}

	bool Viewport::TextLoader::operator()( Viewport const & p_viewport, TextFile & p_file )
	{
		bool l_return = p_file.WriteText( cuT( "\t\tviewport\n\t\t{\n" ) ) > 0;

		if ( l_return )
		{
			l_return = p_file.WriteText( cuT( "\t\t\ttype " ) + Viewport::string_type[p_viewport.GetType()] + cuT( "\n" ) ) > 0;
		}

		if ( l_return )
		{
			if ( p_viewport.GetType() == eVIEWPORT_TYPE_ORTHO || p_viewport.GetType() == eVIEWPORT_TYPE_FRUSTUM )
			{
				l_return = p_file.Print( 256, cuT( "\t\t\tnear %f\n" ), p_viewport.GetNear() ) > 0;
				l_return = p_file.Print( 256, cuT( "\t\t\tfar %f\n" ), p_viewport.GetFar() ) > 0;
				l_return = p_file.Print( 256, cuT( "\t\t\tleft %f\n" ), p_viewport.GetLeft() ) > 0;
				l_return = p_file.Print( 256, cuT( "\t\t\tright %f\n" ), p_viewport.GetRight() ) > 0;
				l_return = p_file.Print( 256, cuT( "\t\t\ttop %f\n" ), p_viewport.GetTop() ) > 0;
				l_return = p_file.Print( 256, cuT( "\t\t\tbottom %f\n" ), p_viewport.GetBottom() ) > 0;
			}
			else
			{
				l_return = p_file.Print( 256, cuT( "\t\t\tnear %f\n" ), p_viewport.GetNear() ) > 0;
				l_return = p_file.Print( 256, cuT( "\t\t\tfar %f\n" ), p_viewport.GetFar() ) > 0;
				l_return = p_file.Print( 256, cuT( "\t\t\tfov_y %f\n" ), p_viewport.GetFovY().degrees() ) > 0;
			}
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( cuT( "\t\t}\n" ) ) > 0;
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
