#include "OverlayCategory.hpp"

#include "Engine.hpp"
#include "MaterialManager.hpp"
#include "OverlayManager.hpp"

#include "BorderPanelOverlay.hpp"
#include "OverlayRenderer.hpp"
#include "PanelOverlay.hpp"
#include "TextOverlay.hpp"

using namespace Castor;

namespace Castor3D
{
	OverlayCategory::TextLoader::TextLoader( File::eENCODING_MODE p_encodingMode )
		:	Loader< OverlayCategory, eFILE_TYPE_TEXT, TextFile >( File::eOPEN_MODE_DUMMY, p_encodingMode )
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

		bool l_return = p_file.Print( 1024, cuT( "%S\tposition " ), l_strTabs.c_str() ) > 0;

		if ( l_return )
		{
			l_return = Point2d::TextLoader()( p_overlay.GetPosition(), p_file );
		}

		if ( l_return )
		{
			l_return = p_file.Print( 1024, cuT( "\n%S\tsize " ), l_strTabs.c_str() ) > 0;
		}

		if ( l_return )
		{
			l_return = Point2d::TextLoader()( p_overlay.GetSize(), p_file );
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( l_strTabs + cuT( "\tvisible " ) + ( p_overlay.IsVisible() ? String( cuT( "true" ) ) : String( cuT( "false" ) ) ) ) > 0;
		}

		if ( l_return && p_overlay.GetMaterial() )
		{
			l_return = p_file.WriteText( l_strTabs + cuT( "\tmaterial " ) + p_overlay.GetMaterial()->GetName() ) > 0;
		}

		for ( auto && l_overlay : p_overlay.GetOverlay() )
		{
			switch ( l_overlay->GetType() )
			{
			case eOVERLAY_TYPE_PANEL:
				l_return = PanelOverlay::TextLoader()( *l_overlay->GetPanelOverlay(), p_file );
				break;

			case eOVERLAY_TYPE_BORDER_PANEL:
				l_return = BorderPanelOverlay::TextLoader()( *l_overlay->GetBorderPanelOverlay(), p_file );
				break;

			case eOVERLAY_TYPE_TEXT:
				l_return = TextOverlay::TextLoader()( *l_overlay->GetTextOverlay(), p_file );
				break;

			default:
				l_return = false;
			}
		}

		return l_return;
	}

	//*************************************************************************************************

	OverlayCategory::BinaryParser::BinaryParser( Path const & p_path )
		:	Castor3D::BinaryParser< OverlayCategory >( p_path )
	{
	}

	bool OverlayCategory::BinaryParser::Fill( OverlayCategory const & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_return = true;

		if ( l_return )
		{
			l_return = DoFillChunk( p_obj.GetType(), eCHUNK_TYPE_OVERLAY_TYPE, p_chunk );
		}

		if ( l_return )
		{
			l_return = DoFillChunk( p_obj.GetPosition(), eCHUNK_TYPE_OVERLAY_POSITION, p_chunk );
		}

		if ( l_return )
		{
			l_return = DoFillChunk( p_obj.GetSize().const_ptr(), 2, eCHUNK_TYPE_OVERLAY_SIZE, p_chunk );
		}

		if ( l_return && p_obj.GetMaterial() )
		{
			l_return = DoFillChunk( p_obj.GetMaterial()->GetName(), eCHUNK_TYPE_OVERLAY_MATERIAL, p_chunk );
		}

		if ( l_return )
		{
			l_return = DoFillChunk( p_obj.IsVisible(), eCHUNK_TYPE_OVERLAY_VISIBLE, p_chunk );
		}

		return l_return;
	}

	bool OverlayCategory::BinaryParser::Parse( OverlayCategory & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_return = true;
		bool l_visible;
		String l_name;

		switch ( p_chunk.GetChunkType() )
		{
		case eCHUNK_TYPE_OVERLAY_POSITION:
			l_return = DoParseChunk( p_obj.GetPosition(), p_chunk );
			break;

		case eCHUNK_TYPE_OVERLAY_SIZE:
			l_return = DoParseChunk( p_obj.GetSize().ptr(), 2, p_chunk );
			break;

		case eCHUNK_TYPE_OVERLAY_MATERIAL:
			l_return = DoParseChunk( l_name, p_chunk );

			if ( l_return )
			{
				p_obj.SetMaterial( p_obj.m_pOverlay->GetEngine()->GetMaterialManager().Find( l_name ) );
			}

			break;

		case eCHUNK_TYPE_OVERLAY_VISIBLE:
			l_return = DoParseChunk( l_visible, p_chunk );

			if ( l_return )
			{
				p_obj.SetVisible( l_visible );
			}

			break;

		default:
			switch ( p_obj.GetType() )
			{
			case eOVERLAY_TYPE_PANEL:
				l_return = PanelOverlay::BinaryParser( m_path ).Parse( *static_cast< PanelOverlay * >( &p_obj ), p_chunk );
				break;

			case eOVERLAY_TYPE_BORDER_PANEL:
				l_return = BorderPanelOverlay::BinaryParser( m_path ).Parse( *static_cast< BorderPanelOverlay * >( &p_obj ), p_chunk );
				break;

			case eOVERLAY_TYPE_TEXT:
				l_return = TextOverlay::BinaryParser( m_path ).Parse( *static_cast< TextOverlay * >( &p_obj ), p_chunk );
				break;

			default:
				l_return = false;
			}

			break;
		}

		if ( !l_return )
		{
			p_chunk.EndParse();
		}

		return l_return;
	}

	//*************************************************************************************************

	OverlayCategory::OverlayCategory( eOVERLAY_TYPE p_type )
		: m_type( p_type )
		, m_visible( true )
		, m_sizeChanged( true )
		, m_positionChanged( true )
		, m_uv( 0, 0, 1, 1 )
		, m_index( 0 )
		, m_level( 0 )
	{
	}

	OverlayCategory::~OverlayCategory()
	{
	}

	void OverlayCategory::Update()
	{
		OverlayRendererSPtr l_renderer = GetOverlay().GetOverlayManager().GetRenderer();

		if ( l_renderer )
		{
			if ( IsChanged() || IsPositionChanged() || IsSizeChanged() || l_renderer->IsSizeChanged() )
			{
				DoUpdate();
				DoUpdatePosition();
				DoUpdateSize();
				DoUpdateBuffer( l_renderer->GetSize() );
				m_sizeChanged = false;
				m_positionChanged = false;
			}
		}
	}

	void OverlayCategory::Render()
	{
		DoRender( GetOverlay().GetOverlayManager().GetRenderer() );
	}

	void OverlayCategory::SetMaterial( MaterialSPtr p_pMaterial )
	{
		m_pMaterial = p_pMaterial;

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

	bool OverlayCategory::IsSizeChanged()const
	{
		bool l_changed = m_sizeChanged;
		OverlaySPtr l_parent = GetOverlay().GetParent();

		if ( !l_changed && l_parent )
		{
			l_changed = l_parent->IsSizeChanged();
		}

		return l_changed;
	}

	bool OverlayCategory::IsPositionChanged()const
	{
		bool l_changed = m_positionChanged;
		OverlaySPtr l_parent = GetOverlay().GetParent();

		if ( !l_changed && l_parent )
		{
			l_changed = l_parent->IsPositionChanged();
		}

		return l_changed;
	}

	Point2d OverlayCategory::DoGetTotalSize()const
	{
		OverlaySPtr l_parent = GetOverlay().GetParent();
		Size l_renderSize = GetOverlay().GetOverlayManager().GetRenderer()->GetSize();
		Point2d l_totalSize( l_renderSize.width(), l_renderSize.height() );

		if ( l_parent )
		{
			Point2d l_parentSize = l_parent->GetAbsoluteSize();
			l_totalSize[0] = l_parentSize[0] * l_totalSize[0];
			l_totalSize[1] = l_parentSize[1] * l_totalSize[1];
		}

		return l_totalSize;
	}

	void OverlayCategory::DoUpdatePosition()
	{
		OverlayRendererSPtr l_renderer = GetOverlay().GetOverlayManager().GetRenderer();

		if ( l_renderer )
		{
			if ( IsPositionChanged() || l_renderer->IsSizeChanged() )
			{
				Point2d l_totalSize = DoGetTotalSize();
				bool l_changed = m_positionChanged;
				Position l_pos = GetPixelPosition();
				Point2d l_ptPos = GetPosition();

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
			}
		}
	}

	void OverlayCategory::DoUpdateSize()
	{
		OverlayRendererSPtr l_renderer = GetOverlay().GetOverlayManager().GetRenderer();

		if ( l_renderer )
		{
			if ( IsSizeChanged() || l_renderer->IsSizeChanged() )
			{
				Point2d l_totalSize = DoGetTotalSize();
				bool l_changed = m_sizeChanged;
				Size l_size = GetPixelSize();
				Point2d l_ptSize = GetSize();

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
	}
}
