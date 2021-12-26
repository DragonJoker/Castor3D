/*
See LICENSE file in root folder
*/
#ifndef ___CI_CTRL_SLIDER_H___
#define ___CI_CTRL_SLIDER_H___

#include "CtrlControl.hpp"

#include <CastorUtils/Math/RangedValue.hpp>

namespace CastorGui
{
	/**
	 *\author		Sylvain DOREMU
	 *\date		16/02/201
	 *\version		0.1.
	 *\brief		Slider contro
	*/
	class SliderCtrl
		: public Control
	{
	public:
		/** Constructor
		 *\param[in]	engine		The engine
		 *\param[in]	p_parent	The parent control, if any
		 *\param[in]	p_id		The control ID
		 */
		SliderCtrl( castor::String const & p_name
			, castor3d::Engine & engine
			, ControlRPtr p_parent
			, uint32_t p_id );

		/** Constructor
		 *\param[in]	engine		The engine
		 *\param[in]	p_parent	The parent control, if any
		 *\param[in]	p_value		The slider initial value, and its range
		 *\param[in]	p_id		The control ID
		 *\param[in]	p_position	The position
		 *\param[in]	p_size		The size
		 *\param[in]	p_flags		The configuration flags
		 *\param[in]	p_visible	Initial visibility status
		 */
		SliderCtrl( castor::String const & p_name
			, castor3d::Engine & engine
			, ControlRPtr p_parent
			, uint32_t p_id
			, castor::RangedValue< int32_t > const & p_value
			, castor::Position const & p_position
			, castor::Size const & p_size
			, uint32_t p_flags = 0
			, bool p_visible = true );

		/** sets the range
		*\param[in]	p_value		The new value
		*/
		void setRange( castor::Range< int32_t > const & p_value );

		/** sets the caption
		*\param[in]	p_value		The new value
		*/
		void setValue( int32_t p_value );

		/** Retrieves the range
		 *\return		The value
		*/
		inline castor::Range< int32_t > const & getRange()const
		{
			return m_value.range();
		}

		/** Retrieves the caption
		 *\return		The valu
		*/
		inline int32_t getValue()const
		{
			return m_value.value();
		}

		/** Connects a function to a slider event
		*\param[in]	p_event		The event type
		*\param[in]	p_function		The function
		 *\return		The internal function index, to be able to disconnect it
		*/
		inline OnSliderEventConnection connect( SliderEvent p_event, OnSliderEventFunction p_function )
		{
			return m_signals[size_t( p_event )].connect( p_function );
		}

	private:
		/** Updates the line and tick position and size
		*/
		void doUpdateLineAndTick();

		/** @copydoc CastorGui::Control::doCreate
		*/
		void doCreate()override;

		/** @copydoc CastorGui::Control::doDestroy
		*/
		void doDestroy()override;

		/** @copydoc CastorGui::Control::doSetPosition
		*/
		void doSetPosition( castor::Position const & p_value )override;

		/** @copydoc CastorGui::Control::doSetSize
		*/
		void doSetSize( castor::Size const & p_value )override;

		/** @copydoc CastorGui::Control::doSetBackgroundMaterial
		*/
		void doSetBackgroundMaterial( castor3d::MaterialRPtr p_material )override;

		/** @copydoc CastorGui::Control::doSetForegroundMaterial
		*/
		void doSetForegroundMaterial( castor3d::MaterialRPtr p_material )override;

		/** sets the visibility
		 *\remarks		Used for derived control specific behaviou
		 *\param[in]	p_value		The new value
		 */
		void doSetVisible( bool p_visible )override;

		/** @copydoc CastorGui::Control::doUpdateFlags
		*/
		void doUpdateFlags()override;

		/** Event when mouse moves over the control
		 *\param[in]	p_event		The mouse event
		 */
		void onMouseMove( castor3d::MouseEvent const & p_event );

		/** Event when mouse leaves the control
		 *\param[in]	p_event		The mouse event
		 */
		void onMouseLeave( castor3d::MouseEvent const & p_event );

		/** Event when mouse left button is released over the control
		 *\param[in]	p_event		The mouse event
		 */
		void onMouseLButtonUp( castor3d::MouseEvent const & p_event );

		/** Event when mouse moves over the tick control
		 *\param[in]	p_control	The tick control
		 *\param[in]	p_event		The mouse event
		 */
		void onTickMouseMove( ControlSPtr p_control, castor3d::MouseEvent const & p_event );

		/** Event when mouse left button is released over the tick control
		 *\param[in]	p_control	The tick control
		 *\param[in]	p_event		The mouse event
		 */
		void onTickMouseLButtonDown( ControlSPtr p_control, castor3d::MouseEvent const & p_event );

		/** Event when mouse left button is released over the tick control
		 *\param[in]	p_control	The tick control
		 *\param[in]	p_event		The mouse event
		 */
		void onTickMouseLButtonUp( ControlSPtr p_control, castor3d::MouseEvent const & p_event );

		/** Event when a keyboard key is pressed
		 *\param[in]	p_event		The keyboard event
		 */
		void onKeyDown( castor3d::KeyboardEvent const & p_event );

		/** Event when a keyboard key is pressed on the active tick or line control
		 *\param[in]	p_event		The keyboard event
		 */
		void onNcKeyDown( ControlSPtr p_control, castor3d::KeyboardEvent const & p_event );

		/** Updates the tick position
		 *\param[in]	p_delta		The position delta
		 */
		void doUpdateTick( castor::Position const & p_delta );

		/** Updates the mouse position
		 *\param[in]	p_mouse		The new mouse position
		 */
		void doMoveMouse( castor::Position const & p_mouse );

	private:
		//! The slider range
		castor::RangedValue< int32_t > m_value;
		//! Tells the tick is moving
		bool m_scrolling;
		//! The previous mouse position
		castor::Position m_mouse;
		//! The panel overlay used to display the line
		StaticCtrlWPtr m_line;
		//! The static used to display the line
		StaticCtrlWPtr m_tick;
		//! The slider events signals
		OnSliderEvent m_signals[size_t( SliderEvent::eCount )];
	};
}

#endif
