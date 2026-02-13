/*
See LICENSE file in root folder
*/
#include "Castor3D/Gui/Theme/StylesHolder.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Gui/ControlsManager.hpp"
#include "Castor3D/Gui/Theme/Theme.hpp"

namespace c3d
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

	ButtonStyleRPtr StylesHolder::createButtonStyle( String name
		, Scene * scene )
	{
		return createControlStyle( c3d::move( name ), scene, m_buttonStyles );
	}

	ComboBoxStyleRPtr StylesHolder::createComboBoxStyle( String name
		, Scene * scene )
	{
		return createControlStyle( c3d::move( name ), scene, m_comboBoxStyles );
	}

	EditStyleRPtr StylesHolder::createEditStyle( String name
		, Scene * scene )
	{
		return createControlStyle( c3d::move( name ), scene, m_editStyles );
	}

	ExpandablePanelStyleRPtr StylesHolder::createExpandablePanelStyle( String name
		, Scene * scene )
	{
		return createControlStyle( c3d::move( name ), scene, m_expandablePanelStyles );
	}

	FrameStyleRPtr StylesHolder::createFrameStyle( String name
		, Scene * scene )
	{
		return createControlStyle( c3d::move( name ), scene, m_frameStyles );
	}

	ListBoxStyleRPtr StylesHolder::createListBoxStyle( String name
		, Scene * scene )
	{
		return createControlStyle( c3d::move( name ), scene, m_listBoxStyles );
	}

	PanelStyleRPtr StylesHolder::createPanelStyle( String name
		, Scene * scene )
	{
		return createControlStyle( c3d::move( name ), scene, m_panelStyles );
	}

	ProgressStyleRPtr StylesHolder::createProgressStyle( String name
		, Scene * scene )
	{
		return createControlStyle( c3d::move( name ), scene, m_progressStyles );
	}

	ScrollBarStyleRPtr StylesHolder::createScrollBarStyle( String name
		, Scene * scene )
	{
		return createControlStyle( c3d::move( name ), scene, m_scrollBarStyles );
	}

	SliderStyleRPtr StylesHolder::createSliderStyle( String name
		, Scene * scene )
	{
		return createControlStyle( c3d::move( name ), scene, m_sliderStyles );
	}

	StaticStyleRPtr StylesHolder::createStaticStyle( String name
		, Scene * scene )
	{
		return createControlStyle( c3d::move( name ), scene, m_staticStyles );
	}

	ButtonStyleRPtr StylesHolder::getButtonStyle( String const & name )const
	{
		return getControlStyle( name, m_buttonStyles );
	}

	ComboBoxStyleRPtr StylesHolder::getComboBoxStyle( String const & name )const
	{
		return getControlStyle( name, m_comboBoxStyles );
	}

	EditStyleRPtr StylesHolder::getEditStyle( String const & name )const
	{
		return getControlStyle( name, m_editStyles );
	}

	ExpandablePanelStyleRPtr StylesHolder::getExpandablePanelStyle( String const & name )const
	{
		return getControlStyle( name, m_expandablePanelStyles );
	}

	FrameStyleRPtr StylesHolder::getFrameStyle( String const & name )const
	{
		return getControlStyle( name, m_frameStyles );
	}

	ListBoxStyleRPtr StylesHolder::getListBoxStyle( String const & name )const
	{
		return getControlStyle( name, m_listBoxStyles );
	}

	PanelStyleRPtr StylesHolder::getPanelStyle( String const & name )const
	{
		return getControlStyle( name, m_panelStyles );
	}

	ProgressStyleRPtr StylesHolder::getProgressStyle( String const & name )const
	{
		return getControlStyle( name, m_progressStyles );
	}

	ScrollBarStyleRPtr StylesHolder::getScrollBarStyle( String const & name )const
	{
		return getControlStyle( name, m_scrollBarStyles );
	}

	SliderStyleRPtr StylesHolder::getSliderStyle( String const & name )const
	{
		return getControlStyle( name, m_sliderStyles );
	}

	StaticStyleRPtr StylesHolder::getStaticStyle( String const & name )const
	{
		return getControlStyle( name, m_staticStyles );
	}

	//************************************************************************************************

	ControlStyleRPtr ButtonStyle::doCreate( String const & cloneName )const
	{
		return getEngine().getControlsManager()->createButtonStyle( cloneName, hasScene() ? &getScene() : nullptr );
	}

	ControlStyleRPtr ComboBoxStyle::doCreate( String const & cloneName )const
	{
		return getEngine().getControlsManager()->createComboBoxStyle( cloneName, hasScene() ? &getScene() : nullptr );
	}

	ControlStyleRPtr EditStyle::doCreate( String const & cloneName )const
	{
		return getEngine().getControlsManager()->createEditStyle( cloneName, hasScene() ? &getScene() : nullptr );
	}

	ControlStyleRPtr ExpandablePanelStyle::doCreate( String const & cloneName )const
	{
		return getEngine().getControlsManager()->createExpandablePanelStyle( cloneName, hasScene() ? &getScene() : nullptr );
	}

	ControlStyleRPtr FrameStyle::doCreate( String const & cloneName )const
	{
		return getEngine().getControlsManager()->createFrameStyle( cloneName, hasScene() ? &getScene() : nullptr );
	}

	ControlStyleRPtr ListBoxStyle::doCreate( String const & cloneName )const
	{
		return getEngine().getControlsManager()->createListBoxStyle( cloneName, hasScene() ? &getScene() : nullptr );
	}

	ControlStyleRPtr PanelStyle::doCreate( String const & cloneName )const
	{
		return getEngine().getControlsManager()->createPanelStyle( cloneName, hasScene() ? &getScene() : nullptr );
	}

	ControlStyleRPtr ProgressStyle::doCreate( String const & cloneName )const
	{
		return getEngine().getControlsManager()->createProgressStyle( cloneName, hasScene() ? &getScene() : nullptr );
	}

	ControlStyleRPtr ScrollBarStyle::doCreate( String const & cloneName )const
	{
		return getEngine().getControlsManager()->createScrollBarStyle( cloneName, hasScene() ? &getScene() : nullptr );
	}

	ControlStyleRPtr SliderStyle::doCreate( String const & cloneName )const
	{
		return getEngine().getControlsManager()->createSliderStyle( cloneName, hasScene() ? &getScene() : nullptr );
	}

	ControlStyleRPtr StaticStyle::doCreate( String const & cloneName )const
	{
		return getEngine().getControlsManager()->createStaticStyle( cloneName, hasScene() ? &getScene() : nullptr );
	}

	void ScrollableStyle::copyScrollableInto( ScrollableStyle & copy )const
	{
		if ( m_verticalStyle )
			copy.setVerticalStyle( static_cast< ScrollBarStyle & >( *m_verticalStyle->clone( m_verticalStyle->getName() + cuT( "Clone" ) ) ) );
		if ( m_horizontalStyle )
			copy.setHorizontalStyle( static_cast< ScrollBarStyle & >( *m_verticalStyle->clone( m_horizontalStyle->getName() + cuT( "Clone" ) ) ) );
	}
	//************************************************************************************************
}
