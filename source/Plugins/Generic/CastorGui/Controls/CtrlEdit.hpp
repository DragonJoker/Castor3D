/*
See LICENSE file in root folder
*/
#ifndef ___CI_CTRL_EDIT_H___
#define ___CI_CTRL_EDIT_H___

#include "CastorGui/Controls/CtrlControl.hpp"
#include "CastorGui/Theme/StyleEdit.hpp"

namespace CastorGui
{
	/**
	\brief		Edit control
	*/
	class EditCtrl
		: public Control
	{
	public:
		/** Constructor
		 *\param[in]	name	The control name
		 *\param[in]	style	The control style
		 *\param[in]	parent	The parent control, if any
		 *\param[in]	id		The control ID
		 */
		EditCtrl( castor::String const & name
			, EditStyleRPtr style
			, ControlRPtr parent
			, uint32_t id );

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
		EditCtrl( castor::String const & name
			, EditStyleRPtr style
			, ControlRPtr parent
			, uint32_t id
			, castor::String const & caption
			, castor::Position const & position
			, castor::Size const & size
			, uint32_t flags = 0
			, bool visible = true );

		/** Retrieves the caption
		 *\return		The value
		 */
		castor::String const & getCaption()const
		{
			return m_caption;
		}

		/** Updates the caption
		 *\param[in]	p_value		The new value
		 */
		void updateCaption( castor::String const & value )
		{
			m_caption = value;
		}

		/** Connects a function to an edit event
		 *\param[in]	p_event		The event type
		 *\param[in]	p_function	The function
		 *\return		The internal function index, to be able to disconnect it
		 */
		OnEditEventConnection connect( EditEvent event
			, OnEditEventFunction function )
		{
			return m_signals[size_t( event )].connect( function );
		}

		/** Retreves the multiline status of the edit.
		 *\return		The value.
		 */
		bool isMultiLine()const
		{
			return castor::checkFlag( getFlags(), EditFlag::eMultiline );
		}

		/**
		*\return	The static style
		*/
		EditStyle const & getStyle()const
		{
			return static_cast< EditStyle const & >( getBaseStyle() );
		}

		static ControlType constexpr Type{ ControlType::eEdit };

	private:
		EditStyle & getStyle()
		{
			return static_cast< EditStyle & >( getBaseStyle() );
		}
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

		/** @copydoc CastorGui::Control::doUpdateStyle
		*/
		void doUpdateStyle()override;

		/** @copydoc CastorGui::Control::doSetCaption
		*/
		void doSetCaption( castor::String const & p_value )override;

		/** @copydoc CastorGui::Control::doSetVisible
		*/
		void doSetVisible( bool p_visible )override;

		/** @copydoc CastorGui::Control::doUpdateFlags
		*/
		void doUpdateFlags()override;

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
		castor::String m_caption;
		castor::string::utf8::const_iterator m_caretIt;
		bool m_active;
		castor3d::TextOverlayWPtr m_text;
		OnEditEvent m_signals[size_t( EditEvent::eCount )];
	};
}

#endif
