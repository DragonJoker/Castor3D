#include "Aria/Model/ListModel.hpp"

#include <CastorUtils/Log/Logger.hpp>

namespace aria
{
	static int my_sort_reverse( int * v1, int * v2 )
	{
		return *v2 - *v1;
	}

	static int my_sort( int * v1, int * v2 )
	{
		return *v1 - *v2;
	}

#define INITIAL_NUMBER_OF_ITEMS 10000

	ListModel::ListModel()
		: wxDataViewVirtualListModel( INITIAL_NUMBER_OF_ITEMS )
	{
		// the first 100 items are really stored in this model;
		// all the others are synthesized on request
		static const unsigned NUMBER_REAL_ITEMS = 100;

		m_textColValues.reserve( NUMBER_REAL_ITEMS );
		m_textColValues.push_back( _( "first row with long label to test ellipsization" ) );

		for ( unsigned int i = 1; i < NUMBER_REAL_ITEMS; i++ )
		{
			m_textColValues.push_back( wxString::Format( "real row %d", i ) );
		}

		m_iconColValues.assign( NUMBER_REAL_ITEMS, "test" );
	}

	void ListModel::Prepend( const wxString & text )
	{
		m_textColValues.Insert( text, 0 );
		RowPrepended();
	}

	void ListModel::DeleteItem( const wxDataViewItem & item )
	{
		unsigned int row = GetRow( item );

		if ( row >= m_textColValues.GetCount() )
			return;

		m_textColValues.RemoveAt( row );
		RowDeleted( row );
	}

	void ListModel::DeleteItems( const wxDataViewItemArray & items )
	{
		unsigned i;
		wxArrayInt rows;
		for ( i = 0; i < items.GetCount(); i++ )
		{
			unsigned int row = GetRow( items[i] );
			if ( row < m_textColValues.GetCount() )
				rows.Add( row );
		}

		if ( rows.GetCount() == 0 )
		{
			// none of the selected items were in the range of the items
			// which we store... for simplicity, don't allow removing them
			castor::Logger::logError( "Cannot remove rows with an index greater than " + castor::string::toString( m_textColValues.GetCount() ) );
			return;
		}

		// Sort in descending order so that the last
		// row will be deleted first. Otherwise the
		// remaining indeces would all be wrong.
		rows.Sort( my_sort_reverse );
		for ( i = 0; i < rows.GetCount(); i++ )
			m_textColValues.RemoveAt( rows[i] );

		// This is just to test if wxDataViewCtrl can
		// cope with removing rows not sorted in
		// descending order
		rows.Sort( my_sort );
		RowsDeleted( rows );
	}

	void ListModel::AddMany()
	{
		Reset( GetCount() + 1000 );
	}

	void ListModel::GetValueByRow( wxVariant & variant, unsigned int row, unsigned int col ) const
	{
		switch ( col )
		{
		case Col_EditableText:
			if ( row >= m_textColValues.GetCount() )
				variant = wxString::Format( "virtual row %d", row );
			else
				variant = m_textColValues[row];
			break;

		case Col_IconText:
			{
				wxString text;
				if ( row >= m_iconColValues.GetCount() )
					text = "virtual icon";
				else
					text = m_iconColValues[row];

				variant << wxDataViewIconText( text, m_icon[row % 2] );
			}
			break;

		case Col_TextWithAttr:
			{
				static const char * labels[5] =
				{
					"blue", "green", "red", "bold cyan", "default",
				};

				variant = labels[row % 5];
			}
			break;

		case Col_Custom:
			variant = wxString::Format( "%d", row % 100 );
			break;

		case Col_Max:
			wxFAIL_MSG( "invalid column" );
		}
	}

	bool ListModel::GetAttrByRow( unsigned int row, unsigned int col, wxDataViewItemAttr & attr ) const
	{
		switch ( col )
		{
		case Col_EditableText:
			return false;

		case Col_IconText:
			if ( !( row % 2 ) )
				return false;
			attr.SetColour( *wxLIGHT_GREY );
			break;

		case Col_TextWithAttr:
		case Col_Custom:
			// do what the labels defined in GetValueByRow() hint at
			switch ( row % 5 )
			{
			case 0:
				attr.SetColour( *wxBLUE );
				break;

			case 1:
				attr.SetColour( *wxGREEN );
				break;

			case 2:
				attr.SetColour( *wxRED );
				break;

			case 3:
				attr.SetColour( *wxCYAN );
				attr.SetBold( true );
				break;

			case 4:
				return false;
			}
			break;

		case Col_Max:
			wxFAIL_MSG( "invalid column" );
		}

		return true;
	}

	bool ListModel::SetValueByRow( const wxVariant & variant, unsigned int row, unsigned int col )
	{
		switch ( col )
		{
		case Col_EditableText:
		case Col_IconText:
			if ( row >= m_textColValues.GetCount() )
			{
				// the item is not in the range of the items
				// which we store... for simplicity, don't allow editing it
				castor::Logger::logError( "Cannot edit rows with an index greater than " + castor::string::toString( m_textColValues.GetCount() ) );
				return false;
			}

			if ( col == Col_EditableText )
			{
				m_textColValues[row] = variant.GetString();
			}
			else // col == Col_IconText
			{
				wxDataViewIconText iconText;
				iconText << variant;
				m_iconColValues[row] = iconText.GetText();
			}
			return true;

		case Col_TextWithAttr:
		case Col_Custom:
			castor::Logger::logError( "Cannot edit the column " + castor::string::toString( col ) );
			break;

		case Col_Max:
			wxFAIL_MSG( "invalid column" );
		}

		return false;
	}
}
