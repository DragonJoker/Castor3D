#include "GuiCommon/Properties/TreeItems/OverlayTreeItemProperty.hpp"

#include "GuiCommon/Properties/Math/PositionProperties.hpp"
#include "GuiCommon/Properties/Math/SizeProperties.hpp"
#include "GuiCommon/Properties/Math/RectangleProperties.hpp"
#include "GuiCommon/Properties/Math/PointProperties.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/MaterialCache.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Overlay/BorderPanelOverlay.hpp>
#include <Castor3D/Overlay/Overlay.hpp>
#include <Castor3D/Overlay/PanelOverlay.hpp>
#include <Castor3D/Overlay/TextOverlay.hpp>

#include <CastorUtils/Graphics/Font.hpp>

#include <wx/propgrid/advprops.h>

namespace GuiCommon
{
	OverlayTreeItemProperty::OverlayTreeItemProperty( ImagesLoader & imagesLoader
		, bool editable
		, c3d::Engine * engine )
		: TreeItemProperty{ engine, imagesLoader, editable }
	{
		CreateTreeItemMenu();
	}

	void OverlayTreeItemProperty::doCreateProperties( wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_OVERLAY = _( "Overlay: " );
		static wxString PROPERTY_OVERLAY_VISIBLE = _( "Visible" );
		static wxString PROPERTY_OVERLAY_POSITION = _( "Position" );
		static wxString PROPERTY_OVERLAY_SIZE = _( "Size" );
		static wxString PROPERTY_OVERLAY_MATERIAL = _( "Material" );

		if ( !m_overlay )
		{
			return;
		}

		auto & overlay = *m_overlay;
		auto & engine = *overlay.getOverlay().getEngine();
		m_materials = getMaterialsList();
		addProperty( grid, PROPERTY_CATEGORY_OVERLAY + wxString( overlay.getOverlayName() ) );
		addPropertyT( grid, PROPERTY_OVERLAY_VISIBLE, overlay.isVisible(), &overlay, &c3d::OverlayCategory::setVisible );
		addProperty( grid, PROPERTY_OVERLAY_POSITION, overlay.computePixelPosition()
			, [&overlay]( wxVariant const & var )
			{
				overlay.setPixelPosition( PositionRefFromVariant( var ) );
			} );
		addProperty( grid, PROPERTY_OVERLAY_SIZE, overlay.computePixelSize()
			, [&overlay]( wxVariant const & var )
			{
				overlay.setPixelSize( SizeRefFromVariant( var ) );
			} );
		addMaterial( grid, engine, PROPERTY_OVERLAY_MATERIAL, m_materials, overlay.getMaterial()
			, [&overlay]( c3d::MaterialObs material ) { overlay.setMaterial( material ); } );

		switch ( overlay.getType() )
		{
		case c3d::OverlayType::ePanel:
			break;
		case c3d::OverlayType::eBorderPanel:
			doCreateBorderPanelOverlayProperties( grid, static_cast< c3d::BorderPanelOverlay & >( overlay ) );
			break;
		case c3d::OverlayType::eText:
			doCreateTextOverlayProperties( grid, static_cast< c3d::TextOverlay & >( overlay ) );
			break;
		default:
			CU_Failure( "Unsupported OverlayType" );
			break;
		}
	}

	void OverlayTreeItemProperty::doCreateBorderPanelOverlayProperties( wxPropertyGrid * grid
		, c3d::BorderPanelOverlay & overlay )
	{
		static wxString PROPERTY_CATEGORY_BORDER_PANEL_OVERLAY = _( "Borders" );
		static wxString PROPERTY_OVERLAY_BORDER_SIZE = _( "Borders Size" );
		static wxString PROPERTY_OVERLAY_BORDER_MATERIAL = _( "Borders Material" );
		static wxString PROPERTY_OVERLAY_BORDER_INNER_UV = _( "Borders Inner UV" );
		static wxString PROPERTY_OVERLAY_BORDER_OUTER_UV = _( "Borders Outer UV" );
		static wxString PROPERTY_OVERLAY_BORDER_POSITION = _( "Borders Position" );
		static c3d::Array< wxString, size_t( c3d::BorderPosition::eCount ) > PROPERTY_OVERLAY_BORDER_POSITION_TEXTS{ _( "Internal" ), _( "Middle" ), _( "External" ) };

		auto & engine = *overlay.getOverlay().getEngine();
		addProperty( grid, PROPERTY_CATEGORY_BORDER_PANEL_OVERLAY );
		wxArrayString choices{ make_wxArrayString( PROPERTY_OVERLAY_BORDER_POSITION_TEXTS ) };
		
		addProperty( grid, PROPERTY_OVERLAY_BORDER_SIZE
			, overlay.computePixelBorderSize()
			, [&overlay]( wxVariant const & var )
			{
				overlay.setPixelBorderSize( Point4uiRefFromVariant( var ) );
			} );
		addMaterial( grid, engine, PROPERTY_OVERLAY_BORDER_MATERIAL, m_materials, overlay.getBorderMaterial()
			, [&overlay]( c3d::MaterialObs material ) { overlay.setBorderMaterial( material ); } );
		addPropertyT( grid, PROPERTY_OVERLAY_BORDER_INNER_UV, overlay.getBorderInnerUV(), &overlay, &c3d::BorderPanelOverlay::setBorderInnerUV );
		addPropertyT( grid, PROPERTY_OVERLAY_BORDER_OUTER_UV, overlay.getBorderOuterUV(), &overlay, &c3d::BorderPanelOverlay::setBorderOuterUV );
		addPropertyET( grid, PROPERTY_OVERLAY_BORDER_POSITION, choices, overlay.getBorderPosition(), &overlay, &c3d::BorderPanelOverlay::setBorderPosition );
	}

	void OverlayTreeItemProperty::doCreateTextOverlayProperties( wxPropertyGrid * grid
		, c3d::TextOverlay & overlay )
	{
		static wxString PROPERTY_CATEGORY_TEXT_OVERLAY = _( "Text" );
		static wxString PROPERTY_OVERLAY_FONT = _( "Font" );
		static wxString PROPERTY_OVERLAY_CAPTION = _( "Caption" );
		static wxString PROPERTY_OVERLAY_HALIGN = _( "Horiz. align." );
		static c3d::Array< wxString, size_t( c3d::HAlign::eCount ) > PROPERTY_OVERLAY_HALIGN_TEXTS{ _( "Left" ), _( "Center" ), _( "Right" ) };
		static wxString PROPERTY_OVERLAY_VALIGN = _( "Vertic. align." );
		static c3d::Array< wxString, size_t( c3d::VAlign::eCount ) > PROPERTY_OVERLAY_VALIGN_TEXTS{ _( "Top" ), _( "Center" ), _( "Bottom" ) };
		static wxString PROPERTY_OVERLAY_WRAPPING = _( "Wrapping" );
		static c3d::Array< wxString, size_t( c3d::TextWrappingMode::eCount ) > PROPERTY_OVERLAY_WRAPPING_TEXTS{ _( "None" ), _( "Letter" ), _( "Word" ) };
		static wxString PROPERTY_OVERLAY_SPACING = _( "Line spacing" );
		static c3d::Array< wxString, size_t( c3d::TextLineSpacingMode::eCount ) > PROPERTY_OVERLAY_SPACING_TEXTS{ _( "Own height" ), _( "Max lines height" ), _( "Max fonts height" ) };
		static wxString PROPERTY_OVERLAY_TEXTURING = _( "Texture mapping" );
		static c3d::Array< wxString, size_t( c3d::TextTexturingMode::eCount ) > PROPERTY_OVERLAY_TEXTURING_TEXTS{ _( "Letter" ), _( "Text" ) };

		wxArrayString haligns{ make_wxArrayString( PROPERTY_OVERLAY_HALIGN_TEXTS ) };
		wxString halign{ PROPERTY_OVERLAY_HALIGN_TEXTS[size_t( overlay.getHAlign() )] };
		wxArrayString valigns{ make_wxArrayString( PROPERTY_OVERLAY_VALIGN_TEXTS ) };
		wxString valign{ PROPERTY_OVERLAY_VALIGN_TEXTS[size_t( overlay.getVAlign() )] };
		wxArrayString wrappings{ make_wxArrayString( PROPERTY_OVERLAY_WRAPPING_TEXTS ) };
		wxString wrapping{ PROPERTY_OVERLAY_WRAPPING_TEXTS[size_t( overlay.getTextWrappingMode() )] };
		wxArrayString spacings{ make_wxArrayString( PROPERTY_OVERLAY_SPACING_TEXTS ) };
		wxString spacing{ PROPERTY_OVERLAY_SPACING_TEXTS[size_t( overlay.getLineSpacingMode() )] };
		wxArrayString texturings{ make_wxArrayString( PROPERTY_OVERLAY_TEXTURING_TEXTS ) };
		wxString texturing{ PROPERTY_OVERLAY_TEXTURING_TEXTS[size_t( overlay.getTexturingMode() )] };

		addProperty( grid, PROPERTY_CATEGORY_TEXT_OVERLAY );
		addProperty( grid, PROPERTY_OVERLAY_FONT, *overlay.getFontTexture()->getFont()
			, [&overlay]( wxVariant const & var )
			{
				overlay.setFont( variantCast< c3d::FontRPtr >( var )->getName() );
			} );
		addPropertyT( grid, PROPERTY_OVERLAY_CAPTION, overlay.getCaption(), &overlay, &c3d::TextOverlay::setCaption );
		addPropertyET( grid, PROPERTY_OVERLAY_HALIGN, haligns, overlay.getHAlign(), &overlay, &c3d::TextOverlay::setHAlign );
		addPropertyET( grid, PROPERTY_OVERLAY_VALIGN, valigns, overlay.getVAlign(), &overlay, &c3d::TextOverlay::setVAlign );
		addPropertyET( grid, PROPERTY_OVERLAY_WRAPPING, wrappings, overlay.getTextWrappingMode(), &overlay, &c3d::TextOverlay::setTextWrappingMode );
		addPropertyET( grid, PROPERTY_OVERLAY_SPACING, spacings, overlay.getLineSpacingMode(), &overlay, &c3d::TextOverlay::setLineSpacingMode );
		addPropertyET( grid, PROPERTY_OVERLAY_TEXTURING, texturings, overlay.getTexturingMode(), &overlay, &c3d::TextOverlay::setTexturingMode );
	}
}
