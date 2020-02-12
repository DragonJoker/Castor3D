/*
See LICENSE file in root folder
*/
#ifndef ___C3D_FaceIndices_H___
#define ___C3D_FaceIndices_H___

#include "ComponentModule.hpp"

#include <array>

namespace castor3d
{
	struct FaceIndices
	{
		//!\~english	The indices.
		//!\~french		Les indices.
		std::array< uint32_t, 3 > m_index;
	};
}

#endif
