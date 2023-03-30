/*
See LICENSE file in root folder
*/
#ifndef ___C3D_EditCtrl_H___
#define ___C3D_EditCtrl_H___

#include "Castor3D/Gui/Controls/CtrlControl.hpp"
#include "Castor3D/Gui/Controls/CtrlScrollable.hpp"
#include "Castor3D/Gui/Theme/StyleEdit.hpp"

namespace castor3d
{
	class EditCtrl
		: public Control
		, public ScrollableCtrl
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

		/** Updates the caption
		 *\param[in]	value		The new value
		 */
		C3D_API void updateCaption( castor::String const & value );

		/** Retrieves the caption
		 *\return		The value
		 */
		castor::U32String const & getCaption()const
		{
			return m_caption;
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
		*\return	The edit style.
		*/
		EditStyle const & getStyle()const
		{
			return static_cast< EditStyle const & >( getBaseStyle() );
		}

		C3D_API static ControlType constexpr Type{ ControlType::eEdit };

	private:
		struct CaretIndices
		{
			uint32_t lineIndex{};
			uint32_t charIndex{};
			uint32_t captionIndex{};
			castor::U32String::const_iterator captionIt{};

			template< typename SizeT >
			void updateIndex( SizeT index
				, castor::U32String const & caption )
			{
				captionIndex = std::min( uint32_t( index ), uint32_t( caption.size() ) );
				captionIt = std::next( caption.begin(), captionIndex );
			}

			void incrementIndex()
			{
				++captionIndex;
				++captionIt;
			}

			void decrementIndex()
			{
				--captionIndex;
				--captionIt;
			}
		};
		struct Selection
		{
			CaretIndices begin{};
			CaretIndices end{};
		};

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

		/** @copydoc Control::doAddChild
		 */
		void doAddChild( ControlSPtr control )override;

		/** @copydoc Control::doUpdateClientRect
		*/
		castor::Point4ui doUpdateClientRect( castor::Point4ui const & clientRect )override;

		/** @copydoc Control::doSetPosition
		*/
		void doSetPosition( castor::Position const & value )override;

		/** @copydoc Control::doSetSize
		*/
		void doSetSize( castor::Size const & value )override;

		/** @copydoc Control::doSetBorderSize
		*/
		void doSetBorderSize( castor::Point4ui const & value )override;

		/** @copydoc Control::doUpdateStyle
		*/
		void doUpdateStyle()override;

		/** @copydoc Control::doSetCaption
		*/
		void doSetCaption( castor::U32String const & value )override;

		/** @copydoc Control::doSetVisible
		*/
		void doSetVisible( bool visible )override;

		/** @copydoc Control::doUpdateFlags
		*/
		void doUpdateFlags()override;

		/** @copydoc Control::doUpdateZIndex
		*/
		void doUpdateZIndex( uint32_t & index )override;

		/** @copydoc Control::doAdjustZIndex
		*/
		void doAdjustZIndex( uint32_t offset )override;

		/** Event when the control is activated
		 *\param[in]	event		The control event
		 */
		void onActivate( HandlerEvent const & event );

		/** Event when the control is deactivated
		 *\param[in]	event		The control event
		 */
		void onDeactivate( HandlerEvent const & event );

		/** @copydoc Control::doOnMouseMove
		*/
		void doOnMouseMove( MouseEvent const & event )override;

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
		void doDeleteCharAtCaret( bool isCtrlDown );

		/** Removes a character before caret index (backspace)
		 */
		void doDeleteCharBeforeCaret( bool isCtrlDown );

		/** Moves the caret to the left.
		 */
		void doMoveCaretLeft( bool isShiftDown
			, bool isCtrlDown );

		/** Moves the caret to the right.
		 */
		void doMoveCaretRight( bool isShiftDown
			, bool isCtrlDown );

		/** Moves the caret one line up.
		 */
		void doMoveCaretUp( bool isShiftDown
			, bool isCtrlDown );

		/** Moves the caret one line down.
		 */
		void doMoveCaretDown( bool isShiftDown
			, bool isCtrlDown );

		/** Moves the caret at the beginning of the line or text.
		 */
		void doMoveCaretHome( bool isShiftDown
			, bool isCtrlDown );

		/** Moves the caret at the end of the line or text.
		 */
		void doMoveCaretEnd( bool isShiftDown
			, bool isCtrlDown );

		/** Updates the input caret from given position.
		 */
		void doUpdateCaretPosition( castor::Position const & position
			, CaretIndices & indices );

		/** Updates the input caret indices.
		 */
		void doUpdate( CaretIndices & indices );

		/** Updates the input caret indices.
		 */
		void doUpdateCaretIndices();

		/** Updates the input caret.
		 */
		void doUpdateCaret();

		/** Updates the text metrics.
		 */
		void doUpdateMetrics();

		/** Updates the caption and text overlay
		 */
		void doUpdateCaption();

		/** Begins the selection.
		 */
		void doBeginSelection( CaretIndices begin );

		/** Updates the selection overlays.
		 */
		void doUpdateSelection();

		/** Clears selection.
		 */
		void doClearSelection();

		/** Normalises selection.
		 */
		Selection doGetNormalisedSelection();

		/** Deletes selected text.
		 */
		bool doDeleteSelection( bool isCtrlDown );

		/** Select all text.
		 */
		void doSelectAllText();

		/** Copies selected text.
		 */
		void doCopyText();

		/** Cuts selected text.
		 */
		void doCutText();

		/** Pastes text at current caret position.
		 */
		void doPasteText();

		/** Scrolls the text (if needed) to make the caret visible.
		 */
		void doAdjustTextPosition();

		void doScrollContent( castor::Position const & position );

		bool hasSelection()const noexcept
		{
			return m_hasSelection;
		}

	private:
		struct Caret : CaretIndices
		{
			bool visible{};
			PanelOverlayWPtr overlay;
		};

		castor::U32String m_caption;
		castor::TextMetrics m_metrics;
		bool m_active;
		TextOverlayWPtr m_text;
		Caret m_caret;
		OnEditEvent m_signals[size_t( EditEvent::eCount )];
		bool m_hasSelection{};
		bool m_isMouseSelecting{};
		Selection m_selection{};
		std::vector< PanelOverlayWPtr > m_selections{};
		OnScrollContentConnection m_onScrollContent;
		uint32_t m_panelsZIndex{};
	};
}

#endif
