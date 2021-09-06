/*
See LICENSE file in root folder
*/
#ifndef ___CI_CTRL_EDIT_H___
#define ___CI_CTRL_EDIT_H___

#include "CastorGui/CtrlControl.hpp"

namespace CastorGui
{
	/**
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
		 *\param[in]	engine	The engine
		 *\param[in]	p_parent	The parent control, if any
		 *\param[in]	p_id		The control ID
		 */
		EditCtrl( castor::String const & p_name
			, castor3d::Engine & engine
			, ControlRPtr p_parent
			, uint32_t p_id );

		/** Constructor
		 *\param[in]	engine	The engine
		 *\param[in]	p_parent	The parent control, if any
		 *\param[in]	p_caption	The caption
		 *\param[in]	p_id		The control ID
		 *\param[in]	p_position	The position
		 *\param[in]	p_size		The size
		 *\param[in]	p_style		The style
		 *\param[in]	p_visible	Initial visibility status
		 */
		EditCtrl( castor::String const & p_name
			, castor3d::Engine & engine
			, ControlRPtr p_parent
			, uint32_t p_id
			, castor::String const & p_caption
			, castor::Position const & p_position
			, castor::Size const & p_size
			, uint32_t p_style = 0
			, bool p_visible = true );

		/** Destructor
		 */
		virtual ~EditCtrl();

		/** sets the caption font.
		*\param[in]	p_font	The new value.
		*/
		void setFont( castor::String const & p_font );

		/** Sets the text material.
		*\param[in]	value	The new value.
		*/
		void setTextMaterial( castor3d::MaterialRPtr value );

		/** Retrieves the caption
		 *\return		The value
		 */
		inline castor::String const & getCaption()const
		{
			return m_caption;
		}

		/** Updates the caption
		 *\param[in]	p_value		The new value
		 */
		inline void updateCaption( castor::String const & p_value )
		{
			m_caption = p_value;
		}

		/** Connects a function to an edit event
		 *\param[in]	p_event		The event type
		 *\param[in]	p_function	The function
		 *\return		The internal function index, to be able to disconnect it
		 */
		inline OnEditEventConnection connect( EditEvent p_event, OnEditEventFunction p_function )
		{
			return m_signals[size_t( p_event )].connect( p_function );
		}

		/** Retreves the multiline status of the edit.
		 *\return		The value.
		 */
		bool isMultiLine()const
		{
			return castor::checkFlag( getStyle(), EditStyle::eMultiline );
		}

		/**
		*\return	The text material
		*/
		inline castor3d::MaterialRPtr getTextMaterial()const
		{
			return m_textMaterial;
		}

	private:
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

		/** @copydoc CastorGui::Control::doSetCaption
		*/
		void doSetCaption( castor::String const & p_value )override;

		/** @copydoc CastorGui::Control::doSetVisible
		*/
		void doSetVisible( bool p_visible )override;

		/** @copydoc CastorGui::Control::doUpdateStyle
		*/
		void doUpdateStyle()override;

		/** Event when the control is activated
		 *\param[in]	p_event		The control event
		 */
		void onActivate( castor3d::HandlerEvent const & p_event );

		/** Event when the control is deactivated
		 *\param[in]	p_event		The control event
		 */
		void onDeactivate( castor3d::HandlerEvent const & p_event );

		/** Event when mouse left button is pushed
		 *\param[in]	p_event		The mouse event
		 */
		void onMouseLButtonDown( castor3d::MouseEvent const & p_event );

		/** Event when mouse left button is released
		 *\param[in]	p_event		The mouse event
		 */
		void onMouseLButtonUp( castor3d::MouseEvent const & p_event );

		/** Event when a printable key is pressed
		 *\param[in]	p_event		The mouse event
		 */
		void onChar( castor3d::KeyboardEvent const & p_event );

		/** Event when a keyboard key is pressed
		 *\param[in]	p_event		The mouse event
		 */
		void onKeyDown( castor3d::KeyboardEvent const & p_event );

		/** Event when a keyboard key is pressed
		 *\param[in]	p_event		The mouse event
		 */
		void onKeyUp( castor3d::KeyboardEvent const & p_event );

		/** adds a character at caret index
		 */
		void doAddCharAtCaret( castor::String const & p_char );

		/** Removes a character at caret index
		 */
		void doDeleteCharAtCaret();

		/** Removes a character before caret index (backspace)
		 */
		void doDeleteCharBeforeCaret();

		/** Updates the caption and text overlay
		 */
		void doUpdateCaption();

		/** Retrieves the caption with caret
		 *\return		The caption and the caret at good position
		 */
		castor::String doGetCaptionWithCaret()const;

	private:
		//! The caption
		castor::String m_caption;
		//! The font material
		castor3d::MaterialRPtr m_textMaterial;
		//! The caret index in the caption
		castor::string::utf8::const_iterator m_caretIt;
		//! The activation status
		bool m_active;
		//! The text overlay used to display the caption
		castor3d::TextOverlayWPtr m_text;
		//! The edit events signals
		OnEditEvent m_signals[size_t( EditEvent::eCount )];
	};
}

#endif
