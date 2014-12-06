#include "Viewport.hpp"
#include "Pipeline.hpp"
#include "Engine.hpp"
#include "RenderSystem.hpp"

using namespace Castor;

namespace Castor3D
{
	Viewport::BinaryParser::BinaryParser( Path const & p_path )
		:	Castor3D::BinaryParser< Viewport >( p_path )
	{
	}

	bool Viewport::BinaryParser::Fill( Viewport const & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_bReturn = true;
		BinaryChunk l_chunk( eCHUNK_TYPE_CAMERA );

		if ( l_bReturn )
		{
			l_bReturn = DoFillChunk( uint8_t( p_obj.GetType() ), eCHUNK_TYPE_VIEWPORT_TYPE, l_chunk );
		}

		if ( l_bReturn )
		{
			l_bReturn = DoFillChunk( double( p_obj.GetNear() ), eCHUNK_TYPE_VIEWPORT_NEAR, l_chunk );
		}

		if ( l_bReturn )
		{
			l_bReturn = DoFillChunk( double( p_obj.GetFar() ), eCHUNK_TYPE_VIEWPORT_FAR, l_chunk );
		}

		if ( p_obj.GetType() == eVIEWPORT_TYPE_2D )
		{
			if ( l_bReturn )
			{
				l_bReturn = DoFillChunk( double( p_obj.GetLeft() ), eCHUNK_TYPE_VIEWPORT_LEFT, l_chunk );
			}

			if ( l_bReturn )
			{
				l_bReturn = DoFillChunk( double( p_obj.GetRight() ), eCHUNK_TYPE_VIEWPORT_RIGHT, l_chunk );
			}

			if ( l_bReturn )
			{
				l_bReturn = DoFillChunk( double( p_obj.GetTop() ), eCHUNK_TYPE_VIEWPORT_TOP, l_chunk );
			}

			if ( l_bReturn )
			{
				l_bReturn = DoFillChunk( double( p_obj.GetBottom() ), eCHUNK_TYPE_VIEWPORT_BOTTOM, l_chunk );
			}
		}
		else
		{
			if ( l_bReturn )
			{
				l_bReturn = DoFillChunk( double( p_obj.GetFovY().Degrees() ), eCHUNK_TYPE_VIEWPORT_FOVY, l_chunk );
			}
		}

		return l_bReturn;
	}

	bool Viewport::BinaryParser::Parse( Viewport & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_bReturn = true;
		uint8_t l_type;
		String l_name;
		double l_value;

		while ( p_chunk.CheckAvailable( 1 ) )
		{
			BinaryChunk l_chunk;
			l_bReturn = p_chunk.GetSubChunk( l_chunk );

			if ( l_bReturn )
			{
				switch ( l_chunk.GetChunkType() )
				{
				case eCHUNK_TYPE_VIEWPORT_TYPE:
					l_bReturn = DoParseChunk( l_type, l_chunk );

					if ( l_bReturn )
					{
						p_obj.SetType( eVIEWPORT_TYPE( l_type ) );
					}

					break;

				case eCHUNK_TYPE_VIEWPORT_NEAR:
					l_bReturn = DoParseChunk( l_value, l_chunk );

					if ( l_bReturn )
					{
						p_obj.SetNear( real( l_value ) );
					}

					break;

				case eCHUNK_TYPE_VIEWPORT_FAR:
					l_bReturn = DoParseChunk( l_value, l_chunk );

					if ( l_bReturn )
					{
						p_obj.SetFar( real( l_value ) );
					}

					break;

				case eCHUNK_TYPE_VIEWPORT_LEFT:
					l_bReturn = DoParseChunk( l_value, l_chunk );

					if ( l_bReturn )
					{
						p_obj.SetLeft( real( l_value ) );
					}

					break;

				case eCHUNK_TYPE_VIEWPORT_RIGHT:
					l_bReturn = DoParseChunk( l_value, l_chunk );

					if ( l_bReturn )
					{
						p_obj.SetRight( real( l_value ) );
					}

					break;

				case eCHUNK_TYPE_VIEWPORT_TOP:
					l_bReturn = DoParseChunk( l_value, l_chunk );

					if ( l_bReturn )
					{
						p_obj.SetTop( real( l_value ) );
					}

					break;

				case eCHUNK_TYPE_VIEWPORT_BOTTOM:
					l_bReturn = DoParseChunk( l_value, l_chunk );

					if ( l_bReturn )
					{
						p_obj.SetBottom( real( l_value ) );
					}

					break;

				case eCHUNK_TYPE_VIEWPORT_FOVY:
					l_bReturn = DoParseChunk( l_value, l_chunk );

					if ( l_bReturn )
					{
						p_obj.SetFovY( Angle::FromDegrees( l_value ) );
					}

					break;
				}
			}

			if ( !l_bReturn )
			{
				p_chunk.EndParse();
			}
		}

		return l_bReturn;
	}

	//*************************************************************************************************

	Viewport::TextLoader::TextLoader( File::eENCODING_MODE p_eEncodingMode )
		:	Loader< Viewport, eFILE_TYPE_TEXT, TextFile >( File::eOPEN_MODE_DUMMY, p_eEncodingMode )
	{
	}

	bool Viewport::TextLoader::operator()( Viewport const & p_viewport, TextFile & p_file )
	{
		bool l_bReturn = p_file.WriteText( cuT( "\t\tviewport\n\t\t{\n" ) ) > 0;

		if ( l_bReturn )
		{
			l_bReturn = p_file.WriteText( cuT( "\t\t\ttype " ) + Viewport::string_type[p_viewport.GetType()] + cuT( "\n" ) ) > 0;
		}

		if ( p_viewport.GetType() == eVIEWPORT_TYPE_2D )
		{
			if ( l_bReturn )
			{
				l_bReturn = p_file.Print( 256, cuT( "\t\t\tnear %f\n" ), p_viewport.GetNear() ) > 0;
				l_bReturn = p_file.Print( 256, cuT( "\t\t\tfar %f\n" ), p_viewport.GetFar() ) > 0;
				l_bReturn = p_file.Print( 256, cuT( "\t\t\tleft %f\n" ), p_viewport.GetLeft() ) > 0;
				l_bReturn = p_file.Print( 256, cuT( "\t\t\tright %f\n" ), p_viewport.GetRight() ) > 0;
				l_bReturn = p_file.Print( 256, cuT( "\t\t\ttop %f\n" ), p_viewport.GetTop() ) > 0;
				l_bReturn = p_file.Print( 256, cuT( "\t\t\tbottom %f\n" ), p_viewport.GetBottom() ) > 0;
			}
		}
		else
		{
			if ( l_bReturn )
			{
				l_bReturn = p_file.Print( 256, cuT( "\t\t\tnear %f\n" ), p_viewport.GetNear() ) > 0;
				l_bReturn = p_file.Print( 256, cuT( "\t\t\tfar %f\n" ), p_viewport.GetFar() ) > 0;
				l_bReturn = p_file.Print( 256, cuT( "\t\t\tfov_y %f\n" ), p_viewport.GetFovY().Degrees() ) > 0;
			}
		}

		if ( l_bReturn )
		{
			l_bReturn = p_file.WriteText( cuT( "\t\t}\n" ) ) > 0;
		}

		return l_bReturn;
	}

	//*************************************************************************************************

	const String Viewport::string_type[eVIEWPORT_TYPE_COUNT] = { cuT( "3d" ), cuT( "2d" ) };

	Viewport::Viewport( Engine * p_pEngine, Size const & p_size, eVIEWPORT_TYPE p_eType )
		:	m_eType( p_eType )
		,	m_size( p_size )
		,	m_rLeft( -1 )
		,	m_rRight( 1 )
		,	m_rTop( 1 )
		,	m_rBottom( -1 )
		,	m_rFar( 20000 )
		,	m_rNear( real( 0.2 ) )
		,	m_aFovY( Angle::FromDegrees( 45 ) )
		,	m_rRatio( 1 )
		,	m_pEngine( p_pEngine )
		,	m_bModified( true )
	{
		if ( m_eType != eVIEWPORT_TYPE_2D )
		{
			m_rNear = real( 0.1 ); // not zero or we have a Z fight (?????)
		}

		m_rRatio = real( p_size.width() ) / p_size.height();
	}

	Viewport::Viewport( Viewport const & p_viewport )
		:	m_eType( p_viewport.m_eType )
		,	m_size( p_viewport.m_size )
		,	m_rLeft( p_viewport.m_rLeft )
		,	m_rRight( p_viewport.m_rRight )
		,	m_rTop( p_viewport.m_rTop )
		,	m_rBottom( p_viewport.m_rBottom )
		,	m_rFar( p_viewport.m_rFar )
		,	m_rNear( p_viewport.m_rNear )
		,	m_aFovY( p_viewport.m_aFovY )
		,	m_rRatio( p_viewport.m_rRatio )
		,	m_pEngine( p_viewport.m_pEngine )
		,	m_bModified( p_viewport.m_bModified )
	{
	}

	Viewport::Viewport( Viewport && p_viewport )
		:	m_eType( std::move( p_viewport.m_eType ) )
		,	m_size( std::move( p_viewport.m_size ) )
		,	m_rLeft( std::move( p_viewport.m_rLeft ) )
		,	m_rRight( std::move( p_viewport.m_rRight ) )
		,	m_rTop( std::move( p_viewport.m_rTop ) )
		,	m_rBottom( std::move( p_viewport.m_rBottom ) )
		,	m_rFar( std::move( p_viewport.m_rFar ) )
		,	m_rNear( std::move( p_viewport.m_rNear ) )
		,	m_aFovY( std::move( p_viewport.m_aFovY ) )
		,	m_rRatio( std::move( p_viewport.m_rRatio ) )
		,	m_pEngine( std::move( p_viewport.m_pEngine ) )
		,	m_bModified( std::move( p_viewport.m_bModified ) )
	{
		p_viewport.m_eType			= eVIEWPORT_TYPE_3D;
		p_viewport.m_rLeft			= -2;
		p_viewport.m_rRight			= 2;
		p_viewport.m_rTop			= 2;
		p_viewport.m_rBottom		= -2;
		p_viewport.m_rFar			= 2000;
		p_viewport.m_rNear			= 1.0;
		p_viewport.m_aFovY			= Angle::FromDegrees( 45.0 );
		p_viewport.m_rRatio			= 1;
		p_viewport.m_pEngine		= NULL;
		p_viewport.m_bModified		= true;
	}

	Viewport::~Viewport()
	{
	}

	Viewport & Viewport::operator =( Viewport const & p_viewport )
	{
		m_eType		= p_viewport.m_eType;
		m_size		= p_viewport.m_size;
		m_rLeft		= p_viewport.m_rLeft;
		m_rRight	= p_viewport.m_rRight;
		m_rTop		= p_viewport.m_rTop;
		m_rBottom	= p_viewport.m_rBottom;
		m_rFar		= p_viewport.m_rFar;
		m_rNear		= p_viewport.m_rNear;
		m_aFovY		= p_viewport.m_aFovY;
		m_rRatio	= p_viewport.m_rRatio;
		m_pEngine	= p_viewport.m_pEngine;
		m_bModified	= p_viewport.m_bModified;
		return * this;
	}

	Viewport & Viewport::operator =( Viewport && p_viewport )
	{
		if ( this != & p_viewport )
		{
			m_eType		= std::move( p_viewport.m_eType );
			m_size		= std::move( p_viewport.m_size );
			m_rLeft		= std::move( p_viewport.m_rLeft );
			m_rRight	= std::move( p_viewport.m_rRight );
			m_rTop		= std::move( p_viewport.m_rTop );
			m_rBottom	= std::move( p_viewport.m_rBottom );
			m_rFar		= std::move( p_viewport.m_rFar );
			m_rNear		= std::move( p_viewport.m_rNear );
			m_aFovY		= std::move( p_viewport.m_aFovY );
			m_rRatio	= std::move( p_viewport.m_rRatio );
			m_pEngine	= std::move( p_viewport.m_pEngine );
			m_bModified	= std::move( p_viewport.m_bModified );
			p_viewport.m_eType			= eVIEWPORT_TYPE_3D;
			p_viewport.m_rLeft			= -2;
			p_viewport.m_rRight			= 2;
			p_viewport.m_rTop			= 2;
			p_viewport.m_rBottom		= -2;
			p_viewport.m_rFar			= 2000;
			p_viewport.m_rNear			= 1.0;
			p_viewport.m_aFovY			= Angle::FromDegrees( 45.0 );
			p_viewport.m_rRatio			= 1;
			p_viewport.m_pEngine		= NULL;
			p_viewport.m_bModified		= true;
		}

		return * this;
	}

	bool Viewport::Render()
	{
		bool l_bReturn = false;

		if ( IsModified() )
		{
			if ( !m_pEngine->GetRenderSystem()->HasNonPowerOfTwoTextures() )
			{
				m_size.set( GetNext2Pow( m_size.width() ), GetNext2Pow( m_size.height() ) );
			}

			Point3r l_ptU( 0, 1, 0 );
			Point3r l_ptD( 0, 0, 1 );
			real l_rFarHeight = 0;
			real l_rFarWidth = 0;
			real l_rNearHeight = 0;
			real l_rNearWidth = 0;

			if ( GetType() == eVIEWPORT_TYPE_3D )
			{
				real l_rTan = tan( GetFovY().Radians() / 2 );
				l_rNearHeight	= 2 * l_rTan * GetNear();
				l_rNearWidth	= l_rNearHeight * GetRatio();
				l_rFarHeight	= 2 * l_rTan * GetFar();
				l_rFarWidth		= l_rFarHeight * GetRatio();
			}
			else if ( GetType() == eVIEWPORT_TYPE_2D )
			{
				l_rNearHeight	= GetBottom() - GetTop();
				l_rNearWidth	= GetRight() - GetLeft();
				l_rFarHeight	= l_rNearHeight;
				l_rFarWidth		= l_rNearWidth;
			}

			// N => Near, F => Far, C => Center, T => Top, L => Left, R => Right, B => Bottom
			Point3r l_ptNC( l_ptD * GetNear() );
			Point3r l_ptFC( l_ptD * GetFar() );
			Point3r l_ptNTL( l_ptNC  + Point3r( real( 0 ), l_rNearHeight / 2, real( 0 ) ) - Point3r( l_rNearWidth / 2, real( 0 ), real( 0 ) ) );
			Point3r l_ptNBL( l_ptNTL - Point3r( real( 0 ), l_rNearHeight, real( 0 ) ) );
			Point3r l_ptNTR( l_ptNTL + Point3r( l_rNearWidth, real( 0 ), real( 0 ) ) );
			Point3r l_ptNBR( l_ptNBL + Point3r( l_rNearWidth, real( 0 ), real( 0 ) ) );
			Point3r l_ptFTL( l_ptFC  + Point3r( real( 0 ), l_rFarHeight / 2, real( 0 ) ) - Point3r( l_rFarWidth / 2, real( 0 ), real( 0 ) ) );
			Point3r l_ptFBL( l_ptFTL - Point3r( real( 0 ), l_rFarHeight, real( 0 ) ) );
			Point3r l_ptFTR( l_ptFTL + Point3r( l_rFarWidth, real( 0 ), real( 0 ) ) );
			Point3r l_ptFBR( l_ptFBL + Point3r( l_rFarWidth, real( 0 ), real( 0 ) ) );
			m_planes[eFRUSTUM_PLANE_NEAR	].Set( l_ptNBL, l_ptNTL, l_ptNTR );
			m_planes[eFRUSTUM_PLANE_FAR		].Set( l_ptFBR, l_ptFTR, l_ptFTL );
			m_planes[eFRUSTUM_PLANE_LEFT	].Set( l_ptFBL, l_ptFTL, l_ptNTL );
			m_planes[eFRUSTUM_PLANE_RIGHT	].Set( l_ptNBR, l_ptNTR, l_ptFTR );
			m_planes[eFRUSTUM_PLANE_TOP		].Set( l_ptNTL, l_ptFTL, l_ptFTR );
			m_planes[eFRUSTUM_PLANE_BOTTOM	].Set( l_ptNBR, l_ptFBR, l_ptFBL );
			m_bModified = false;
			l_bReturn = true;
		}

		Pipeline * l_pPipeline = m_pEngine->GetRenderSystem()->GetPipeline();
#if DX_DEBUG
		Size l_size = m_pEngine->GetRenderSystem()->GetCurrentContext()->GetMaxSize( m_size );
		l_pPipeline->ApplyViewport( l_size.width(), l_size.height() );
#else
		l_pPipeline->ApplyViewport( m_size.width(), m_size.height() );
#endif
		l_pPipeline->MatrixMode( eMTXMODE_PROJECTION );
		l_pPipeline->LoadIdentity();

		if ( m_eType == eVIEWPORT_TYPE_3D )
		{
			l_pPipeline->Perspective( m_aFovY, m_rRatio, m_rNear, m_rFar );
		}
		else if ( m_eType == eVIEWPORT_TYPE_2D )
		{
			l_pPipeline->Ortho( m_rLeft, m_rRight, m_rBottom, m_rTop, m_rNear, m_rFar );
		}

		return l_bReturn;
	}

	void Viewport::GetDirection( Point2i const & p_ptMouse, Point3r & p_ptResult )
	{
		Point4r l_viewport( real( 0 ), real( 0 ), real( m_size.width() ), real( m_size.height() ) );
		m_pEngine->GetRenderSystem()->GetPipeline()->UnProject( Point3i( p_ptMouse[0], p_ptMouse[1], 1 ), l_viewport, p_ptResult );
	}
}
