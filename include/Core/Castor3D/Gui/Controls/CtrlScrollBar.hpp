/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ScrollBarCtrl_H___
#define ___C3D_ScrollBarCtrl_H___

#include "Castor3D/Gui/Controls/CtrlButton.hpp"
#include "Castor3D/Gui/Controls/CtrlPanel.hpp"
#include "Castor3D/Gui/Theme/StyleScrollBar.hpp"

#include <CastorUtils/Math/RangedValue.hpp>

namespace castor3d
{
	class ScrollBarCtrl
		: public Control
	{
		friend class ScrollableCtrl;

	public:
		/** Constructor
		 *\param[in]	name	The control name
		 *\param[in]	style	The control style
		 *\param[in]	parent	The parent control, if any
		 *\param[in]	id		The control ID
		 */
		C3D_API ScrollBarCtrl( SceneRPtr scene
			, castor::String const & name
			, ScrollBarStyleRPtr style
			, ControlRPtr parent );

		/** Constructor
		 *\param[in]	name		The control name
		 *\param[in]	style		The control style
		 *\param[in]	parent		The parent control, if any
		 *\param[in]	caption		The caption
		 *\param[in]	id			The control ID
		 *\param[in]	position	The position
		 *\param[in]	size		The size
		 *\param[in]	flags		The configuration flags
		 *\param[in]	visible		Initial visibility status
		 */
		C3D_API ScrollBarCtrl( SceneRPtr scene
			, castor::String const & name
			, ScrollBarStyleRPtr style
			, ControlRPtr parent
			, castor::RangedValue< float > const & value
			, castor::Position const & position
			, castor::Size const & size
			, ControlFlagType flags = 0
			, bool visible = true );

		/** Sets the range
		*\param[in]	value		The new value
		*/
		C3D_API void setRange( castor::Range< uint32_t > const & value );

		/** Sets the thumb position, from external control.
		*\param[in]	value		The new value
		*/
		C3D_API void updateThumb( int32_t value );

		/** Connects a function to a button event
		 *\param[in]	event		The event type
		 *\param[in]	function	The function
		 *\return		The internal function index, to be able to disconnect it
		 */
		OnScrollBarEventConnection connect( ScrollBarEvent event
			, OnScrollBarEventFunction function )
		{
			return m_signals[size_t( event )].connect( function );
		}

		/**
		*\return	The scrollbar style.
		*/
		ScrollBarStyle const & getStyle()const
		{
			return static_cast< ScrollBarStyle const & >( getBaseStyle() );
		}

		C3D_API static ControlType constexpr Type{ ControlType::eScrollBar };

		int32_t getScrollPosition()const noexcept
		{
			return m_scrollPosition;
		}

		bool isVertical()const noexcept
		{
			return castor::checkFlag( getFlags(), ScrollBarFlag::eVertical );
		}

		bool isHorizontal()const noexcept
		{
			return castor::checkFlag( getFlags(), ScrollBarFlag::eHorizontal );
		}

	private:
		ScrollBarStyle & getStyle()
		{
			return static_cast< ScrollBarStyle & >( getBaseStyle() );
		}

		/** @copydoc Control::doCreate
		*/
		void doCreate()override;

		/** @copydoc Control::doDestroy
		*/
		void doDestroy()override;

		/** @copydoc Control::doSetPosition
		*/
		void doSetPosition( castor::Position const & value )override;

		/** @copydoc Control::doSetSize
		*/
		void doSetSize( castor::Size const & value )override;

		/** @copydoc Control::doUpdateStyle
		*/
		void doUpdateStyle()override;

		/** @copydoc Control::doSetCaption
		*/
		void doSetCaption( castor::U32String const & caption )override;

		/** @copydoc Control::doSetVisible
		*/
		void doSetVisible( bool visible )override;

		/** @copydoc Control::doUpdateFlags
		*/
		void doUpdateFlags()override;

		/** @copydoc Control::doOnMouseMove
		 */
		void doOnMouseMove( MouseEvent const & event )override;

		/** @copydoc Control::doOnMouseButtonUp
		*/
		void doOnMouseButtonUp( MouseEvent const & event )override;

		/** @copydoc Control::doOnMouseLeave
		*/
		void doOnMouseLeave( MouseEvent const & event )override;

		/** @copydoc Control::doUpdateZIndex
		*/
		void doUpdateZIndex( uint32_t & index )override;

		/** @copydoc Control::doAdjustZIndex
		*/
		void doAdjustZIndex( uint32_t offset )override;

		/** Event when mouse moves over the thumb control
		 *\param[in]	control	The thumb control
		 *\param[in]	event	The mouse event
		 */
		void onThumbMouseMove( ControlSPtr control
			, MouseEvent const & event );

		/** Event when mouse left button is released over the thumb control
		 *\param[in]	control	The thumb control
		 *\param[in]	event	The mouse event
		 */
		void onThumbMouseButtonDown( ControlSPtr control
			, MouseEvent const & event );

		/** Event when mouse left button is released over the thumb control
		 *\param[in]	control	The thumb control
		 *\param[in]	event	The mouse event
		 */
		void onThumbMouseButtonUp( ControlSPtr control
			, MouseEvent const & event );

		/** Event when a keyboard key is pressed
		 *\param[in]	event	The keyboard event
		 */
		void onKeyDown( KeyboardEvent const & event );

		/** Event when a keyboard key is pressed on the active thumb or bar control
		 *\param[in]	control	The control
		 *\param[in]	event	The keyboard event
		 */
		void onNcKeyDown( ControlSPtr control
			, KeyboardEvent const & event );

		/** Updates the mouse position
		 *\param[in]	mouse		The new mouse position
		 */
		void doMoveMouse( castor::Position const & mouse );

		/** Updates the thumb position
		 *\param[in]	delta		The position delta
		 */
		void doUpdateThumb( castor::Point2f const & delta );

		/** Updates the position and size of sub-controls.
		*/
		void doUpdatePosSize();

		/** Scrolls the thumb by \p v pixels.
		*/
		void doScroll( int32_t v );

	private:
		OnScrollBarEvent m_signals[size_t( ScrollBarEvent::eCount )];
		ButtonCtrlSPtr m_begin;
		PanelCtrlSPtr m_bar;
		PanelCtrlSPtr m_thumb;
		ButtonCtrlSPtr m_end;
		castor::RangedValue< float > m_value;
		castor::Range< uint32_t > m_totalRange;
		bool m_scrolling;
		castor::Point2i m_mouse;
		int32_t m_scrollPosition{};
		OnButtonEventConnection m_onBeginClick;
		OnButtonEventConnection m_onEndClick;
	};
}

#endif
