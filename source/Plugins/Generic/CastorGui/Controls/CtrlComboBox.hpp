/*
See LICENSE file in root folder
*/
#ifndef ___CI_CTRL_COMBO_BOX_H___
#define ___CI_CTRL_COMBO_BOX_H___

#include "CtrlButton.hpp"
#include "CtrlListBox.hpp"

namespace CastorGui
{
	/**
	 *\author		Sylvain DOREMUS
	 *\version		0.1.0
	 *\brief		ComboBox control.
	*/
	class ComboBoxCtrl
		: public Control
	{
	public:
		/** Constructor
		 *\param[in]	engine	The engine
		 *\param[in]	parent	The parent control, if any
		 *\param[in]	id		The control ID
		 */
		ComboBoxCtrl( castor::String const & name
			, castor3d::Engine & engine
			, ControlRPtr parent
			, uint32_t id );

		/** Constructor
		 *\param[in]	engine		The engine
		 *\param[in]	parent		The parent control, if any
		 *\param[in]	values		The list value
		 *\param[in]	selected	The selected value
		 *\param[in]	id			The control ID
		 *\param[in]	position	The position
		 *\param[in]	size		The size
		 *\param[in]	style		The style
		 *\param[in]	visible		Initial visibility status
		 */
		ComboBoxCtrl( castor::String const & name
			, castor3d::Engine & engine
			, ControlRPtr parent
			, uint32_t id
			, castor::StringArray const & values
			, int selected
			, castor::Position const & position
			, castor::Size const & size
			, uint32_t flags = 0
			, bool visible = true );

		/** Constructor
		*\param[in]	parent		The parent control, if an
		*\param[in]	values		The list value
		*\param[in]	selected	The selected valu
		*\param[in]	id			The control ID
		*\param[in]	position	The positio
		*\param[in]	size		The size
		*\param[in]	flags		The configuration flags
		*\param[in]	visible		Initial visibility statu
		*/
		template< size_t N >
		ComboBoxCtrl( castor::String const & name
			, castor3d::Engine & engine
			, ControlRPtr parent
			, castor::String const( & values )[N]
			, int selected
			, uint32_t id
			, castor::Position const & position
			, castor::Size const & size
			, uint32_t style = 0
			, bool visible = true )
			: Control( ControlType::eComboBox, name, engine, parent, id, position, size, style, visible )
			, m_values( castor::StringArray( &values[0], &values[N] ) )
			, m_selected( selected )
		{
		}

		/** sets the text material.
		 *\param[in]	p_material	The new value.
		 */
		void setTextMaterial( castor3d::MaterialRPtr p_material );

		/** sets the background material for selected item
		 *\param[in]	material		The new value
		 */
		void setSelectedItemBackgroundMaterial( castor3d::MaterialRPtr material );

		/** sets the foreground material for selected item
		 *\param[in]	material		The new value
		 */
		void setSelectedItemForegroundMaterial( castor3d::MaterialRPtr material );

		/** sets the background material for an highlighted item
		 *\param[in]	material	The new value
		 */
		void setHighlightedItemBackgroundMaterial( castor3d::MaterialRPtr material );

		/** sets the background material for an unselected item
		 *\param[in]	material	The new value
		 */
		void setItemBackgroundMaterial( castor3d::MaterialRPtr material );

		/** Retrieves the background material for selected item
		 *\return		The value
		 */
		castor3d::MaterialRPtr getSelectedItemBackgroundMaterial()const;

		/** Retrieves the foreground material for selected item
		 *\return		The value
		 */
		castor3d::MaterialRPtr getSelectedItemForegroundMaterial()const;

		/** Retrieves the background material for an highlighted item
		 *\return		The value
		 */
		castor3d::MaterialRPtr getHighlightedItemBackgroundMaterial()const;

		/** Retrieves the background material for an unselected item
		 *\return		The value
		 */
		castor3d::MaterialRPtr getItemBackgroundMaterial()const;

		/** Appends a new item
		*\param[in]	value		The item
		*/
		void appendItem( castor::String  const & value );

		/** Removes an item
		*\param[in]	value		The item index
		*/
		void removeItem( int value );

		/** sets an item text
		*\param[in]	index		The item index
		*\param[in]	text		The item text
		*/
		void setItemText( int index, castor::String const & text );

		/** Clears the items
		*/
		void clear();

		/** sets the selected item
		*\param[in]	value		The new value
		*/
		void setSelected( int value );

		/** Retrieves the items
		 *\return		The value
		*/
		castor::StringArray const & getItems()const;

		/** Retrieves the items count
		 *\return		The value
		*/
		uint32_t getItemCount()const;

		/** Retrieves the selected item index
		 *\return		The value
		*/
		int getSelected()const;

		/** sets the caption font.
		*\param[in]	font	The new value.
		*/
		void setFont( castor::String const & font );

		/**
		*\return	The text material
		*/
		inline castor3d::MaterialRPtr getTextMaterial()const
		{
			return m_expand->getTextMaterial();
		}

		/** Connects a function to a combobox event
		*\param[in]	event			The event type
		*\param[in]	function		The function
		 *\return		The internal function index, to be able to disconnect it
		*/
		inline OnComboEventConnection connect( ComboBoxEvent event, OnComboEventFunction function )
		{
			return m_signals[size_t( event )].connect( function );
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

		/** @copydoc CastorGui::Control::doCatchesMouseEvents
		*/
		bool doCatchesMouseEvents()const override;

		/** @copydoc CastorGui::Control::doSetVisible
		*/
		void doSetVisible( bool visible )override;

		/** Event raised when an item is selected in the choices listbox
		*\param[in]	selected		The item index
		*/
		void onSelected( int selected );

		/** Event when a keyboard key is pressed
		*\param[in]	event		The keyboard event
		*/
		void onKeyDown( castor3d::KeyboardEvent const & event );

		/** Event when a keyboard key is pressed on the active tick or line control
		*\param[in]	event		The keyboard event
		*/
		void onNcKeyDown( ControlSPtr control, castor3d::KeyboardEvent const & event );

		/** Switch the combobox list
		*/
		void doSwitchExpand();

	private:
		//! The text overlay used to display the caption
		castor3d::TextOverlayWPtr m_text;
		//! The button used to expand the combo box
		ButtonCtrlSPtr m_expand;
		//! The list used to hold the combo box choices
		ListBoxCtrlSPtr m_choices;
		//! The combo box string choices
		castor::StringArray m_values;
		//! Teh selected value index
		int m_selected;
		//! The combobox events signals
		OnComboEvent m_signals[size_t( ComboBoxEvent::eCount )];
		//| The connection to the mouse clicked event for the expand button.
		OnButtonEventConnection m_expandClickedConnection;
		//| The connection to the selected item of the choices list.
		OnListEventConnection m_choicesSelectedConnection;
	};
}

#endif
