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
#include <Castor3D/Gui/Controls/CtrlListBox.hpp>
#include <Castor3D/Gui/Controls/CtrlPanel.hpp>
#include <Castor3D/Gui/Controls/CtrlSlider.hpp>
#include <Castor3D/Gui/Controls/CtrlStatic.hpp>

#include <CastorUtils/Graphics/Font.hpp>

#include <wx/propgrid/advprops.h>

namespace GuiCommon
{
	ControlTreeItemProperty::ControlTreeItemProperty( bool editable
		, castor3d::Control & control )
		: TreeItemProperty{ &control.getEngine(), editable }
		, m_control{ control }
	{
		CreateTreeItemMenu();
	}

	void ControlTreeItemProperty::doCreateProperties( wxPGEditor * editor
		, wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_CONTROL = _( "Control: " );
		static wxString PROPERTY_CONTROL_VISIBLE = _( "Visible" );
		static wxString PROPERTY_CONTROL_POSITION = _( "Position" );
		static wxString PROPERTY_CONTROL_SIZE = _( "Size" );
		static wxString PROPERTY_CONTROL_BORDERS_SIZE = _( "Borders" );

		auto & control = getControl();
		addProperty( grid, PROPERTY_CATEGORY_CONTROL + wxString( control.getName() ) );
		addPropertyT( grid, PROPERTY_CONTROL_VISIBLE, control.isVisible(), &control, &castor3d::Control::setVisible );
		addPropertyT( grid, PROPERTY_CONTROL_POSITION, control.getPosition(), &control, &castor3d::Control::setPosition );
		addPropertyT( grid, PROPERTY_CONTROL_SIZE, control.getSize(), &control, &castor3d::Control::setSize );
		addPropertyT( grid, PROPERTY_CONTROL_BORDERS_SIZE, control.getBorderSize(), &control, &castor3d::Control::setBorderSize );

		switch ( control.getType() )
		{
		case castor3d::ControlType::ePanel:
			doCreateControlProperties( grid, static_cast< castor3d::PanelCtrl & >( control ) );
			break;
		case castor3d::ControlType::eStatic:
			doCreateControlProperties( grid, static_cast< castor3d::StaticCtrl & >( control ) );
			break;
		case castor3d::ControlType::eEdit:
			doCreateControlProperties( grid, static_cast< castor3d::EditCtrl & >( control ) );
			break;
		case castor3d::ControlType::eSlider:
			doCreateControlProperties( grid, static_cast< castor3d::SliderCtrl & >( control ) );
			break;
		case castor3d::ControlType::eComboBox:
			doCreateControlProperties( grid, static_cast< castor3d::ComboBoxCtrl & >( control ) );
			break;
		case castor3d::ControlType::eListBox:
			doCreateControlProperties( grid, static_cast< castor3d::ListBoxCtrl & >( control ) );
			break;
		case castor3d::ControlType::eButton:
			doCreateControlProperties( grid, static_cast< castor3d::ButtonCtrl & >( control ) );
			break;
		case castor3d::ControlType::eExpandablePanel:
			doCreateControlProperties( grid, static_cast< castor3d::ExpandablePanelCtrl & >( control ) );
			break;
		default:
			CU_Failure( "Unsupported ControlType" );
			break;
		}
	}

	void ControlTreeItemProperty::doCreateControlProperties( wxPropertyGrid * grid
		, castor3d::ButtonCtrl & control )
	{
		static wxString PROPERTY_CAPTION = _( "Caption" );

		static wxString PROPERTY_HALIGN = _( "Horiz. align." );
		static std::array< wxString, size_t( castor3d::HAlign::eCount ) > PROPERTY_HALIGN_TEXTS{ _( "Left" ), _( "Center" ), _( "Right" ) };
		static wxString PROPERTY_VALIGN = _( "Vertic. align." );
		static std::array< wxString, size_t( castor3d::VAlign::eCount ) > PROPERTY_VALIGN_TEXTS{ _( "Top" ), _( "Center" ), _( "Bottom" ) };

		wxArrayString haligns{ make_wxArrayString( PROPERTY_HALIGN_TEXTS ) };
		wxString halign{ PROPERTY_HALIGN_TEXTS[size_t( control.getHAlign() )] };
		wxArrayString valigns{ make_wxArrayString( PROPERTY_VALIGN_TEXTS ) };
		wxString valign{ PROPERTY_VALIGN_TEXTS[size_t( control.getVAlign() )] };

		addPropertyT( grid, PROPERTY_CAPTION, control.getCaption(), &control, &castor3d::ButtonCtrl::setCaption );
		addPropertyET( grid, PROPERTY_HALIGN, haligns, control.getHAlign(), &control, &castor3d::ButtonCtrl::setHAlign );
		addPropertyET( grid, PROPERTY_VALIGN, valigns, control.getVAlign(), &control, &castor3d::ButtonCtrl::setVAlign );
	}

	void ControlTreeItemProperty::doCreateControlProperties( wxPropertyGrid * grid
		, castor3d::ComboBoxCtrl & control )
	{
	}

	void ControlTreeItemProperty::doCreateControlProperties( wxPropertyGrid * grid
		, castor3d::EditCtrl & control )
	{
		static wxString PROPERTY_CAPTION = _( "Caption" );

		addPropertyT( grid, PROPERTY_CAPTION, control.getCaption(), &control, &castor3d::EditCtrl::setCaption );
	}

	void ControlTreeItemProperty::doCreateControlProperties( wxPropertyGrid * grid
		, castor3d::ExpandablePanelCtrl & control )
	{
	}

	void ControlTreeItemProperty::doCreateControlProperties( wxPropertyGrid * grid
		, castor3d::ListBoxCtrl & control )
	{
	}

	void ControlTreeItemProperty::doCreateControlProperties( wxPropertyGrid * grid
		, castor3d::PanelCtrl & control )
	{
	}

	void ControlTreeItemProperty::doCreateControlProperties( wxPropertyGrid * grid
		, castor3d::SliderCtrl & control )
	{
	}

	void ControlTreeItemProperty::doCreateControlProperties( wxPropertyGrid * grid
		, castor3d::StaticCtrl & control )
	{
		static wxString PROPERTY_CAPTION = _( "Caption" );

		static wxString PROPERTY_HALIGN = _( "Horiz. align." );
		static std::array< wxString, size_t( castor3d::HAlign::eCount ) > PROPERTY_HALIGN_TEXTS{ _( "Left" ), _( "Center" ), _( "Right" ) };
		static wxString PROPERTY_VALIGN = _( "Vertic. align." );
		static std::array< wxString, size_t( castor3d::VAlign::eCount ) > PROPERTY_VALIGN_TEXTS{ _( "Top" ), _( "Center" ), _( "Bottom" ) };

		wxArrayString haligns{ make_wxArrayString( PROPERTY_HALIGN_TEXTS ) };
		wxString halign{ PROPERTY_HALIGN_TEXTS[size_t( control.getHAlign() )] };
		wxArrayString valigns{ make_wxArrayString( PROPERTY_VALIGN_TEXTS ) };
		wxString valign{ PROPERTY_VALIGN_TEXTS[size_t( control.getVAlign() )] };

		addPropertyT( grid, PROPERTY_CAPTION, control.getCaption(), &control, &castor3d::StaticCtrl::setCaption );
		addPropertyET( grid, PROPERTY_HALIGN, haligns, control.getHAlign(), &control, &castor3d::StaticCtrl::setHAlign );
		addPropertyET( grid, PROPERTY_VALIGN, valigns, control.getVAlign(), &control, &castor3d::StaticCtrl::setVAlign );
	}
}
