/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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
#ifndef ___CI_CTRL_EDIT_H___
#define ___CI_CTRL_EDIT_H___

#include "CtrlControl.hpp"

namespace CastorGui
{
	/*!
	\author		Sylvain DOREMUS
	\date		16/02/201
	\version	0.1.
	\brief		Edit control
	*/
	class EditCtrl
		: public Control
	{
	public:
		/** Constructor
		 *\param[in]	p_engine	The engine
		 *\param[in]	p_parent	The parent control, if any
		 *\param[in]	p_id		The control ID
		 */
		EditCtrl( Castor3D::Engine * p_engine, ControlRPtr p_parent, uint32_t p_id );

		/** Constructor
		 *\param[in]	p_engine	The engine
		 *\param[in]	p_parent	The parent control, if any
		 *\param[in]	p_caption	The caption
		 *\param[in]	p_id		The control ID
		 *\param[in]	p_position	The position
		 *\param[in]	p_size		The size
		 *\param[in]	p_style		The style
		 *\param[in]	p_visible	Initial visibility status
		 */
		EditCtrl( Castor3D::Engine * p_engine, ControlRPtr p_parent, uint32_t p_id, Castor::String const & p_caption, Castor::Position const & p_position, Castor::Size const & p_size, uint32_t p_style = 0, bool p_visible = true );

		/** Destructor
		 */
		virtual ~EditCtrl();

		/** Sets the caption
		 *\param[in]	p_value		The new value
		 */
		void SetCaption( Castor::String const & p_value );

		/** Sets the caption font.
		*\param[in]	p_font	The new value.
		*/
		void SetFont( Castor::String const & p_font );

		/** Retrieves the caption
		 *\return		The value
		 */
		inline Castor::String const & GetCaption()const
		{
			return m_caption;
		}

		/** Updates the caption
		 *\param[in]	p_value		The new value
		 */
		inline void UpdateCaption( Castor::String const & p_value )
		{
			m_caption = p_value;
		}

		/** Connects a function to an edit event
		 *\param[in]	p_event		The event type
		 *\param[in]	p_function	The function
		 *\return		The internal function index, to be able to disconnect it
		 */
		inline uint32_t Connect( eEDIT_EVENT p_event, std::function< void( Castor::String const & ) > p_function )
		{
			return m_signals[p_event].connect( p_function );
		}

		/** Disconnects a function from an edit event
		 *\param[in]	p_event		The event type
		 *\param[in]	p_index		The function index
		 */
		inline void Disconnect( eEDIT_EVENT p_event, uint32_t p_index )
		{
			m_signals[p_event].disconnect( p_index );
		}

		/** Retreves the multiline status of the edit.
		 *\return		The value.
		 */
		bool IsMultiLine()const
		{
			return ( GetStyle() & eEDIT_STYLE_MULTILINE ) == eEDIT_STYLE_MULTILINE;
		}

	private:
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

		/** @copydoc CastorGui::Control::DoSetVisible
		*/
		virtual void DoSetVisible( bool p_visible );

		/** @copydoc CastorGui::Control::DoUpdateStyle
		*/
		virtual void DoUpdateStyle();

		/** Event when the control is activated
		 *\param[in]	p_event		The control event
		 */
		void OnActivate( Castor3D::HandlerEvent const & p_event );

		/** Event when the control is deactivated
		 *\param[in]	p_event		The control event
		 */
		void OnDeactivate( Castor3D::HandlerEvent const & p_event );

		/** Event when mouse left button is pushed
		 *\param[in]	p_event		The mouse event
		 */
		void OnMouseLButtonDown( Castor3D::MouseEvent const & p_event );

		/** Event when mouse left button is released
		 *\param[in]	p_event		The mouse event
		 */
		void OnMouseLButtonUp( Castor3D::MouseEvent const & p_event );

		/** Event when a printable key is pressed
		 *\param[in]	p_event		The mouse event
		 */
		void OnChar( Castor3D::KeyboardEvent const & p_event );

		/** Event when a keyboard key is pressed
		 *\param[in]	p_event		The mouse event
		 */
		void OnKeyDown( Castor3D::KeyboardEvent const & p_event );

		/** Event when a keyboard key is pressed
		 *\param[in]	p_event		The mouse event
		 */
		void OnKeyUp( Castor3D::KeyboardEvent const & p_event );

		/** Adds a character at caret index
		 */
		void DoAddCharAtCaret( Castor::String const & p_char );

		/** Removes a character at caret index
		 */
		void DoDeleteCharAtCaret();

		/** Removes a character before caret index (backspace)
		 */
		void DoDeleteCharBeforeCaret();

		/** Updates the caption and text overlay
		 */
		void DoUpdateCaption();

		/** Retrieves the caption with caret
		 *\return		The caption and the caret at good position
		 */
		Castor::String DoGetCaptionWithCaret()const;

	private:
		//! The caption
		Castor::String m_caption;
		//! The caret index in the caption
		Castor::string::utf8::const_iterator m_caretIt;
		//! The activation status
		bool m_active;
		//! The text overlay used to display the caption
		Castor3D::TextOverlayWPtr m_text;
		//! The edit events signals
		Castor::Signal< std::function< void( Castor::String const & ) > > m_signals[eEDIT_EVENT_COUNT];
		//! Tells if the Edit is a multiline one.
		bool m_multiLine;
	};
}

#endif
