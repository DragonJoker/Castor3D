/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LineIndices_H___
#define ___C3D_LineIndices_H___

#include "Castor3D/Castor3DPrerequisites.hpp"

namespace castor3d
{
	struct LineIndices
	{
		//!\~english	The indices.
		//!\~french		Les indices.
		std::array< uint32_t, 2 > m_index;
	};
}

#endif
