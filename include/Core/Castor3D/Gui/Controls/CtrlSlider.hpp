/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SliderCtrl_H___
#define ___C3D_SliderCtrl_H___

#include "Castor3D/Gui/Controls/CtrlControl.hpp"
#include "Castor3D/Gui/Theme/StyleSlider.hpp"

#include <CastorUtils/Math/RangedValue.hpp>

namespace castor3d
{
	class SliderCtrl
		: public Control
	{
	public:
		/** Constructor
		 *\param[in]	scene	The parent scene (nullptr for global).
		 *\param[in]	name	The control name
		 *\param[in]	style	The control style
		 *\param[in]	parent	The parent control, if any
		 */
		C3D_API SliderCtrl( SceneRPtr scene
			, castor::String const & name
			, SliderStyleRPtr style
			, ControlRPtr parent );

		/** Constructor
		 *\param[in]	scene		The parent scene (nullptr for global).
		 *\param[in]	name		The control name
		 *\param[in]	style		The control style
		 *\param[in]	parent		The parent control, if any
		 *\param[in]	value		The slider initial value, and its range
		 *\param[in]	position	The position
		 *\param[in]	size		The size
		 *\param[in]	flags		The configuration flags
		 *\param[in]	visible		Initial visibility status
		 */
		C3D_API SliderCtrl( SceneRPtr scene
			, castor::String const & name
			, SliderStyleRPtr style
			, ControlRPtr parent
			, castor::RangedValue< int32_t > const & value
			, castor::Position const & position
			, castor::Size const & size
			, ControlFlagType flags = 0
			, bool visible = true );

		C3D_API ~SliderCtrl()noexcept override;

		/** sets the range
		*\param[in]	value		The new value
		*/
		C3D_API void setRange( castor::Range< int32_t > const & value );

		/** sets the caption
		*\param[in]	value		The new value
		*/
		C3D_API void setValue( int32_t value );

		/** Retrieves the range
		 *\return		The value
		*/
		castor::Range< int32_t > const & getRange()const
		{
			return m_value.range();
		}

		/** Retrieves the caption
		 *\return		The valu
		*/
		int32_t getValue()const
		{
			return m_value.value();
		}

		/** Connects a function to a slider event
		 *\param[in]	event		The event type
		 *\param[in]	function	The function
		 *\return		The internal function index, to be able to disconnect it
		*/
		OnSliderEventConnection connect( SliderEvent event
			, OnSliderEventFunction function )
		{
			return m_signals[size_t( event )].connect( castor::move( function ) );
		}

		/**
		*\return	The slider style.
		*/
		SliderStyle const & getStyle()const
		{
			return static_cast< SliderStyle const & >( getBaseStyle() );
		}

		C3D_API static ControlType constexpr Type{ ControlType::eSlider };

	private:
		SliderStyle & getStyle()
		{
			return static_cast< SliderStyle & >( getBaseStyle() );
		}
		/** Updates the line and tick position and size
		*/
		void doUpdateLineAndTick();

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

		/** sets the visibility
		 *\remarks		Used for derived control specific behaviou
		 *\param[in]	visible		The new value
		 */
		void doSetVisible( bool visible )override;

		/** @copydoc Control::doUpdateFlags
		*/
		void doUpdateFlags()override;

		/** Event when mouse moves over the control
		 *\param[in]	event		The mouse event
		 */
		void doOnMouseMove( MouseEvent const & event )override;

		/** Event when mouse leaves the control
		 *\param[in]	event		The mouse event
		 */
		void doOnMouseLeave( MouseEvent const & event )override;

		/** Event when mouse left button is released over the control
		 *\param[in]	event		The mouse event
		 */
		void doOnMouseButtonUp( MouseEvent const & event )override;

		/** Event when mouse moves over the tick control
		 *\param[in]	event		The mouse event
		 */
		void onTickMouseMove( MouseEvent const & event );

		/** Event when mouse left button is released over the tick control
		 *\param[in]	event		The mouse event
		 */
		void onTickMouseButtonDown( MouseEvent const & event );

		/** Event when mouse left button is released over the tick control
		 *\param[in]	event	The mouse event
		 */
		void onTickMouseButtonUp( MouseEvent const & event );

		/** Event when a keyboard key is pressed
		 *\param[in]	event		The keyboard event
		 */
		void onKeyDown( KeyboardEvent const & event );

		/** Event when a keyboard key is pressed on the active tick or line control
		 *\param[in]	event	The keyboard event
		 */
		void onNcKeyDown( KeyboardEvent const & event );

		/** Updates the tick position
		 *\param[in]	delta		The position delta
		 */
		void doUpdateTick( castor::Position const & delta );

		/** Updates the mouse position
		 *\param[in]	mouse		The new mouse position
		 */
		void doMoveMouse( castor::Position const & mouse );

	private:
		castor::RangedValue< int32_t > m_value;
		bool m_scrolling{};
		castor::Position m_mouse{};
		StaticCtrlRPtr m_line{};
		StaticCtrlRPtr m_tick{};
		castor::Array< OnSliderEvent, size_t( SliderEvent::eCount ) > m_signals;
	};
}

#endif
