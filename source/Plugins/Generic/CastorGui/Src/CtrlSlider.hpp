/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___CI_CTRL_SLIDER_H___
#define ___CI_CTRL_SLIDER_H___

#include "CtrlControl.hpp"

#include <Math/RangedValue.hpp>

namespace CastorGui
{
	/*!
	 *\author		Sylvain DOREMU
	 *\date		16/02/201
	 *\version		0.1.
	 *\brief		Slider contro
	*/
	class SliderCtrl
		: public Control
	{
	public:
		using OnEventFunction = std::function< void( int ) >;
		using OnEvent = Castor::Signal< OnEventFunction >;
		using OnEventConnection = OnEvent::connection;

	public:
		/** Constructor
		 *\param[in]	p_engine	The engine
		 *\param[in]	p_parent	The parent control, if any
		 *\param[in]	p_id		The control ID
		 */
		SliderCtrl( Castor3D::Engine * p_engine, ControlRPtr p_parent, uint32_t p_id );

		/** Constructor
		 *\param[in]	p_engine	The engine
		 *\param[in]	p_parent	The parent control, if any
		 *\param[in]	p_value		The slider initial value, and its range
		 *\param[in]	p_id		The control ID
		 *\param[in]	p_position	The position
		 *\param[in]	p_size		The size
		 *\param[in]	p_style		The style
		 *\param[in]	p_visible	Initial visibility status
		 */
		SliderCtrl( Castor3D::Engine * p_engine
			, ControlRPtr p_parent
			, uint32_t p_id
			, Castor::RangedValue< int32_t > const & p_value
			, Castor::Position const & p_position
			, Castor::Size const & p_size
			, uint32_t p_style = 0
			, bool p_visible = true );

		/** Destructor
		*/
		virtual ~SliderCtrl();

		/** Sets the range
		*\param[in]	p_value		The new value
		*/
		void SetRange( Castor::Range< int32_t > const & p_value );

		/** Sets the caption
		*\param[in]	p_value		The new value
		*/
		void SetValue( int32_t p_value );

		/** Retrieves the range
		 *\return		The value
		*/
		inline Castor::Range< int32_t > const & GetRange()const
		{
			return m_value.range();
		}

		/** Retrieves the caption
		 *\return		The valu
		*/
		inline int32_t GetValue()const
		{
			return m_value.value();
		}

		/** Connects a function to a slider event
		*\param[in]	p_event		The event type
		*\param[in]	p_function		The function
		 *\return		The internal function index, to be able to disconnect it
		*/
		inline OnEventConnection Connect( SliderEvent p_event, OnEventFunction p_function )
		{
			return m_signals[size_t( p_event )].connect( p_function );
		}

	private:
		/** Updates the line and tick position and size
		*/
		void DoUpdateLineAndTick();

		/** @copydoc CastorGui::Control::DoCreate
		*/
		virtual void DoCreate();

		/** @copydoc CastorGui::Control::DoDestroy
		*/
		virtual void DoDestroy();

		/** @copydoc CastorGui::Control::DoSetPosition
		*/
		virtual void DoSetPosition( Castor::Position const & p_value );

		/** @copydoc CastorGui::Control::DoSetSize
		*/
		virtual void DoSetSize( Castor::Size const & p_value );

		/** @copydoc CastorGui::Control::DoSetBackgroundMaterial
		*/
		virtual void DoSetBackgroundMaterial( Castor3D::MaterialSPtr p_material );

		/** @copydoc CastorGui::Control::DoSetForegroundMaterial
		*/
		virtual void DoSetForegroundMaterial( Castor3D::MaterialSPtr p_material );

		/** Sets the visibility
		 *\remarks		Used for derived control specific behaviou
		 *\param[in]	p_value		The new value
		 */
		virtual void DoSetVisible( bool p_visible );

		/** @copydoc CastorGui::Control::DoUpdateStyle
		*/
		virtual void DoUpdateStyle();

		/** Event when mouse moves over the control
		 *\param[in]	p_event		The mouse event
		 */
		void OnMouseMove( Castor3D::MouseEvent const & p_event );

		/** Event when mouse leaves the control
		 *\param[in]	p_event		The mouse event
		 */
		void OnMouseLeave( Castor3D::MouseEvent const & p_event );

		/** Event when mouse left button is released over the control
		 *\param[in]	p_event		The mouse event
		 */
		void OnMouseLButtonUp( Castor3D::MouseEvent const & p_event );

		/** Event when mouse moves over the tick control
		 *\param[in]	p_control	The tick control
		 *\param[in]	p_event		The mouse event
		 */
		void OnTickMouseMove( ControlSPtr p_control, Castor3D::MouseEvent const & p_event );

		/** Event when mouse left button is released over the tick control
		 *\param[in]	p_control	The tick control
		 *\param[in]	p_event		The mouse event
		 */
		void OnTickMouseLButtonDown( ControlSPtr p_control, Castor3D::MouseEvent const & p_event );

		/** Event when mouse left button is released over the tick control
		 *\param[in]	p_control	The tick control
		 *\param[in]	p_event		The mouse event
		 */
		void OnTickMouseLButtonUp( ControlSPtr p_control, Castor3D::MouseEvent const & p_event );

		/** Event when a keyboard key is pressed
		 *\param[in]	p_event		The keyboard event
		 */
		void OnKeyDown( Castor3D::KeyboardEvent const & p_event );

		/** Event when a keyboard key is pressed on the active tick or line control
		 *\param[in]	p_event		The keyboard event
		 */
		void OnNcKeyDown( ControlSPtr p_control, Castor3D::KeyboardEvent const & p_event );

		/** Updates the tick position
		 *\param[in]	p_delta		The position delta
		 */
		void DoUpdateTick( Castor::Position const & p_delta );

		/** Updates the mouse position
		 *\param[in]	p_mouse		The new mouse position
		 */
		void DoMoveMouse( Castor::Position const & p_mouse );

	private:
		//! The slider range
		Castor::RangedValue< int32_t > m_value;
		//! Tells the tick is moving
		bool m_scrolling;
		//! The previous mouse position
		Castor::Position m_mouse;
		//! The panel overlay used to display the line
		StaticCtrlWPtr m_line;
		//! The static used to display the line
		StaticCtrlWPtr m_tick;
		//! The slider events signals
		OnEvent m_signals[size_t( SliderEvent::eCount )];
	};
}

#endif
