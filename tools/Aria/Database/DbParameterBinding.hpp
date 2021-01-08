/*
See LICENSE file in root folder
*/
#ifndef ___CTP_DbParameterBinding_HPP___
#define ___CTP_DbParameterBinding_HPP___

#include "DbPrerequisites.hpp"

#include "DbStatementParameter.hpp"
#include "DbDateTimeHelpers.hpp"

#include <CastorUtils/Log/Logger.hpp>

namespace aria::db
{
	static const std::string ERROR_SQLITE_PARAMETER_VALUE = "Can't set parameter value";
	static const std::string ERROR_SQLITE_UPDATE_UNIMPLEMENTED = "updateValue not implemented for this data type";

	static const std::string INFO_SQLITE_SET_PARAMETER_VALUE = "Set parameter value: ";
	static const std::string INFO_SQLITE_SET_PARAMETER_NULL = "Set parameter NULL";

	static const std::string SQLITE_FORMAT_STMT_SDATE = "%Y-%m-%d";
	static const std::string SQLITE_FORMAT_STMT_STIME = "%H:%M:%S";
	static const std::string SQLITE_FORMAT_STMT_SDATETIME = "%Y-%m-%d %H:%M:%S";

	template< FieldType FieldTypeT >
	struct ParameterBindingT
		: public ParameterBinding
	{
		ParameterBindingT( sqlite3_stmt * statement
			, sqlite3 * connection
			, uint16_t index
			, ValueT< FieldTypeT > const & value )
			: ParameterBinding{ statement, connection, index }
			, mm_value{ value }
		{
		}

		void updateValue() override
		{
			throw std::runtime_error{ ERROR_SQLITE_UPDATE_UNIMPLEMENTED };
		}

		ValueT< FieldTypeT > mm_value;
	};

	template<>
	struct ParameterBindingT< FieldType::eBIT >
		: public ParameterBinding
	{
		ParameterBindingT( sqlite3_stmt * statement, sqlite3 * connection, uint16_t index, ValueT< FieldType::eBIT > const & value )
			: ParameterBinding( statement, connection, index )
			, m_value{ value }
		{
		}

		void updateValue() override
		{
			if ( m_value.isNull() )
			{
				sqliteCheck( sqlite3_bind_null( statement, index ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_NULL, connection );
			}
			else
			{
				sqliteCheck( sqlite3_bind_int( statement, index, m_value.getValue() ? 1 : 0 ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_VALUE << m_value.getValue(), connection );
			}
		}

		ValueT< FieldType::eBIT > const & m_value;
	};

	struct IntegerParameterBinding
		: public ParameterBinding
	{
		IntegerParameterBinding( sqlite3_stmt * statement, sqlite3 * connection, uint16_t index )
			: ParameterBinding( statement, connection, index )
		{
		}

		void updateValue( bool null, int value )
		{
			if ( null )
			{
				sqliteCheck( sqlite3_bind_null( statement, index ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_NULL, connection );
			}
			else
			{
				sqliteCheck( sqlite3_bind_int( statement, index, value ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_VALUE << value, connection );
			}
		}
	};

	template<>
	struct ParameterBindingT< FieldType::eSINT32 >
		: public IntegerParameterBinding
	{
		ParameterBindingT( sqlite3_stmt * statement, sqlite3 * connection, uint16_t index, ValueT< FieldType::eSINT32 > const & value )
			: IntegerParameterBinding( statement, connection, index )
			, m_value{ value }
		{
		}

		void updateValue() override
		{
			IntegerParameterBinding::updateValue( m_value.isNull(), int( m_value.getValue() ) );
		}

		ValueT< FieldType::eSINT32 > const & m_value;
	};

	template<>
	struct ParameterBindingT< FieldType::eUINT32 >
		: public IntegerParameterBinding
	{
		ParameterBindingT( sqlite3_stmt * statement, sqlite3 * connection, uint16_t index, ValueT< FieldType::eUINT32 > const & value )
			: IntegerParameterBinding( statement, connection, index )
			, m_value{ value }
		{
		}

		void updateValue() override
		{
			IntegerParameterBinding::updateValue( m_value.isNull(), int( m_value.getValue() ) );
		}

		ValueT< FieldType::eUINT32 > const & m_value;
	};

	template<>
	struct ParameterBindingT< FieldType::eSINT64 >
		: public ParameterBinding
	{
		ParameterBindingT( sqlite3_stmt * statement, sqlite3 * connection, uint16_t index, ValueT< FieldType::eSINT64 > const & value )
			: ParameterBinding( statement, connection, index )
			, m_value{ value }
		{
		}

		void updateValue() override
		{
			if ( m_value.isNull() )
			{
				sqliteCheck( sqlite3_bind_null( statement, index ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_NULL, connection );
			}
			else
			{
				sqliteCheck( sqlite3_bind_int64( statement, index, m_value.getValue() ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_VALUE << m_value.getValue(), connection );
			}
		}

		ValueT< FieldType::eSINT64 > const & m_value;
	};

	template<>
	struct ParameterBindingT< FieldType::eUINT64 >
		: public ParameterBinding
	{
		ParameterBindingT( sqlite3_stmt * statement, sqlite3 * connection, uint16_t index, ValueT< FieldType::eUINT64 > const & value )
			: ParameterBinding( statement, connection, index )
			, m_value{ value }
		{
		}

		void updateValue() override
		{
			if ( m_value.isNull() )
			{
				sqliteCheck( sqlite3_bind_null( statement, index ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_NULL, connection );
			}
			else
			{
				sqliteCheck( sqlite3_bind_int64( statement, index, m_value.getValue() ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_VALUE << m_value.getValue(), connection );
			}
		}

		ValueT< FieldType::eUINT64 > const & m_value;
	};

	template<>
	struct ParameterBindingT< FieldType::eFLOAT32 >
		: public ParameterBinding
	{
		ParameterBindingT( sqlite3_stmt * statement, sqlite3 * connection, uint16_t index, ValueT< FieldType::eFLOAT32 > const & value )
			: ParameterBinding( statement, connection, index )
			, m_value{ value }
		{
		}

		void updateValue() override
		{
			if ( m_value.isNull() )
			{
				sqliteCheck( sqlite3_bind_null( statement, index ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_NULL, connection );
			}
			else
			{
				sqliteCheck( sqlite3_bind_double( statement, index, m_value.getValue() ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_VALUE << m_value.getValue(), connection );
			}
		}

		ValueT< FieldType::eFLOAT32 > const & m_value;
	};

	template<>
	struct ParameterBindingT< FieldType::eFLOAT64 >
		: public ParameterBinding
	{
		ParameterBindingT( sqlite3_stmt * statement, sqlite3 * connection, uint16_t index, ValueT< FieldType::eFLOAT64 > const & value )
			: ParameterBinding( statement, connection, index )
			, m_value{ value }
		{
		}

		void updateValue() override
		{
			if ( m_value.isNull() )
			{
				sqliteCheck( sqlite3_bind_null( statement, index ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_NULL, connection );
			}
			else
			{
				sqliteCheck( sqlite3_bind_double( statement, index, m_value.getValue() ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_VALUE << m_value.getValue(), connection );
			}
		}

		ValueT< FieldType::eFLOAT64 > const & m_value;
	};

	template<>
	struct ParameterBindingT< FieldType::eCHAR >
		: public ParameterBinding
	{
		ParameterBindingT( sqlite3_stmt * statement, sqlite3 * connection, uint16_t index, ValueT< FieldType::eCHAR > const & value )
			: ParameterBinding( statement, connection, index )
			, m_value{ value }
		{
		}

		void updateValue() override
		{
			if ( m_value.isNull() )
			{
				sqliteCheck( sqlite3_bind_null( statement, index ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_NULL, connection );
			}
			else
			{
				sqliteCheck( sqlite3_bind_text64( statement, index, ( const char * )m_value.getPtrValue(), m_value.getPtrSize(), SQLITE_STATIC, SQLITE_UTF8 ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_VALUE << "[" <<  m_value.getValue() << "]", connection );
			}
		}

		ValueT< FieldType::eCHAR > const & m_value;
	};

	template<>
	struct ParameterBindingT< FieldType::eVARCHAR >
		: public ParameterBinding
	{
		ParameterBindingT( sqlite3_stmt * statement, sqlite3 * connection, uint16_t index, ValueT< FieldType::eVARCHAR > const & value )
			: ParameterBinding( statement, connection, index )
			, m_value{ value }
		{
		}

		void updateValue() override
		{
			if ( m_value.isNull() )
			{
				sqliteCheck( sqlite3_bind_null( statement, index ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_NULL, connection );
			}
			else
			{
				sqliteCheck( sqlite3_bind_text64( statement, index, ( const char * )m_value.getPtrValue(), m_value.getPtrSize(), SQLITE_STATIC, SQLITE_UTF8 ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_VALUE << "[" <<  m_value.getValue() << "]", connection );
			}
		}

		ValueT< FieldType::eVARCHAR > const & m_value;
	};

	template<>
	struct ParameterBindingT< FieldType::eTEXT >
		: public ParameterBinding
	{
		ParameterBindingT( sqlite3_stmt * statement, sqlite3 * connection, uint16_t index, ValueT< FieldType::eTEXT > const & value )
			: ParameterBinding( statement, connection, index )
			, m_value{ value }
		{
		}

		void updateValue() override
		{
			if ( m_value.isNull() )
			{
				sqliteCheck( sqlite3_bind_null( statement, index ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_NULL, connection );
			}
			else
			{
				sqliteCheck( sqlite3_bind_text64( statement, index, ( const char * )m_value.getPtrValue(), m_value.getPtrSize(), SQLITE_STATIC, SQLITE_UTF8 ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_VALUE << "[" <<  m_value.getValue() << "]", connection );
			}
		}

		ValueT< FieldType::eTEXT > const & m_value;
	};

	template<>
	struct ParameterBindingT< FieldType::eDATE >
		: public ParameterBinding
	{
		ParameterBindingT( sqlite3_stmt * statement, sqlite3 * connection, uint16_t index, ValueT< FieldType::eDATE > const & value )
			: ParameterBinding( statement, connection, index )
			, m_value{ value }
		{
		}

		void updateValue() override
		{
			if ( m_value.isNull() )
			{
				sqliteCheck( sqlite3_bind_null( statement, index ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_NULL, connection );
			}
			else
			{
				m_holder = date::format( m_value.getValue(), SQLITE_FORMAT_STMT_SDATE );
				sqliteCheck( sqlite3_bind_text( statement, index, m_holder.c_str(), int( m_holder.size() ), SQLITE_STATIC ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_VALUE << m_holder, connection );
			}
		}

		ValueT< FieldType::eDATE > const & m_value;
		std::string m_holder;
	};

	template<>
	struct ParameterBindingT< FieldType::eDATETIME >
		: public ParameterBinding
	{
		ParameterBindingT( sqlite3_stmt * statement, sqlite3 * connection, uint16_t index, ValueT< FieldType::eDATETIME > const & value )
			: ParameterBinding( statement, connection, index )
			, m_value{ value }
		{
		}

		void updateValue() override
		{
			if ( m_value.isNull()
				|| !db::date_time::isValid( m_value.getValue() ) )
			{
				sqliteCheck( sqlite3_bind_null( statement, index ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_NULL, connection );
			}
			else
			{
				m_holder = date_time::format( m_value.getValue(), SQLITE_FORMAT_STMT_SDATETIME );
				sqliteCheck( sqlite3_bind_text( statement, index, m_holder.c_str(), int( m_holder.size() ), SQLITE_STATIC ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_VALUE << m_holder, connection );
			}
		}

		ValueT< FieldType::eDATETIME > const & m_value;
		std::string m_holder;
	};

	template<>
	struct ParameterBindingT< FieldType::eTIME >
		: public ParameterBinding
	{
		ParameterBindingT( sqlite3_stmt * statement, sqlite3 * connection, uint16_t index, ValueT< FieldType::eTIME > const & value )
			: ParameterBinding( statement, connection, index )
			, m_value{ value }
		{
		}

		void updateValue() override
		{
			if ( m_value.isNull() )
			{
				sqliteCheck( sqlite3_bind_null( statement, index ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_NULL, connection );
			}
			else
			{
				m_holder = time::format( m_value.getValue(), SQLITE_FORMAT_STMT_STIME );
				sqliteCheck( sqlite3_bind_text( statement, index, m_holder.c_str(), int( m_holder.size() ), SQLITE_STATIC ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_VALUE << m_holder, connection );
			}
		}

		ValueT< FieldType::eTIME > const & m_value;
		std::string m_holder;
	};

	template<>
	struct ParameterBindingT< FieldType::eBINARY >
		: public ParameterBinding
	{
		ParameterBindingT( sqlite3_stmt * statement, sqlite3 * connection, uint16_t index, ValueT< FieldType::eBINARY > const & value )
			: ParameterBinding( statement, connection, index )
			, m_value{ value }
		{
		}

		void updateValue() override
		{
			if ( m_value.isNull() )
			{
				sqliteCheck( sqlite3_bind_null( statement, index ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_NULL, connection );
			}
			else
			{
				sqliteCheck( sqlite3_bind_blob( statement, index, m_value.getPtrValue(), m_value.getPtrSize(), SQLITE_STATIC ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_VALUE << m_value.getPtrValue(), connection );
			}
		}

		ValueT< FieldType::eBINARY > const & m_value;
	};

	template<>
	struct ParameterBindingT< FieldType::eVARBINARY >
		: public ParameterBinding
	{
		ParameterBindingT( sqlite3_stmt * statement, sqlite3 * connection, uint16_t index, ValueT< FieldType::eVARBINARY > const & value )
			: ParameterBinding( statement, connection, index )
			, m_value{ value }
		{
		}

		void updateValue() override
		{
			if ( m_value.isNull() )
			{
				sqliteCheck( sqlite3_bind_null( statement, index ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_NULL, connection );
			}
			else
			{
				sqliteCheck( sqlite3_bind_blob( statement, index, m_value.getPtrValue(), m_value.getPtrSize(), SQLITE_STATIC ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_VALUE << m_value.getPtrValue(), connection );
			}
		}

		ValueT< FieldType::eVARBINARY > const & m_value;
	};

	template<>
	struct ParameterBindingT< FieldType::eBLOB >
		: public ParameterBinding
	{
		ParameterBindingT( sqlite3_stmt * statement, sqlite3 * connection, uint16_t index, ValueT< FieldType::eBLOB > const & value )
			: ParameterBinding( statement, connection, index )
			, m_value{ value }
		{
		}

		void updateValue() override
		{
			if ( m_value.isNull() )
			{
				sqliteCheck( sqlite3_bind_null( statement, index ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_NULL, connection );
			}
			else
			{
				sqliteCheck( sqlite3_bind_blob( statement, index, m_value.getPtrValue(), m_value.getPtrSize(), SQLITE_STATIC ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_VALUE << m_value.getPtrValue(), connection );
			}
		}

		ValueT< FieldType::eBLOB > const & m_value;
	};

	template< FieldType FieldTypeT >
	std::unique_ptr< ParameterBinding > makeSqliteBind( sqlite3_stmt * statement
		, Connection & connection
		, uint16_t index
		, ValueBase const & value )
	{
		return std::make_unique< ParameterBindingT< FieldTypeT > >( statement
			, connection.getConnection()
			, index
			, static_cast< ValueT< FieldTypeT > const & >( value ) );
	}
}

#endif
