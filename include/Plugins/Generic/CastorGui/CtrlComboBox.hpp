/*
See LICENSE file in root folder
*/
#ifndef ___CI_CTRL_COMBO_BOX_H___
#define ___CI_CTRL_COMBO_BOX_H___

#include "CastorGui/CtrlButton.hpp"
#include "CastorGui/CtrlListBox.hpp"

namespace CastorGui
{
	/*!
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
		 *\param[in]	p_parent	The parent control, if any
		 *\param[in]	p_id		The control ID
		 */
		ComboBoxCtrl( castor::String const & p_name
			, castor3d::Engine & engine
			, ControlRPtr p_parent
			, uint32_t p_id );

		/** Constructor
		 *\param[in]	engine	The engine
		 *\param[in]	p_parent	The parent control, if any
		 *\param[in]	p_values	The list value
		 *\param[in]	p_selected	The selected value
		 *\param[in]	p_id		The control ID
		 *\param[in]	p_position	The position
		 *\param[in]	p_size		The size
		 *\param[in]	p_style		The style
		 *\param[in]	p_visible	Initial visibility status
		 */
		ComboBoxCtrl( castor::String const & p_name
			, castor3d::Engine & engine
			, ControlRPtr p_parent
			, uint32_t p_id
			, castor::StringArray const & p_values
			, int p_selected
			, castor::Position const & p_position
			, castor::Size const & p_size
			, uint32_t p_style = 0
			, bool p_visible = true );

		/** Constructor
		*\param[in]	p_parent		The parent control, if an
		*\param[in]	p_values		The list value
		*\param[in]	p_selected		The selected valu
		*\param[in]	p_id		The I
		*\param[in]	p_position		The positio
		*\param[in]	p_size		The siz
		*\param[in]	p_style		The styl
		*\param[in]	p_visible		Initial visibility statu
		*/
		template< size_t N >
		ComboBoxCtrl( ControlSPtr p_parent
			, castor::String const( & p_values )[N]
			, int p_selected
			, uint32_t p_id
			, castor::Position const & p_position
			, castor::Size const & p_size
			, uint32_t p_style = 0
			, bool p_visible = true )
			: Control( ControlType::eComboBox, p_parent, p_id, p_position, p_size, p_style, p_visible )
			, m_values( castor::StringArray( &p_values[0], &p_values[N] ) )
			, m_selected( p_selected )
		{
		}

		/** Destructor
		*/
		virtual ~ComboBoxCtrl();

		/** sets the background material for selected item
		 *\param[in]	p_material		The new value
		 */
		void setSelectedItemBackgroundMaterial( castor3d::MaterialSPtr p_material );

		/** sets the foreground material for selected item
		 *\param[in]	p_material		The new value
		 */
		void setSelectedItemForegroundMaterial( castor3d::MaterialSPtr p_material );

		/** sets the background material for an highlighted item
		 *\param[in]	p_material	The new value
		 */
		void setHighlightedItemBackgroundMaterial( castor3d::MaterialSPtr p_material );

		/** sets the background material for an unselected item
		 *\param[in]	p_material	The new value
		 */
		void setItemBackgroundMaterial( castor3d::MaterialSPtr p_material );

		/** Retrieves the background material for selected item
		 *\return		The value
		 */
		castor3d::MaterialSPtr getSelectedItemBackgroundMaterial()const;

		/** Retrieves the foreground material for selected item
		 *\return		The value
		 */
		castor3d::MaterialSPtr getSelectedItemForegroundMaterial()const;

		/** Retrieves the background material for an highlighted item
		 *\return		The value
		 */
		castor3d::MaterialSPtr getHighlightedItemBackgroundMaterial()const;

		/** Retrieves the background material for an unselected item
		 *\return		The value
		 */
		castor3d::MaterialSPtr getItemBackgroundMaterial()const;

		/** Appends a new item
		*\param[in]	p_value		The item
		*/
		void appendItem( castor::String  const & p_value );

		/** Removes an item
		*\param[in]	p_value		The item index
		*/
		void removeItem( int p_value );

		/** sets an item text
		*\param[in]	p_index		The item index
		*\param[in]	p_text		The item text
		*/
		void setItemText( int p_index, castor::String const & p_text );

		/** Clears the items
		*/
		void clear();

		/** sets the selected item
		*\param[in]	p_value		The new value
		*/
		void setSelected( int p_value );

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
		*\param[in]	p_font	The new value.
		*/
		void setFont( castor::String const & p_font );

		/** Connects a function to a combobox event
		*\param[in]	p_event			The event type
		*\param[in]	p_function		The function
		 *\return		The internal function index, to be able to disconnect it
		*/
		inline OnComboEventConnection connect( ComboBoxEvent p_event, OnComboEventFunction p_function )
		{
			return m_signals[size_t( p_event )].connect( p_function );
		}

	private:
		/** @copydoc CastorGui::Control::doCreate
		*/
		virtual void doCreate();

		/** @copydoc CastorGui::Control::doDestroy
		*/
		virtual void doDestroy();

		/** @copydoc CastorGui::Control::doSetPosition
		*/
		virtual void doSetPosition( castor::Position const & p_value );

		/** @copydoc CastorGui::Control::doSetSize
		*/
		virtual void doSetSize( castor::Size const & p_value );

		/** @copydoc CastorGui::Control::doSetBackgroundMaterial
		*/
		virtual void doSetBackgroundMaterial( castor3d::MaterialSPtr p_material );

		/** @copydoc CastorGui::Control::doSetForegroundMaterial
		*/
		virtual void doSetForegroundMaterial( castor3d::MaterialSPtr p_material );

		/** @copydoc CastorGui::Control::doCatchesMouseEvents
		*/
		virtual bool doCatchesMouseEvents()const;

		/** @copydoc CastorGui::Control::doSetVisible
		*/
		virtual void doSetVisible( bool p_visible );

		/** Event raised when an item is selected in the choices listbox
		*\param[in]	p_selected		The item index
		*/
		void onSelected( int p_selected );

		/** Event when a keyboard key is pressed
		*\param[in]	p_event		The keyboard event
		*/
		void onKeyDown( castor3d::KeyboardEvent const & p_event );

		/** Event when a keyboard key is pressed on the active tick or line control
		*\param[in]	p_event		The keyboard event
		*/
		void onNcKeyDown( ControlSPtr p_control, castor3d::KeyboardEvent const & p_event );

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
