/*
See LICENSE file in root folder
*/
#include "Castor3D/Gui/Theme/StylesHolder.hpp"

#include "Castor3D/Gui/Theme/Theme.hpp"

namespace castor3d
{
	//************************************************************************************************

	bool isStylesHolder( ControlType type )
	{
		return type == ControlType::ePanel;
	}

	bool isStylesHolder( ControlStyle const & style )
	{
		return isStylesHolder( style.getType() );
	}

	//************************************************************************************************

	ButtonStyleRPtr StylesHolder::createButtonStyle( castor::String name
		, Scene * scene )
	{
		return createControlStyle( castor::move( name ), scene, m_buttonStyles );
	}

	ComboBoxStyleRPtr StylesHolder::createComboBoxStyle( castor::String name
		, Scene * scene )
	{
		return createControlStyle( castor::move( name ), scene, m_comboBoxStyles );
	}

	EditStyleRPtr StylesHolder::createEditStyle( castor::String name
		, Scene * scene )
	{
		return createControlStyle( castor::move( name ), scene, m_editStyles );
	}

	ExpandablePanelStyleRPtr StylesHolder::createExpandablePanelStyle( castor::String name
		, Scene * scene )
	{
		return createControlStyle( castor::move( name ), scene, m_expandablePanelStyles );
	}

	FrameStyleRPtr StylesHolder::createFrameStyle( castor::String name
		, Scene * scene )
	{
		return createControlStyle( castor::move( name ), scene, m_frameStyles );
	}

	ListBoxStyleRPtr StylesHolder::createListBoxStyle( castor::String name
		, Scene * scene )
	{
		return createControlStyle( castor::move( name ), scene, m_listBoxStyles );
	}

	PanelStyleRPtr StylesHolder::createPanelStyle( castor::String name
		, Scene * scene )
	{
		return createControlStyle( castor::move( name ), scene, m_panelStyles );
	}

	ProgressStyleRPtr StylesHolder::createProgressStyle( castor::String name
		, Scene * scene )
	{
		return createControlStyle( castor::move( name ), scene, m_progressStyles );
	}

	ScrollBarStyleRPtr StylesHolder::createScrollBarStyle( castor::String name
		, Scene * scene )
	{
		return createControlStyle( castor::move( name ), scene, m_scrollBarStyles );
	}

	SliderStyleRPtr StylesHolder::createSliderStyle( castor::String name
		, Scene * scene )
	{
		return createControlStyle( castor::move( name ), scene, m_sliderStyles );
	}

	StaticStyleRPtr StylesHolder::createStaticStyle( castor::String name
		, Scene * scene )
	{
		return createControlStyle( castor::move( name ), scene, m_staticStyles );
	}

	ButtonStyleRPtr StylesHolder::getButtonStyle( castor::String const & name )const
	{
		return getControlStyle( name, m_buttonStyles );
	}

	ComboBoxStyleRPtr StylesHolder::getComboBoxStyle( castor::String const & name )const
	{
		return getControlStyle( name, m_comboBoxStyles );
	}

	EditStyleRPtr StylesHolder::getEditStyle( castor::String const & name )const
	{
		return getControlStyle( name, m_editStyles );
	}

	ExpandablePanelStyleRPtr StylesHolder::getExpandablePanelStyle( castor::String const & name )const
	{
		return getControlStyle( name, m_expandablePanelStyles );
	}

	FrameStyleRPtr StylesHolder::getFrameStyle( castor::String const & name )const
	{
		return getControlStyle( name, m_frameStyles );
	}

	ListBoxStyleRPtr StylesHolder::getListBoxStyle( castor::String const & name )const
	{
		return getControlStyle( name, m_listBoxStyles );
	}

	PanelStyleRPtr StylesHolder::getPanelStyle( castor::String const & name )const
	{
		return getControlStyle( name, m_panelStyles );
	}

	ProgressStyleRPtr StylesHolder::getProgressStyle( castor::String const & name )const
	{
		return getControlStyle( name, m_progressStyles );
	}

	ScrollBarStyleRPtr StylesHolder::getScrollBarStyle( castor::String const & name )const
	{
		return getControlStyle( name, m_scrollBarStyles );
	}

	SliderStyleRPtr StylesHolder::getSliderStyle( castor::String const & name )const
	{
		return getControlStyle( name, m_sliderStyles );
	}

	StaticStyleRPtr StylesHolder::getStaticStyle( castor::String const & name )const
	{
		return getControlStyle( name, m_staticStyles );
	}

	//************************************************************************************************
}
