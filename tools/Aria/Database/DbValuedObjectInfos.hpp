/*
See LICENSE file in root folder
*/
#ifndef ___CTP_DbValuedObjectInfos_HPP___
#define ___CTP_DbValuedObjectInfos_HPP___

#include "DbPrerequisites.hpp"

namespace aria::db
{
	class ValuedObjectInfos
	{
	public:
		explicit ValuedObjectInfos( const std::string & name );
		ValuedObjectInfos( const std::string & name, FieldType type );
		ValuedObjectInfos( const std::string & name, FieldType type, uint32_t limits );

		void setType( FieldType type );
		void setType( FieldType type, uint32_t limits );

		FieldType getType() const
		{
			return m_type;
		}

		const std::string & getName() const
		{
			return m_name;
		}

		const uint32_t & getLimits() const
		{
			return m_limits;
		}

	protected:
		std::string m_name{};
		FieldType m_type{};
		uint32_t m_limits{};
	};
}

#endif
