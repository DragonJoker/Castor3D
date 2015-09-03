#include "OverlayCategory.hpp"

#include "BorderPanelOverlay.hpp"
#include "Engine.hpp"
#include "Material.hpp"
#include "MaterialManager.hpp"
#include "Overlay.hpp"
#include "OverlayManager.hpp"
#include "OverlayRenderer.hpp"
#include "PanelOverlay.hpp"
#include "TextOverlay.hpp"

using namespace Castor;

namespace Castor3D
{
	OverlayCategory::TextLoader::TextLoader( File::eENCODING_MODE p_eEncodingMode )
		:	Loader< OverlayCategory, eFILE_TYPE_TEXT, TextFile >( File::eOPEN_MODE_DUMMY, p_eEncodingMode )
	{
	}

	bool OverlayCategory::TextLoader::operator()( OverlayCategory const & p_overlay, TextFile & p_file )
	{
		String l_strTabs;
		OverlaySPtr l_pParent = p_overlay.GetOverlay().GetParent();

		while ( l_pParent )
		{
			l_strTabs += cuT( '\t' );
			l_pParent = l_pParent->GetParent();
		}

		bool l_bReturn = p_file.Print( 1024, cuT( "%S\tposition " ), l_strTabs.c_str() ) > 0;

		if ( l_bReturn )
		{
			l_bReturn = Point2d::TextLoader()( p_overlay.GetPosition(), p_file );
		}

		if ( l_bReturn )
		{
			l_bReturn = p_file.Print( 1024, cuT( "\n%S\tsize " ), l_strTabs.c_str() ) > 0;
		}

		if ( l_bReturn )
		{
			l_bReturn = Point2d::TextLoader()( p_overlay.GetSize(), p_file );
		}

		if ( l_bReturn )
		{
			l_bReturn = p_file.WriteText( l_strTabs + cuT( "\tvisible " ) + ( p_overlay.IsVisible() ? String( cuT( "true" ) ) : String( cuT( "false" ) ) ) ) > 0;
		}

		if ( l_bReturn && p_overlay.GetMaterial() )
		{
			l_bReturn = p_file.WriteText( l_strTabs + cuT( "\tmaterial " ) + p_overlay.GetMaterial()->GetName() ) > 0;
		}

		for ( auto && l_overlay : p_overlay.GetOverlay() )
		{
			switch ( l_overlay->GetType() )
			{
			case eOVERLAY_TYPE_PANEL:
				l_bReturn = PanelOverlay::TextLoader()( *l_overlay->GetPanelOverlay(), p_file );
				break;

			case eOVERLAY_TYPE_BORDER_PANEL:
				l_bReturn = BorderPanelOverlay::TextLoader()( *l_overlay->GetBorderPanelOverlay(), p_file );
				break;

			case eOVERLAY_TYPE_TEXT:
				l_bReturn = TextOverlay::TextLoader()( *l_overlay->GetTextOverlay(), p_file );
				break;

			default:
				l_bReturn = false;
			}
		}

		return l_bReturn;
	}

	//*************************************************************************************************

	OverlayCategory::BinaryParser::BinaryParser( Path const & p_path )
		:	Castor3D::BinaryParser< OverlayCategory >( p_path )
	{
	}

	bool OverlayCategory::BinaryParser::Fill( OverlayCategory const & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_bReturn = true;

		if ( l_bReturn )
		{
			l_bReturn = DoFillChunk( p_obj.GetType(), eCHUNK_TYPE_OVERLAY_TYPE, p_chunk );
		}

		if ( l_bReturn )
		{
			l_bReturn = DoFillChunk( p_obj.GetPosition(), eCHUNK_TYPE_OVERLAY_POSITION, p_chunk );
		}

		if ( l_bReturn )
		{
			l_bReturn = DoFillChunk( p_obj.GetSize().const_ptr(), 2, eCHUNK_TYPE_OVERLAY_SIZE, p_chunk );
		}

		if ( l_bReturn && p_obj.GetMaterial() )
		{
			l_bReturn = DoFillChunk( p_obj.GetMaterial()->GetName(), eCHUNK_TYPE_OVERLAY_MATERIAL, p_chunk );
		}

		if ( l_bReturn )
		{
			l_bReturn = DoFillChunk( p_obj.IsVisible(), eCHUNK_TYPE_OVERLAY_VISIBLE, p_chunk );
		}

		return l_bReturn;
	}

	bool OverlayCategory::BinaryParser::Parse( OverlayCategory & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_bReturn = true;
		bool l_visible;
		String l_name;

		switch ( p_chunk.GetChunkType() )
		{
		case eCHUNK_TYPE_OVERLAY_POSITION:
			l_bReturn = DoParseChunk( p_obj.GetPosition(), p_chunk );
			break;

		case eCHUNK_TYPE_OVERLAY_SIZE:
			l_bReturn = DoParseChunk( p_obj.GetSize().ptr(), 2, p_chunk );
			break;

		case eCHUNK_TYPE_OVERLAY_MATERIAL:
			l_bReturn = DoParseChunk( l_name, p_chunk );

			if ( l_bReturn )
			{
				p_obj.SetMaterial( p_obj.m_pOverlay->GetEngine()->GetMaterialManager().find( l_name ) );
			}

			break;

		case eCHUNK_TYPE_OVERLAY_VISIBLE:
			l_bReturn = DoParseChunk( l_visible, p_chunk );

			if ( l_bReturn )
			{
				p_obj.SetVisible( l_visible );
			}

			break;

		default:
			switch ( p_obj.GetType() )
			{
			case eOVERLAY_TYPE_PANEL:
				l_bReturn = PanelOverlay::BinaryParser( m_path ).Parse( *static_cast< PanelOverlay * >( &p_obj ), p_chunk );
				break;

			case eOVERLAY_TYPE_BORDER_PANEL:
				l_bReturn = BorderPanelOverlay::BinaryParser( m_path ).Parse( *static_cast< BorderPanelOverlay * >( &p_obj ), p_chunk );
				break;

			case eOVERLAY_TYPE_TEXT:
				l_bReturn = TextOverlay::BinaryParser( m_path ).Parse( *static_cast< TextOverlay * >( &p_obj ), p_chunk );
				break;

			default:
				l_bReturn = false;
			}

			break;
		}

		if ( !l_bReturn )
		{
			p_chunk.EndParse();
		}

		return l_bReturn;
	}

	//*************************************************************************************************

	OverlayCategory::OverlayCategory( eOVERLAY_TYPE p_eType )
		: m_eType( p_eType )
		, m_bVisible( true )
		, m_changed( true )
		, m_uv( 0, 0, 1, 1 )
		, m_index( 0 )
		, m_level( 0 )
	{
	}

	OverlayCategory::~OverlayCategory()
	{
	}

	void OverlayCategory::Render()
	{
		OverlayRendererSPtr l_renderer = GetOverlay().GetOverlayManager().GetRenderer();

		if ( l_renderer )
		{
			if ( m_changed )
			{
				DoUpdate( l_renderer );
				m_changed = false;
			}

			DoRender( l_renderer );
		}
	}

	void OverlayCategory::SetMaterial( MaterialSPtr p_pMaterial )
	{
		m_pMaterial = p_pMaterial;
		m_changed = true;

		if ( p_pMaterial )
		{
			m_strMatName = p_pMaterial->GetName();
		}
		else
		{
			m_strMatName = cuEmptyString;
		}
	}

	String const & OverlayCategory::GetOverlayName()const
	{
		return m_pOverlay->GetName();
	}

	void OverlayCategory::UpdatePositionAndSize()
	{
		OverlayRendererSPtr l_renderer = GetOverlay().GetOverlayManager().GetRenderer();

		if ( l_renderer )
		{
			OverlaySPtr l_parent = GetOverlay().GetParent();
			Size l_sz = l_renderer->GetSize();
			Point2d l_totalSize( l_sz.width(), l_sz.height() );

			if ( l_parent )
			{
				Point2d l_parentSize = l_parent->GetAbsoluteSize();
				l_totalSize[0] = l_parentSize[0] * l_totalSize[0];
				l_totalSize[1] = l_parentSize[1] * l_totalSize[1];
			}

			Position l_pos = GetPixelPosition();
			Point2d l_ptPos = GetPosition();
			bool l_changed = m_changed;

			if ( l_pos.x() )
			{
				l_changed = !Castor::Policy< double >::equals( l_ptPos[0], l_pos.x() / l_totalSize[0] );
				l_ptPos[0] = l_pos.x() / l_totalSize[0];
			}

			if ( l_pos.y() )
			{
				l_changed = !Castor::Policy< double >::equals( l_ptPos[1], l_pos.y() / l_totalSize[1] );
				l_ptPos[1] = l_pos.y() / l_totalSize[1];
			}

			if ( l_changed )
			{
				SetPosition( l_ptPos );
			}

			Size l_size = GetPixelSize();
			Point2d l_ptSize = GetSize();
			l_changed = m_changed;

			if ( l_size.width() )
			{
				l_changed = !Castor::Policy< double >::equals( l_ptSize[0], l_size.width() / l_totalSize[0] );
				l_ptSize[0] = l_size.width() / l_totalSize[0];
			}

			if ( l_size.height() )
			{
				l_changed = !Castor::Policy< double >::equals( l_ptSize[1], l_size.height() / l_totalSize[1] );
				l_ptSize[1] = l_size.height() / l_totalSize[1];
			}

			if ( l_changed )
			{
				SetSize( l_ptSize );
			}
		}
	}

	Position OverlayCategory::GetAbsolutePosition( Castor::Size const & p_size )const
	{
		Point2d l_position = GetAbsolutePosition();
		return Position( int32_t( p_size.width() * l_position[0] ), int32_t( p_size.height() * l_position[1] ) );
	}

	Size OverlayCategory::GetAbsoluteSize( Castor::Size const & p_size )const
	{
		Point2d l_size = GetAbsoluteSize();
		return Size( uint32_t( p_size.width() * l_size[0] ), uint32_t( p_size.height() * l_size[1] ) );
	}

	Point2d OverlayCategory::GetAbsolutePosition()const
	{
		Point2d l_position = GetPosition();
		OverlaySPtr l_parent = GetOverlay().GetParent();

		if ( l_parent )
		{
			l_position *= l_parent->GetAbsoluteSize();
			l_position += l_parent->GetAbsolutePosition();
		}

		return l_position;
	}

	Point2d OverlayCategory::GetAbsoluteSize()const
	{
		Point2d l_size = GetSize();
		OverlaySPtr l_parent = GetOverlay().GetParent();

		if ( l_parent )
		{
			l_size *= l_parent->GetAbsoluteSize();
		}

		return l_size;
	}
}
