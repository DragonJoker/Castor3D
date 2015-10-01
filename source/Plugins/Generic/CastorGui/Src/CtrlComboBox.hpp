/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (At your option ) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___CI_CTRL_COMBO_BOX_H___
#define ___CI_CTRL_COMBO_BOX_H___

#include "CtrlControl.hpp"

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
		/** Constructor
		*\param[in]	p_parent		The parent control, if an
		*\param[in]	p_id		The control ID
		*/
		ComboBoxCtrl( ControlSPtr p_parent, uint32_t p_id );

		/** Constructor
		*\param[in]	p_parent		The parent control, if an
		*\param[in]	p_values		The list value
		*\param[in]	p_selected		The selected valu
		*\param[in]	p_id		The control I
		*\param[in]	p_position		The positio
		*\param[in]	p_size		The siz
		*\param[in]	p_style		The styl
		*\param[in]	p_visible		Initial visibility statu
		*/
		ComboBoxCtrl( ControlSPtr p_parent, uint32_t p_id, Castor::StringArray const & p_values, int p_selected, Castor::Position const & p_position, Castor::Size const & p_size, uint32_t p_style = 0, bool p_visible = true );

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
			: Control( eCONTROL_TYPE_COMBO, p_parent, p_id, p_position, p_size, p_style, p_visible )
			, m_values( Castor::StringArray( &p_values[0], &p_values[N] ) )
			, m_selected( p_selected )
		{
		}

		/** Destructor
		*/
		virtual ~ComboBoxCtrl();

		/** Appends a new item
		*\param[in]	p_value		The ite
		*/
		void AppendItem( Castor::String  const & p_value );

		/** Removes an item
		*\param[in]	p_value		The item inde
		*/
		void RemoveItem( int p_value );

		/** Sets an item text
		*\param[in]	p_index		The item inde
		*\param[in]	p_text		The item tex
		*/
		void SetItemText( int p_index, Castor::String const & p_text );

		/** Clears the items
		*/
		void Clear();

		/** Sets the selected item
		*\param[in]	p_value		The new valu
		*/
		void SetSelected( int p_value );

		/** Retrieves the items
		 *\return		The valu
		*/
		Castor::StringArray const & GetItems()const;

		/** Retrieves the items count
		 *\return		The valu
		*/
		uint32_t GetItemCount()const;

		/** Retrieves the selected item index
		 *\return		The valu
		*/
		int GetSelected()const;

		/** Connects a function to a combobox event
		*\param[in]	p_event		The event typ
		*\param[in]	p_function		The functio
		 *\return		The internal function index, to be able to disconnect i
		*/
		inline uint32_t Connect( eCOMBOBOX_EVENT p_event, std::function< void( int ) > p_function )
		{
			return m_signals[p_event].Connect( p_function );
		}

		/** Disconnects a function from a combobox event
		*\param[in]	p_event		The event typ
		*\param[in]	p_index		The function inde
		*/
		inline void Disconnect( eCOMBOBOX_EVENT p_event, uint32_t p_index )
		{
			m_signals[p_event].Disconnect( p_index );
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
		*\param[in]	p_selected		The item inde
		*/
		void OnSelected( int p_selected );

		/** Event when a keyboard key is pressed
		*\param[in]	p_event		The keyboard even
		*/
		void OnKeyDown( KeyboardEvent const & p_event );

		/** Event when a keyboard key is pressed on the active tick or line control
		*\param[in]	p_event		The keyboard even
		*/
		void OnNcKeyDown( ControlSPtr p_control, KeyboardEvent const & p_event );

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
		Signal< std::function< void( int ) > > m_signals[eCOMBOBOX_EVENT_COUNT];
	};
}

#endif
