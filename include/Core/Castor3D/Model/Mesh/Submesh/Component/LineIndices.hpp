/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LineIndices_H___
#define ___C3D_LineIndices_H___

#include "ComponentModule.hpp"

namespace castor3d
{
	struct LineIndices
	{
		//!\~english	The indices.
		//!\~french		Les indices.
		std::array< uint32_t, 2 > m_index;

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
