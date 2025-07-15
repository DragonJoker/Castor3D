/*
See LICENSE file in root folder
*/
#ifndef ___CU_PointData_H___
#define ___CU_PointData_H___

#include "CastorUtils/Math/MathModule.hpp"

namespace c3d
{
	template< typename T, uint32_t TCount >
	struct PointData
	{
		Array< T, TCount > data;
	};

	template< typename T >
	struct PointData< T, 2u >
	{
		T x{};
		T y{};
	};

	template< typename T >
	struct PointData< T, 3u >
	{
		T x{};
		T y{};
		T z{};
	};

	template< typename T >
	struct PointData< T, 4u >
	{
		T x{};
		T y{};
		T z{};
		T w{};
	};
}

#endif
