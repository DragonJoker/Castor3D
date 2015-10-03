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

#include "PositionProperties.hpp"
#include "SizeProperties.hpp"
#include "RectangleProperties.hpp"
#include "PointProperties.hpp"

#include <wx/propgrid/advprops.h>

using namespace Castor;
using namespace Castor3D;

namespace GuiCommon
{
	namespace
	{
		static wxString PROPERTY_CATEGORY_OVERLAY = _( "Overlay: " );
		static wxString PROPERTY_CATEGORY_BORDER_PANEL_OVERLAY = _( "Border Panel Overlay" );
		static wxString PROPERTY_CATEGORY_TEXT_OVERLAY = _( "Text Overlay" );
		static wxString PROPERTY_OVERLAY_POSITION = _( "Position" );
		static wxString PROPERTY_OVERLAY_SIZE = _( "Size" );
		static wxString PROPERTY_OVERLAY_MATERIAL = _( "Material" );
		static wxString PROPERTY_OVERLAY_BORDER_SIZE = _( "Border Size" );
		static wxString PROPERTY_OVERLAY_BORDER_MATERIAL = _( "Border Material" );
		static wxString PROPERTY_OVERLAY_BORDER_INNER_UV = _( "Border Inner UV" );
		static wxString PROPERTY_OVERLAY_BORDER_OUTER_UV = _( "Border Outer UV" );
		static wxString PROPERTY_OVERLAY_BORDER_POSITION = _( "Border Position" );
		static wxString PROPERTY_OVERLAY_BORDER_POSITION_INTERNAL = _( "Internal" );
		static wxString PROPERTY_OVERLAY_BORDER_POSITION_MIDDLE = _( "Middle" );
		static wxString PROPERTY_OVERLAY_BORDER_POSITION_EXTERNAL = _( "External" );
		static wxString PROPERTY_OVERLAY_FONT = _( "Font" );
		static wxString PROPERTY_OVERLAY_CAPTION = _( "Caption" );
	}

	OverlayTreeItemProperty::OverlayTreeItemProperty( bool p_editable, Castor3D::OverlayCategorySPtr p_overlay )
		: TreeItemProperty( p_overlay->GetOverlay().GetEngine(), p_editable, ePROPERTY_DATA_TYPE_OVERLAY )
		, m_overlay( p_overlay )
	{
		PROPERTY_CATEGORY_OVERLAY = _( "Overlay: " );
		PROPERTY_CATEGORY_BORDER_PANEL_OVERLAY = _( "Border Panel Overlay" );
		PROPERTY_CATEGORY_TEXT_OVERLAY = _( "Text Overlay" );
		PROPERTY_OVERLAY_POSITION = _( "Position" );
		PROPERTY_OVERLAY_SIZE = _( "Size" );
		PROPERTY_OVERLAY_MATERIAL = _( "Material" );
		PROPERTY_OVERLAY_BORDER_SIZE = _( "Border Size" );
		PROPERTY_OVERLAY_BORDER_MATERIAL = _( "Border Material" );
		PROPERTY_OVERLAY_BORDER_INNER_UV = _( "Border Inner UV" );
		PROPERTY_OVERLAY_BORDER_OUTER_UV = _( "Border Outer UV" );
		PROPERTY_OVERLAY_BORDER_POSITION = _( "Border Position" );
		PROPERTY_OVERLAY_BORDER_POSITION_INTERNAL = _( "Internal" );
		PROPERTY_OVERLAY_BORDER_POSITION_MIDDLE = _( "Middle" );
		PROPERTY_OVERLAY_BORDER_POSITION_EXTERNAL = _( "External" );
		PROPERTY_OVERLAY_FONT = _( "Font" );
		PROPERTY_OVERLAY_CAPTION = _( "Caption" );

		CreateTreeItemMenu();
	}

	OverlayTreeItemProperty::~OverlayTreeItemProperty()
	{
	}

	void OverlayTreeItemProperty::DoCreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid )
	{
		OverlayCategorySPtr l_overlay = GetOverlay();

		if ( l_overlay )
		{
			p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_OVERLAY + wxString( l_overlay->GetOverlayName() ) ) );
			p_grid->Append( new PositionProperty( PROPERTY_OVERLAY_POSITION ) )->SetValue( wxVariant( l_overlay->GetPixelPosition() ) );
			p_grid->Append( new SizeProperty( PROPERTY_OVERLAY_SIZE ) )->SetValue( wxVariant( l_overlay->GetPixelSize() ) );
			p_grid->Append( DoCreateMaterialProperty( PROPERTY_OVERLAY_MATERIAL ) )->SetValue( wxVariant( l_overlay->GetMaterial()->GetName() ) );

			switch ( l_overlay->GetType() )
			{
			case eOVERLAY_TYPE_PANEL:
				DoCreatePanelOverlayProperties( p_grid, std::static_pointer_cast< PanelOverlay >( GetOverlay() ) );
				break;

			case eOVERLAY_TYPE_BORDER_PANEL:
				DoCreateBorderPanelOverlayProperties( p_grid, std::static_pointer_cast< BorderPanelOverlay >( GetOverlay() ) );
				break;

			case eOVERLAY_TYPE_TEXT:
				DoCreateTextOverlayProperties( p_grid, std::static_pointer_cast< TextOverlay >( GetOverlay() ) );
				break;
			}
		}
	}

	void OverlayTreeItemProperty::DoPropertyChange( wxPropertyGridEvent & p_event )
	{
		wxPGProperty * l_property = p_event.GetProperty();
		OverlayCategorySPtr l_overlay = GetOverlay();

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

	void OverlayTreeItemProperty::DoCreatePanelOverlayProperties( wxPropertyGrid * p_grid, PanelOverlaySPtr p_overlay )
	{
	}

	void OverlayTreeItemProperty::DoCreateBorderPanelOverlayProperties( wxPropertyGrid * p_grid, BorderPanelOverlaySPtr p_overlay )
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
		p_grid->Append( DoCreateMaterialProperty( PROPERTY_OVERLAY_BORDER_MATERIAL ) )->SetValue( wxVariant( p_overlay->GetBorderMaterial()->GetName() ) );
		p_grid->Append( new Point4dProperty( GC_POINT_XYZW, PROPERTY_OVERLAY_BORDER_INNER_UV ) )->SetValue( wxVariant() << p_overlay->GetBorderInnerUV() );
		p_grid->Append( new Point4dProperty( GC_POINT_XYZW, PROPERTY_OVERLAY_BORDER_OUTER_UV ) )->SetValue( wxVariant() << p_overlay->GetBorderOuterUV() );
		p_grid->Append( new wxEnumProperty( PROPERTY_OVERLAY_BORDER_POSITION, PROPERTY_OVERLAY_BORDER_POSITION, l_choices ) )->SetValue( l_selected );
	}

	void OverlayTreeItemProperty::DoCreateTextOverlayProperties( wxPropertyGrid * p_grid, TextOverlaySPtr p_overlay )
	{
		p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_TEXT_OVERLAY ) );
		FontSPtr l_font = p_overlay->GetFont();
		wxFontInfo l_info( l_font->GetHeight() );
		l_info.FaceName( l_font->GetFaceName() );
		p_grid->Append( new wxFontProperty( PROPERTY_OVERLAY_FONT ) )->SetValue( wxVariant( wxFont( l_info ) ) );
		p_grid->Append( new wxStringProperty( PROPERTY_OVERLAY_CAPTION ) )->SetValue( p_overlay->GetCaption() );
	}

	void OverlayTreeItemProperty::OnPanelOverlayPropertyChanged( wxPropertyGridEvent & p_event )
	{
	}

	void OverlayTreeItemProperty::OnBorderPanelOverlayPropertyChanged( wxPropertyGridEvent & p_event )
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

	void OverlayTreeItemProperty::OnTextOverlayPropertyChanged( wxPropertyGridEvent & p_event )
	{
		wxPGProperty * l_property = p_event.GetProperty();

		if ( l_property )
		{
			if ( l_property->GetName() == PROPERTY_OVERLAY_FONT )
			{
				wxFont l_wxfont;
				l_wxfont << l_property->GetValue();
				FontSPtr l_font = make_Font( GetOverlay()->GetOverlay().GetEngine(), l_wxfont );

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

	void OverlayTreeItemProperty::OnMaterialChange( Castor::String const & p_name )
	{
		OverlayCategorySPtr l_overlay = GetOverlay();

		DoApplyChange( [p_name, l_overlay]()
		{
			MaterialManager & l_manager = l_overlay->GetOverlay().GetEngine()->GetMaterialManager();
			MaterialSPtr l_material = l_manager.find( p_name );

			if ( l_material )
			{
				l_overlay->SetMaterial( l_material );
			}
		} );
	}

	void OverlayTreeItemProperty::OnPositionChange( Castor::Position const & p_position )
	{
		OverlayCategorySPtr l_overlay = GetOverlay();

		DoApplyChange( [p_position, l_overlay]()
		{
			l_overlay->SetPixelPosition( p_position );
		} );
	}

	void OverlayTreeItemProperty::OnSizeChange( Castor::Size const & p_size )
	{
		OverlayCategorySPtr l_overlay = GetOverlay();

		DoApplyChange( [p_size, l_overlay]()
		{
			l_overlay->SetPixelSize( p_size );
		} );
	}

	void OverlayTreeItemProperty::OnBorderMaterialChange( Castor::String const & p_name )
	{
		BorderPanelOverlaySPtr l_overlay = std::static_pointer_cast< BorderPanelOverlay >( GetOverlay() );
		CASTOR_ASSERT( l_overlay->GetType() == eOVERLAY_TYPE_BORDER_PANEL );

		DoApplyChange( [p_name, l_overlay]()
		{
			MaterialManager & l_manager = l_overlay->GetOverlay().GetEngine()->GetMaterialManager();
			MaterialSPtr l_material = l_manager.find( p_name );

			if ( l_material )
			{
				l_overlay->SetBorderMaterial( l_material );
			}
		} );
	}

	void OverlayTreeItemProperty::OnBorderSizeChange( Castor::Rectangle const & p_size )
	{
		BorderPanelOverlaySPtr l_overlay = std::static_pointer_cast< BorderPanelOverlay >( GetOverlay() );
		CASTOR_ASSERT( l_overlay->GetType() == eOVERLAY_TYPE_BORDER_PANEL );

		DoApplyChange( [p_size, l_overlay]()
		{
			l_overlay->SetBorderPixelSize( p_size );
		} );
	}

	void OverlayTreeItemProperty::OnBorderInnerUVChange( Castor::Point4d const & p_value )
	{
		BorderPanelOverlaySPtr l_overlay = std::static_pointer_cast< BorderPanelOverlay >( GetOverlay() );
		CASTOR_ASSERT( l_overlay->GetType() == eOVERLAY_TYPE_BORDER_PANEL );
		double x = p_value[0];
		double y = p_value[1];
		double z = p_value[2];
		double w = p_value[3];

		DoApplyChange( [x, y, z, w, l_overlay]()
		{
			l_overlay->SetBorderInnerUV( Point4d( x, y, z, w ) );
		} );
	}

	void OverlayTreeItemProperty::OnBorderOuterUVChange( Castor::Point4d const & p_value )
	{
		BorderPanelOverlaySPtr l_overlay = std::static_pointer_cast< BorderPanelOverlay >( GetOverlay() );
		CASTOR_ASSERT( l_overlay->GetType() == eOVERLAY_TYPE_BORDER_PANEL );
		double x = p_value[0];
		double y = p_value[1];
		double z = p_value[2];
		double w = p_value[3];

		DoApplyChange( [x, y, z, w, l_overlay]()
		{
			l_overlay->SetBorderOuterUV( Point4d( x, y, z, w ) );
		} );
	}

	void OverlayTreeItemProperty::OnBorderPositionChange( Castor3D::eBORDER_POSITION p_position )
	{
		BorderPanelOverlaySPtr l_overlay = std::static_pointer_cast< BorderPanelOverlay >( GetOverlay() );
		CASTOR_ASSERT( l_overlay->GetType() == eOVERLAY_TYPE_BORDER_PANEL );

		DoApplyChange( [p_position, l_overlay]()
		{
			l_overlay->SetBorderPosition( p_position );
		} );
	}

	void OverlayTreeItemProperty::OnCaptionChange( Castor::String const & p_caption )
	{
		TextOverlaySPtr l_overlay = std::static_pointer_cast< TextOverlay >( GetOverlay() );
		CASTOR_ASSERT( l_overlay->GetType() == eOVERLAY_TYPE_TEXT );

		DoApplyChange( [p_caption, l_overlay]()
		{
			l_overlay->SetCaption( p_caption );
		} );
	}

	void OverlayTreeItemProperty::OnFontChange( Castor::FontSPtr p_font )
	{
		TextOverlaySPtr l_overlay = std::static_pointer_cast< TextOverlay >( GetOverlay() );
		CASTOR_ASSERT( l_overlay->GetType() == eOVERLAY_TYPE_TEXT );

		DoApplyChange( [p_font, l_overlay]()
		{
			l_overlay->SetFont( p_font->GetName() );
		} );
	}
}
