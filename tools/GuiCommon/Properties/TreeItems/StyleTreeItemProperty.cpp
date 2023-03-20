#include "GuiCommon/Properties/TreeItems/StyleTreeItemProperty.hpp"

#include "GuiCommon/Properties/Math/PositionProperties.hpp"
#include "GuiCommon/Properties/Math/SizeProperties.hpp"
#include "GuiCommon/Properties/Math/RectangleProperties.hpp"
#include "GuiCommon/Properties/Math/PointProperties.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/MaterialCache.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Gui/Theme/Theme.hpp>

#include <CastorUtils/Graphics/Font.hpp>

#include <wx/propgrid/advprops.h>

namespace GuiCommon
{
	StyleTreeItemProperty::StyleTreeItemProperty( bool editable
		, castor3d::ControlStyle & style )
		: TreeItemProperty{ &style.getEngine(), editable }
		, m_style{ style }
	{
		CreateTreeItemMenu();
	}

	void StyleTreeItemProperty::doCreateProperties( wxPGEditor * editor
		, wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY = _( "Style: " );
		static wxString PROPERTY_BACKGROUND_MATERIAL = _( "Background Material" );
		static wxString PROPERTY_FOREGROUND_MATERIAL = _( "Foreground Material" );

		auto & style = getStyle();
		auto & engine = style.getEngine();
		m_materials = getMaterialsList();
		m_fonts = getFontsList();
		addProperty( grid, PROPERTY_CATEGORY + wxString( style.getName() ) );
		addMaterial( grid, engine, PROPERTY_BACKGROUND_MATERIAL, m_materials, style.getBackgroundMaterial()
			, [&style]( castor3d::MaterialRPtr material ) { style.setBackgroundMaterial( material ); } );
		addMaterial( grid, engine, PROPERTY_FOREGROUND_MATERIAL, m_materials, style.getForegroundMaterial()
			, [&style]( castor3d::MaterialRPtr material ) { style.setForegroundMaterial( material ); } );

		switch ( style.getType() )
		{
		case castor3d::ControlType::ePanel:
			doCreateStyleProperties( grid, static_cast< castor3d::PanelStyle & >( style ) );
			break;
		case castor3d::ControlType::eStatic:
			doCreateStyleProperties( grid, static_cast< castor3d::StaticStyle & >( style ) );
			break;
		case castor3d::ControlType::eEdit:
			doCreateStyleProperties( grid, static_cast< castor3d::EditStyle & >( style ) );
			break;
		case castor3d::ControlType::eSlider:
			doCreateStyleProperties( grid, static_cast< castor3d::SliderStyle & >( style ) );
			break;
		case castor3d::ControlType::eComboBox:
			doCreateStyleProperties( grid, static_cast< castor3d::ComboBoxStyle & >( style ) );
			break;
		case castor3d::ControlType::eListBox:
			doCreateStyleProperties( grid, static_cast< castor3d::ListBoxStyle & >( style ) );
			break;
		case castor3d::ControlType::eButton:
			doCreateStyleProperties( grid, static_cast< castor3d::ButtonStyle & >( style ) );
			break;
		case castor3d::ControlType::eExpandablePanel:
			doCreateStyleProperties( grid, static_cast< castor3d::ExpandablePanelStyle & >( style ) );
			break;
		default:
			CU_Failure( "Unsupported ControlType" );
			break;
		}
	}

	void StyleTreeItemProperty::doCreateStyleProperties( wxPropertyGrid * grid
		, castor3d::ButtonStyle & style )
	{
		static wxString PROPERTY_FONT = _( "Font" );
		static wxString PROPERTY_TEXT_MATERIAL = _( "Text Material" );
		static wxString PROPERTY_HL_BACKGROUND_MATERIAL = _( "Highlighted Bg Mat." );
		static wxString PROPERTY_HL_FOREGROUND_MATERIAL = _( "Highlighted Fg Mat." );
		static wxString PROPERTY_HL_TEXT_MATERIAL = _( "Highlighted Text Mat." );
		static wxString PROPERTY_PS_BACKGROUND_MATERIAL = _( "Pushed Bg Mat." );
		static wxString PROPERTY_PS_FOREGROUND_MATERIAL = _( "Pushed Fg Mat." );
		static wxString PROPERTY_PS_TEXT_MATERIAL = _( "Pushed Text Mat." );
		static wxString PROPERTY_DS_BACKGROUND_MATERIAL = _( "Disabled Bg Mat." );
		static wxString PROPERTY_DS_FOREGROUND_MATERIAL = _( "Disabled Fg Mat." );
		static wxString PROPERTY_DS_TEXT_MATERIAL = _( "Disabled Text Mat." );

		auto & engine = style.getEngine();
		addProperty( grid, PROPERTY_FONT, m_fonts, style.getFontName()
			, [this, &style]( wxVariant const & var )
			{
				auto name = make_String( m_fonts[size_t( var.GetLong() )] );
				style.setFont( name );
			} );
		addMaterial( grid, engine, PROPERTY_TEXT_MATERIAL, m_materials, style.getTextMaterial()
			, [&style]( castor3d::MaterialRPtr material ) { style.setTextMaterial( material ); } );

		addMaterial( grid, engine, PROPERTY_HL_BACKGROUND_MATERIAL, m_materials, style.getHighlightedBackgroundMaterial()
			, [&style]( castor3d::MaterialRPtr material ) { style.setHighlightedBackgroundMaterial( material ); } );
		addMaterial( grid, engine, PROPERTY_HL_FOREGROUND_MATERIAL, m_materials, style.getHighlightedForegroundMaterial()
			, [&style]( castor3d::MaterialRPtr material ) { style.setHighlightedForegroundMaterial( material ); } );
		addMaterial( grid, engine, PROPERTY_HL_TEXT_MATERIAL, m_materials, style.getHighlightedTextMaterial()
			, [&style]( castor3d::MaterialRPtr material ) { style.setHighlightedTextMaterial( material ); } );

		addMaterial( grid, engine, PROPERTY_HL_BACKGROUND_MATERIAL, m_materials, style.getPushedBackgroundMaterial()
			, [&style]( castor3d::MaterialRPtr material ) { style.setPushedBackgroundMaterial( material ); } );
		addMaterial( grid, engine, PROPERTY_HL_FOREGROUND_MATERIAL, m_materials, style.getPushedForegroundMaterial()
			, [&style]( castor3d::MaterialRPtr material ) { style.setPushedForegroundMaterial( material ); } );
		addMaterial( grid, engine, PROPERTY_HL_TEXT_MATERIAL, m_materials, style.getPushedTextMaterial()
			, [&style]( castor3d::MaterialRPtr material ) { style.setPushedTextMaterial( material ); } );

		addMaterial( grid, engine, PROPERTY_HL_BACKGROUND_MATERIAL, m_materials, style.getDisabledBackgroundMaterial()
			, [&style]( castor3d::MaterialRPtr material ) { style.setDisabledBackgroundMaterial( material ); } );
		addMaterial( grid, engine, PROPERTY_HL_FOREGROUND_MATERIAL, m_materials, style.getDisabledForegroundMaterial()
			, [&style]( castor3d::MaterialRPtr material ) { style.setDisabledForegroundMaterial( material ); } );
		addMaterial( grid, engine, PROPERTY_HL_TEXT_MATERIAL, m_materials, style.getDisabledTextMaterial()
			, [&style]( castor3d::MaterialRPtr material ) { style.setDisabledTextMaterial( material ); } );
	}

	void StyleTreeItemProperty::doCreateStyleProperties( wxPropertyGrid * grid
		, castor3d::ComboBoxStyle & style )
	{
	}

	void StyleTreeItemProperty::doCreateStyleProperties( wxPropertyGrid * grid
		, castor3d::EditStyle & style )
	{
	}

	void StyleTreeItemProperty::doCreateStyleProperties( wxPropertyGrid * grid
		, castor3d::ExpandablePanelStyle & style )
	{
	}

	void StyleTreeItemProperty::doCreateStyleProperties( wxPropertyGrid * grid
		, castor3d::ListBoxStyle & style )
	{
	}

	void StyleTreeItemProperty::doCreateStyleProperties( wxPropertyGrid * grid
		, castor3d::PanelStyle & style )
	{
	}

	void StyleTreeItemProperty::doCreateStyleProperties( wxPropertyGrid * grid
		, castor3d::SliderStyle & style )
	{
	}

	void StyleTreeItemProperty::doCreateStyleProperties( wxPropertyGrid * grid
		, castor3d::StaticStyle & style )
	{
		static wxString PROPERTY_FONT = _( "Font" );
		static wxString PROPERTY_TEXT_MATERIAL = _( "Text Material" );

		auto & engine = style.getEngine();
		addProperty( grid, PROPERTY_FONT, m_fonts, style.getFontName()
			, [this, &style]( wxVariant const & var )
			{
				auto name = make_String( m_fonts[size_t( var.GetLong() )] );
				style.setFont( name );
			} );
		addMaterial( grid, engine, PROPERTY_TEXT_MATERIAL, m_materials, style.getTextMaterial()
			, [&style]( castor3d::MaterialRPtr material ) { style.setTextMaterial( material ); } );
	}
}
