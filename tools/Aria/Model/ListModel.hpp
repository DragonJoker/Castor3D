/*
See LICENSE file in root folder
*/
#ifndef ___CTP_ListModel_HPP___
#define ___CTP_ListModel_HPP___

#include "Aria/Prerequisites.hpp"

#include <wx/dataview.h>

namespace aria
{
	class ListModel
		: public wxDataViewVirtualListModel
	{
	public:
		enum
		{
			Col_EditableText,
			Col_IconText,
			Col_TextWithAttr,
			Col_Custom,
			Col_Max
		};

		ListModel();

		// helper methods to change the model

		void Prepend( const wxString & text );
		void DeleteItem( const wxDataViewItem & item );
		void DeleteItems( const wxDataViewItemArray & items );
		void AddMany();


		// implementation of base class virtuals to define model

		virtual unsigned int GetColumnCount() const
		{
			return Col_Max;
		}

		virtual wxString GetColumnType( unsigned int col ) const
		{
			if ( col == Col_IconText )
			{
				return wxT( "wxDataViewIconText" );
			}

			return wxT( "string" );
		}

		virtual void GetValueByRow( wxVariant & variant, unsigned int row, unsigned int col ) const;
		virtual bool GetAttrByRow( unsigned int row, unsigned int col, wxDataViewItemAttr & attr ) const;
		virtual bool SetValueByRow( const wxVariant & variant, unsigned int row, unsigned int col );

	private:
		wxArrayString m_textColValues;
		wxArrayString m_iconColValues;
		wxIcon m_icon[2];
	};
}

#endif
