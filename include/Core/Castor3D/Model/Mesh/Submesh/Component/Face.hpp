/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Face_H___
#define ___C3D_Face_H___

#include "Castor3D/Model/Mesh/Submesh/Component/FaceIndices.hpp"

namespace castor3d
{
	class Face
	{
	public:
		Face( uint32_t a, uint32_t b, uint32_t c )noexcept
			: m_face{ { { a, b, c } } }
		{
		}

		Face()noexcept
			: Face{ 0u, 0u, 0u }
		{
		}

		uint32_t const & operator[]( uint32_t index )const noexcept
		{
			CU_Require( index < 3 );
			return m_face.m_index[index];
		}

		uint32_t & operator[]( uint32_t index )noexcept
		{
			CU_Require( index < 3 );
			return m_face.m_index[index];
		}

		uint32_t const * data()const noexcept
		{
			return m_face.m_index.data();
		}

		uint32_t * data()noexcept
		{
			return m_face.m_index.data();
		}

	private:
		//!\~english	The face vertex indices.
		//!\~french		Les indices des sommets de la face.
		FaceIndices m_face;
	};
}

#endif
