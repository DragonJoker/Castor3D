/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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
#ifndef ___CI_CTRL_LIST_BOX_H___
#define ___CI_CTRL_LIST_BOX_H___

#include "CtrlControl.hpp"

namespace CastorGui
{
	/*!
	*\author	Sylvain DOREMUS
	*\version	0.1.0
	*\brief		ListBox control
	*/
	class ListBoxCtrl
		: public Control
	{
	public:
		/** Constructor
		 *\param[in]	p_engine	The engine
		 *\param[in]	p_parent	The parent control, if any
		 *\param[in]	p_id		The control ID
		 */
		ListBoxCtrl( Castor3D::Engine * p_engine, ControlRPtr p_parent, uint32_t p_id );

		/** Constructor
		 *\param[in]	p_engine		The engine
		 *\param[in]	p_parent		The parent control, if any
		 *\param[in]	p_values		The values list
		 *\param[in]	p_selected		The selected value index (-1 for no selection)
		 *\param[in]	p_id			The control ID
		 *\param[in]	p_position		The position
		 *\param[in]	p_size			The size
		 *\param[in]	p_style			The style
		 *\param[in]	p_visible		Initial visibility status
		 */
		ListBoxCtrl( Castor3D::Engine * p_engine, ControlRPtr p_parent, uint32_t p_id, Castor::StringArray const & p_values, int p_selected, Castor::Position const & p_position, Castor::Size const & p_size, uint32_t p_style = 0, bool p_visible = true );

		/** Constructor
		 *\param[in]	p_parent		The parent control, if any
		 *\param[in]	p_values		The values list
		 *\param[in]	p_selected		The selected value index (-1 for no selection)
		 *\param[in]	p_id			The control ID
		 *\param[in]	p_position		The position
		 *\param[in]	p_size			The size
		 *\param[in]	p_style			The style
		 *\param[in]	p_visible		Initial visibility status
		 */
		template< size_t N >
		ListBoxCtrl( ControlSPtr p_parent, Castor::String const( & p_values )[N], int p_selected, uint32_t p_id, Castor::Position const & p_position, Castor::Size const & p_size, uint32_t p_style = 0, bool p_visible = true )
			: Control( eCONTROL_TYPE_LIST, p_parent, p_id, p_position, p_size, p_style, p_visible )
			, m_values( Castor::StringArray( &p_values[0], &p_values[N] ) )
			, m_selected( p_selected )
		{
		}

		/** Destructor
		 */
		virtual ~ListBoxCtrl();

		/** Sets the background colour for selected item
		 *\param[in]	p_colour		The new value
		 */
		void SetSelectedItemBackgroundMaterial( Castor3D::MaterialSPtr p_colour );

		/** Sets the foreground colour for selected item
		 *\param[in]	p_colour		The new value
		 */
		void SetSelectedItemForegroundMaterial( Castor3D::MaterialSPtr p_colour );

		/** Appends a new item
		 *\param[in]	p_value		The item
		 */
		void AppendItem( Castor::String  const & p_value );

		/** Removes an item
		 *\param[in]	p_index		The item index
		 */
		void RemoveItem( int p_index );

		/** Sets an item text
		 *\param[in]	p_index		The item index
		 *\param[in]	p_text		The item text
		 */
		void SetItemText( int p_index, Castor::String const & p_text );

		/** Retrieves an item text
		 *\param[in]	p_index		The item index
		 *\return		The item text
		 */
		Castor::String GetItemText( int p_index );

		/** Clears the items
		 */
		void Clear();

		/** Sets the selected item
		 *\param[in]	p_index		The new value
		 */
		void SetSelected( int p_index );

		/** Sets the caption font.
		*\param[in]	p_font	The new value.
		*/
		void SetFont( Castor::String const & p_font );

		/** Retrieves the items
		 *\return		The value
		 */
		inline Castor::StringArray const & GetItems()const
		{
			return m_values;
		}

		/** Retrieves the items count
		 *\return		The value
		 */
		inline uint32_t GetItemCount()const
		{
			return uint32_t( m_values.size() );
		}

		/** Retrieves the selected item index
		 *\return		The value
		 */
		inline int GetSelected()const
		{
			return m_selected;
		}

		/** Retrieves the background colour for selected item
		 *\return		The value
		 */
		inline Castor3D::MaterialSPtr GetSelectedItemBackgroundMaterial()const
		{
			return m_selectedItemBackgroundMaterial.lock();
		}

		/** Retrieves the foreground colour for selected item
		 *\return		The value
		 */
		inline Castor3D::MaterialSPtr GetSelectedItemForegroundMaterial()const
		{
			return m_selectedItemForegroundMaterial.lock();
		}

		/** Retrieves the background colour for an highlighted item
		 *\return		The value
		 */
		inline Castor3D::MaterialSPtr GetHighlightedItemBackgroundMaterial()const
		{
			return m_highlightedItemBackgroundMaterial.lock();
		}

		/** Retrieves the background colour for an unselected item
		 *\return		The value
		 */
		inline Castor3D::MaterialSPtr GetItemBackgroundMaterial()const
		{
			return m_itemBackgroundMaterial.lock();
		}

		/** Sets the background colour for an highlighted item
		 *\param[in]	p_material	The new value
		 */
		inline void SetHighlightedItemBackgroundMaterial( Castor3D::MaterialSPtr p_material )
		{
			m_highlightedItemBackgroundMaterial = p_material;
		}

		/** Sets the background colour for an unselected item
		 *\param[in]	p_material	The new value
		 */
		inline void SetItemBackgroundMaterial( Castor3D::MaterialSPtr p_material )
		{
			m_itemBackgroundMaterial = p_material;
		}

		/** Connects a function to a listbox event
		 *\param[in]	p_event		The event type
		 *\param[in]	p_function		The function
		 *\return		The internal function index, to be able to disconnect it
		 */
		inline uint32_t Connect( eLISTBOX_EVENT p_event, std::function< void( int ) > p_function )
		{
			return m_signals[p_event].connect( p_function );
		}

		/** Disconnects a function from a listbox event
		 *\param[in]	p_event		The event type
		 *\param[in]	p_index		The function index
		 */
		inline void Disconnect( eLISTBOX_EVENT p_event, uint32_t p_index )
		{
			m_signals[p_event].disconnect( p_index );
		}

	private:
		/** Creates a sub-control
		 *\param[in]	p_value		The control label
		 *\return		The static control.
		 */
		StaticCtrlSPtr DoCreateItemCtrl( Castor::String const & p_value );

		/** Creates a sub-control, and it's Castor3D counterpart.
		 *\param[in]	p_value		The control label
		 */
		void DoCreateItem( Castor::String const & p_value );

		/** Recomputes the items positions, according to their position in the items array
		 */
		void DoUpdateItems();

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

		/** @copydoc CastorGui::Control::DoSetVisible
		 */
		virtual void DoSetVisible( bool p_visible );

		/** Event when mouse enters an item
		 *\param[in]	p_control	The item
		 *\param[in]	p_event		The mouse event
		 */
		void OnItemMouseEnter( ControlSPtr p_control, Castor3D::MouseEvent const & p_event );

		/** Event when mouse leaves an item
		 *\param[in]	p_control	The item
		 *\param[in]	p_event		The mouse event
		 */
		void OnItemMouseLeave( ControlSPtr p_control, Castor3D::MouseEvent const & p_event );

		/** Event when mouse left button is released on an item
		 *\param[in]	p_control	The item
		 *\param[in]	p_event		The mouse event
		 */
		void OnItemMouseLButtonUp( ControlSPtr p_control, Castor3D::MouseEvent const & p_event );

		/** Event when a keyboard key is pressed on the active tick or line control
		 *\param[in]	p_event		The keyboard event
		 */
		void OnItemKeyDown( ControlSPtr p_control, Castor3D::KeyboardEvent const & p_event );

		/** Event when a keyboard key is pressed
		 *\param[in]	p_event		The keyboard event
		 */
		void OnKeyDown( Castor3D::KeyboardEvent const & p_event );

		/** Common construction method.
		 */
		void DoConstruct();

	private:
		//! The values given on construction.
		Castor::StringArray m_initialValues;
		//! All the combo box values
		Castor::StringArray m_values;
		//! The selected value
		int m_selected;
		//! The selected item
		StaticCtrlWPtr m_selectedItem;
		//! All the items.
		std::vector< StaticCtrlSPtr > m_items;
		//! The background colour, for normal item
		Castor3D::MaterialWPtr m_itemBackgroundMaterial;
		//! The background colour, for highlighted item
		Castor3D::MaterialWPtr m_highlightedItemBackgroundMaterial;
		//! The background colour
		Castor3D::MaterialWPtr m_selectedItemBackgroundMaterial;
		//! The foreground colour
		Castor3D::MaterialWPtr m_selectedItemForegroundMaterial;
		//! The listbox events signals
		Castor::Signal< std::function< void( int ) > > m_signals[eLISTBOX_EVENT_COUNT];
		//! The items font name.
		Castor::String m_fontName;
	};
}

#endif
