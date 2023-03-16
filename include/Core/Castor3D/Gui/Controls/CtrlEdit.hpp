/*
See LICENSE file in root folder
*/
#ifndef ___C3D_EditCtrl_H___
#define ___C3D_EditCtrl_H___

#include "Castor3D/Gui/Controls/CtrlControl.hpp"
#include "Castor3D/Gui/Theme/StyleEdit.hpp"

namespace castor3d
{
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
		C3D_API EditCtrl( SceneRPtr scene
			, castor::String const & name
			, EditStyleRPtr style
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
		C3D_API EditCtrl( SceneRPtr scene
			, castor::String const & name
			, EditStyleRPtr style
			, ControlRPtr parent
			, castor::String const & caption
			, castor::Position const & position
			, castor::Size const & size
			, ControlFlagType flags = 0
			, bool visible = true );

		/** Retrieves the caption
		 *\return		The value
		 */
		castor::String const & getCaption()const
		{
			return m_caption;
		}

		/** Updates the caption
		 *\param[in]	value		The new value
		 */
		void updateCaption( castor::String const & value )
		{
			m_caption = value;
		}

		/** Connects a function to an edit event
		 *\param[in]	event		The event type
		 *\param[in]	function	The function
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

		C3D_API static ControlType constexpr Type{ ControlType::eEdit };

	private:
		EditStyle & getStyle()
		{
			return static_cast< EditStyle & >( getBaseStyle() );
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
		void doSetCaption( castor::String const & value )override;

		/** @copydoc Control::doSetVisible
		*/
		void doSetVisible( bool visible )override;

		/** @copydoc Control::doUpdateFlags
		*/
		void doUpdateFlags()override;

		/** @copydoc Control::doUpdateZIndex
		*/
		void doUpdateZIndex( uint32_t & index )override;

		/** Event when the control is activated
		 *\param[in]	event		The control event
		 */
		void onActivate( HandlerEvent const & event );

		/** Event when the control is deactivated
		 *\param[in]	event		The control event
		 */
		void onDeactivate( HandlerEvent const & event );

		/** @copydoc Control::doOnMouseButtonDown
		*/
		void doOnMouseButtonDown( MouseEvent const & event )override;

		/** @copydoc Control::doOnMouseButtonUp
		*/
		void doOnMouseButtonUp( MouseEvent const & event )override;

		/** Event when a printable key is pressed
		 *\param[in]	event		The mouse event
		 */
		void onChar( KeyboardEvent const & event );

		/** Event when a keyboard key is pressed
		 *\param[in]	event		The mouse event
		 */
		void onKeyDown( KeyboardEvent const & event );

		/** Event when a keyboard key is pressed
		 *\param[in]	event		The mouse event
		 */
		void onKeyUp( KeyboardEvent const & event );

		/** adds a character at caret index
		 */
		void doAddCharAtCaret( castor::String const & c );

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
		TextOverlayWPtr m_text;
		OnEditEvent m_signals[size_t( EditEvent::eCount )];
	};
}

#endif
