/*
See LICENSE file in root folder
*/
#ifndef ___CU_HeightMapToNormalMap_H___
#define ___CU_HeightMapToNormalMap_H___

#include "CastorUtils/Graphics/GraphicsModule.hpp"

namespace castor
{
	CU_API bool convertToNormalMap( float strength
		, castor::Image & image );
}

#endif
