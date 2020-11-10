#include "CastorTestParser/Database/DbStatementParameter.hpp"

#include "CastorTestParser/Database/DbConnection.hpp"
#include "CastorTestParser/Database/DbParameterBinding.hpp"

#include "CastorTestParser/Database/DbValuedObjectInfos.hpp"

#include <CastorUtils/Log/Logger.hpp>
#include <CastorUtils/Miscellaneous/StringUtils.hpp>

namespace test_parser::db
{
	int StatementParameter::SqliteDataTypes[size_t( FieldType::eCount )] =
	{
		SQLITE_NULL,     //!< FieldType::eNULL
		SQLITE_INTEGER,  //!< FieldType::eBIT
		SQLITE_INTEGER,  //!< FieldType::eSINT32
		SQLITE_INTEGER,  //!< FieldType::eSINT64
		SQLITE_INTEGER,  //!< FieldType::eUINT32
		SQLITE_INTEGER,  //!< FieldType::eUINT64
		SQLITE_FLOAT,    //!< FieldType::eFLOAT32
		SQLITE_FLOAT,    //!< FieldType::eFLOAT64
		SQLITE3_TEXT,    //!< FieldType::eCHAR
		SQLITE3_TEXT,    //!< FieldType::eVARCHAR
		SQLITE3_TEXT,    //!< FieldType::eTEXT
		SQLITE_INTEGER,  //!< FieldType::eDATE
		SQLITE_INTEGER,  //!< FieldType::eDATETIME
		SQLITE_INTEGER,  //!< FieldType::eTIME
		SQLITE_BLOB,     //!< FieldType::eBINARY
		SQLITE_BLOB,     //!< FieldType::eVARBINARY
		SQLITE_BLOB,     //!< FieldType::eBLOB
	};

	static const std::string ERROR_SQLITE_PARAMETER_TYPE = "Undefined parameter type when trying to set its binding.";

	StatementParameter::StatementParameter( Connection & connection
		, ValuedObjectInfos infos
		, unsigned short index
		, ParameterType parameterType
		, std::unique_ptr< ValueUpdater > updater )
		: Parameter{ connection, infos, index, parameterType, std::move( updater ) }
		, m_dataType( SqliteDataTypes[size_t( getType() )] )
	{
	}

	const int & StatementParameter::getDataType() const
	{
		return m_dataType;
	}

	void StatementParameter::setNull()
	{
		Parameter::setNull();
		m_binding->updateValue();
	}

	void StatementParameter::setStatement( sqlite3_stmt * statement )
	{
		m_statement = statement;

		switch ( getType() )
		{
		case FieldType::eBIT:
			m_binding = makeSqliteBind< FieldType::eBIT >( m_statement, getConnection(), getIndex(), getObjectValue() );
			break;

		case FieldType::eSINT32:
			m_binding = makeSqliteBind< FieldType::eSINT32 >( m_statement, getConnection(), getIndex(), getObjectValue() );
			break;

		case FieldType::eSINT64:
			m_binding = makeSqliteBind< FieldType::eSINT64 >( m_statement, getConnection(), getIndex(), getObjectValue() );
			break;

		case FieldType::eUINT32:
			m_binding = makeSqliteBind< FieldType::eUINT32 >( m_statement, getConnection(), getIndex(), getObjectValue() );
			break;

		case FieldType::eUINT64:
			m_binding = makeSqliteBind< FieldType::eUINT64 >( m_statement, getConnection(), getIndex(), getObjectValue() );
			break;

		case FieldType::eFLOAT32:
			m_binding = makeSqliteBind< FieldType::eFLOAT32 >( m_statement, getConnection(), getIndex(), getObjectValue() );
			break;

		case FieldType::eFLOAT64:
			m_binding = makeSqliteBind< FieldType::eFLOAT64 >( m_statement, getConnection(), getIndex(), getObjectValue() );
			break;

		case FieldType::eCHAR:
			m_binding = makeSqliteBind< FieldType::eCHAR >( m_statement, getConnection(), getIndex(), getObjectValue() );
			break;

		case FieldType::eVARCHAR:
			m_binding = makeSqliteBind< FieldType::eVARCHAR >( m_statement, getConnection(), getIndex(), getObjectValue() );
			break;

		case FieldType::eTEXT:
			m_binding = makeSqliteBind< FieldType::eTEXT >( m_statement, getConnection(), getIndex(), getObjectValue() );
			break;

		case FieldType::eDATE:
			m_binding = makeSqliteBind< FieldType::eDATE >( m_statement, getConnection(), getIndex(), getObjectValue() );
			break;

		case FieldType::eDATETIME:
			m_binding = makeSqliteBind< FieldType::eDATETIME >( m_statement, getConnection(), getIndex(), getObjectValue() );
			break;

		case FieldType::eTIME:
			m_binding = makeSqliteBind< FieldType::eTIME >( m_statement, getConnection(), getIndex(), getObjectValue() );
			break;

		case FieldType::eBINARY:
			m_binding = makeSqliteBind< FieldType::eBINARY >( m_statement, getConnection(), getIndex(), getObjectValue() );
			break;

		case FieldType::eVARBINARY:
			m_binding = makeSqliteBind< FieldType::eVARBINARY >( m_statement, getConnection(), getIndex(), getObjectValue() );
			break;

		case FieldType::eBLOB:
			m_binding = makeSqliteBind< FieldType::eBLOB >( m_statement, getConnection(), getIndex(), getObjectValue() );
			break;

		default:
			throw std::runtime_error{ ERROR_SQLITE_PARAMETER_TYPE };
			break;
		}
	}
}
