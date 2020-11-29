/*
See LICENSE file in root folder
*/
#ifndef ___CTP_ListStoreDerivedModel_HPP___
#define ___CTP_ListStoreDerivedModel_HPP___

#include "Aria/Prerequisites.hpp"

#include <wx/dataview.h>

namespace aria
{
	class ListStoreDerivedModel
		: public wxDataViewListStore
	{
	public:
		virtual bool IsEnabledByRow( unsigned int row, unsigned int col ) const;
	};
}

#endif
