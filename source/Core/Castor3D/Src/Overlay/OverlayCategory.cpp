#include "OverlayCategory.hpp"

#include "Engine.hpp"
#include "Cache/MaterialCache.hpp"
#include "Cache/OverlayCache.hpp"

#include "BorderPanelOverlay.hpp"
#include "OverlayRenderer.hpp"
#include "PanelOverlay.hpp"
#include "TextOverlay.hpp"

#include "Material/Material.hpp"

using namespace Castor;

namespace Castor3D
{
	OverlayCategory::TextWriter::TextWriter( String const & p_tabs )
		: Castor::TextWriter< OverlayCategory >{ p_tabs }
	{
	}

	bool OverlayCategory::TextWriter::operator()( OverlayCategory const & p_overlay, TextFile & p_file )
	{
		bool l_return = p_file.WriteText( m_tabs + cuT( "\tposition " ) ) > 0
						&& Point2d::TextWriter{ String{} }( p_overlay.GetPosition(), p_file )
						&& p_file.WriteText( cuT( "\n" ) ) > 0;
		Castor::TextWriter< OverlayCategory >::CheckError( l_return, "OverlayCategory position" );

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "\tsize " ) ) > 0
					   && Point2d::TextWriter{ String{} }( p_overlay.GetSize(), p_file )
					   && p_file.WriteText( cuT( "\n" ) ) > 0;
			Castor::TextWriter< OverlayCategory >::CheckError( l_return, "OverlayCategory size" );
		}

		if ( l_return && p_overlay.GetMaterial() )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "\tmaterial \"" ) + p_overlay.GetMaterial()->GetName() + cuT( "\"\n" ) ) > 0;
			Castor::TextWriter< OverlayCategory >::CheckError( l_return, "OverlayCategory material" );
		}

		for ( auto l_overlay : p_overlay.GetOverlay() )
		{
			switch ( l_overlay->GetType() )
			{
			case OverlayType::ePanel:
				l_return &= PanelOverlay::TextWriter( m_tabs + cuT( "\t" ) )( *l_overlay->GetPanelOverlay(), p_file );
				break;

			case OverlayType::eBorderPanel:
				l_return &= BorderPanelOverlay::TextWriter( m_tabs + cuT( "\t" ) )( *l_overlay->GetBorderPanelOverlay(), p_file );
				break;

			case OverlayType::eText:
				l_return &= TextOverlay::TextWriter( m_tabs + cuT( "\t" ) )( *l_overlay->GetTextOverlay(), p_file );
				break;

			default:
				l_return = false;
			}
		}

		return l_return;
	}

	//*************************************************************************************************

	OverlayCategory::OverlayCategory( OverlayType p_type )
		: m_type( p_type )
	{
	}

	OverlayCategory::~OverlayCategory()
	{
	}

	void OverlayCategory::Update()
	{
		OverlayRendererSPtr l_renderer = GetOverlay().GetEngine()->GetOverlayCache().GetRenderer();

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
		DoRender( GetOverlay().GetEngine()->GetOverlayCache().GetRenderer() );
	}

	void OverlayCategory::SetMaterial( MaterialSPtr p_material )
	{
		m_pMaterial = p_material;

		if ( p_material )
		{
			m_strMatName = p_material->GetName();
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
		Size l_renderSize = GetOverlay().GetEngine()->GetOverlayCache().GetRenderer()->GetSize();
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
		OverlayRendererSPtr l_renderer = GetOverlay().GetEngine()->GetOverlayCache().GetRenderer();

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
		OverlayRendererSPtr l_renderer = GetOverlay().GetEngine()->GetOverlayCache().GetRenderer();

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
