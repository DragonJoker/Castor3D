/*
See LICENSE file in root folder
*/
#include "Castor3D/Gui/Theme/Theme.hpp"

namespace castor3d
{
	ButtonStyleRPtr StylesHolder::createButtonStyle( castor::String name
		, Scene * scene )
	{
		return createControlStyle( std::move( name ), scene, m_buttonStyles );
	}

	ComboBoxStyleRPtr StylesHolder::createComboBoxStyle( castor::String name
		, Scene * scene )
	{
		return createControlStyle( std::move( name ), scene, m_comboBoxStyles );
	}

	EditStyleRPtr StylesHolder::createEditStyle( castor::String name
		, Scene * scene )
	{
		return createControlStyle( std::move( name ), scene, m_editStyles );
	}

	ListBoxStyleRPtr StylesHolder::createListBoxStyle( castor::String name
		, Scene * scene )
	{
		return createControlStyle( std::move( name ), scene, m_listBoxStyles );
	}

	SliderStyleRPtr StylesHolder::createSliderStyle( castor::String name
		, Scene * scene )
	{
		return createControlStyle( std::move( name ), scene, m_sliderStyles );
	}

	StaticStyleRPtr StylesHolder::createStaticStyle( castor::String name
		, Scene * scene )
	{
		return createControlStyle( std::move( name ), scene, m_staticStyles );
	}

	PanelStyleRPtr StylesHolder::createPanelStyle( castor::String name
		, Scene * scene )
	{
		return createControlStyle( std::move( name ), scene, m_panelStyles );
	}

	ExpandablePanelStyleRPtr StylesHolder::createExpandablePanelStyle( castor::String name
		, Scene * scene )
	{
		return createControlStyle( std::move( name ), scene, m_expandablePanelStyles );
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

	ListBoxStyleRPtr StylesHolder::getListBoxStyle( castor::String const & name )const
	{
		return getControlStyle( name, m_listBoxStyles );
	}

	SliderStyleRPtr StylesHolder::getSliderStyle( castor::String const & name )const
	{
		return getControlStyle( name, m_sliderStyles );
	}

	StaticStyleRPtr StylesHolder::getStaticStyle( castor::String const & name )const
	{
		return getControlStyle( name, m_staticStyles );
	}

	PanelStyleRPtr StylesHolder::getPanelStyle( castor::String const & name )const
	{
		return getControlStyle( name, m_panelStyles );
	}

	ExpandablePanelStyleRPtr StylesHolder::getExpandablePanelStyle( castor::String const & name )const
	{
		return getControlStyle( name, m_expandablePanelStyles );
	}
}
