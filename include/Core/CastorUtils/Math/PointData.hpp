/*
See LICENSE file in root folder
*/
#ifndef ___CU_PointData_H___
#define ___CU_PointData_H___

namespace castor
{
	/**
	\~english
	\brief		Data holder for a point.
	\~french
	\brief		Conteneur des données d'un point.
	*/
	template< typename T, uint32_t TCount >
	struct PointData
	{
		std::array< T, TCount > data;
	};

	template< typename T >
	struct PointData< T, 2u >
	{
		T x;
		T y;
	};

	template< typename T >
	struct PointData< T, 3u >
	{
		T x;
		T y;
		T z;
	};

	template< typename T >
	struct PointData< T, 4u >
	{
		T x;
		T y;
		T z;
		T w;
	};
}

#endif
