#include "GuiCommon/Properties/TreeItems/ControlTreeItemProperty.hpp"

#include "GuiCommon/Properties/Math/PositionProperties.hpp"
#include "GuiCommon/Properties/Math/SizeProperties.hpp"
#include "GuiCommon/Properties/Math/RectangleProperties.hpp"
#include "GuiCommon/Properties/Math/PointProperties.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/MaterialCache.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Gui/Controls/CtrlButton.hpp>
#include <Castor3D/Gui/Controls/CtrlComboBox.hpp>
#include <Castor3D/Gui/Controls/CtrlEdit.hpp>
#include <Castor3D/Gui/Controls/CtrlExpandablePanel.hpp>
#include <Castor3D/Gui/Controls/CtrlFrame.hpp>
#include <Castor3D/Gui/Controls/CtrlListBox.hpp>
#include <Castor3D/Gui/Controls/CtrlPanel.hpp>
#include <Castor3D/Gui/Controls/CtrlSlider.hpp>
#include <Castor3D/Gui/Controls/CtrlStatic.hpp>

#include <CastorUtils/Graphics/Font.hpp>

#include <wx/propgrid/advprops.h>

namespace GuiCommon
{
	ControlTreeItemProperty::ControlTreeItemProperty( bool editable
		, c3d::Engine * engine )
		: TreeItemProperty{ engine, editable }
	{
		CreateTreeItemMenu();
	}

	void ControlTreeItemProperty::doCreateProperties( wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_CONTROL = _( "Control: " );
		static wxString PROPERTY_CONTROL_VISIBLE = _( "Visible" );
		static wxString PROPERTY_CONTROL_POSITION = _( "Position" );
		static wxString PROPERTY_CONTROL_SIZE = _( "Size" );
		static wxString PROPERTY_CONTROL_BORDERS_SIZE = _( "Borders" );

		if ( !m_control )
		{
			return;
		}

		auto & control = *m_control;
		addProperty( grid, PROPERTY_CATEGORY_CONTROL + wxString( control.getName() ) );

		if ( m_full )
		{
			addPropertyT( grid, PROPERTY_CONTROL_VISIBLE, control.isVisible(), &control, &c3d::Control::setVisible );

			if ( !m_inLayout )
			{
				addPropertyT( grid, PROPERTY_CONTROL_POSITION, control.getPosition(), &control, &c3d::Control::setPosition );
			}

			addPropertyT( grid, PROPERTY_CONTROL_SIZE, control.getSize(), &control, &c3d::Control::setSize );
		}

		addPropertyT( grid, PROPERTY_CONTROL_BORDERS_SIZE, control.getBorderSize(), &control, &c3d::Control::setBorderSize );

		switch ( control.getType() )
		{
		case c3d::ControlType::ePanel:
			doCreateControlProperties( grid, static_cast< c3d::PanelCtrl & >( control ) );
			break;
		case c3d::ControlType::eStatic:
			doCreateControlProperties( grid, static_cast< c3d::StaticCtrl & >( control ) );
			break;
		case c3d::ControlType::eEdit:
			doCreateControlProperties( grid, static_cast< c3d::EditCtrl & >( control ) );
			break;
		case c3d::ControlType::eSlider:
			doCreateControlProperties( grid, static_cast< c3d::SliderCtrl & >( control ) );
			break;
		case c3d::ControlType::eComboBox:
			doCreateControlProperties( grid, static_cast< c3d::ComboBoxCtrl & >( control ) );
			break;
		case c3d::ControlType::eListBox:
			doCreateControlProperties( grid, static_cast< c3d::ListBoxCtrl & >( control ) );
			break;
		case c3d::ControlType::eButton:
			doCreateControlProperties( grid, static_cast< c3d::ButtonCtrl & >( control ) );
			break;
		case c3d::ControlType::eExpandablePanel:
			doCreateControlProperties( grid, static_cast< c3d::ExpandablePanelCtrl & >( control ) );
			break;
		case c3d::ControlType::eFrame:
			doCreateControlProperties( grid, static_cast< c3d::FrameCtrl & >( control ) );
			break;
		default:
			CU_Failure( "Unsupported ControlType" );
			break;
		}
	}

	void ControlTreeItemProperty::doCreateControlProperties( wxPropertyGrid * grid
		, c3d::ButtonCtrl & control )
	{
		static wxString PROPERTY_CAPTION = _( "Caption" );

		static wxString PROPERTY_HALIGN = _( "Horiz. align." );
		static c3d::Array< wxString, size_t( c3d::HAlign::eCount ) > PROPERTY_HALIGN_TEXTS{ _( "Left" ), _( "Center" ), _( "Right" ) };
		static wxString PROPERTY_VALIGN = _( "Vertic. align." );
		static c3d::Array< wxString, size_t( c3d::VAlign::eCount ) > PROPERTY_VALIGN_TEXTS{ _( "Top" ), _( "Center" ), _( "Bottom" ) };

		wxArrayString haligns{ make_wxArrayString( PROPERTY_HALIGN_TEXTS ) };
		wxString halign{ PROPERTY_HALIGN_TEXTS[size_t( control.getHAlign() )] };
		wxArrayString valigns{ make_wxArrayString( PROPERTY_VALIGN_TEXTS ) };
		wxString valign{ PROPERTY_VALIGN_TEXTS[size_t( control.getVAlign() )] };

		addPropertyT( grid, PROPERTY_CAPTION, control.getCaption(), &control, &c3d::ButtonCtrl::setCaption );
		addPropertyET( grid, PROPERTY_HALIGN, haligns, control.getHAlign(), &control, &c3d::ButtonCtrl::setHAlign );
		addPropertyET( grid, PROPERTY_VALIGN, valigns, control.getVAlign(), &control, &c3d::ButtonCtrl::setVAlign );
	}

	void ControlTreeItemProperty::doCreateControlProperties( wxPropertyGrid * grid
		, c3d::ComboBoxCtrl & control )
	{
	}

	void ControlTreeItemProperty::doCreateControlProperties( wxPropertyGrid * grid
		, c3d::EditCtrl & control )
	{
		static wxString PROPERTY_CAPTION = _( "Caption" );

		addPropertyT( grid, PROPERTY_CAPTION, control.getCaption(), &control, &c3d::EditCtrl::setCaption );
	}

	void ControlTreeItemProperty::doCreateControlProperties( wxPropertyGrid * grid
		, c3d::ExpandablePanelCtrl & control )
	{
		static wxString PROPERTY_EXPAND_CAPTION = _( "Expand Caption" );
		static wxString PROPERTY_RETRACT_CAPTION = _( "Retract Caption" );

		addPropertyT( grid, PROPERTY_EXPAND_CAPTION, control.getExpandCaption(), &control, &c3d::ExpandablePanelCtrl::setExpandCaption );
		addPropertyT( grid, PROPERTY_RETRACT_CAPTION, control.getRetractCaption(), &control, &c3d::ExpandablePanelCtrl::setRetractCaption );
	}

	void ControlTreeItemProperty::doCreateControlProperties( wxPropertyGrid * grid
		, c3d::FrameCtrl & control )
	{
		static wxString PROPERTY_CAPTION = _( "Caption" );

		static wxString PROPERTY_HALIGN = _( "Horiz. align." );
		static c3d::Array< wxString, size_t( c3d::HAlign::eCount ) > PROPERTY_HALIGN_TEXTS{ _( "Left" ), _( "Center" ), _( "Right" ) };
		static wxString PROPERTY_VALIGN = _( "Vertic. align." );
		static c3d::Array< wxString, size_t( c3d::VAlign::eCount ) > PROPERTY_VALIGN_TEXTS{ _( "Top" ), _( "Center" ), _( "Bottom" ) };

		wxArrayString haligns{ make_wxArrayString( PROPERTY_HALIGN_TEXTS ) };
		wxString halign{ PROPERTY_HALIGN_TEXTS[size_t( control.getHeaderHAlign() )] };
		wxArrayString valigns{ make_wxArrayString( PROPERTY_VALIGN_TEXTS ) };
		wxString valign{ PROPERTY_VALIGN_TEXTS[size_t( control.getHeaderVAlign() )] };

		addPropertyT( grid, PROPERTY_CAPTION, control.getHeaderCaption(), &control, &c3d::FrameCtrl::setHeaderCaption );
		addPropertyET( grid, PROPERTY_HALIGN, haligns, control.getHeaderHAlign(), &control, &c3d::FrameCtrl::setHeaderHAlign );
		addPropertyET( grid, PROPERTY_VALIGN, valigns, control.getHeaderVAlign(), &control, &c3d::FrameCtrl::setHeaderVAlign );
	}

	void ControlTreeItemProperty::doCreateControlProperties( wxPropertyGrid * grid
		, c3d::ListBoxCtrl & control )
	{
	}

	void ControlTreeItemProperty::doCreateControlProperties( wxPropertyGrid * grid
		, c3d::PanelCtrl & control )
	{
	}

	void ControlTreeItemProperty::doCreateControlProperties( wxPropertyGrid * grid
		, c3d::SliderCtrl & control )
	{
	}

	void ControlTreeItemProperty::doCreateControlProperties( wxPropertyGrid * grid
		, c3d::StaticCtrl & control )
	{
		static wxString PROPERTY_CAPTION = _( "Caption" );

		static wxString PROPERTY_HALIGN = _( "Horiz. align." );
		static c3d::Array< wxString, size_t( c3d::HAlign::eCount ) > PROPERTY_HALIGN_TEXTS{ _( "Left" ), _( "Center" ), _( "Right" ) };
		static wxString PROPERTY_VALIGN = _( "Vertic. align." );
		static c3d::Array< wxString, size_t( c3d::VAlign::eCount ) > PROPERTY_VALIGN_TEXTS{ _( "Top" ), _( "Center" ), _( "Bottom" ) };

		wxArrayString haligns{ make_wxArrayString( PROPERTY_HALIGN_TEXTS ) };
		wxString halign{ PROPERTY_HALIGN_TEXTS[size_t( control.getHAlign() )] };
		wxArrayString valigns{ make_wxArrayString( PROPERTY_VALIGN_TEXTS ) };
		wxString valign{ PROPERTY_VALIGN_TEXTS[size_t( control.getVAlign() )] };

		addPropertyT( grid, PROPERTY_CAPTION, control.getCaption(), &control, &c3d::StaticCtrl::setCaption );
		addPropertyET( grid, PROPERTY_HALIGN, haligns, control.getHAlign(), &control, &c3d::StaticCtrl::setHAlign );
		addPropertyET( grid, PROPERTY_VALIGN, valigns, control.getVAlign(), &control, &c3d::StaticCtrl::setVAlign );
	}
}
