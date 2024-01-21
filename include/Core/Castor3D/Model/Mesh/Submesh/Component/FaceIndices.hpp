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
		castor::Array< uint32_t, 3 > m_index;

		uint32_t const & operator[]( size_t i )const noexcept
		{
			return m_index[i];
		}

		uint32_t & operator[]( size_t i )noexcept
		{
			return m_index[i];
		}
	};
}

#endif
