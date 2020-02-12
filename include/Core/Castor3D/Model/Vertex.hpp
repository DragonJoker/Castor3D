/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SubmeshVertex_H___
#define ___C3D_SubmeshVertex_H___

#include "VertexGroup.hpp"

#include <cstdint>

namespace castor3d
{
	struct SubmeshVertex
	{
		uint32_t m_index;
		InterleavedVertex m_vertex;
	};
}

#endif
