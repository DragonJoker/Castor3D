#include "Aria/Model/ListStoreDerivedModel.hpp"

namespace aria
{
	bool ListStoreDerivedModel::IsEnabledByRow( unsigned int row, unsigned int col ) const
	{
		// disabled the last two checkboxes
		return !( col == 0 && 8 <= row && row <= 9 );
	}
}
