#include "Castor3D/Gui/Controls/CtrlLayoutControl.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"
#include "Castor3D/Gui/ControlsManager.hpp"
#include "Castor3D/Gui/Layout/Layout.hpp"
#include "Castor3D/Gui/Theme/StyleScrollable.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Overlay/BorderPanelOverlay.hpp"
#include "Castor3D/Overlay/TextOverlay.hpp"
#include "Castor3D/Scene/Scene.hpp"

CU_ImplementSmartPtr( castor3d, LayoutControl )

namespace castor3d
{
	//************************************************************************************************

	bool isLayoutControl( ControlType type )
	{
		return type == ControlType::ePanel;
	}

	bool isLayoutControl( Control const & control )
	{
		return isLayoutControl( control.getType() );
	}

	//************************************************************************************************

	LayoutControl::LayoutControl( ControlType type
		, SceneRPtr scene
		, castor::String const & name
		, ControlStyleRPtr controlStyle
		, ScrollableStyleRPtr scrollableStyle
		, ControlRPtr parent
		, castor::Position const & position
		, castor::Size const & size
		, ControlFlagType flags
		, bool visible )
		: Control{ type
			, scene
			, name
			, controlStyle
			, parent
			, position
			, size
			, flags
			, visible }
		, ScrollableCtrl{ *this
			, scrollableStyle }
	{
		doUpdateFlags();
	}

	void LayoutControl::setLayout( LayoutUPtr layout )
	{
		m_layout = castor::move( layout );
	}

	void LayoutControl::doCreate()
	{
		createScrollBars();
	}

	void LayoutControl::doDestroy()
	{
		m_layout.reset();
		destroyScrollBars();
	}

	void LayoutControl::doAddChild( ControlRPtr control )
	{
		registerControl( *control );
	}

	void LayoutControl::doRemoveChild( ControlRPtr control )
	{
		unregisterControl( *control );
	}

	void LayoutControl::doUpdateStyle()
	{
		updateScrollBarsStyle();
	}

	void LayoutControl::doUpdateFlags()
	{
		checkScrollBarFlags();
	}

	void LayoutControl::doUpdateZIndex( uint32_t & index )
	{
		updateScrollZIndex( index );
	}

	void LayoutControl::doAdjustZIndex( uint32_t offset )
	{
		adjustScrollZIndex( offset );
	}

	castor::Point4ui LayoutControl::doUpdateClientRect( castor::Point4ui const & clientRect )
	{
		return doSubUpdateClientRect( updateScrollableClientRect( clientRect ) );
	}

	void LayoutControl::doSetBorderSize( castor::Point4ui const & value )
	{
		if ( m_layout )
		{
			m_layout->markDirty();
		}

		doSubSetBorderSize( value );
		updateScrollBars();
	}

	void LayoutControl::doSetPosition( castor::Position const & value )
	{
		if ( m_layout )
		{
			m_layout->markDirty();
		}

		doSubSetPosition( value );
		updateScrollBars();
	}

	void LayoutControl::doSetSize( castor::Size const & value )
	{
		if ( m_layout )
		{
			m_layout->markDirty();
		}

		doSubSetSize( value );
		updateScrollBars();
	}

	void LayoutControl::doSetCaption( castor::U32String const & caption )
	{
		if ( m_layout )
		{
			m_layout->markDirty();
		}

		doSubSetCaption( caption );
	}

	void LayoutControl::doSetVisible( bool value )
	{
		if ( m_layout )
		{
			m_layout->markDirty();
		}

		doSubSetVisible( value );
		setScrollBarsVisible( value );
	}

	//************************************************************************************************
}
