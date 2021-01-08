#include "DbValuePolicy.hpp"

namespace aria::db
{
	//*********************************************************************************************

	template< typename PolicyT >
	class ValueT< FieldType::eCHAR, PolicyT >
		: public ValueBase
		, private PolicyT
	{
	public:
		typedef typename PolicyT::value_type value_type;

	public:
		ValueT( Connection & connection )
			: ValueBase( connection )
		{
		}

		inline void setValue( ValueBase const & value ) override
		{
			ValueT< FieldType::eCHAR > const & val = static_cast< ValueT< FieldType::eCHAR > const & >( value );
			setValue( val.m_tValue.c_str(), val.getPtrSize() );
		}

		inline std::string getQueryValue() const override
		{
			return PolicyT::toQueryValue( m_tValue, !isNull(), m_connection );
		}

		inline void * getPtrValue() override
		{
			return PolicyT::ptr( m_tValue );
		}

		inline const void * getPtrValue() const override
		{
			return PolicyT::ptr( m_tValue );
		}

		inline const value_type & getValue()const
		{
			return m_tValue;
		}

		inline void setValue( const value_type & tValue )
		{
			setNull( !PolicyT::set( tValue, m_tValue, m_valueSize, m_connection ) );
		}

		inline void setValue( const char * tValue, uint32_t limits )
		{
			value_type value;

			if ( limits != 0 )
			{
				size_t length = strlen( tValue );

				if ( length < limits )
				{
					std::stringstream stream;
					stream.width( limits );
					stream.fill( ' ' );
					stream << std::left << tValue;
					value = stream.str();
				}
				else
				{
					value.assign( tValue, tValue + limits );
				}
			}

			setValue( value );
		}

	private:
		inline void doSetNull() override
		{
			m_tValue = value_type();
			m_valueSize = 0;
		}

	private:
		value_type m_tValue{};
	};

	//*********************************************************************************************

	template< typename PolicyT >
	class ValueT< FieldType::eVARCHAR, PolicyT >
		: public ValueBase
		, private PolicyT
	{
	public:
		typedef typename PolicyT::value_type value_type;

	public:
		ValueT( Connection & connection )
			: ValueBase( connection )
		{
		}

		inline void setValue( ValueBase const & value ) override
		{
			setValue( static_cast< ValueT< FieldType::eVARCHAR > const & >( value ).m_tValue );
		}

		inline void setValue( const value_type & tValue )
		{
			setNull( !PolicyT::set( tValue, m_tValue, m_valueSize, m_connection ) );
		}

		inline void setValue( const char * tValue, uint32_t limits )
		{
			value_type value;

			if ( limits != 0 )
			{
				value.assign( tValue, tValue + std::min< uint32_t >( limits, uint32_t( strlen( tValue ) ) ) );
			}

			setValue( value );
		}

		inline std::string getQueryValue() const override
		{
			return PolicyT::toQueryValue( m_tValue, !isNull(), m_connection );
		}

		inline void * getPtrValue() override
		{
			return PolicyT::ptr( m_tValue );
		}

		inline const void * getPtrValue() const override
		{
			return PolicyT::ptr( m_tValue );
		}

		inline const value_type & getValue()const
		{
			return m_tValue;
		}

	private:
		inline void doSetNull() override
		{
			m_tValue = value_type();
			m_valueSize = 0;
		}

	private:
		value_type m_tValue{};
	};

	//*********************************************************************************************

	template< typename PolicyT >
	class ValueT< FieldType::eTEXT, PolicyT >
		: public ValueBase
		, private PolicyT
	{
	public:
		typedef typename PolicyT::value_type value_type;

	public:
		ValueT( Connection & connection )
			: ValueBase( connection )
		{
		}

		inline void setValue( ValueBase const & value ) override
		{
			setValue( static_cast< ValueT< FieldType::eTEXT > const & >( value ).m_tValue );
		}

		inline void setValue( const value_type & tValue )
		{
			setNull( !PolicyT::set( tValue, m_tValue, m_valueSize, m_connection ) );
		}

		inline void setValue( const char * tValue, uint32_t limits )
		{
			value_type value;

			if ( limits != 0 )
			{
				value.assign( tValue, tValue + std::min< uint32_t >( limits, uint32_t( strlen( tValue ) ) ) );
			}

			setValue( value );
		}

		inline std::string getQueryValue() const override
		{
			return PolicyT::toQueryValue( m_tValue, !isNull(), m_connection );
		}

		inline void * getPtrValue() override
		{
			return PolicyT::ptr( m_tValue );
		}

		inline const void * getPtrValue() const override
		{
			return PolicyT::ptr( m_tValue );
		}

		inline const value_type & getValue()const
		{
			return m_tValue;
		}

	private:
		inline void doSetNull() override
		{
			m_tValue = value_type();
			m_valueSize = 0;
		}

	private:
		value_type m_tValue{};
	};

	//*********************************************************************************************

	template< typename PolicyT >
	class ValueT< FieldType::eBINARY, PolicyT >
		: public ValueBase
		, private PolicyT
	{
	public:
		typedef typename PolicyT::value_type value_type;

	public:
		ValueT( Connection & connection )
			: ValueBase( connection )
		{
		}

		inline void setValue( ValueBase const & value ) override
		{
			setValue( static_cast< ValueT< FieldType::eBINARY > const & >( value ).m_tValue );
		}

		inline void setValue( const value_type & tValue )
		{
			setNull( !PolicyT::set( tValue, m_tValue, m_valueSize, m_connection ) );
		}

		inline void setValue( const uint8_t * tValue, uint32_t size )
		{
			value_type value;

			if ( tValue && size )
			{
				value.insert( value.end(), tValue, tValue + size );
			}

			setValue( value );
		}

		inline std::string getQueryValue() const override
		{
			return PolicyT::toQueryValue( m_tValue, !isNull(), m_connection );
		}

		inline void * getPtrValue() override
		{
			return PolicyT::ptr( m_tValue );
		}

		inline const void * getPtrValue() const override
		{
			return PolicyT::ptr( m_tValue );
		}

		inline const value_type & getValue()const
		{
			return m_tValue;
		}

	private:
		inline void doSetNull() override
		{
			m_tValue = value_type( 0 );
			m_valueSize = 0;
		}

	private:
		value_type m_tValue{};
	};

	//*********************************************************************************************

	template< typename PolicyT >
	class ValueT< FieldType::eVARBINARY, PolicyT >
		: public ValueBase
		, private PolicyT
	{
	public:
		typedef typename PolicyT::value_type value_type;

	public:
		ValueT( Connection & connection )
			: ValueBase( connection )
		{
		}

		inline void setValue( ValueBase const & value ) override
		{
			setValue( static_cast< ValueT< FieldType::eVARBINARY > const & >( value ).m_tValue );
		}

		inline void setValue( const value_type & tValue )
		{
			setNull( !PolicyT::set( tValue, m_tValue, m_valueSize, m_connection ) );
		}

		inline void setValue( const uint8_t * tValue, uint32_t size )
		{
			value_type value;

			if ( tValue && size )
			{
				value.insert( value.end(), tValue, tValue + size );
			}

			setValue( value );
		}

		inline std::string getQueryValue() const override
		{
			return PolicyT::toQueryValue( m_tValue, !isNull(), m_connection );
		}

		inline void * getPtrValue() override
		{
			return PolicyT::ptr( m_tValue );
		}

		inline const void * getPtrValue() const override
		{
			return PolicyT::ptr( m_tValue );
		}

		inline const value_type & getValue()const
		{
			return m_tValue;
		}

	private:
		inline void doSetNull() override
		{
			m_tValue = value_type();
			m_valueSize = 0;
		}

	private:
		value_type m_tValue{};
	};

	//*********************************************************************************************

	template< typename PolicyT >
	class ValueT< FieldType::eBLOB, PolicyT >
		: public ValueBase
		, private PolicyT
	{
	public:
		typedef typename PolicyT::value_type value_type;

	public:
		ValueT( Connection & connection )
			: ValueBase( connection )
		{
		}

		inline void setValue( ValueBase const & value ) override
		{
			setValue( static_cast< ValueT< FieldType::eBLOB > const & >( value ).m_tValue );
		}

		inline void setValue( const value_type & tValue )
		{
			setNull( !PolicyT::set( tValue, m_tValue, m_valueSize, m_connection ) );
		}

		inline void setValue( const uint8_t * tValue, uint32_t size )
		{
			value_type value;

			if ( tValue && size )
			{
				value.insert( value.end(), tValue, tValue + size );
			}

			setValue( value );
		}

		inline std::string getQueryValue() const override
		{
			return PolicyT::toQueryValue( m_tValue, !isNull(), m_connection );
		}

		inline void * getPtrValue() override
		{
			return PolicyT::ptr( m_tValue );
		}

		inline const void * getPtrValue() const override
		{
			return PolicyT::ptr( m_tValue );
		}

		inline const value_type & getValue()const
		{
			return m_tValue;
		}

	private:
		inline void doSetNull() override
		{
			m_tValue = value_type();
			m_valueSize = 0;
		}

	private:
		value_type m_tValue{ 0 };
	};

	//*********************************************************************************************
}
