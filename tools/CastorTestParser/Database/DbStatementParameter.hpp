/*
See LICENSE file in root folder
*/
#ifndef ___CTP_DbStatementParameter_HPP___
#define ___CTP_DbStatementParameter_HPP___

#include "DbParameter.hpp"

namespace test_parser::db
{
	struct ParameterBinding
	{
		ParameterBinding( sqlite3_stmt * statement, sqlite3 * connection, uint16_t index )
			: statement{ statement }
			, connection{ connection }
			, index{ index }
		{
		}

		virtual ~ParameterBinding() = default;

		virtual void updateValue() = 0;

		sqlite3_stmt * statement;
		sqlite3 * connection;
		uint16_t index;
	};

	class StatementParameter
		: public Parameter
	{
	public:
		StatementParameter( Connection & connection
			, ValuedObjectInfos infos
			, unsigned short index
			, ParameterType parameterType
			, std::unique_ptr< ValueUpdater > updater );

		const int & getDataType()const;
		void setNull() override;
		void setStatement( sqlite3_stmt * statement );

	private:
		template< typename T >
		void doSetAndUpdateValue( const T & value )
		{
			ValuedObject::doSetValue( value );
			m_binding->updateValue();
		}

		template< typename T >
		void doSetAndUpdateValueFast( const T * value )
		{
			ValuedObject::doSetValue( value );
			m_binding->updateValue();
		}

		virtual void doSetValue( const bool & value )
		{
			doSetAndUpdateValue( value );
		}

		virtual void doSetValue( const int32_t & value )
		{
			doSetAndUpdateValue( value );
		}

		virtual void doSetValue( const uint32_t & value )
		{
			doSetAndUpdateValue( value );
		}

		virtual void doSetValue( const int64_t & value )
		{
			doSetAndUpdateValue( value );
		}

		virtual void doSetValue( const uint64_t & value )
		{
			doSetAndUpdateValue( value );
		}

		virtual void doSetValue( const float & value )
		{
			doSetAndUpdateValue( value );
		}

		virtual void doSetValue( const double & value )
		{
			doSetAndUpdateValue( value );
		}

		virtual void doSetValue( const std::string & value )
		{
			doSetAndUpdateValue( value );
		}

		virtual void doSetValue( const Date & value )
		{
			doSetAndUpdateValue( value );
		}

		virtual void doSetValue( const DateTime & value )
		{
			doSetAndUpdateValue( value );
		}

		virtual void doSetValue( const Time & value )
		{
			doSetAndUpdateValue( value );
		}

		virtual void doSetValue( const ByteArray & value )
		{
			doSetAndUpdateValue( value );
		}

	public:
		//! The SQLite data types associated to Database::EFieldType
		static int SqliteDataTypes[size_t( FieldType::eCount )];

	private:
		std::unique_ptr< ParameterBinding > m_binding;
		sqlite3_stmt * m_statement{};
		int m_dataType{};
	};
}

#endif
