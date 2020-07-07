/*
See LICENSE file in root folder
*/
#ifndef ___CI_CTRL_LIST_BOX_H___
#define ___CI_CTRL_LIST_BOX_H___

#include "CastorGui/CtrlControl.hpp"

namespace CastorGui
{
	/**
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
		 *\param[in]	parent	The parent control, if any
		 *\param[in]	id		The control ID
		 */
		ListBoxCtrl( castor::String const & name
			, castor3d::Engine & engine
			, ControlRPtr parent
			, uint32_t id );

		/** Constructor
		 *\param[in]	engine		The engine
		 *\param[in]	parent		The parent control, if any
		 *\param[in]	values		The values list
		 *\param[in]	selected		The selected value index (-1 for no selection)
		 *\param[in]	id			The control ID
		 *\param[in]	position		The position
		 *\param[in]	size			The size
		 *\param[in]	style			The style
		 *\param[in]	visible		Initial visibility status
		 */
		ListBoxCtrl( castor::String const & name
			, castor3d::Engine & engine
			, ControlRPtr parent
			, uint32_t id
			, castor::StringArray const & values
			, int selected
			, castor::Position const & position
			, castor::Size const & size
			, uint32_t style = 0
			, bool visible = true );

		/** Constructor
		 *\param[in]	parent		The parent control, if any
		 *\param[in]	values		The values list
		 *\param[in]	selected		The selected value index (-1 for no selection)
		 *\param[in]	id			The control ID
		 *\param[in]	position		The position
		 *\param[in]	size			The size
		 *\param[in]	style			The style
		 *\param[in]	visible		Initial visibility status
		 */
		template< size_t N >
		ListBoxCtrl( castor::String const & name
			, castor3d::Engine & engine
			, ControlRPtr parent
			, castor::String const( & values )[N]
			, int selected
			, uint32_t id
			, castor::Position const & position
			, castor::Size const & size
			, uint32_t style = 0
			, bool visible = true )
			: Control( ControlType::eListBox, name, engine, parent, id, position, size, style, visible )
			, m_values( castor::StringArray( &values[0], &values[N] ) )
			, m_selected( selected )
		{
		}

		/** Destructor
		 */
		virtual ~ListBoxCtrl();

		/** sets the text material.
		 *\param[in]	p_material	The new value.
		 */
		void setTextMaterial( castor3d::MaterialSPtr p_material );

		/** sets the background colour for selected item
		 *\param[in]	colour		The new value
		 */
		void setSelectedItemBackgroundMaterial( castor3d::MaterialSPtr colour );

		/** sets the foreground colour for selected item
		 *\param[in]	colour		The new value
		 */
		void setSelectedItemForegroundMaterial( castor3d::MaterialSPtr colour );

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

		/** sets the caption font.
		*\param[in]	font	The new value.
		*/
		void setFont( castor::String const & font );

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

		/**
		*\return	The text material
		*/
		inline castor3d::MaterialSPtr getTextMaterial()const
		{
			return m_textMaterial.lock();
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
		 *\param[in]	material	The new value
		 */
		inline void setHighlightedItemBackgroundMaterial( castor3d::MaterialSPtr material )
		{
			m_highlightedItemBackgroundMaterial = material;
		}

		/** sets the background colour for an unselected item
		 *\param[in]	material	The new value
		 */
		inline void setItemBackgroundMaterial( castor3d::MaterialSPtr material )
		{
			m_itemBackgroundMaterial = material;
		}

		/** Connects a function to a listbox event
		 *\param[in]	event		The event type
		 *\param[in]	function		The function
		 *\return		The internal function index, to be able to disconnect it
		 */
		inline OnListEventConnection connect( ListBoxEvent event, OnListEventFunction function )
		{
			return m_signals[size_t( event )].connect( function );
		}

	private:
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
		virtual void doCreate();

		/** @copydoc CastorGui::Control::doDestroy
		*/
		virtual void doDestroy();

		/** @copydoc CastorGui::Control::doSetPosition
		 */
		virtual void doSetPosition( castor::Position const & value );

		/** @copydoc CastorGui::Control::doSetSize
		 */
		virtual void doSetSize( castor::Size const & value );

		/** @copydoc CastorGui::Control::doSetBackgroundMaterial
		 */
		virtual void doSetBackgroundMaterial( castor3d::MaterialSPtr material );

		/** @copydoc CastorGui::Control::doSetForegroundMaterial
		 */
		virtual void doSetForegroundMaterial( castor3d::MaterialSPtr material );

		/** @copydoc CastorGui::Control::doSetVisible
		 */
		virtual void doSetVisible( bool visible );

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
		//! The text material.
		castor3d::MaterialWPtr m_textMaterial;
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
