/*
See LICENSE file in root folder
*/
#ifndef ___C3D_FACE_INDICES_H___
#define ___C3D_FACE_INDICES_H___

#include "Castor3D/Castor3DPrerequisites.hpp"

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
