/*
See LICENSE file in root folder
*/
#ifndef ___CU_DataHolder_H___
#define ___CU_DataHolder_H___

#include "CastorUtils/Design/DesignModule.hpp"

namespace castor
{
	template< class Data >
	class DataHolderT
	{
	public:
		DataHolderT()noexcept = default;

		DataHolderT( Data d )noexcept
			: m_data{ std::move( d ) }
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
			m_data = std::move( data );
		}

	private:
		Data m_data;
	};
}

#endif
