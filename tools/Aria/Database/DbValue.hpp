/*
See LICENSE file in root folder
*/
#ifndef ___CTP_DbValue_HPP___
#define ___CTP_DbValue_HPP___

#include "DbValueBase.hpp"

namespace aria::db
{
	template< FieldType FieldTypeT, typename PolicyT >
	class ValueT
		: public ValueBase
		, private PolicyT
	{
	public:
		using value_type = typename PolicyT::value_type;

	public:
		explicit ValueT( Connection & connection )
			: ValueBase{ connection }
		{
		}

		void setValue( const value_type & value )
		{
			setNull( !PolicyT::set( value, m_tValue, m_valueSize, m_connection ) );
		}

		void setValue( ValueBase const & value ) override
		{
			setValue( static_cast< ValueT const & >( value ).m_tValue );
		}

		std::string getQueryValue() const override
		{
			return PolicyT::toQueryValue( m_tValue, !isNull(), m_connection );
		}

		void * getPtrValue() override
		{
			return PolicyT::ptr( m_tValue );
		}

		const void * getPtrValue() const override
		{
			if ( !isNull() )
			{
				return PolicyT::ptr( m_tValue );
			}
			else
			{
				return nullptr;
			}
		}

		const value_type & getValue()const
		{
			return m_tValue;
		}

	private:
		void doSetNull() override
		{
			m_tValue = value_type();
			m_valueSize = 0;
		}

	private:
		value_type m_tValue{};
	};
}

#include "DbValue.inl"

#endif
