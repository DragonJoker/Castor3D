/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___CI_CTRL_COMBO_BOX_H___
#define ___CI_CTRL_COMBO_BOX_H___

#include "CtrlButton.hpp"
#include "CtrlListBox.hpp"

namespace CastorGui
{
	/*!
	 *\author		Sylvain DOREMU
	 *\version		0.1.
	 *\brief		ComboBox contro
	*/
	class ComboBoxCtrl
		: public Control
	{
	public:
		using OnEventFunction = std::function< void( int ) >;
		using OnEvent = Castor::Signal< OnEventFunction >;
		using OnEventConnection = OnEvent::connection;

	public:
		/** Constructor
		 *\param[in]	p_engine	The engine
		 *\param[in]	p_parent	The parent control, if any
		 *\param[in]	p_id		The control ID
		 */
		ComboBoxCtrl( Castor3D::Engine * p_engine, ControlRPtr p_parent, uint32_t p_id );

		/** Constructor
		 *\param[in]	p_engine	The engine
		 *\param[in]	p_parent	The parent control, if any
		 *\param[in]	p_values	The list value
		 *\param[in]	p_selected	The selected value
		 *\param[in]	p_id		The control ID
		 *\param[in]	p_position	The position
		 *\param[in]	p_size		The size
		 *\param[in]	p_style		The style
		 *\param[in]	p_visible	Initial visibility status
		 */
		ComboBoxCtrl( Castor3D::Engine * p_engine, ControlRPtr p_parent, uint32_t p_id, Castor::StringArray const & p_values, int p_selected, Castor::Position const & p_position, Castor::Size const & p_size, uint32_t p_style = 0, bool p_visible = true );

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
		ComboBoxCtrl( ControlSPtr p_parent, Castor::String const( & p_values )[N], int p_selected, uint32_t p_id, Castor::Position const & p_position, Castor::Size const & p_size, uint32_t p_style = 0, bool p_visible = true )
			: Control( ControlType::eComboBox, p_parent, p_id, p_position, p_size, p_style, p_visible )
			, m_values( Castor::StringArray( &p_values[0], &p_values[N] ) )
			, m_selected( p_selected )
		{
		}

		/** Destructor
		*/
		virtual ~ComboBoxCtrl();

		/** Sets the background material for selected item
		 *\param[in]	p_material		The new value
		 */
		void SetSelectedItemBackgroundMaterial( Castor3D::MaterialSPtr p_material );

		/** Sets the foreground material for selected item
		 *\param[in]	p_material		The new value
		 */
		void SetSelectedItemForegroundMaterial( Castor3D::MaterialSPtr p_material );

		/** Sets the background material for an highlighted item
		 *\param[in]	p_material	The new value
		 */
		void SetHighlightedItemBackgroundMaterial( Castor3D::MaterialSPtr p_material );

		/** Sets the background material for an unselected item
		 *\param[in]	p_material	The new value
		 */
		void SetItemBackgroundMaterial( Castor3D::MaterialSPtr p_material );

		/** Retrieves the background material for selected item
		 *\return		The value
		 */
		Castor3D::MaterialSPtr GetSelectedItemBackgroundMaterial()const;

		/** Retrieves the foreground material for selected item
		 *\return		The value
		 */
		Castor3D::MaterialSPtr GetSelectedItemForegroundMaterial()const;

		/** Retrieves the background material for an highlighted item
		 *\return		The value
		 */
		Castor3D::MaterialSPtr GetHighlightedItemBackgroundMaterial()const;

		/** Retrieves the background material for an unselected item
		 *\return		The value
		 */
		Castor3D::MaterialSPtr GetItemBackgroundMaterial()const;

		/** Appends a new item
		*\param[in]	p_value		The item
		*/
		void AppendItem( Castor::String  const & p_value );

		/** Removes an item
		*\param[in]	p_value		The item index
		*/
		void RemoveItem( int p_value );

		/** Sets an item text
		*\param[in]	p_index		The item index
		*\param[in]	p_text		The item text
		*/
		void SetItemText( int p_index, Castor::String const & p_text );

		/** Clears the items
		*/
		void Clear();

		/** Sets the selected item
		*\param[in]	p_value		The new value
		*/
		void SetSelected( int p_value );

		/** Retrieves the items
		 *\return		The value
		*/
		Castor::StringArray const & GetItems()const;

		/** Retrieves the items count
		 *\return		The value
		*/
		uint32_t GetItemCount()const;

		/** Retrieves the selected item index
		 *\return		The value
		*/
		int GetSelected()const;

		/** Sets the caption font.
		*\param[in]	p_font	The new value.
		*/
		void SetFont( Castor::String const & p_font );

		/** Connects a function to a combobox event
		*\param[in]	p_event			The event type
		*\param[in]	p_function		The function
		 *\return		The internal function index, to be able to disconnect it
		*/
		inline OnEventConnection Connect( ComboBoxEvent p_event, OnEventFunction p_function )
		{
			return m_signals[size_t( p_event )].connect( p_function );
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

		/** @copydoc CastorGui::Control::DoCatchesMouseEvents
		*/
		virtual bool DoCatchesMouseEvents()const;

		/** @copydoc CastorGui::Control::DoSetVisible
		*/
		virtual void DoSetVisible( bool p_visible );

		/** Event raised when an item is selected in the choices listbox
		*\param[in]	p_selected		The item index
		*/
		void OnSelected( int p_selected );

		/** Event when a keyboard key is pressed
		*\param[in]	p_event		The keyboard event
		*/
		void OnKeyDown( Castor3D::KeyboardEvent const & p_event );

		/** Event when a keyboard key is pressed on the active tick or line control
		*\param[in]	p_event		The keyboard event
		*/
		void OnNcKeyDown( ControlSPtr p_control, Castor3D::KeyboardEvent const & p_event );

		/** Switch the combobox list
		*/
		void DoSwitchExpand();

	private:
		//! The text overlay used to display the caption
		Castor3D::TextOverlayWPtr m_text;
		//! The button used to expand the combo box
		ButtonCtrlSPtr m_expand;
		//! The list used to hold the combo box choices
		ListBoxCtrlSPtr m_choices;
		//! The combo box string choices
		Castor::StringArray m_values;
		//! Teh selected value index
		int m_selected;
		//! The combobox events signals
		OnEvent m_signals[size_t( ComboBoxEvent::eCount )];
		//| The connection to the mouse clicked event for the expand button.
		ButtonCtrl::OnEventConnection m_expandClickedConnection;
		//| The connection to the selected item of the choices list.
		ListBoxCtrl::OnEventConnection m_choicesSelectedConnection;
	};
}

#endif
