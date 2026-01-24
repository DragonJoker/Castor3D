#include "Castor3D/Gui/Controls/CtrlLayoutControl.hpp"

#include "Castor3D/Gui/Layout/Layout.hpp"

CU_ImplementSmartPtr( c3d, LayoutControl )

namespace c3d
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
		, String const & name
		, ControlStyleRPtr controlStyle
		, ScrollableStyleRPtr scrollableStyle
		, ControlRPtr parent
		, Position const & position
		, Size const & size
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
		checkScrollBarFlags();
	}

	void LayoutControl::setLayout( LayoutUPtr layout )
	{
		m_layout = c3d::move( layout );
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

	Point4ui LayoutControl::doUpdateClientRect( Point4ui const & clientRect )
	{
		return doSubUpdateClientRect( updateScrollableClientRect( clientRect ) );
	}

	void LayoutControl::doSetBorderSize( Point4ui const & value )
	{
		if ( m_layout )
		{
			m_layout->markDirty();
		}

		doSubSetBorderSize( value );
		updateScrollBars();
	}

	void LayoutControl::doSetPosition( Position const & value )
	{
		if ( m_layout )
		{
			m_layout->markDirty();
		}

		doSubSetPosition( value );
		updateScrollBars();
	}

	void LayoutControl::doSetSize( Size const & value )
	{
		if ( m_layout )
		{
			m_layout->markDirty();
		}

		doSubSetSize( value );
		updateScrollBars();
	}

	void LayoutControl::doSetCaption( U32String const & caption )
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
