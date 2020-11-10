/*
See LICENSE file in root folder
*/
#ifndef ___CTP_ListStoreDerivedModel_HPP___
#define ___CTP_ListStoreDerivedModel_HPP___

#include "CastorTestParser/Prerequisites.hpp"

#include <wx/dataview.h>

namespace test_parser
{
	class ListStoreDerivedModel
		: public wxDataViewListStore
	{
	public:
		virtual bool IsEnabledByRow( unsigned int row, unsigned int col ) const;
	};
}

#endif
