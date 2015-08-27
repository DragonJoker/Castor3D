#include "OverlayTreeItemProperty.hpp"

#include <BorderPanelOverlay.hpp>
#include <Engine.hpp>
#include <FunctorEvent.hpp>
#include <Material.hpp>
#include <MaterialManager.hpp>
#include <Overlay.hpp>
#include <PanelOverlay.hpp>
#include <TextOverlay.hpp>

#include <Font.hpp>

#include "AdditionalProperties.hpp"
#include <wx/propgrid/advprops.h>

using namespace Castor;
using namespace Castor3D;

namespace GuiCommon
{
	namespace
	{
		static const wxString PROPERTY_CATEGORY_OVERLAY = _( "Overlay: " );
		static const wxString PROPERTY_CATEGORY_BORDER_PANEL_OVERLAY = _( "Border Panel Overlay" );
		static const wxString PROPERTY_CATEGORY_TEXT_OVERLAY = _( "Text Overlay" );
		static const wxString PROPERTY_OVERLAY_POSITION = _( "Position" );
		static const wxString PROPERTY_OVERLAY_SIZE = _( "Size" );
		static const wxString PROPERTY_OVERLAY_MATERIAL = _( "Material" );
		static const wxString PROPERTY_OVERLAY_BORDER_SIZE = _( "Border Size" );
		static const wxString PROPERTY_OVERLAY_BORDER_MATERIAL = _( "Border Material" );
		static const wxString PROPERTY_OVERLAY_BORDER_INNER_UV = _( "Border Inner UV" );
		static const wxString PROPERTY_OVERLAY_BORDER_OUTER_UV = _( "Border Outer UV" );
		static const wxString PROPERTY_OVERLAY_BORDER_POSITION = _( "Border Position" );
		static const wxString PROPERTY_OVERLAY_BORDER_POSITION_INTERNAL = _( "Internal" );
		static const wxString PROPERTY_OVERLAY_BORDER_POSITION_MIDDLE = _( "Middle" );
		static const wxString PROPERTY_OVERLAY_BORDER_POSITION_EXTERNAL = _( "External" );
		static const wxString PROPERTY_OVERLAY_FONT = _( "Font" );
		static const wxString PROPERTY_OVERLAY_CAPTION = _( "Caption" );
	}

	wxOverlayTreeItemProperty::wxOverlayTreeItemProperty( Castor3D::OverlaySPtr p_overlay )
		: wxTreeItemProperty( ePROPERTY_DATA_TYPE_OVERLAY )
		, m_overlay( p_overlay )
	{
	}

	wxOverlayTreeItemProperty::~wxOverlayTreeItemProperty()
	{
	}

	void wxOverlayTreeItemProperty::CreateProperties( wxPropertyGrid * p_grid )
	{
		OverlaySPtr l_overlay = GetOverlay();

		if ( l_overlay )
		{
			p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_OVERLAY + wxString( l_overlay->GetName() ) ) );
			p_grid->Append( new PositionProperty( PROPERTY_OVERLAY_POSITION ) )->SetValue( wxVariant( l_overlay->GetPixelPosition() ) );
			p_grid->Append( new SizeProperty( PROPERTY_OVERLAY_SIZE ) )->SetValue( wxVariant( l_overlay->GetPixelSize() ) );
			p_grid->Append( DoCreateMaterialProperty( PROPERTY_OVERLAY_MATERIAL, l_overlay->GetEngine() ) )->SetValue( wxVariant( l_overlay->GetMaterial()->GetName() ) );

			switch ( l_overlay->GetType() )
			{
			case eOVERLAY_TYPE_PANEL:
				DoCreatePanelOverlayProperties( p_grid, std::static_pointer_cast< PanelOverlay >( GetOverlay()->GetOverlayCategory() ) );
				break;

			case eOVERLAY_TYPE_BORDER_PANEL:
				DoCreateBorderPanelOverlayProperties( p_grid, std::static_pointer_cast< BorderPanelOverlay >( GetOverlay()->GetOverlayCategory() ) );
				break;

			case eOVERLAY_TYPE_TEXT:
				DoCreateTextOverlayProperties( p_grid, std::static_pointer_cast< TextOverlay >( GetOverlay()->GetOverlayCategory() ) );
				break;
			}
		}
	}

	void wxOverlayTreeItemProperty::OnPropertyChange( wxPropertyGridEvent & p_event )
	{
		wxPGProperty * l_property = p_event.GetProperty();
		OverlaySPtr l_overlay = GetOverlay();

		if ( l_property && l_overlay )
		{
			if ( l_property->GetName() == PROPERTY_OVERLAY_POSITION )
			{
				const Position & l_position = PositionRefFromVariant( l_property->GetValue() );
				OnPositionChange( l_position );
			}
			else if ( l_property->GetName() == PROPERTY_OVERLAY_SIZE )
			{
				const Size & l_size = SizeRefFromVariant( l_property->GetValue() );
				OnSizeChange( l_size );
			}
			else if ( l_property->GetName() == PROPERTY_OVERLAY_MATERIAL )
			{
				OnMaterialChange( String( l_property->GetValueAsString().c_str() ) );
			}

			switch ( l_overlay->GetType() )
			{
			case eOVERLAY_TYPE_PANEL:
				OnPanelOverlayPropertyChanged( p_event );
				break;

			case eOVERLAY_TYPE_BORDER_PANEL:
				OnBorderPanelOverlayPropertyChanged( p_event );
				break;

			case eOVERLAY_TYPE_TEXT:
				OnTextOverlayPropertyChanged( p_event );
				break;
			}
		}
	}

	void wxOverlayTreeItemProperty::DoCreatePanelOverlayProperties( wxPropertyGrid * p_grid, PanelOverlaySPtr p_overlay )
	{
	}

	void wxOverlayTreeItemProperty::DoCreateBorderPanelOverlayProperties( wxPropertyGrid * p_grid, BorderPanelOverlaySPtr p_overlay )
	{
		p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_BORDER_PANEL_OVERLAY ) );
		wxPGChoices l_choices;
		l_choices.Add( PROPERTY_OVERLAY_BORDER_POSITION_INTERNAL );
		l_choices.Add( PROPERTY_OVERLAY_BORDER_POSITION_MIDDLE );
		l_choices.Add( PROPERTY_OVERLAY_BORDER_POSITION_EXTERNAL );
		wxString l_selected;

		switch ( p_overlay->GetBorderPosition() )
		{
		case eBORDER_POSITION_INTERNAL:
			l_selected = PROPERTY_OVERLAY_BORDER_POSITION_INTERNAL;
			break;

		case eBORDER_POSITION_MIDDLE:
			l_selected = PROPERTY_OVERLAY_BORDER_POSITION_MIDDLE;
			break;

		case eBORDER_POSITION_EXTERNAL:
			l_selected = PROPERTY_OVERLAY_BORDER_POSITION_EXTERNAL;
			break;
		}

		Point4i l_rec = p_overlay->GetBorderPixelSize();
		p_grid->Append( new RectangleProperty( PROPERTY_OVERLAY_BORDER_SIZE ) )->SetValue( wxVariant( l_rec ) );
		p_grid->Append( DoCreateMaterialProperty( PROPERTY_OVERLAY_BORDER_MATERIAL, p_overlay->GetOverlay().GetEngine() ) )->SetValue( wxVariant( p_overlay->GetBorderMaterial()->GetName() ) );
		p_grid->Append( new Point4dProperty( PROPERTY_OVERLAY_BORDER_INNER_UV ) )->SetValue( wxVariant( p_overlay->GetBorderInnerUV() ) );
		p_grid->Append( new Point4dProperty( PROPERTY_OVERLAY_BORDER_OUTER_UV ) )->SetValue( wxVariant( p_overlay->GetBorderOuterUV() ) );
		p_grid->Append( new wxEnumProperty( PROPERTY_OVERLAY_BORDER_POSITION, PROPERTY_OVERLAY_BORDER_POSITION, l_choices ) )->SetValue( l_selected );
	}

	void wxOverlayTreeItemProperty::DoCreateTextOverlayProperties( wxPropertyGrid * p_grid, TextOverlaySPtr p_overlay )
	{
		p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_TEXT_OVERLAY ) );
		FontSPtr l_font = p_overlay->GetFont();
		wxFontInfo l_info( l_font->GetHeight() );
		l_info.FaceName( l_font->GetFaceName() );
		p_grid->Append( new wxFontProperty( PROPERTY_OVERLAY_FONT ) )->SetValue( wxVariant( wxFont( l_info ) ) );
		p_grid->Append( new wxStringProperty( PROPERTY_OVERLAY_CAPTION ) )->SetValue( p_overlay->GetCaption() );
	}

	void wxOverlayTreeItemProperty::OnPanelOverlayPropertyChanged( wxPropertyGridEvent & p_event )
	{
	}

	void wxOverlayTreeItemProperty::OnBorderPanelOverlayPropertyChanged( wxPropertyGridEvent & p_event )
	{
		wxPGProperty * l_property = p_event.GetProperty();

		if ( l_property )
		{
			if ( l_property->GetName() == PROPERTY_OVERLAY_BORDER_SIZE )
			{
				const Point4i & l_size = Point4iRefFromVariant( l_property->GetValue() );
				OnBorderSizeChange( Castor::Rectangle( l_size[0], l_size[1], l_size[2], l_size[3] ) );
			}
			else if ( l_property->GetName() == PROPERTY_OVERLAY_BORDER_MATERIAL )
			{
				OnBorderMaterialChange( String( l_property->GetValueAsString().c_str() ) );
			}
			else if ( l_property->GetName() == PROPERTY_OVERLAY_BORDER_POSITION )
			{
				if ( l_property->GetValueAsString() == PROPERTY_OVERLAY_BORDER_POSITION_INTERNAL )
				{
					OnBorderPositionChange( eBORDER_POSITION_INTERNAL );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_OVERLAY_BORDER_POSITION_MIDDLE )
				{
					OnBorderPositionChange( eBORDER_POSITION_MIDDLE );
				}
				else
				{
					OnBorderPositionChange( eBORDER_POSITION_EXTERNAL );
				}
			}
			else if ( l_property->GetName() == PROPERTY_OVERLAY_BORDER_INNER_UV )
			{
				OnBorderInnerUVChange( Point4d( l_property->GetValue() ) );
			}
			else if ( l_property->GetName() == PROPERTY_OVERLAY_BORDER_OUTER_UV )
			{
				OnBorderOuterUVChange( Point4d( l_property->GetValue() ) );
			}
		}
	}

	void wxOverlayTreeItemProperty::OnTextOverlayPropertyChanged( wxPropertyGridEvent & p_event )
	{
		wxPGProperty * l_property = p_event.GetProperty();

		if ( l_property )
		{
			if ( l_property->GetName() == PROPERTY_OVERLAY_FONT )
			{
				wxFont l_wxfont;
				l_wxfont << l_property->GetValue();
				FontSPtr l_font = wxLoadFont( GetOverlay()->GetEngine(), l_wxfont );

				if ( l_font )
				{
					OnFontChange( l_font );
				}
			}
			else if ( l_property->GetName() == PROPERTY_OVERLAY_CAPTION )
			{
				OnCaptionChange( String( l_property->GetValueAsString().c_str() ) );
			}
		}
	}

	void wxOverlayTreeItemProperty::OnMaterialChange( Castor::String const & p_name )
	{
		OverlaySPtr l_overlay = GetOverlay();

		l_overlay->GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [p_name, l_overlay]()
		{
			MaterialManager & l_manager = l_overlay->GetEngine()->GetMaterialManager();
			MaterialSPtr l_material = l_manager.find( p_name );

			if ( l_material )
			{
				l_overlay->SetMaterial( l_material );
			}
		} ) );
	}

	void wxOverlayTreeItemProperty::OnPositionChange( Castor::Position const & p_position )
	{
		OverlaySPtr l_overlay = GetOverlay();

		l_overlay->GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [p_position, l_overlay]()
		{
			l_overlay->SetPixelPosition( p_position );
		} ) );
	}

	void wxOverlayTreeItemProperty::OnSizeChange( Castor::Size const & p_size )
	{
		OverlaySPtr l_overlay = GetOverlay();

		l_overlay->GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [p_size, l_overlay]()
		{
			l_overlay->SetPixelSize( p_size );
		} ) );
	}

	void wxOverlayTreeItemProperty::OnBorderMaterialChange( Castor::String const & p_name )
	{
		OverlaySPtr l_overlay = GetOverlay();
		CASTOR_ASSERT( l_overlay->GetType() == eOVERLAY_TYPE_BORDER_PANEL );

		l_overlay->GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [p_name, l_overlay]()
		{
			MaterialManager & l_manager = l_overlay->GetEngine()->GetMaterialManager();
			MaterialSPtr l_material = l_manager.find( p_name );

			if ( l_material )
			{
				std::static_pointer_cast< BorderPanelOverlay >( l_overlay->GetOverlayCategory() )->SetBorderMaterial( l_material );
			}
		} ) );
	}

	void wxOverlayTreeItemProperty::OnBorderSizeChange( Castor::Rectangle const & p_size )
	{
		OverlaySPtr l_overlay = GetOverlay();
		CASTOR_ASSERT( l_overlay->GetType() == eOVERLAY_TYPE_BORDER_PANEL );

		l_overlay->GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [p_size, l_overlay]()
		{
			std::static_pointer_cast< BorderPanelOverlay >( l_overlay->GetOverlayCategory() )->SetBorderPixelSize( p_size );
		} ) );
	}

	void wxOverlayTreeItemProperty::OnBorderInnerUVChange( Castor::Point4d const & p_uv )
	{
		OverlaySPtr l_overlay = GetOverlay();
		CASTOR_ASSERT( l_overlay->GetType() == eOVERLAY_TYPE_BORDER_PANEL );

		l_overlay->GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [p_uv, l_overlay]()
		{
			std::static_pointer_cast< BorderPanelOverlay >( l_overlay->GetOverlayCategory() )->SetBorderInnerUV( p_uv );
		} ) );
	}

	void wxOverlayTreeItemProperty::OnBorderOuterUVChange( Castor::Point4d const & p_uv )
	{
		OverlaySPtr l_overlay = GetOverlay();
		CASTOR_ASSERT( l_overlay->GetType() == eOVERLAY_TYPE_BORDER_PANEL );

		l_overlay->GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [p_uv, l_overlay]()
		{
			std::static_pointer_cast< BorderPanelOverlay >( l_overlay->GetOverlayCategory() )->SetBorderOuterUV( p_uv );
		} ) );
	}

	void wxOverlayTreeItemProperty::OnBorderPositionChange( Castor3D::eBORDER_POSITION p_position )
	{
		OverlaySPtr l_overlay = GetOverlay();
		CASTOR_ASSERT( l_overlay->GetType() == eOVERLAY_TYPE_BORDER_PANEL );

		l_overlay->GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [p_position, l_overlay]()
		{
			std::static_pointer_cast< BorderPanelOverlay >( l_overlay->GetOverlayCategory() )->SetBorderPosition( p_position );
		} ) );
	}

	void wxOverlayTreeItemProperty::OnCaptionChange( Castor::String const & p_caption )
	{
		OverlaySPtr l_overlay = GetOverlay();
		CASTOR_ASSERT( l_overlay->GetType() == eOVERLAY_TYPE_TEXT );

		l_overlay->GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [p_caption, l_overlay]()
		{
			std::static_pointer_cast< TextOverlay >( l_overlay->GetOverlayCategory() )->SetCaption( p_caption );
		} ) );
	}

	void wxOverlayTreeItemProperty::OnFontChange( Castor::FontSPtr p_font )
	{
		OverlaySPtr l_overlay = GetOverlay();
		CASTOR_ASSERT( l_overlay->GetType() == eOVERLAY_TYPE_TEXT );

		l_overlay->GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [p_font, l_overlay]()
		{
			std::static_pointer_cast< TextOverlay >( l_overlay->GetOverlayCategory() )->SetFont( p_font->GetName() );
		} ) );
	}
}
