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
	OverlayTreeItemProperty::OverlayTreeItemProperty( bool p_editable
		, castor3d::OverlayCategorySPtr p_overlay )
		: TreeItemProperty( p_overlay->getOverlay().getEngine(), p_editable )
		, m_overlay( p_overlay )
	{
		CreateTreeItemMenu();
	}

	void OverlayTreeItemProperty::doCreateProperties( wxPGEditor * editor
		, wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_OVERLAY = _( "Overlay: " );
		static wxString PROPERTY_OVERLAY_VISIBLE = _( "Visible" );
		static wxString PROPERTY_OVERLAY_POSITION = _( "Position" );
		static wxString PROPERTY_OVERLAY_SIZE = _( "Size" );
		static wxString PROPERTY_OVERLAY_MATERIAL = _( "Material" );

		castor3d::OverlayCategorySPtr overlay = getOverlay();

		if ( overlay )
		{
			addProperty( grid, PROPERTY_CATEGORY_OVERLAY + wxString( overlay->getOverlayName() ) );
			addPropertyT( grid, PROPERTY_OVERLAY_VISIBLE, overlay->isVisible(), overlay.get(), &castor3d::OverlayCategory::setVisible );
			addPropertyT( grid, PROPERTY_OVERLAY_POSITION, overlay->getPixelPosition(), overlay.get(), &castor3d::OverlayCategory::setPixelPosition );
			addPropertyT( grid, PROPERTY_OVERLAY_SIZE, overlay->getPixelSize(), overlay.get(), &castor3d::OverlayCategory::setPixelSize );
			addProperty( grid, PROPERTY_OVERLAY_MATERIAL, getMaterialsList(), overlay->getMaterial()->getName()
				, [this]( wxVariant const & var )
				{
					castor3d::OverlayCategorySPtr ov = getOverlay();
					auto & engine = *ov->getOverlay().getEngine();
					auto material = engine.findMaterial( variantCast< castor::String >( var ) ).lock().get();

					if ( material )
					{
						ov->setMaterial( material );
					}
				} );

			switch ( overlay->getType() )
			{
			case castor3d::OverlayType::ePanel:
				doCreatePanelOverlayProperties( grid, std::static_pointer_cast< castor3d::PanelOverlay >( getOverlay() ) );
				break;
			case castor3d::OverlayType::eBorderPanel:
				doCreateBorderPanelOverlayProperties( grid, std::static_pointer_cast< castor3d::BorderPanelOverlay >( getOverlay() ) );
				break;
			case castor3d::OverlayType::eText:
				doCreateTextOverlayProperties( grid, std::static_pointer_cast< castor3d::TextOverlay >( getOverlay() ) );
				break;
			default:
				CU_Failure( "Unsupported OverlayType" );
				break;
			}
		}
	}

	void OverlayTreeItemProperty::doCreatePanelOverlayProperties( wxPropertyGrid * grid
		, castor3d::PanelOverlaySPtr overlay )
	{
		static wxString PROPERTY_CATEGORY_PANEL_OVERLAY = _( "Panel Overlay" );

		addProperty( grid, PROPERTY_CATEGORY_PANEL_OVERLAY );
	}

	void OverlayTreeItemProperty::doCreateBorderPanelOverlayProperties( wxPropertyGrid * grid
		, castor3d::BorderPanelOverlaySPtr overlay )
	{
		static wxString PROPERTY_CATEGORY_BORDER_PANEL_OVERLAY = _( "Border Panel Overlay" );
		static wxString PROPERTY_OVERLAY_BORDER_SIZE = _( "Border Size" );
		static wxString PROPERTY_OVERLAY_BORDER_MATERIAL = _( "Border Material" );
		static wxString PROPERTY_OVERLAY_BORDER_INNER_UV = _( "Border Inner UV" );
		static wxString PROPERTY_OVERLAY_BORDER_OUTER_UV = _( "Border Outer UV" );
		static wxString PROPERTY_OVERLAY_BORDER_POSITION = _( "Border Position" );
		static std::array< wxString, size_t( castor3d::BorderPosition::eCount ) > PROPERTY_OVERLAY_BORDER_POSITION_TEXTS{ _( "Internal" ), _( "Middle" ), _( "External" ) };

		addProperty( grid, PROPERTY_CATEGORY_BORDER_PANEL_OVERLAY );
		wxArrayString choices{ make_wxArrayString( PROPERTY_OVERLAY_BORDER_POSITION_TEXTS ) };

		addPropertyT( grid, PROPERTY_OVERLAY_BORDER_SIZE, overlay->getBorderPixelSize(), overlay.get(), &castor3d::BorderPanelOverlay::setBorderPixelSize );

		if ( overlay->getBorderMaterial() )
		{
			addProperty( grid, PROPERTY_OVERLAY_BORDER_MATERIAL, getMaterialsList(), overlay->getBorderMaterial()->getName()
			, [this]( wxVariant const & var )
				{
					castor3d::BorderPanelOverlaySPtr ov = std::static_pointer_cast< castor3d::BorderPanelOverlay >( getOverlay() );
					CU_Require( ov->getType() == castor3d::OverlayType::eBorderPanel );
					auto & engine = *ov->getOverlay().getEngine();
					auto material = engine.findMaterial( variantCast< castor::String >( var ) ).lock().get();

					if ( material )
					{
						ov->setBorderMaterial( material );
					}
				} );
		}
		else
		{
			addProperty( grid
				, PROPERTY_OVERLAY_BORDER_MATERIAL
				, getMaterialsList()
				, [this]( wxVariant const & var )
				{
					castor3d::BorderPanelOverlaySPtr ov = std::static_pointer_cast< castor3d::BorderPanelOverlay >( getOverlay() );
					CU_Require( ov->getType() == castor3d::OverlayType::eBorderPanel );
					auto & engine = *ov->getOverlay().getEngine();
					auto material = engine.findMaterial( variantCast< castor::String >( var ) ).lock().get();

					if ( material )
					{
						ov->setBorderMaterial( material );
					}
				} );
		}

		addPropertyT( grid, PROPERTY_OVERLAY_BORDER_INNER_UV, overlay->getBorderInnerUV(), overlay.get(), &castor3d::BorderPanelOverlay::setBorderInnerUV );
		addPropertyT( grid, PROPERTY_OVERLAY_BORDER_OUTER_UV, overlay->getBorderOuterUV(), overlay.get(), &castor3d::BorderPanelOverlay::setBorderOuterUV );
		addPropertyET( grid, PROPERTY_OVERLAY_BORDER_POSITION, choices, overlay->getBorderPosition(), overlay.get(), &castor3d::BorderPanelOverlay::setBorderPosition );
	}

	void OverlayTreeItemProperty::doCreateTextOverlayProperties( wxPropertyGrid * grid
		, castor3d::TextOverlaySPtr overlay )
	{
		static wxString PROPERTY_CATEGORY_TEXT_OVERLAY = _( "Text Overlay" );
		static wxString PROPERTY_OVERLAY_FONT = _( "Font" );
		static wxString PROPERTY_OVERLAY_CAPTION = _( "Caption" );
		static wxString PROPERTY_OVERLAY_HALIGN = _( "Horiz. align." );
		static std::array< wxString, size_t( castor3d::HAlign::eCount ) > PROPERTY_OVERLAY_HALIGN_TEXTS{ _( "Left" ), _( "Center" ), _( "Right" ) };
		static wxString PROPERTY_OVERLAY_VALIGN = _( "Vertic. align." );
		static std::array< wxString, size_t( castor3d::VAlign::eCount ) > PROPERTY_OVERLAY_VALIGN_TEXTS{ _( "Top" ), _( "Center" ), _( "Bottom" ) };
		static wxString PROPERTY_OVERLAY_WRAPPING = _( "Wrapping" );
		static std::array< wxString, size_t( castor3d::TextWrappingMode::eCount ) > PROPERTY_OVERLAY_WRAPPING_TEXTS{ _( "None" ), _( "Letter" ), _( "Word" ) };
		static wxString PROPERTY_OVERLAY_SPACING = _( "Line spacing" );
		static std::array< wxString, size_t( castor3d::TextLineSpacingMode::eCount ) > PROPERTY_OVERLAY_SPACING_TEXTS{ _( "Own height" ), _( "Max lines height" ), _( "Max fonts height" ) };
		static wxString PROPERTY_OVERLAY_TEXTURING = _( "Texture mapping" );
		static std::array< wxString, size_t( castor3d::TextTexturingMode::eCount ) > PROPERTY_OVERLAY_TEXTURING_TEXTS{ _( "Letter" ), _( "Text" ) };

		wxArrayString haligns{ make_wxArrayString( PROPERTY_OVERLAY_HALIGN_TEXTS ) };
		wxString halign{ PROPERTY_OVERLAY_HALIGN_TEXTS[size_t( overlay->getHAlign() )] };
		wxArrayString valigns{ make_wxArrayString( PROPERTY_OVERLAY_VALIGN_TEXTS ) };
		wxString valign{ PROPERTY_OVERLAY_VALIGN_TEXTS[size_t( overlay->getVAlign() )] };
		wxArrayString wrappings{ make_wxArrayString( PROPERTY_OVERLAY_WRAPPING_TEXTS ) };
		wxString wrapping{ PROPERTY_OVERLAY_WRAPPING_TEXTS[size_t( overlay->getTextWrappingMode() )] };
		wxArrayString spacings{ make_wxArrayString( PROPERTY_OVERLAY_SPACING_TEXTS ) };
		wxString spacing{ PROPERTY_OVERLAY_SPACING_TEXTS[size_t( overlay->getLineSpacingMode() )] };
		wxArrayString texturings{ make_wxArrayString( PROPERTY_OVERLAY_TEXTURING_TEXTS ) };
		wxString texturing{ PROPERTY_OVERLAY_TEXTURING_TEXTS[size_t( overlay->getTexturingMode() )] };

		addProperty( grid, PROPERTY_CATEGORY_TEXT_OVERLAY );
		addProperty( grid, PROPERTY_OVERLAY_FONT, *overlay->getFontTexture()->getFont()
			, [this]( wxVariant const & var )
			{
				castor3d::TextOverlaySPtr ov = std::static_pointer_cast< castor3d::TextOverlay >( getOverlay() );
				CU_Require( ov->getType() == castor3d::OverlayType::eText );
				ov->setFont( variantCast< castor::FontSPtr >( var )->getName() );
			} );
		addPropertyT( grid, PROPERTY_OVERLAY_CAPTION, overlay->getCaption(), overlay.get(), &castor3d::TextOverlay::setCaption );
		addPropertyET( grid, PROPERTY_OVERLAY_HALIGN, haligns, overlay->getHAlign(), overlay.get(), &castor3d::TextOverlay::setHAlign );
		addPropertyET( grid, PROPERTY_OVERLAY_VALIGN, haligns, overlay->getVAlign(), overlay.get(), &castor3d::TextOverlay::setVAlign );
		addPropertyET( grid, PROPERTY_OVERLAY_WRAPPING, wrappings, overlay->getTextWrappingMode(), overlay.get(), &castor3d::TextOverlay::setTextWrappingMode );
		addPropertyET( grid, PROPERTY_OVERLAY_SPACING, spacings, overlay->getLineSpacingMode(), overlay.get(), &castor3d::TextOverlay::setLineSpacingMode );
		addPropertyET( grid, PROPERTY_OVERLAY_TEXTURING, texturings, overlay->getTexturingMode(), overlay.get(), &castor3d::TextOverlay::setTexturingMode );
	}
}
