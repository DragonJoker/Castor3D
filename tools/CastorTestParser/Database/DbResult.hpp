/*
See LICENSE file in root folder
*/
#ifndef ___CTP_DbResult_HPP___
#define ___CTP_DbResult_HPP___

#include "DbRow.hpp"

namespace test_parser::db
{
	class Result
	{
	public:
		explicit Result( const ValuedObjectInfosArray & fieldInfos );

		uint32_t getFieldCount() const;
		ValuedObjectInfos const & getFieldInfos( uint32_t index ) const;
		void addRow( Row row );

		auto begin()const
		{
			return m_rows.begin();
		}

		auto end()const
		{
			return m_rows.end();
		}

		auto cbegin()const
		{
			return m_rows.cbegin();
		}

		auto cend()const
		{
			return m_rows.cend();
		}

		auto size()
		{
			return m_rows.size();
		}

		auto empty()
		{
			return m_rows.empty();
		}

	protected:
		RowArray m_rows;
		ValuedObjectInfosArray m_fieldInfos;
	};
}

#endif
