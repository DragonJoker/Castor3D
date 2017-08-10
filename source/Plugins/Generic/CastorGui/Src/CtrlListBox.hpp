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
		 *\param[in]	engine	The engine
		 *\param[in]	p_parent	The parent control, if any
		 *\param[in]	p_id		The control ID
		 */
		ListBoxCtrl( castor::String const & p_name
			, castor3d::Engine & engine
			, ControlRPtr p_parent
			, uint32_t p_id );

		/** Constructor
		 *\param[in]	engine		The engine
		 *\param[in]	p_parent		The parent control, if any
		 *\param[in]	p_values		The values list
		 *\param[in]	p_selected		The selected value index (-1 for no selection)
		 *\param[in]	p_id			The control ID
		 *\param[in]	p_position		The position
		 *\param[in]	p_size			The size
		 *\param[in]	p_style			The style
		 *\param[in]	p_visible		Initial visibility status
		 */
		ListBoxCtrl( castor::String const & p_name
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
		ListBoxCtrl( ControlSPtr p_parent, castor::String const( & p_values )[N], int p_selected, uint32_t p_id, castor::Position const & p_position, castor::Size const & p_size, uint32_t p_style = 0, bool p_visible = true )
			: Control( ControlType::eListBox, p_parent, p_id, p_position, p_size, p_style, p_visible )
			, m_values( castor::StringArray( &p_values[0], &p_values[N] ) )
			, m_selected( p_selected )
		{
		}

		/** Destructor
		 */
		virtual ~ListBoxCtrl();

		/** sets the background colour for selected item
		 *\param[in]	p_colour		The new value
		 */
		void setSelectedItemBackgroundMaterial( castor3d::MaterialSPtr p_colour );

		/** sets the foreground colour for selected item
		 *\param[in]	p_colour		The new value
		 */
		void setSelectedItemForegroundMaterial( castor3d::MaterialSPtr p_colour );

		/** Appends a new item
		 *\param[in]	p_value		The item
		 */
		void appendItem( castor::String  const & p_value );

		/** Removes an item
		 *\param[in]	p_index		The item index
		 */
		void removeItem( int p_index );

		/** sets an item text
		 *\param[in]	p_index		The item index
		 *\param[in]	p_text		The item text
		 */
		void setItemText( int p_index, castor::String const & p_text );

		/** Retrieves an item text
		 *\param[in]	p_index		The item index
		 *\return		The item text
		 */
		castor::String getItemText( int p_index );

		/** Clears the items
		 */
		void clear();

		/** sets the selected item
		 *\param[in]	p_index		The new value
		 */
		void setSelected( int p_index );

		/** sets the caption font.
		*\param[in]	p_font	The new value.
		*/
		void setFont( castor::String const & p_font );

		/** Retrieves the items
		 *\return		The value
		 */
		inline castor::StringArray const & getItems()const
		{
			return m_values;
		}

		/** Retrieves the items count
		 *\return		The value
		 */
		inline uint32_t getItemCount()const
		{
			return uint32_t( m_values.size() );
		}

		/** Retrieves the selected item index
		 *\return		The value
		 */
		inline int getSelected()const
		{
			return m_selected;
		}

		/** Retrieves the background colour for selected item
		 *\return		The value
		 */
		inline castor3d::MaterialSPtr getSelectedItemBackgroundMaterial()const
		{
			return m_selectedItemBackgroundMaterial.lock();
		}

		/** Retrieves the foreground colour for selected item
		 *\return		The value
		 */
		inline castor3d::MaterialSPtr getSelectedItemForegroundMaterial()const
		{
			return m_selectedItemForegroundMaterial.lock();
		}

		/** Retrieves the background colour for an highlighted item
		 *\return		The value
		 */
		inline castor3d::MaterialSPtr getHighlightedItemBackgroundMaterial()const
		{
			return m_highlightedItemBackgroundMaterial.lock();
		}

		/** Retrieves the background colour for an unselected item
		 *\return		The value
		 */
		inline castor3d::MaterialSPtr getItemBackgroundMaterial()const
		{
			return m_itemBackgroundMaterial.lock();
		}

		/** sets the background colour for an highlighted item
		 *\param[in]	p_material	The new value
		 */
		inline void setHighlightedItemBackgroundMaterial( castor3d::MaterialSPtr p_material )
		{
			m_highlightedItemBackgroundMaterial = p_material;
		}

		/** sets the background colour for an unselected item
		 *\param[in]	p_material	The new value
		 */
		inline void setItemBackgroundMaterial( castor3d::MaterialSPtr p_material )
		{
			m_itemBackgroundMaterial = p_material;
		}

		/** Connects a function to a listbox event
		 *\param[in]	p_event		The event type
		 *\param[in]	p_function		The function
		 *\return		The internal function index, to be able to disconnect it
		 */
		inline OnListEventConnection connect( ListBoxEvent p_event, OnListEventFunction p_function )
		{
			return m_signals[size_t( p_event )].connect( p_function );
		}

	private:
		/** Creates a sub-control
		 *\param[in]	p_value		The control label
		 *\return		The static control.
		 */
		StaticCtrlSPtr doCreateItemCtrl( castor::String const & p_value );

		/** Creates a sub-control, and it's Castor3D counterpart.
		 *\param[in]	p_value		The control label
		 */
		void doCreateItem( castor::String const & p_value );

		/** Recomputes the items positions, according to their position in the items array
		 */
		void doUpdateItems();

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

		/** @copydoc CastorGui::Control::doSetVisible
		 */
		virtual void doSetVisible( bool p_visible );

		/** Event when mouse enters an item
		 *\param[in]	p_control	The item
		 *\param[in]	p_event		The mouse event
		 */
		void onItemMouseEnter( ControlSPtr p_control, castor3d::MouseEvent const & p_event );

		/** Event when mouse leaves an item
		 *\param[in]	p_control	The item
		 *\param[in]	p_event		The mouse event
		 */
		void onItemMouseLeave( ControlSPtr p_control, castor3d::MouseEvent const & p_event );

		/** Event when mouse left button is released on an item
		 *\param[in]	p_control	The item
		 *\param[in]	p_event		The mouse event
		 */
		void onItemMouseLButtonUp( ControlSPtr p_control, castor3d::MouseEvent const & p_event );

		/** Event when a keyboard key is pressed on the active tick or line control
		 *\param[in]	p_event		The keyboard event
		 */
		void onItemKeyDown( ControlSPtr p_control, castor3d::KeyboardEvent const & p_event );

		/** Event when a keyboard key is pressed
		 *\param[in]	p_event		The keyboard event
		 */
		void onKeyDown( castor3d::KeyboardEvent const & p_event );

		/** Common construction method.
		 */
		void doConstruct();

	private:
		//! The values given on construction.
		castor::StringArray m_initialValues;
		//! All the combo box values
		castor::StringArray m_values;
		//! The selected value
		int m_selected;
		//! The selected item
		StaticCtrlWPtr m_selectedItem;
		//! All the items.
		std::vector< StaticCtrlSPtr > m_items;
		//! The background colour, for normal item
		castor3d::MaterialWPtr m_itemBackgroundMaterial;
		//! The background colour, for highlighted item
		castor3d::MaterialWPtr m_highlightedItemBackgroundMaterial;
		//! The background colour
		castor3d::MaterialWPtr m_selectedItemBackgroundMaterial;
		//! The foreground colour
		castor3d::MaterialWPtr m_selectedItemForegroundMaterial;
		//! The listbox events signals
		OnListEvent m_signals[size_t( ListBoxEvent::eCount )];
		//! The items font name.
		castor::String m_fontName;
	};
}

#endif
