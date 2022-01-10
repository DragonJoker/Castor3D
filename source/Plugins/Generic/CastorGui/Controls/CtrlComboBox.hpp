/*
See LICENSE file in root folder
*/
#ifndef ___CI_CTRL_COMBO_BOX_H___
#define ___CI_CTRL_COMBO_BOX_H___

#include "CastorGui/Controls/CtrlButton.hpp"
#include "CastorGui/Controls/CtrlListBox.hpp"
#include "CastorGui/Theme/StyleComboBox.hpp"

namespace CastorGui
{
	/**
	 *\brief		ComboBox control.
	*/
	class ComboBoxCtrl
		: public Control
	{
	public:
		/** Constructor
		 *\param[in]	name	The control name
		 *\param[in]	style	The control style
		 *\param[in]	parent	The parent control, if any
		 *\param[in]	id		The control ID
		 */
		ComboBoxCtrl( castor::String const & name
			, ComboBoxStyle * style
			, ControlRPtr parent
			, uint32_t id );

		/** Constructor
		 *\param[in]	name		The control name
		 *\param[in]	style		The control style
		 *\param[in]	parent		The parent control, if any
		 *\param[in]	values		The list value
		 *\param[in]	selected	The selected value
		 *\param[in]	id			The control ID
		 *\param[in]	position	The position
		 *\param[in]	size		The size
		 *\param[in]	flags		The configuration flags
		 *\param[in]	visible		Initial visibility status
		 */
		ComboBoxCtrl( castor::String const & name
			, ComboBoxStyle * style
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
			, ComboBoxStyle * style
			, ControlRPtr parent
			, castor::String const( & values )[N]
			, int selected
			, uint32_t id
			, castor::Position const & position
			, castor::Size const & size
			, uint32_t flags = 0
			, bool visible = true )
			: Control{ ControlType::eComboBox
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

		/** Connects a function to a combobox event
		*\param[in]	event			The event type
		*\param[in]	function		The function
		 *\return		The internal function index, to be able to disconnect it
		*/
		OnComboEventConnection connect( ComboBoxEvent event
			, OnComboEventFunction function )
		{
			return m_signals[size_t( event )].connect( function );
		}

		/**
		*\return	The combobox style
		*/
		ComboBoxStyle const & getStyle()const
		{
			return static_cast< ComboBoxStyle const & >( getBaseStyle() );
		}

		static ControlType constexpr Type{ ControlType::eComboBox };

	private:
		ComboBoxStyle & getStyle()
		{
			return static_cast< ComboBoxStyle & >( getBaseStyle() );
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
		castor3d::TextOverlayWPtr m_text;
		ButtonCtrlSPtr m_expand;
		ListBoxCtrlSPtr m_choices;
		castor::StringArray m_values;
		int m_selected;
		OnComboEvent m_signals[size_t( ComboBoxEvent::eCount )];
		OnButtonEventConnection m_expandClickedConnection;
		OnListEventConnection m_choicesSelectedConnection;
	};
}

#endif
