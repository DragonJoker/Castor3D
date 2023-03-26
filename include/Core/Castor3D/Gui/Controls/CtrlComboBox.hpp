/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ComboBoxCtrl_H___
#define ___C3D_ComboBoxCtrl_H___

#include "Castor3D/Gui/Controls/CtrlButton.hpp"
#include "Castor3D/Gui/Controls/CtrlListBox.hpp"
#include "Castor3D/Gui/Theme/StyleComboBox.hpp"

namespace castor3d
{
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
		C3D_API ComboBoxCtrl( SceneRPtr scene
			, castor::String const & name
			, ComboBoxStyle * style
			, ControlRPtr parent );

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
		C3D_API ComboBoxCtrl( SceneRPtr scene
			, castor::String const & name
			, ComboBoxStyle * style
			, ControlRPtr parent
			, castor::StringArray const & values
			, int selected
			, castor::Position const & position
			, castor::Size const & size
			, ControlFlagType flags = 0
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
		ComboBoxCtrl( SceneRPtr scene
			, castor::String const & name
			, ComboBoxStyle * style
			, ControlRPtr parent
			, castor::String const( & values )[N]
			, int selected
			, castor::Position const & position
			, castor::Size const & size
			, ControlFlagType flags = 0
			, bool visible = true )
			: Control{ ControlType::eComboBox
				, scene
				, name
				, style
				, parent
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
		C3D_API void appendItem( castor::String  const & value );

		/** Removes an item
		*\param[in]	value		The item index
		*/
		C3D_API void removeItem( int value );

		/** sets an item text
		*\param[in]	index		The item index
		*\param[in]	text		The item text
		*/
		C3D_API void setItemText( int index, castor::String const & text );

		/** Clears the items
		*/
		C3D_API void clear();

		/** sets the selected item
		*\param[in]	value		The new value
		*/
		C3D_API void setSelected( int value );

		/** Retrieves the items
		 *\return		The value
		*/
		C3D_API castor::StringArray const & getItems()const;

		/** Retrieves the items count
		 *\return		The value
		*/
		C3D_API uint32_t getItemCount()const;

		/** Retrieves the selected item index
		 *\return		The value
		*/
		C3D_API int getSelected()const;

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
		*\return	The combobox style.
		*/
		ComboBoxStyle const & getStyle()const
		{
			return static_cast< ComboBoxStyle const & >( getBaseStyle() );
		}

		C3D_API static ControlType constexpr Type{ ControlType::eComboBox };

	private:
		ComboBoxStyle & getStyle()
		{
			return static_cast< ComboBoxStyle & >( getBaseStyle() );
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

		/** @copydoc Control::doSetBorderSize
		*/
		void doSetBorderSize( castor::Point4ui const & value )override;

		/** @copydoc Control::doUpdateStyle
		*/
		void doUpdateStyle()override;

		/** @copydoc Control::doCatchesMouseEvents
		*/
		bool doCatchesMouseEvents()const override;

		/** @copydoc Control::doSetVisible
		*/
		void doSetVisible( bool visible )override;

		/** @copydoc Control::doUpdateZIndex
		*/
		void doUpdateZIndex( uint32_t & index )override;

		/** @copydoc Control::doAdjustZIndex
		*/
		void doAdjustZIndex( uint32_t offset )override;

		/** Event raised when an item is selected in the choices listbox
		*\param[in]	selected		The item index
		*/
		void onSelected( int selected );

		/** Event when a keyboard key is pressed
		*\param[in]	event		The keyboard event
		*/
		void onKeyDown( KeyboardEvent const & event );

		/** Event when a keyboard key is pressed on the active tick or line control
		*\param[in]	event		The keyboard event
		*/
		void onNcKeyDown( ControlSPtr control, KeyboardEvent const & event );

		/** Switch the combobox list
		*/
		void doSwitchExpand();

	private:
		TextOverlayWPtr m_text;
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
