#include "OverlayCategory.hpp"

#include "Engine.hpp"

#include "BorderPanelOverlay.hpp"
#include "OverlayRenderer.hpp"
#include "PanelOverlay.hpp"

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
		bool result = p_file.WriteText( m_tabs + cuT( "\tposition " ) ) > 0
						&& Point2d::TextWriter{ String{} }( p_overlay.GetPosition(), p_file )
						&& p_file.WriteText( cuT( "\n" ) ) > 0;
		Castor::TextWriter< OverlayCategory >::CheckError( result, "OverlayCategory position" );

		if ( result )
		{
			result = p_file.WriteText( m_tabs + cuT( "\tsize " ) ) > 0
					   && Point2d::TextWriter{ String{} }( p_overlay.GetSize(), p_file )
					   && p_file.WriteText( cuT( "\n" ) ) > 0;
			Castor::TextWriter< OverlayCategory >::CheckError( result, "OverlayCategory size" );
		}

		if ( result && p_overlay.GetMaterial() )
		{
			result = p_file.WriteText( m_tabs + cuT( "\tmaterial \"" ) + p_overlay.GetMaterial()->GetName() + cuT( "\"\n" ) ) > 0;
			Castor::TextWriter< OverlayCategory >::CheckError( result, "OverlayCategory material" );
		}

		for ( auto overlay : p_overlay.GetOverlay() )
		{
			switch ( overlay->GetType() )
			{
			case OverlayType::ePanel:
				result &= PanelOverlay::TextWriter( m_tabs + cuT( "\t" ) )( *overlay->GetPanelOverlay(), p_file );
				break;

			case OverlayType::eBorderPanel:
				result &= BorderPanelOverlay::TextWriter( m_tabs + cuT( "\t" ) )( *overlay->GetBorderPanelOverlay(), p_file );
				break;

			case OverlayType::eText:
				result &= TextOverlay::TextWriter( m_tabs + cuT( "\t" ) )( *overlay->GetTextOverlay(), p_file );
				break;

			default:
				result = false;
			}
		}

		return result;
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
		OverlayRendererSPtr renderer = GetOverlay().GetEngine()->GetOverlayCache().GetRenderer();

		if ( renderer )
		{
			if ( IsPositionChanged() || renderer->IsSizeChanged() )
			{
				DoUpdatePosition();
			}

			if ( IsSizeChanged() || renderer->IsSizeChanged() )
			{
				DoUpdateSize();
			}

			if ( IsChanged() || IsSizeChanged() || renderer->IsSizeChanged() )
			{
				DoUpdate();
				DoUpdateBuffer( renderer->GetSize() );
			}

			m_positionChanged = false;
			m_sizeChanged = false;
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
		Point2d position = GetAbsolutePosition();
		return Position( int32_t( p_size.width() * position[0] ), int32_t( p_size.height() * position[1] ) );
	}

	Size OverlayCategory::GetAbsoluteSize( Castor::Size const & p_size )const
	{
		Point2d size = GetAbsoluteSize();
		return Size( uint32_t( p_size.width() * size[0] ), uint32_t( p_size.height() * size[1] ) );
	}

	Point2d OverlayCategory::GetAbsolutePosition()const
	{
		Point2d position = GetPosition();
		OverlaySPtr parent = GetOverlay().GetParent();

		if ( parent )
		{
			position *= parent->GetAbsoluteSize();
			position += parent->GetAbsolutePosition();
		}

		return position;
	}

	Point2d OverlayCategory::GetAbsoluteSize()const
	{
		Point2d size = GetSize();
		OverlaySPtr parent = GetOverlay().GetParent();

		if ( parent )
		{
			size *= parent->GetAbsoluteSize();
		}

		return size;
	}

	bool OverlayCategory::IsSizeChanged()const
	{
		bool changed = m_sizeChanged;
		OverlaySPtr parent = GetOverlay().GetParent();

		if ( !changed && parent )
		{
			changed = parent->IsSizeChanged();
		}

		return changed;
	}

	bool OverlayCategory::IsPositionChanged()const
	{
		bool changed = m_positionChanged;
		OverlaySPtr parent = GetOverlay().GetParent();

		if ( !changed && parent )
		{
			changed = parent->IsPositionChanged();
		}

		return changed;
	}

	Point2d OverlayCategory::DoGetTotalSize()const
	{
		OverlaySPtr parent = GetOverlay().GetParent();
		Size renderSize = GetOverlay().GetEngine()->GetOverlayCache().GetRenderer()->GetSize();
		Point2d totalSize( renderSize.width(), renderSize.height() );

		if ( parent )
		{
			Point2d parentSize = parent->GetAbsoluteSize();
			totalSize[0] = parentSize[0] * totalSize[0];
			totalSize[1] = parentSize[1] * totalSize[1];
		}

		return totalSize;
	}

	void OverlayCategory::DoUpdatePosition()
	{
		OverlayRendererSPtr renderer = GetOverlay().GetEngine()->GetOverlayCache().GetRenderer();

		if ( renderer )
		{
			if ( IsPositionChanged() || renderer->IsSizeChanged() )
			{
				Point2d totalSize = DoGetTotalSize();
				bool changed = m_positionChanged;
				Position pos = GetPixelPosition();
				Point2d ptPos = GetPosition();

				if ( pos.x() )
				{
					changed = !Castor::Policy< double >::equals( ptPos[0], pos.x() / totalSize[0] );
					ptPos[0] = pos.x() / totalSize[0];
				}

				if ( pos.y() )
				{
					changed = !Castor::Policy< double >::equals( ptPos[1], pos.y() / totalSize[1] );
					ptPos[1] = pos.y() / totalSize[1];
				}

				if ( changed )
				{
					SetPosition( ptPos );
				}
			}
		}
	}

	void OverlayCategory::DoUpdateSize()
	{
		OverlayRendererSPtr renderer = GetOverlay().GetEngine()->GetOverlayCache().GetRenderer();

		if ( renderer )
		{
			if ( IsSizeChanged() || renderer->IsSizeChanged() )
			{
				Point2d totalSize = DoGetTotalSize();
				bool changed = m_sizeChanged;
				Size size = GetPixelSize();
				Point2d ptSize = GetSize();

				if ( size.width() )
				{
					changed = !Castor::Policy< double >::equals( ptSize[0], size.width() / totalSize[0] );
					ptSize[0] = size.width() / totalSize[0];
				}

				if ( size.height() )
				{
					changed = !Castor::Policy< double >::equals( ptSize[1], size.height() / totalSize[1] );
					ptSize[1] = size.height() / totalSize[1];
				}

				if ( changed )
				{
					SetSize( ptSize );
				}
			}
		}
	}
}
