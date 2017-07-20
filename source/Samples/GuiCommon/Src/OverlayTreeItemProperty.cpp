#include "OverlayTreeItemProperty.hpp"

#include <Engine.hpp>
#include <Material/Material.hpp>
#include <Event/Frame/FunctorEvent.hpp>
#include <Overlay/BorderPanelOverlay.hpp>
#include <Overlay/Overlay.hpp>
#include <Overlay/PanelOverlay.hpp>
#include <Overlay/TextOverlay.hpp>

#include <Graphics/Font.hpp>

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
		static std::array< wxString, size_t( BorderPosition::eCount ) > PROPERTY_OVERLAY_BORDER_POSITION_TEXTS{ _( "Internal" ), _( "Middle" ), _( "External" ) };
		static std::array< int, size_t( BorderPosition::eCount ) > PROPERTY_OVERLAY_BORDER_POSITION_VALUES { int( BorderPosition::eInternal ), int( BorderPosition::eMiddle ), int( BorderPosition::eExternal ) };
		static wxString PROPERTY_OVERLAY_FONT = _( "Font" );
		static wxString PROPERTY_OVERLAY_CAPTION = _( "Caption" );
		static wxString PROPERTY_OVERLAY_HALIGN = _( "Horiz. align." );
		static std::array< wxString, size_t( HAlign::eCount ) > PROPERTY_OVERLAY_HALIGN_TEXTS{ _( "Left" ), _( "Center" ), _( "Right" ) };
		static std::array< int, size_t( HAlign::eCount ) > PROPERTY_OVERLAY_HALIGN_VALUES{ int( HAlign::eLeft ), int( VAlign::eCenter ), int( HAlign::eRight ) };
		static wxString PROPERTY_OVERLAY_VALIGN = _( "Vertic. align." );
		static std::array< wxString, size_t( VAlign::eCount ) > PROPERTY_OVERLAY_VALIGN_TEXTS{ _( "Top" ), _( "Center" ), _( "Bottom" ) };
		static std::array< int, size_t( VAlign::eCount ) > PROPERTY_OVERLAY_VALIGN_VALUES{ int( VAlign::eTop ), int( VAlign::eCenter ), int( VAlign::eBottom ) };
		static wxString PROPERTY_OVERLAY_WRAPPING = _( "Wrapping" );
		static std::array< wxString, size_t( TextWrappingMode::eCount ) > PROPERTY_OVERLAY_WRAPPING_TEXTS{ _( "None" ), _( "Letter" ), _( "Word" ) };
		static std::array< int, size_t( TextWrappingMode::eCount ) > PROPERTY_OVERLAY_WRAPPING_VALUES{ int( TextWrappingMode::eNone ), int( TextWrappingMode::eBreak ), int( TextWrappingMode::eBreakWords ) };
		static wxString PROPERTY_OVERLAY_SPACING = _( "Line spacing" );
		static std::array< wxString, size_t( TextLineSpacingMode::eCount ) > PROPERTY_OVERLAY_SPACING_TEXTS{ _( "Own height" ), _( "Max lines height" ), _( "Max fonts height" ) };
		static std::array< int, size_t( TextLineSpacingMode::eCount ) > PROPERTY_OVERLAY_SPACING_VALUES{ int( TextLineSpacingMode::eOwnHeight ), int( TextLineSpacingMode::eMaxLineHeight ), int( TextLineSpacingMode::eMaxFontHeight ) };
		static wxString PROPERTY_OVERLAY_TEXTURING = _( "Texture mapping" );
		static std::array< wxString, size_t( TextTexturingMode::eCount ) > PROPERTY_OVERLAY_TEXTURING_TEXTS{ _( "Letter" ), _( "Text" ) };
		static std::array< int, size_t( TextTexturingMode::eCount ) > PROPERTY_OVERLAY_TEXTURING_VALUES{ int( TextTexturingMode::eLetter ), int( TextTexturingMode::eText ) };
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
		PROPERTY_OVERLAY_BORDER_POSITION_TEXTS = { _( "Internal" ), _( "Middle" ), _( "External" ) };
		PROPERTY_OVERLAY_BORDER_POSITION_VALUES = { int( BorderPosition::eInternal ), int( BorderPosition::eMiddle ), int( BorderPosition::eExternal ) };
		PROPERTY_OVERLAY_FONT = _( "Font" );
		PROPERTY_OVERLAY_CAPTION = _( "Caption" );
		PROPERTY_OVERLAY_HALIGN = _( "Horiz. align." );
		PROPERTY_OVERLAY_HALIGN_TEXTS = { _( "Left" ), _( "Center" ), _( "Right" ) };
		PROPERTY_OVERLAY_HALIGN_VALUES = { int( HAlign::eLeft ), int( VAlign::eCenter ), int( HAlign::eRight ) };
		PROPERTY_OVERLAY_VALIGN = _( "Vertic. align." );
		PROPERTY_OVERLAY_VALIGN_TEXTS = { _( "Top" ), _( "Center" ), _( "Bottom" ) };
		PROPERTY_OVERLAY_VALIGN_VALUES = { int( VAlign::eTop ), int( VAlign::eCenter ), int( VAlign::eBottom ) };
		PROPERTY_OVERLAY_WRAPPING = _( "Wrapping" );
		PROPERTY_OVERLAY_WRAPPING_TEXTS = { _( "None" ), _( "Letter" ), _( "Word" ) };
		PROPERTY_OVERLAY_WRAPPING_VALUES = { int( TextWrappingMode::eNone ), int( TextWrappingMode::eBreak ), int( TextWrappingMode::eBreakWords ) };
		PROPERTY_OVERLAY_SPACING = _( "Line spacing" );
		PROPERTY_OVERLAY_SPACING_TEXTS = { _( "Own height" ), _( "Max lines height" ), _( "Max fonts height" ) };
		PROPERTY_OVERLAY_SPACING_VALUES = { int( TextLineSpacingMode::eOwnHeight ), int( TextLineSpacingMode::eMaxLineHeight ), int( TextLineSpacingMode::eMaxFontHeight ) };
		PROPERTY_OVERLAY_TEXTURING = _( "Texture mapping" );
		PROPERTY_OVERLAY_TEXTURING_TEXTS = { _( "Letter" ), _( "Text" ) };
		PROPERTY_OVERLAY_TEXTURING_VALUES = { int( TextTexturingMode::eLetter ), int( TextTexturingMode::eText ) };

		CreateTreeItemMenu();
	}

	OverlayTreeItemProperty::~OverlayTreeItemProperty()
	{
	}

	void OverlayTreeItemProperty::DoCreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid )
	{
		OverlayCategorySPtr overlay = GetOverlay();

		if ( overlay )
		{
			p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_OVERLAY + wxString( overlay->GetOverlayName() ) ) );
			p_grid->Append( new PositionProperty( PROPERTY_OVERLAY_POSITION ) )->SetValue( WXVARIANT( overlay->GetPixelPosition() ) );
			p_grid->Append( new SizeProperty( PROPERTY_OVERLAY_SIZE ) )->SetValue( WXVARIANT( overlay->GetPixelSize() ) );
			p_grid->Append( DoCreateMaterialProperty( PROPERTY_OVERLAY_MATERIAL ) )->SetValue( wxVariant( make_wxString( overlay->GetMaterial()->GetName() ) ) );

			switch ( overlay->GetType() )
			{
			case OverlayType::ePanel:
				DoCreatePanelOverlayProperties( p_grid, std::static_pointer_cast< PanelOverlay >( GetOverlay() ) );
				break;

			case OverlayType::eBorderPanel:
				DoCreateBorderPanelOverlayProperties( p_grid, std::static_pointer_cast< BorderPanelOverlay >( GetOverlay() ) );
				break;

			case OverlayType::eText:
				DoCreateTextOverlayProperties( p_grid, std::static_pointer_cast< TextOverlay >( GetOverlay() ) );
				break;
			}
		}
	}

	void OverlayTreeItemProperty::DoPropertyChange( wxPropertyGridEvent & p_event )
	{
		wxPGProperty * property = p_event.GetProperty();
		OverlayCategorySPtr overlay = GetOverlay();

		if ( property && overlay )
		{
			if ( property->GetName() == PROPERTY_OVERLAY_POSITION )
			{
				const Position & position = PositionRefFromVariant( property->GetValue() );
				OnPositionChange( position );
			}
			else if ( property->GetName() == PROPERTY_OVERLAY_SIZE )
			{
				const Size & size = SizeRefFromVariant( property->GetValue() );
				OnSizeChange( size );
			}
			else if ( property->GetName() == PROPERTY_OVERLAY_MATERIAL )
			{
				OnMaterialChange( String( property->GetValueAsString().c_str() ) );
			}

			switch ( overlay->GetType() )
			{
			case OverlayType::ePanel:
				OnPanelOverlayPropertyChanged( p_event );
				break;

			case OverlayType::eBorderPanel:
				OnBorderPanelOverlayPropertyChanged( p_event );
				break;

			case OverlayType::eText:
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
		wxPGChoices choices{ make_wxArrayString( PROPERTY_OVERLAY_BORDER_POSITION_TEXTS ), make_wxArrayInt( PROPERTY_OVERLAY_BORDER_POSITION_VALUES ) };
		wxString selected{ PROPERTY_OVERLAY_BORDER_POSITION_TEXTS[size_t( p_overlay->GetBorderPosition() )] };

		Point4i rec = p_overlay->GetBorderPixelSize();
		p_grid->Append( new RectangleProperty( PROPERTY_OVERLAY_BORDER_SIZE ) )->SetValue( WXVARIANT( rec ) );

		if ( p_overlay->GetBorderMaterial() )
		{
			p_grid->Append( DoCreateMaterialProperty( PROPERTY_OVERLAY_BORDER_MATERIAL ) )->SetValue( WXVARIANT( make_wxString( p_overlay->GetBorderMaterial()->GetName() ) ) );
		}
		else
		{
			p_grid->Append( DoCreateMaterialProperty( PROPERTY_OVERLAY_BORDER_MATERIAL ) );
		}

		p_grid->Append( new Point4dProperty( GC_POINT_XYZW, PROPERTY_OVERLAY_BORDER_INNER_UV ) )->SetValue( WXVARIANT( p_overlay->GetBorderInnerUV() ) );
		p_grid->Append( new Point4dProperty( GC_POINT_XYZW, PROPERTY_OVERLAY_BORDER_OUTER_UV ) )->SetValue( WXVARIANT( p_overlay->GetBorderOuterUV() ) );
		p_grid->Append( new wxEnumProperty( PROPERTY_OVERLAY_BORDER_POSITION, PROPERTY_OVERLAY_BORDER_POSITION, choices ) )->SetValue( selected );
	}

	void OverlayTreeItemProperty::DoCreateTextOverlayProperties( wxPropertyGrid * p_grid, TextOverlaySPtr p_overlay )
	{
		wxPGChoices haligns{ make_wxArrayString( PROPERTY_OVERLAY_HALIGN_TEXTS ), make_wxArrayInt( PROPERTY_OVERLAY_HALIGN_VALUES ) };
		wxString halign{ PROPERTY_OVERLAY_HALIGN_TEXTS[size_t( p_overlay->GetHAlign() )] };
		wxPGChoices valigns{ make_wxArrayString( PROPERTY_OVERLAY_VALIGN_TEXTS ), make_wxArrayInt( PROPERTY_OVERLAY_VALIGN_VALUES ) };
		wxString valign{ PROPERTY_OVERLAY_VALIGN_TEXTS[size_t( p_overlay->GetVAlign() )] };
		wxPGChoices wrappings{ make_wxArrayString( PROPERTY_OVERLAY_WRAPPING_TEXTS ), make_wxArrayInt( PROPERTY_OVERLAY_WRAPPING_VALUES ) };
		wxString wrapping{ PROPERTY_OVERLAY_WRAPPING_TEXTS[size_t( p_overlay->GetTextWrappingMode() )] };
		wxPGChoices spacings{ make_wxArrayString( PROPERTY_OVERLAY_SPACING_TEXTS ), make_wxArrayInt( PROPERTY_OVERLAY_SPACING_VALUES ) };
		wxString spacing{ PROPERTY_OVERLAY_SPACING_TEXTS[size_t( p_overlay->GetLineSpacingMode() )] };
		wxPGChoices texturings{ make_wxArrayString( PROPERTY_OVERLAY_TEXTURING_TEXTS ), make_wxArrayInt( PROPERTY_OVERLAY_TEXTURING_VALUES ) };
		wxString texturing{ PROPERTY_OVERLAY_TEXTURING_TEXTS[size_t( p_overlay->GetTexturingMode() )] };

		p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_TEXT_OVERLAY ) );
		FontSPtr font = p_overlay->GetFontTexture()->GetFont();
		wxFontInfo info( font->GetHeight() );
		info.FaceName( font->GetFaceName() );
		p_grid->Append( new wxFontProperty( PROPERTY_OVERLAY_FONT ) )->SetValue( WXVARIANT( wxFont( info ) ) );
		p_grid->Append( new wxStringProperty( PROPERTY_OVERLAY_CAPTION ) )->SetValue( p_overlay->GetCaption() );
		p_grid->Append( new wxEnumProperty( PROPERTY_OVERLAY_HALIGN, PROPERTY_OVERLAY_HALIGN, haligns ) )->SetValue( halign );
		p_grid->Append( new wxEnumProperty( PROPERTY_OVERLAY_VALIGN, PROPERTY_OVERLAY_VALIGN, valigns ) )->SetValue( valign );
		p_grid->Append( new wxEnumProperty( PROPERTY_OVERLAY_WRAPPING, PROPERTY_OVERLAY_WRAPPING, wrappings ) )->SetValue( wrapping );
		p_grid->Append( new wxEnumProperty( PROPERTY_OVERLAY_SPACING, PROPERTY_OVERLAY_SPACING, spacings ) )->SetValue( spacing );
		p_grid->Append( new wxEnumProperty( PROPERTY_OVERLAY_TEXTURING, PROPERTY_OVERLAY_TEXTURING, texturings ) )->SetValue( texturing );
	}

	void OverlayTreeItemProperty::OnPanelOverlayPropertyChanged( wxPropertyGridEvent & p_event )
	{
	}

	void OverlayTreeItemProperty::OnBorderPanelOverlayPropertyChanged( wxPropertyGridEvent & p_event )
	{
		wxPGProperty * property = p_event.GetProperty();

		if ( property )
		{
			if ( property->GetName() == PROPERTY_OVERLAY_BORDER_SIZE )
			{
				const Point4i & size = Point4iRefFromVariant( property->GetValue() );
				OnBorderSizeChange( Castor::Rectangle( size[0], size[1], size[2], size[3] ) );
			}
			else if ( property->GetName() == PROPERTY_OVERLAY_BORDER_MATERIAL )
			{
				OnBorderMaterialChange( String( property->GetValueAsString().c_str() ) );
			}
			else if ( property->GetName() == PROPERTY_OVERLAY_BORDER_POSITION )
			{
				OnBorderPositionChange( BorderPosition( reinterpret_cast< wxEnumProperty * >( property )->GetChoiceSelection() ) );
			}
			else if ( property->GetName() == PROPERTY_OVERLAY_BORDER_INNER_UV )
			{
				OnBorderInnerUVChange( Point4dRefFromVariant( property->GetValue() ) );
			}
			else if ( property->GetName() == PROPERTY_OVERLAY_BORDER_OUTER_UV )
			{
				OnBorderOuterUVChange( Point4dRefFromVariant( property->GetValue() ) );
			}
		}
	}

	void OverlayTreeItemProperty::OnTextOverlayPropertyChanged( wxPropertyGridEvent & p_event )
	{
		wxPGProperty * property = p_event.GetProperty();

		if ( property )
		{
			if ( property->GetName() == PROPERTY_OVERLAY_FONT )
			{
				wxFont wxfont;
				wxfont << property->GetValue();
				FontSPtr font = make_Font( GetOverlay()->GetOverlay().GetEngine(), wxfont );

				if ( font )
				{
					OnFontChange( font );
				}
			}
			else if ( property->GetName() == PROPERTY_OVERLAY_CAPTION )
			{
				OnCaptionChange( String( property->GetValueAsString().c_str() ) );
			}
			else if ( property->GetName() == PROPERTY_OVERLAY_HALIGN )
			{
				OnHAlignChange( HAlign( reinterpret_cast< wxEnumProperty * >( property )->GetChoiceSelection() ) );
			}
			else if ( property->GetName() == PROPERTY_OVERLAY_VALIGN )
			{
				OnVAlignChange( VAlign( reinterpret_cast< wxEnumProperty * >( property )->GetChoiceSelection() ) );
			}
			else if ( property->GetName() == PROPERTY_OVERLAY_WRAPPING )
			{
				OnWrappingChange( TextWrappingMode( reinterpret_cast< wxEnumProperty * >( property )->GetChoiceSelection() ) );
			}
			else if ( property->GetName() == PROPERTY_OVERLAY_SPACING )
			{
				OnSpacingChange( TextLineSpacingMode( reinterpret_cast< wxEnumProperty * >( property )->GetChoiceSelection() ) );
			}
			else if ( property->GetName() == PROPERTY_OVERLAY_TEXTURING )
			{
				OnTexturingChange( TextTexturingMode( reinterpret_cast< wxEnumProperty * >( property )->GetChoiceSelection() ) );
			}
		}
	}

	void OverlayTreeItemProperty::OnMaterialChange( Castor::String const & p_name )
	{
		OverlayCategorySPtr overlay = GetOverlay();

		DoApplyChange( [p_name, overlay]()
		{
			auto & cache = overlay->GetOverlay().GetEngine()->GetMaterialCache();
			MaterialSPtr material = cache.Find( p_name );

			if ( material )
			{
				overlay->SetMaterial( material );
			}
		} );
	}

	void OverlayTreeItemProperty::OnPositionChange( Castor::Position const & p_position )
	{
		OverlayCategorySPtr overlay = GetOverlay();

		DoApplyChange( [p_position, overlay]()
		{
			overlay->SetPixelPosition( p_position );
		} );
	}

	void OverlayTreeItemProperty::OnSizeChange( Castor::Size const & p_size )
	{
		OverlayCategorySPtr overlay = GetOverlay();

		DoApplyChange( [p_size, overlay]()
		{
			overlay->SetPixelSize( p_size );
		} );
	}

	void OverlayTreeItemProperty::OnBorderMaterialChange( Castor::String const & p_name )
	{
		BorderPanelOverlaySPtr overlay = std::static_pointer_cast< BorderPanelOverlay >( GetOverlay() );
		REQUIRE( overlay->GetType() == OverlayType::eBorderPanel );

		DoApplyChange( [p_name, overlay]()
		{
			auto & cache = overlay->GetOverlay().GetEngine()->GetMaterialCache();
			MaterialSPtr material = cache.Find( p_name );

			if ( material )
			{
				overlay->SetBorderMaterial( material );
			}
		} );
	}

	void OverlayTreeItemProperty::OnBorderSizeChange( Castor::Rectangle const & p_size )
	{
		BorderPanelOverlaySPtr overlay = std::static_pointer_cast< BorderPanelOverlay >( GetOverlay() );
		REQUIRE( overlay->GetType() == OverlayType::eBorderPanel );

		DoApplyChange( [p_size, overlay]()
		{
			overlay->SetBorderPixelSize( p_size );
		} );
	}

	void OverlayTreeItemProperty::OnBorderInnerUVChange( Castor::Point4d const & p_value )
	{
		BorderPanelOverlaySPtr overlay = std::static_pointer_cast< BorderPanelOverlay >( GetOverlay() );
		REQUIRE( overlay->GetType() == OverlayType::eBorderPanel );
		double x = p_value[0];
		double y = p_value[1];
		double z = p_value[2];
		double w = p_value[3];

		DoApplyChange( [x, y, z, w, overlay]()
		{
			overlay->SetBorderInnerUV( Point4d( x, y, z, w ) );
		} );
	}

	void OverlayTreeItemProperty::OnBorderOuterUVChange( Castor::Point4d const & p_value )
	{
		BorderPanelOverlaySPtr overlay = std::static_pointer_cast< BorderPanelOverlay >( GetOverlay() );
		REQUIRE( overlay->GetType() == OverlayType::eBorderPanel );
		double x = p_value[0];
		double y = p_value[1];
		double z = p_value[2];
		double w = p_value[3];

		DoApplyChange( [x, y, z, w, overlay]()
		{
			overlay->SetBorderOuterUV( Point4d( x, y, z, w ) );
		} );
	}

	void OverlayTreeItemProperty::OnBorderPositionChange( Castor3D::BorderPosition p_position )
	{
		BorderPanelOverlaySPtr overlay = std::static_pointer_cast< BorderPanelOverlay >( GetOverlay() );
		REQUIRE( overlay->GetType() == OverlayType::eBorderPanel );

		DoApplyChange( [p_position, overlay]()
		{
			overlay->SetBorderPosition( p_position );
		} );
	}

	void OverlayTreeItemProperty::OnCaptionChange( Castor::String const & p_caption )
	{
		TextOverlaySPtr overlay = std::static_pointer_cast< TextOverlay >( GetOverlay() );
		REQUIRE( overlay->GetType() == OverlayType::eText );

		DoApplyChange( [p_caption, overlay]()
		{
			overlay->SetCaption( p_caption );
		} );
	}

	void OverlayTreeItemProperty::OnFontChange( Castor::FontSPtr p_font )
	{
		TextOverlaySPtr overlay = std::static_pointer_cast< TextOverlay >( GetOverlay() );
		REQUIRE( overlay->GetType() == OverlayType::eText );

		DoApplyChange( [p_font, overlay]()
		{
			overlay->SetFont( p_font->GetName() );
		} );
	}

	void OverlayTreeItemProperty::OnHAlignChange( Castor3D::HAlign p_value )
	{
		TextOverlaySPtr overlay = std::static_pointer_cast< TextOverlay >( GetOverlay() );
		REQUIRE( overlay->GetType() == OverlayType::eText );

		DoApplyChange( [p_value, overlay]()
		{
			overlay->SetHAlign( p_value );
		} );
	}

	void OverlayTreeItemProperty::OnVAlignChange( Castor3D::VAlign p_value )
	{
		TextOverlaySPtr overlay = std::static_pointer_cast< TextOverlay >( GetOverlay() );
		REQUIRE( overlay->GetType() == OverlayType::eText );

		DoApplyChange( [p_value, overlay]()
		{
			overlay->SetVAlign( p_value );
		} );
	}

	void OverlayTreeItemProperty::OnWrappingChange( Castor3D::TextWrappingMode p_value )
	{
		TextOverlaySPtr overlay = std::static_pointer_cast< TextOverlay >( GetOverlay() );
		REQUIRE( overlay->GetType() == OverlayType::eText );

		DoApplyChange( [p_value, overlay]()
		{
			overlay->SetTextWrappingMode( p_value );
		} );
	}

	void OverlayTreeItemProperty::OnSpacingChange( Castor3D::TextLineSpacingMode p_value )
	{
		TextOverlaySPtr overlay = std::static_pointer_cast< TextOverlay >( GetOverlay() );
		REQUIRE( overlay->GetType() == OverlayType::eText );

		DoApplyChange( [p_value, overlay]()
		{
			overlay->SetLineSpacingMode( p_value );
		} );
	}

	void OverlayTreeItemProperty::OnTexturingChange( Castor3D::TextTexturingMode p_value )
	{
		TextOverlaySPtr overlay = std::static_pointer_cast< TextOverlay >( GetOverlay() );
		REQUIRE( overlay->GetType() == OverlayType::eText );

		DoApplyChange( [p_value, overlay]()
		{
			overlay->SetTexturingMode( p_value );
		} );
	}
}
