/*
See LICENSE file in root folder
*/
#ifndef ___CI_CTRL_LIST_BOX_H___
#define ___CI_CTRL_LIST_BOX_H___

#include "CastorGui/Controls/CtrlControl.hpp"
#include "CastorGui/Theme/StyleListBox.hpp"

namespace CastorGui
{
	/**
	*\brief		ListBox control.
	*/
	class ListBoxCtrl
		: public Control
	{
	public:
		/** Constructor
		 *\param[in]	name	The control name
		 *\param[in]	style	The control style
		 *\param[in]	parent	The parent control, if any
		 *\param[in]	id		The control ID
		 */
		ListBoxCtrl( castor::String const & name
			, ListBoxStyle * style
			, ControlRPtr parent
			, uint32_t id );

		/** Constructor
		 *\param[in]	name		The control name
		 *\param[in]	style		The control style
		 *\param[in]	parent		The parent control, if any
		 *\param[in]	values		The values list
		 *\param[in]	selected	The selected value index (-1 for no selection)
		 *\param[in]	id			The control ID
		 *\param[in]	position	The position
		 *\param[in]	size		The size
		 *\param[in]	flags		The configuration flags
		 *\param[in]	visible		Initial visibility status
		 */
		ListBoxCtrl( castor::String const & name
			, ListBoxStyle * style
			, ControlRPtr parent
			, uint32_t id
			, castor::StringArray const & values
			, int selected
			, castor::Position const & position
			, castor::Size const & size
			, uint32_t flags = 0
			, bool visible = true );

		/** Constructor
		 *\param[in]	name		The control name
		 *\param[in]	style		The control style
		 *\param[in]	parent		The parent control, if any
		 *\param[in]	values		The values list
		 *\param[in]	selected	The selected value index (-1 for no selection)
		 *\param[in]	id			The control ID
		 *\param[in]	position	The position
		 *\param[in]	size		The size
		 *\param[in]	style		The style
		 *\param[in]	visible		Initial visibility status
		 */
		template< size_t N >
		ListBoxCtrl( castor::String const & name
			, ListBoxStyle * style
			, ControlRPtr parent
			, castor::String const( & values )[N]
			, int selected
			, uint32_t id
			, castor::Position const & position
			, castor::Size const & size
			, uint32_t flags = 0
			, bool visible = true )
			: Control{ ControlType::eListBox
				, name
				, style
				, parent
				, id
				, position
				, size
				, flags
				, visible }
			, m_values{ castor::StringArray( &values[0], &values[N] ) }
			, m_selected{ selected }
		{
		}

		/** Appends a new item
		 *\param[in]	value		The item
		 */
		void appendItem( castor::String  const & value );

		/** Removes an item
		 *\param[in]	index		The item index
		 */
		void removeItem( int index );

		/** sets an item text
		 *\param[in]	index		The item index
		 *\param[in]	text		The item text
		 */
		void setItemText( int index, castor::String const & text );

		/** Retrieves an item text
		 *\param[in]	index		The item index
		 *\return		The item text
		 */
		castor::String getItemText( int index );

		/** Clears the items
		 */
		void clear();

		/** sets the selected item
		 *\param[in]	index		The new value
		 */
		void setSelected( int index );

		/** Retrieves the items
		 *\return		The value
		 */
		castor::StringArray const & getItems()const
		{
			return m_values;
		}

		/** Retrieves the items count
		 *\return		The value
		 */
		uint32_t getItemCount()const
		{
			return uint32_t( m_values.size() );
		}

		/** Retrieves the selected item index
		 *\return		The value
		 */
		int getSelected()const
		{
			return m_selected;
		}

		/** Connects a function to a listbox event
		 *\param[in]	event		The event type
		 *\param[in]	function		The function
		 *\return		The internal function index, to be able to disconnect it
		 */
		OnListEventConnection connect( ListBoxEvent event, OnListEventFunction function )
		{
			return m_signals[size_t( event )].connect( function );
		}

		/**
		*\return	The listbox style
		*/
		ListBoxStyle const & getStyle()const
		{
			return static_cast< ListBoxStyle const & >( getBaseStyle() );
		}

		static ControlType constexpr Type{ ControlType::eListBox };

	private:
		ListBoxStyle & getStyle()
		{
			return static_cast< ListBoxStyle & >( getBaseStyle() );
		}
		/** Creates a sub-control
		 *\param[in]	value		The control label
		 *\return		The static control.
		 */
		StaticCtrlSPtr doCreateItemCtrl( castor::String const & value
			, uint32_t itemIndex );

		/** Creates a sub-control, and it's Castor3D counterpart.
		 *\param[in]	value		The control label
		 */
		void doCreateItem( castor::String const & value
			, uint32_t itemIndex );

		/** Recomputes the items positions, according to their position in the items array
		 */
		void doUpdateItems();

		/** @copydoc CastorGui::Control::doCreate
		 */
		void doCreate()override;

		/** @copydoc CastorGui::Control::doDestroy
		*/
		void doDestroy()override;

		/** @copydoc CastorGui::Control::doSetPosition
		 */
		void doSetPosition( castor::Position const & value )override;

		/** @copydoc CastorGui::Control::doSetSize
		 */
		void doSetSize( castor::Size const & value )override;

		/** @copydoc CastorGui::Control::doUpdateStyle
		*/
		void doUpdateStyle()override;

		/** @copydoc CastorGui::Control::doSetVisible
		 */
		void doSetVisible( bool visible )override;

		/** Event when mouse enters an item
		 *\param[in]	control	The item
		 *\param[in]	event		The mouse event
		 */
		void onItemMouseEnter( ControlSPtr control, castor3d::MouseEvent const & event );

		/** Event when mouse leaves an item
		 *\param[in]	control	The item
		 *\param[in]	event		The mouse event
		 */
		void onItemMouseLeave( ControlSPtr control, castor3d::MouseEvent const & event );

		/** Event when mouse left button is released on an item
		 *\param[in]	control	The item
		 *\param[in]	event		The mouse event
		 */
		void onItemMouseLButtonUp( ControlSPtr control, castor3d::MouseEvent const & event );

		/** Event when a keyboard key is pressed on the active tick or line control
		 *\param[in]	event		The keyboard event
		 */
		void onItemKeyDown( ControlSPtr control, castor3d::KeyboardEvent const & event );

		/** Event when a keyboard key is pressed
		 *\param[in]	event		The keyboard event
		 */
		void onKeyDown( castor3d::KeyboardEvent const & event );

		/** Common construction method.
		 */
		void doConstruct();

	private:
		castor::StringArray m_initialValues;
		castor::StringArray m_values;
		int m_selected;
		StaticCtrlWPtr m_selectedItem;
		std::vector< StaticCtrlSPtr > m_items;
		OnListEvent m_signals[size_t( ListBoxEvent::eCount )];
	};
}

#endif
