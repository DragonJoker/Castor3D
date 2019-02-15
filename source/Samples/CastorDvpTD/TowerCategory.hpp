#pragma once

#include "Tower.hpp"

namespace castortd
{
	struct LongRangeTower
		: public Tower::Category
	{
		LongRangeTower();
	};

	struct ShortRangeTower
		: public Tower::Category
	{
		ShortRangeTower();
	};
}
