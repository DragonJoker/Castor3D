#include "Viewport.hpp"
#include "Pipeline.hpp"
#include "Engine.hpp"
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

		if ( p_obj.GetType() == eVIEWPORT_TYPE_2D )
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
				l_return = DoFillChunk( double( p_obj.GetFovY().Degrees() ), eCHUNK_TYPE_VIEWPORT_FOVY, l_chunk );
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
						p_obj.SetType( eVIEWPORT_TYPE( l_type ) );
					}

					break;

				case eCHUNK_TYPE_VIEWPORT_NEAR:
					l_return = DoParseChunk( l_value, l_chunk );

					if ( l_return )
					{
						p_obj.SetNear( real( l_value ) );
					}

					break;

				case eCHUNK_TYPE_VIEWPORT_FAR:
					l_return = DoParseChunk( l_value, l_chunk );

					if ( l_return )
					{
						p_obj.SetFar( real( l_value ) );
					}

					break;

				case eCHUNK_TYPE_VIEWPORT_LEFT:
					l_return = DoParseChunk( l_value, l_chunk );

					if ( l_return )
					{
						p_obj.SetLeft( real( l_value ) );
					}

					break;

				case eCHUNK_TYPE_VIEWPORT_RIGHT:
					l_return = DoParseChunk( l_value, l_chunk );

					if ( l_return )
					{
						p_obj.SetRight( real( l_value ) );
					}

					break;

				case eCHUNK_TYPE_VIEWPORT_TOP:
					l_return = DoParseChunk( l_value, l_chunk );

					if ( l_return )
					{
						p_obj.SetTop( real( l_value ) );
					}

					break;

				case eCHUNK_TYPE_VIEWPORT_BOTTOM:
					l_return = DoParseChunk( l_value, l_chunk );

					if ( l_return )
					{
						p_obj.SetBottom( real( l_value ) );
					}

					break;

				case eCHUNK_TYPE_VIEWPORT_FOVY:
					l_return = DoParseChunk( l_value, l_chunk );

					if ( l_return )
					{
						p_obj.SetFovY( Angle::FromDegrees( l_value ) );
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

		if ( p_viewport.GetType() == eVIEWPORT_TYPE_2D )
		{
			if ( l_return )
			{
				l_return = p_file.Print( 256, cuT( "\t\t\tnear %f\n" ), p_viewport.GetNear() ) > 0;
				l_return = p_file.Print( 256, cuT( "\t\t\tfar %f\n" ), p_viewport.GetFar() ) > 0;
				l_return = p_file.Print( 256, cuT( "\t\t\tleft %f\n" ), p_viewport.GetLeft() ) > 0;
				l_return = p_file.Print( 256, cuT( "\t\t\tright %f\n" ), p_viewport.GetRight() ) > 0;
				l_return = p_file.Print( 256, cuT( "\t\t\ttop %f\n" ), p_viewport.GetTop() ) > 0;
				l_return = p_file.Print( 256, cuT( "\t\t\tbottom %f\n" ), p_viewport.GetBottom() ) > 0;
			}
		}
		else
		{
			if ( l_return )
			{
				l_return = p_file.Print( 256, cuT( "\t\t\tnear %f\n" ), p_viewport.GetNear() ) > 0;
				l_return = p_file.Print( 256, cuT( "\t\t\tfar %f\n" ), p_viewport.GetFar() ) > 0;
				l_return = p_file.Print( 256, cuT( "\t\t\tfov_y %f\n" ), p_viewport.GetFovY().Degrees() ) > 0;
			}
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( cuT( "\t\t}\n" ) ) > 0;
		}

		return l_return;
	}

	//*************************************************************************************************

	const String Viewport::string_type[eVIEWPORT_TYPE_COUNT] = { cuT( "3d" ), cuT( "2d" ) };

	Viewport::Viewport( Engine * p_engine, Size const & p_size, eVIEWPORT_TYPE p_type )
		:	m_type( p_type )
		,	m_size( p_size )
		,	m_left( -1 )
		,	m_right( 1 )
		,	m_top( 1 )
		,	m_bottom( -1 )
		,	m_far( 20000 )
		,	m_near( real( 0.2 ) )
		,	m_fovY( Angle::FromDegrees( 45 ) )
		,	m_ratio( 1 )
		,	m_engine( p_engine )
		,	m_modified( true )
	{
		if ( m_type != eVIEWPORT_TYPE_2D )
		{
			m_near = real( 0.1 ); // not zero or we have a Z fight (?????)
		}

		m_ratio = real( p_size.width() ) / p_size.height();
	}

	Viewport::Viewport( Viewport const & p_viewport )
		:	m_type( p_viewport.m_type )
		,	m_size( p_viewport.m_size )
		,	m_left( p_viewport.m_left )
		,	m_right( p_viewport.m_right )
		,	m_top( p_viewport.m_top )
		,	m_bottom( p_viewport.m_bottom )
		,	m_far( p_viewport.m_far )
		,	m_near( p_viewport.m_near )
		,	m_fovY( p_viewport.m_fovY )
		,	m_ratio( p_viewport.m_ratio )
		,	m_engine( p_viewport.m_engine )
		,	m_modified( p_viewport.m_modified )
	{
	}

	Viewport::Viewport( Viewport && p_viewport )
		:	m_type( std::move( p_viewport.m_type ) )
		,	m_size( std::move( p_viewport.m_size ) )
		,	m_left( std::move( p_viewport.m_left ) )
		,	m_right( std::move( p_viewport.m_right ) )
		,	m_top( std::move( p_viewport.m_top ) )
		,	m_bottom( std::move( p_viewport.m_bottom ) )
		,	m_far( std::move( p_viewport.m_far ) )
		,	m_near( std::move( p_viewport.m_near ) )
		,	m_fovY( std::move( p_viewport.m_fovY ) )
		,	m_ratio( std::move( p_viewport.m_ratio ) )
		,	m_engine( std::move( p_viewport.m_engine ) )
		,	m_modified( std::move( p_viewport.m_modified ) )
	{
		p_viewport.m_type = eVIEWPORT_TYPE_3D;
		p_viewport.m_left = -2;
		p_viewport.m_right = 2;
		p_viewport.m_top = 2;
		p_viewport.m_bottom = -2;
		p_viewport.m_far = 2000;
		p_viewport.m_near = 1.0;
		p_viewport.m_fovY = Angle::FromDegrees( 45.0 );
		p_viewport.m_ratio = 1;
		p_viewport.m_engine = NULL;
		p_viewport.m_modified = true;
	}

	Viewport::~Viewport()
	{
	}

	Viewport & Viewport::operator =( Viewport const & p_viewport )
	{
		m_type = p_viewport.m_type;
		m_size = p_viewport.m_size;
		m_left = p_viewport.m_left;
		m_right = p_viewport.m_right;
		m_top = p_viewport.m_top;
		m_bottom = p_viewport.m_bottom;
		m_far = p_viewport.m_far;
		m_near = p_viewport.m_near;
		m_fovY = p_viewport.m_fovY;
		m_ratio = p_viewport.m_ratio;
		m_engine = p_viewport.m_engine;
		m_modified = p_viewport.m_modified;
		return * this;
	}

	Viewport & Viewport::operator =( Viewport && p_viewport )
	{
		if ( this != & p_viewport )
		{
			m_type = std::move( p_viewport.m_type );
			m_size = std::move( p_viewport.m_size );
			m_left = std::move( p_viewport.m_left );
			m_right = std::move( p_viewport.m_right );
			m_top = std::move( p_viewport.m_top );
			m_bottom = std::move( p_viewport.m_bottom );
			m_far = std::move( p_viewport.m_far );
			m_near = std::move( p_viewport.m_near );
			m_fovY = std::move( p_viewport.m_fovY );
			m_ratio = std::move( p_viewport.m_ratio );
			m_engine = std::move( p_viewport.m_engine );
			m_modified = std::move( p_viewport.m_modified );
			p_viewport.m_type = eVIEWPORT_TYPE_3D;
			p_viewport.m_left = -2;
			p_viewport.m_right = 2;
			p_viewport.m_top = 2;
			p_viewport.m_bottom = -2;
			p_viewport.m_far = 2000;
			p_viewport.m_near = 1.0;
			p_viewport.m_fovY = Angle::FromDegrees( 45.0 );
			p_viewport.m_ratio = 1;
			p_viewport.m_engine = NULL;
			p_viewport.m_modified = true;
		}

		return * this;
	}

	bool Viewport::Render()
	{
		bool l_return = false;

		if ( IsModified() )
		{
			if ( !m_engine->GetRenderSystem()->HasNonPowerOfTwoTextures() )
			{
				m_size.set( GetNext2Pow( m_size.width() ), GetNext2Pow( m_size.height() ) );
			}

			Point3r l_d( 0, 0, 1 );
			real l_farHeight = 0;
			real l_farWidth = 0;
			real l_nearHeight = 0;
			real l_nearWidth = 0;

			if ( GetType() == eVIEWPORT_TYPE_3D )
			{
				real l_tan = real( tan( GetFovY().Radians() / 2 ) );
				l_nearHeight = 2 * l_tan * GetNear();
				l_nearWidth = l_nearHeight * GetRatio();
				l_farHeight = 2 * l_tan * GetFar();
				l_farWidth = l_farHeight * GetRatio();
			}
			else if ( GetType() == eVIEWPORT_TYPE_2D )
			{
				l_nearHeight = GetBottom() - GetTop();
				l_nearWidth = GetRight() - GetLeft();
				l_farHeight = l_nearHeight;
				l_farWidth = l_nearWidth;
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

			if ( m_type == eVIEWPORT_TYPE_3D )
			{
				matrix::perspective( m_projection, m_fovY, m_ratio, m_near, m_far );
			}
			else if ( m_type == eVIEWPORT_TYPE_2D )
			{
				matrix::ortho( m_projection, m_left, m_right, m_bottom, m_top, m_near, m_far );
			}

			m_modified = false;
			l_return = true;
		}

		m_engine->GetRenderSystem()->GetPipeline().ApplyViewport( m_size.width(), m_size.height() );
		m_engine->GetRenderSystem()->GetPipeline().SetProjectionMatrix( m_projection );
		return l_return;
	}

	void Viewport::GetDirection( Point2i const & p_ptMouse, Point3r & p_ptResult )
	{
		Point4r l_viewport( real( 0 ), real( 0 ), real( m_size.width() ), real( m_size.height() ) );
		m_engine->GetRenderSystem()->GetPipeline().UnProject( Point3i( p_ptMouse[0], p_ptMouse[1], 1 ), l_viewport, p_ptResult );
	}
}
