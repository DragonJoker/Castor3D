/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (At your option ) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___CI_CTRL_SLIDER_H___
#define ___CI_CTRL_SLIDER_H___

#include "CtrlControl.hpp"

namespace CastorGui
{
	typedef std::pair< int, int > Range;
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
		/** Constructor
		 *\param[in]	p_engine	The engine
		 *\param[in]	p_parent	The parent control, if any
		 *\param[in]	p_id		The control ID
		 */
		SliderCtrl( Castor3D::Engine * p_engine, ControlRPtr p_parent, uint32_t p_id );

		/** Constructor
		 *\param[in]	p_engine	The engine
		 *\param[in]	p_parent	The parent control, if any
		 *\param[in]	p_range		The slider min and max value
		 *\param[in]	p_value		The slider initial value
		 *\param[in]	p_id		The control ID
		 *\param[in]	p_position	The position
		 *\param[in]	p_size		The size
		 *\param[in]	p_style		The style
		 *\param[in]	p_visible	Initial visibility status
		 */
		SliderCtrl( Castor3D::Engine * p_engine, ControlRPtr p_parent, uint32_t p_id, Range const & p_range, int p_value, Castor::Position const & p_position, Castor::Size const & p_size, uint32_t p_style = 0, bool p_visible = true );

		/** Destructor
		*/
		virtual ~SliderCtrl();

		/** Sets the range
		*\param[in]	p_value		The new value
		*/
		void SetRange( Range const & p_value );

		/** Sets the caption
		*\param[in]	p_value		The new value
		*/
		void SetValue( int32_t p_value );

		/** Retrieves the range
		 *\return		The value
		*/
		inline Range const & GetRange()const
		{
			return m_range;
		}

		/** Retrieves the caption
		 *\return		The valu
		*/
		inline int32_t GetValue()const
		{
			return m_value;
		}

		/** Connects a function to a slider event
		*\param[in]	p_event		The event type
		*\param[in]	p_function		The function
		 *\return		The internal function index, to be able to disconnect it
		*/
		inline uint32_t Connect( eSLIDER_EVENT p_event, std::function< void( int ) > p_function )
		{
			return m_signals[p_event].connect( p_function );
		}

		/** Disconnects a function from a slider event
		*\param[in]	p_event		The event type
		*\param[in]	p_index		The function index
		*/
		inline void Disconnect( eSLIDER_EVENT p_event, uint32_t p_index )
		{
			m_signals[p_event].disconnect( p_index );
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
		Range m_range;
		//! The slider current value
		int32_t m_value;
		//! Tells the tick is moving
		bool m_scrolling;
		//! The previous mouse position
		Castor::Position m_mouse;
		//! The panel overlay used to display the line
		StaticCtrlWPtr m_line;
		//! The static used to display the line
		StaticCtrlWPtr m_tick;
		//! The slider events signals
		Castor::Signal< std::function< void( int ) > > m_signals[eSLIDER_EVENT_COUNT];
	};
}

#endif
