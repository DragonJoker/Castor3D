/*
See LICENSE file in root folder
*/
#ifndef ___CU_DataHolder_H___
#define ___CU_DataHolder_H___

#include "CastorUtils/Design/DesignModule.hpp"

namespace c3d
{
	template< class Data, size_t Index >
	class DataHolderT
	{
	public:
		DataHolderT()noexcept = default;

		explicit DataHolderT( Data d )noexcept
			: m_data{ c3d::move( d ) }
		{
		}

		Data & getData()
		{
			return m_data;
		}

		Data const & getData()const
		{
			return m_data;
		}

		void setData( Data data )
		{
			m_data = c3d::move( data );
		}

	private:
		Data m_data;
	};
}

#endif
